#ifndef PATTY_CAKE_TEST_COMMON_PACKETS_SIMPLE_PACKETS_H_
#define PATTY_CAKE_TEST_COMMON_PACKETS_SIMPLE_PACKETS_H_

#include <string>

#include "../packet.h"


namespace packet {


using std::string;


struct BroadcastSimpleString : public Packet {
  string value;

public: // methods
  PacketType type () const override { return PacketType::BROADCAST_SIMPLE_STRING; }

private: // methods
  bool _onWriteTo  (patty_cake::PattyCakePieceMaker * maker ) override;
  bool _onReadFrom (patty_cake::PattyCakePieceSlicer* slicer) override;
};


}
#endif  // ifndef PATTY_CAKE_TEST_COMMON_PACKETS_SIMPLE_PACKETS_H_

