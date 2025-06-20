#include "./web_rtc_packets.h"

#include "../../../src/cakes/_piece.h"


namespace packet {


bool SendWebRtcSdp::_onWriteTo (patty_cake::PattyCakePieceMaker* maker) {
  maker->append(client_id);
  maker->append(sdp);
  return true;
}


bool SendWebRtcSdp::_onReadFrom (patty_cake::PattyCakePieceSlicer* slicer) {
  slicer->slice(client_id);
  slicer->slice(sdp);
  return true;
}




bool SendWebRtcIce::_onWriteTo (patty_cake::PattyCakePieceMaker* maker) {
  maker->append(client_id);
  maker->append(ice);
  return true;
}


bool SendWebRtcIce::_onReadFrom (patty_cake::PattyCakePieceSlicer* slicer) {
  slicer->slice(client_id);
  slicer->slice(ice);
  return true;
}


}
