#include "./packet.h"

#include "../../src/cakes/_piece.h"


namespace packet {


// stt
PacketType Packet::getType (uint8_t const* data) {
  return (PacketType)*((uint16_t*)data);
}


bool Packet::writeTo (patty_cake::PattyCakePieceMaker * maker) {
  maker->append((uint16_t)type());

  return _onWriteTo(maker);
}


bool Packet::readFrom (patty_cake::PattyCakePieceSlicer* slicer) {
  slicer->slice<uint16_t>();

  return _onReadFrom(slicer);
}


}
