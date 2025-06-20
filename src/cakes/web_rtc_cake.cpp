#include "./web_rtc_cake.h"

#include <iostream>

#include "./_piece.h"



namespace patty_cake {


using std::cout;
using std::endl;



WebRtcCake::WebRtcCake (ConnectConfig const& cnf)
:next_client_id_(1)
{

  // make peer_connection
  default_connection_ =
      std::make_shared<WebRtcConnection>(
        "",
        this,
        WebRtcConnection::Type::CALLER,
        cnf.on_message_func,
        cnf.on_state_change_func,
        cnf.on_local_sdp_func,
        cnf.on_local_ice_func
      );


  // make data_channel
  default_connection_->createDataChannel(
      cnf.name,
      cnf.ordered,
      cnf.max_retransmits,
      cnf.on_message_func,
      cnf.on_state_change_func
  );


  // create SDP offer
  auto peer_connection = default_connection_->peer_connection();
  peer_connection->setLocalDescription();
}


WebRtcCake::WebRtcCake (ListenConfig const& cnf)
:next_client_id_(1)
{
}


WebRtcCake::~WebRtcCake () {
  disconnect();
}


PattyCake::Type WebRtcCake::type () const {
  return Type::WEB_RTC;
}


void WebRtcCake::disconnect () {
  default_connection_ = nullptr;
}


static bool __send (std::shared_ptr<WebRtcConnection> const& connection, std::vector<uint8_t> const& data) {
  if (connection == nullptr)
    return false;

  auto data_channel = connection->data_channel();
  if (data_channel == nullptr)
    return false;

  return data_channel->send((std::byte const*)data.data(), data.size());
}


bool WebRtcCake::send (std::vector<uint8_t> const& data) {
  return __send(default_connection_, data);
}


bool WebRtcCake::send (std::string const& id, std::vector<uint8_t> const& data) {
  auto client_info = findClientInfo(id);
  if (client_info == nullptr) {
    cout << "[patty_cake Error] cannot find client_info --- id: " << id << endl;
    return false;
  }

  if (client_info->is_alive() == false) {
    cout << "[patty_cake Error] client_info is not alive --- id: " << id << endl;
    return false;
  }

  auto connection = ((WebRtcClientInfo*)client_info.get())->connection;
  return __send(connection, data);
}


void WebRtcCake::poll () {
  // No-op for now
}


std::shared_ptr<WebRtcClientInfo> WebRtcCake::addListenConnection (ListenConfig const& cnf) {
  // make peer_connection
  auto client_id = std::to_string(_issueClientId());
  auto connection =
      std::make_shared<WebRtcConnection>(
        client_id,
        this,
        WebRtcConnection::Type::CALLEE,
        cnf.on_message_func,
        cnf.on_state_change_func,
        cnf.on_local_sdp_func,
        cnf.on_local_ice_func
      );


  auto client_info =
      std::make_shared<WebRtcClientInfo>(
        client_id,
        connection
      );
  _addClientInfo(client_info);

  return client_info;
}


uint32_t WebRtcCake::_issueClientId () {
  return next_client_id_.fetch_add(1);
}




WebRtcConnection::WebRtcConnection (
    std::string const& id,
    WebRtcCake* cake,
    Type type,
    PattyCake::OnMessageFunc on_message_func,
    PattyCake::OnStateChangeFunc on_state_change_func,
    PattyCake::OnLocalSdpFunc on_local_sdp_func,
    PattyCake::OnLocalIceFunc on_local_ice_func
)
:id_(id),
 cake_(cake),
 type_(type)
{
  rtc::Configuration config;
  config.iceServers.emplace_back("stun:stun.l.google.com:19302");

  peer_connection_ = std::make_shared<rtc::PeerConnection>(config);

  peer_connection_->onLocalDescription(
      [on_local_sdp_func, conn=this](rtc::Description desc) {
        if (on_local_sdp_func != nullptr)
          on_local_sdp_func(conn, std::string(desc));
      }
  );

  peer_connection_->onLocalCandidate(
      [on_local_ice_func, conn=this](rtc::Candidate cand) {
        if (on_local_ice_func != nullptr)
          on_local_ice_func(conn, cand.candidate());
      }
  );

  peer_connection_->onDataChannel(
      [this, on_message_func, on_state_change_func](std::shared_ptr<rtc::DataChannel> dc) {
        _setupDataChannel(dc, on_message_func, on_state_change_func);
      }
  );

}


WebRtcConnection::~WebRtcConnection () {
  disconnect();
}


void WebRtcConnection::disconnect () {
  data_channel_    = nullptr;
  peer_connection_ = nullptr;
}


void WebRtcConnection::createDataChannel (
    std::string const&           name,
    bool                         ordered,
    int                          max_retransmits,
    PattyCake::OnMessageFunc     on_message_func,
    PattyCake::OnStateChangeFunc on_state_change_func
) {

  rtc::DataChannelInit init;
  init.reliability.unordered      = !ordered;
  init.reliability.maxRetransmits = max_retransmits;

  auto dc = peer_connection_->createDataChannel(name, init);
  _setupDataChannel(dc, on_message_func, on_state_change_func);
}


void WebRtcConnection::receiveIceSdp (std::string const& sdp) {
  peer_connection_->addRemoteCandidate(rtc::Candidate(sdp));
}


void WebRtcConnection::receiveSdp (std::string const& sdp) {

  switch (type_) {
    case Type::CALLER:
      peer_connection_->setRemoteDescription({sdp, "answer"});
      break;

    case Type::CALLEE:
      peer_connection_->setRemoteDescription({sdp, "offer"});
      peer_connection_->setLocalDescription();
      break;

    default:
      cout << "[patty_cake Error] unhandled WebRtcConnection::Type --- " << (uint32_t)type_ << endl;
      break;
  }
}


void WebRtcConnection::_setupDataChannel (
    std::shared_ptr<rtc::DataChannel> const& dc,
    PattyCake::OnMessageFunc     on_message_func,
    PattyCake::OnStateChangeFunc on_state_change_func
) {
  data_channel_ = dc;

  data_channel_->onOpen([this, on_state_change_func]() { on_state_change_func(cake_, PattyCake::State::CONNECTED); });

  data_channel_->onClosed([this, on_state_change_func]() { on_state_change_func(cake_, PattyCake::State::DISCONNECTED); });

  data_channel_->onMessage(
      [this, on_message_func](rtc::message_variant msg) {
        if (std::holds_alternative<rtc::binary>(msg)) {
          const auto& data = std::get<rtc::binary>(msg);

          PattyCakePiece piece;
          piece.sender_id = id_;
          piece.data.resize(data.size());
          memcpy(piece.data.data(), data.data(), data.size());

          on_message_func(cake_, piece);
        }
      }
  );
}




WebRtcClientInfo::WebRtcClientInfo (
    std::string const& id,
    std::shared_ptr<WebRtcConnection> connection
)
:super(id),
 connection(connection)
{
}


bool WebRtcClientInfo::is_alive () const {
  return
      connection != nullptr
      && connection->cake() != nullptr
      && connection->cake()->state() == PattyCake::State::CONNECTED;
}


} // namespace patty_cake
