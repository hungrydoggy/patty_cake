#ifndef PATTY_CAKE_CAKES_WEB_RTC_CAKE_H_
#define PATTY_CAKE_CAKES_WEB_RTC_CAKE_H_

#include <memory>
#include <string>
#include <vector>

#if defined(_LINUX)
#define WEBRTC_LINUX 1
#define WEBRTC_POSIX 1
#endif

#include <api/create_peerconnection_factory.h>
#include <rtc_base/ssl_adapter.h>
#include <rtc_base/thread.h>
#include <system_wrappers/include/field_trial.h>

#include "./base.h"



namespace patty_cake {


class WebRtcCake : public PattyCake {
  friend PattyCake;

public: // inner types
  using super = PattyCake;


public: // getter/setter
  Type type () const override;


public: // methods
  ~WebRtcCake ();

  void disconnect () override;

  bool send (std::vector<uint8_t> const& data) override;

  bool send (std::string const& id, std::vector<uint8_t> const& data) override;

  void poll () override;


private: // vars
  // WebRTC 관련 객체들
  webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
  webrtc::scoped_refptr<webrtc::DataChannelInterface   > data_channel_;

  std::shared_ptr<webrtc::PeerConnectionObserver> peer_connection_observer_;
  std::shared_ptr<webrtc::DataChannelObserver   > data_channel_observer_;


private:// methods
  WebRtcCake (ConnectConfig const& cnf);
  WebRtcCake (ListenConfig const& cnf);

  void _createPeerConnection (
      webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory,
      OnStateChangeFunc on_state_change_func,
      OnMessageFunc on_message_func
  );
};



}
#endif  // ifndef PATTY_CAKE_CAKES_WEB_RTC_CAKE_H_
