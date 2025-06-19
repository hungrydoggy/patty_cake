#include "./web_rtc_cake.h"

#include <iostream>



namespace patty_cake {


using std::cout;
using std::endl;
using std::make_shared;



class PeerConnectionObserver_ : public webrtc::PeerConnectionObserver {
public:// inner types
  using OnDataChannelFunc = std::function<void(webrtc::scoped_refptr<webrtc::DataChannelInterface> channel)>;

public: // methods
  PeerConnectionObserver_ (OnDataChannelFunc on_data_channel_func)
  :on_data_channel_func_(on_data_channel_func)
  {
  }

  void OnDataChannel (webrtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {
    on_data_channel_func_(channel);
  }

private: // vars
  OnDataChannelFunc on_data_channel_func_;
};


class DataChannelObserver_ : public webrtc::DataChannelObserver{
public: // methods
  DataChannelObserver_ (
      webrtc::scoped_refptr<webrtc::DataChannelInterface> dc,
      PattyCake::OnMessageFunc     on_message_func,
      PattyCake::OnStateChangeFunc on_state_change_func
  )
  :dc_(dc),
   on_message_func_(on_message_func),
   on_state_change_func_(on_state_change_func)
  {
    dc_->RegisterObserver(this);
  }

  ~DataChannelObserver_ () {
    dc_->UnregisterObserver();
  }

  void OnStateChange () override {
  }

  void OnMessage (webrtc::DataBuffer const& msg) override {
  }

private: // vars
  webrtc::scoped_refptr<webrtc::DataChannelInterface> dc_;
  PattyCake::OnMessageFunc     on_message_func_;
  PattyCake::OnStateChangeFunc on_state_change_func_;
};



WebRtcCake::WebRtcCake (ConnectConfig const& cnf) {

  // make factory
  auto network   = webrtc::Thread::CreateWithSocketServer();
  auto worker    = webrtc::Thread::Create();
  auto signaling = webrtc::Thread::Create();
  network  ->Start();
  worker   ->Start();
  signaling->Start();

  auto factory =
      webrtc::CreatePeerConnectionFactory(
        network.get(), worker.get(), signaling.get(),
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
      );


  // make peer_connection_
  _createPeerConnection(factory, cnf.on_state_change_func, cnf.on_message_func);


  // make data_channel_
  {
    webrtc::DataChannelInit init;
    init.ordered        = cnf.ordered;
    init.maxRetransmits = cnf.max_retransmits;

    auto result = peer_connection_->CreateDataChannelOrError(cnf.name, &init);
    if (result.ok() == false) {
      cout << "[patty_cake Error] cannot create data_channel" << endl;
      return;
    }
    data_channel_ = result.MoveValue();

    data_channel_observer_ =
        make_shared<DataChannelObserver_>(
          data_channel_,
          cnf.on_state_change_func,
          cnf.on_message_func
        );
  }
}


WebRtcCake::WebRtcCake (ListenConfig const& cnf) {
  // make factory
  auto network   = webrtc::Thread::CreateWithSocketServer();
  auto worker    = webrtc::Thread::Create();
  auto signaling = webrtc::Thread::Create();
  network  ->Start();
  worker   ->Start();
  signaling->Start();

  auto factory =
      webrtc::CreatePeerConnectionFactory(
        network.get(), worker.get(), signaling.get(),
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
      );


  // make peer_connection_
  _createPeerConnection(factory, cnf.on_state_change_func, cnf.on_message_func);
}


WebRtcCake::~WebRtcCake () {
  disconnect();
}


PattyCake::Type WebRtcCake::type () const {
  return Type::WEB_RTC;
}


void WebRtcCake::disconnect () {
  if (data_channel_)
    data_channel_->UnregisterObserver();

  if (peer_connection_)
    peer_connection_->Close();
}


bool WebRtcCake::send (std::vector<uint8_t> const& data) {
  if (data_channel_ == nullptr)
    return false;

  return data_channel_->Send(webrtc::DataBuffer(webrtc::CopyOnWriteBuffer(data.data(), data.size()), true));
}


bool WebRtcCake::send (std::string const& id, std::vector<uint8_t> const& data) {
  //TODO
  return send(data);
}


void WebRtcCake::poll () {
  // No-op for now
}


void WebRtcCake::_createPeerConnection (
    webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory,
    OnStateChangeFunc on_state_change_func,
    OnMessageFunc on_message_func
) {
  peer_connection_observer_ =
      make_shared<PeerConnectionObserver_>(
        this,
        [this, on_state_change_func, on_message_func]
            (webrtc::scoped_refptr<webrtc::DataChannelInterface> channel) {
              data_channel_ = channel;

              data_channel_observer_ =
                  make_shared<DataChannelObserver_>(
                    data_channel_,
                    on_state_change_func,
                    on_message_func
                  );
            }
      );

  webrtc::PeerConnectionInterface::RTCConfiguration config;
  webrtc::PeerConnectionDependencies deps(peer_connection_observer_.get());
  auto result = factory->CreatePeerConnectionOrError(config, std::move(deps));
  if (result.ok() == false) {
    cout << "[patty_cake Error] cannot create peer_connection" << endl;
    return;
  }

  peer_connection_ = result.MoveValue();
}


void WebRtcCake::OnDataChannel (rtc::scoped_refptr<webrtc::DataChannelInterface> channel) {
  data_channel_ = channel;
  data_channel_->RegisterObserver(this);
}


void WebRtcCake::OnMessage (const webrtc::DataBuffer& buffer) {
  auto vec = std::vector<uint8_t>(buffer.data.data<uint8_t>(), buffer.data.data<uint8_t>() + buffer.data.size());
  if (on_recv) on_recv("webrtc", vec);
}



} // namespace patty_cake
