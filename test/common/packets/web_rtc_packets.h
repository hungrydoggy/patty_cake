#ifndef PATTY_CAKE_TEST_COMMON_PACKETS_WEB_RTC_PACKETS_H_
#define PATTY_CAKE_TEST_COMMON_PACKETS_WEB_RTC_PACKETS_H_

#include <string>

#include "../packet.h"


namespace packet {


using std::string;


struct SendWebRtcSdp : public Packet {
  string client_id;
  string sdp;

public: // methods
  PacketType type () const override { return PacketType::SEND_WEB_RPC_SDP; }

private: // methods
  bool _onWriteTo  (patty_cake::PattyCakePieceMaker * maker ) override;
  bool _onReadFrom (patty_cake::PattyCakePieceSlicer* slicer) override;
};


struct SendWebRtcIce : public Packet {
  string client_id;
  string ice;

public: // methods
  PacketType type () const override { return PacketType::SEND_WEB_RPC_ICE; }

private: // methods
  bool _onWriteTo  (patty_cake::PattyCakePieceMaker * maker ) override;
  bool _onReadFrom (patty_cake::PattyCakePieceSlicer* slicer) override;
};

}
#endif  // ifndef PATTY_CAKE_TEST_COMMON_PACKETS_WEB_RTC_PACKETS_H_
