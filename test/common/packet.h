#ifndef PATTY_CAKE_TEST_COMMON_PACKET_H_
#define PATTY_CAKE_TEST_COMMON_PACKET_H_

#include "./_packet_types.h"

#include <cstdint>


namespace patty_cake {
class PattyCakePieceMaker;
class PattyCakePieceSlicer;
}


namespace packet {

struct Packet {
public: // static methods
  static PacketType getType (uint8_t const* data);


public: // getter/setter
  virtual PacketType type () const = 0;


public: // methods
  bool writeTo  (patty_cake::PattyCakePieceMaker * maker );
  bool readFrom (patty_cake::PattyCakePieceSlicer* slicer);


protected: // methods
  virtual bool _onWriteTo  (patty_cake::PattyCakePieceMaker * maker ) = 0;
  virtual bool _onReadFrom (patty_cake::PattyCakePieceSlicer* slicer) = 0;
};

}


#endif  // ifndef PATTY_CAKE_TEST_COMMON_PACKET_H_


#include "./packets/simple_packets.h"
#include "./packets/web_rtc_packets.h"
