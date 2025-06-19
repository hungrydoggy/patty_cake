#ifndef PATTY_CAKE_CAKES_WEB_RTC_CAKE_H_
#define PATTY_CAKE_CAKES_WEB_RTC_CAKE_H_

#include <memory>
#include <string>
#include <vector>

#include <rtc/rtc.hpp>

#include "./base.h"



namespace patty_cake {


class WebRtcConnection;


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
  std::shared_ptr<WebRtcConnection> default_connection_;


private:// methods
  WebRtcCake (ConnectConfig const& cnf);
  WebRtcCake (ListenConfig const& cnf);
};


class WebRtcConnection {
public: // inner types
  enum class Type {
    CALLER,
    CALLEE,
  };


public: // getter/setter
  inline std::string const& id () const { return id_; }

  inline WebRtcCake* cake () const { return cake_; }

  inline Type type () const { return type_; }

  inline std::shared_ptr<rtc::PeerConnection> peer_connection () const { return peer_connection_; }

  inline std::shared_ptr<rtc::DataChannel> data_channel () const { return data_channel_; }


public: // methods
  WebRtcConnection (
      std::string const& id,
      WebRtcCake* cake,
      Type type,
      PattyCake::OnMessageFunc on_message_func,
      PattyCake::OnStateChangeFunc on_state_change_func,
      PattyCake::OnLocalSdpFunc on_local_sdp_func,
      PattyCake::OnLocalIceFunc on_local_ice_func
  );
  ~WebRtcConnection ();

  void disconnect ();

  void createDataChannel (
      std::string const&           name,
      bool                         ordered,
      int                          max_retransmits,
      PattyCake::OnMessageFunc     on_message_func,
      PattyCake::OnStateChangeFunc on_state_change_func
  );

  void receiveIceSdp (std::string const& sdp);

  void receiveSdp (std::string const& sdp);


private: // vars
  std::string id_;
  WebRtcCake* cake_;
  Type type_;

  std::shared_ptr<rtc::PeerConnection> peer_connection_;
  std::shared_ptr<rtc::DataChannel   > data_channel_;


private: // methods
  void _setupDataChannel (
      std::shared_ptr<rtc::DataChannel> const& dc,
      PattyCake::OnMessageFunc     on_message_func,
      PattyCake::OnStateChangeFunc on_state_change_func
  );
};


struct WebRtcClientInfo : public PattyClientInfo {
  using super = PattyClientInfo;

  std::shared_ptr<WebRtcConnection> connection;

  WebRtcClientInfo (
      std::string const& id,
      std::shared_ptr<WebRtcConnection> connection
  );

  bool is_alive () const override;
};



}
#endif  // ifndef PATTY_CAKE_CAKES_WEB_RTC_CAKE_H_
