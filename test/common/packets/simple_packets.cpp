#include "./simple_packets.h"

#include "../../../src/cakes/_piece.h"


namespace packet {


bool BroadcastSimpleString::_onWriteTo (patty_cake::PattyCakePieceMaker* maker) {
  maker->append(value);
  return true;
}


bool BroadcastSimpleString::_onReadFrom (patty_cake::PattyCakePieceSlicer* slicer) {
  slicer->slice(value);
  return true;
}



}
