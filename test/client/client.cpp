#include "client.h"

#include <iostream>

#include "../../src/cakes/web_rtc_cake.h"
#include "../../src/cakes/web_socket_cake.h"
#include "../../src/patty_cake.h"
#include "../common/packet.h"


using namespace patty_cake;

using std::cout;
using std::endl;


static void __onWsMessage (WebSocketCake* ws_cake, WebRtcCake* rtc_cake, PattyCakePiece const& piece) {

  PattyCakePieceSlicer slicer(&piece);
  switch (packet::Packet::getType(piece.data.data())) {
    case packet::PacketType::SEND_WEB_RPC_SDP: {
      packet::SendWebRtcSdp p;
      p.readFrom(&slicer);

      cout << "# SEND_WEB_RPC_SDP" << endl << p.sdp << endl << endl;

      auto& conn = rtc_cake->default_connection();
      conn->id(p.client_id);
      conn->receiveSdp(p.sdp);
      break;
    }

    case packet::PacketType::SEND_WEB_RPC_ICE: {
      packet::SendWebRtcIce p;
      p.readFrom(&slicer);

      cout << "# SEND_WEB_RPC_ICE" << endl << p.ice << endl << endl;

      auto& conn = rtc_cake->default_connection();
      conn->receiveIceSdp(p.ice);
      break;
    }

    case packet::PacketType::BROADCAST_SIMPLE_STRING: {
      packet::BroadcastSimpleString p;
      p.readFrom(&slicer);
      cout << "Receive broadcasted simple string: " << p.value << endl;
      break;
    }
  }

}


static void __onRtcMessage (
    WebRtcCake* rtc_cake,
    PattyCakePiece const& piece
) {
  PattyCakePieceSlicer slicer(&piece);
  switch (packet::Packet::getType(piece.data.data())) {
    case packet::PacketType::BROADCAST_SIMPLE_STRING: {
      packet::BroadcastSimpleString p;
      p.readFrom(&slicer);
      cout << "Receive broadcasted simple string: " << p.value << endl;
      break;
    }

    default:
      break;
  }
}


int main (int argc, char** argv) {

  std::shared_ptr<PattyCake> rtc_cake;

  // connect websocket
  auto ws_cake = PattyCake::connect(
      {
        .type = PattyCake::Type::WEB_SOCKET,
        .url = "ws://127.0.0.1:9991",
        .on_message_func =
            [&rtc_cake](PattyCake* ws_cake, PattyCakePiece const& piece) {
              __onWsMessage((WebSocketCake*)ws_cake, (WebRtcCake*)rtc_cake.get(), piece);
            }
      }
  );

  while (ws_cake->state() != PattyCake::State::CONNECTED) {
  }



  // connect webrtc
  rtc_cake = PattyCake::connect(
      {
        .type = PattyCake::Type::WEB_RTC,

        .on_message_func =
            [](PattyCake* cake, PattyCakePiece const& piece) {
              __onRtcMessage((WebRtcCake*)cake, piece);
            },

        .on_local_sdp_func =
            [ws_cake](WebRtcConnection* conn, std::string const& sdp) {
              PattyCakePieceMaker piece;

              packet::SendWebRtcSdp p;
              p.client_id = conn->id();
              p.sdp = sdp;
              p.writeTo(&piece);

              ws_cake->send(piece.data());
            },

        .on_local_ice_func =
            [ws_cake](WebRtcConnection* conn, std::string const& ice) {
              PattyCakePieceMaker piece;

              packet::SendWebRtcIce p;
              p.client_id = conn->id();
              p.ice = ice;
              p.writeTo(&piece);

              ws_cake->send(piece.data());
            },
      }
  );


  while (ws_cake->state() == PattyCake::State::DISCONNECTED) {
  }



  /// Display a prompt
  cout << "> " << std::flush;

  std::string text;
  // Read text from the console and send messages in text mode.
  // Exit with Ctrl-D on Unix or Ctrl-Z on Windows.
  while (ws_cake->state() != PattyCake::State::DISCONNECTED && std::getline(std::cin, text)) {
    PattyCakePieceMaker piece;

    packet::BroadcastSimpleString p;
    p.value = text;
    p.writeTo(&piece);

    rtc_cake->send(piece.data());
    cout << "> " << std::flush;
  }

  return 0;
}
