#include "server.h"

#include <iostream>
#include <unordered_map>

#include "../../src/cakes/web_rtc_cake.h"
#include "../../src/cakes/web_socket_cake.h"
#include "../../src/patty_cake.h"
#include "../common/packet.h"


using namespace patty_cake;

using std::cout;
using std::endl;


static void __onWsMessage (
    std::unordered_map<std::string, std::string>& rtcid_wsid_map,
    WebSocketCake* ws_cake,
    WebRtcCake* rtc_cake,
    PattyCake::ListenConfig const& rtc_listen_cnf,
    PattyCakePiece const& piece
) {

  PattyCakePieceSlicer slicer(&piece);
  switch (packet::Packet::getType(piece.data.data())) {
    case packet::PacketType::SEND_WEB_RPC_SDP: {
      packet::SendWebRtcSdp p;
      p.readFrom(&slicer);

      cout << "# SEND_WEB_RPC_SDP" << endl << p.sdp << endl << endl;

      auto client_info = rtc_cake->findClientInfo(p.client_id);
      if (client_info == nullptr)
        client_info = rtc_cake->addListenConnection(rtc_listen_cnf);

      rtcid_wsid_map[client_info->id] = piece.sender_id;

      ((WebRtcClientInfo*)client_info.get())->connection->receiveSdp(p.sdp);
      break;
    }

    case packet::PacketType::SEND_WEB_RPC_ICE: {
      packet::SendWebRtcIce p;
      p.readFrom(&slicer);

      cout << "# SEND_WEB_RPC_ICE" << endl << p.ice << endl << endl;

      auto client_info = rtc_cake->findClientInfo(p.client_id);
      if (client_info == nullptr)
        return;

      ((WebRtcClientInfo*)client_info.get())->connection->receiveIceSdp(p.ice);
      break;
    }

    case packet::PacketType::BROADCAST_SIMPLE_STRING: {
      // broadcast echo
      for (auto& itr : ws_cake->client_info_map()) {
        auto& id = itr.first;
        ws_cake->send(id, piece.data);
      }
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
      // broadcast echo
      for (auto& itr : rtc_cake->client_info_map()) {
        auto& id = itr.first;
        rtc_cake->send(id, piece.data);
      }
      break;
    }

    default:
      break;
  }
}


int main (int argc, char** argv) {

  std::shared_ptr<PattyCake> ws_cake;
  std::shared_ptr<PattyCake> rtc_cake;

  std::unordered_map<std::string, std::string> rtcid_wsid_map;

  // webrtc listen config
  PattyCake::ListenConfig rtc_listen_cnf =
      {
        .type = PattyCake::Type::WEB_RTC,

        .on_message_func =
            [](PattyCake* cake, PattyCakePiece const& piece) {
              __onRtcMessage((WebRtcCake*)cake, piece);
            },

        .on_local_sdp_func =
            [&ws_cake, &rtcid_wsid_map](WebRtcConnection* conn, std::string const& sdp) {
              PattyCakePieceMaker piece;

              packet::SendWebRtcSdp p;
              p.client_id = conn->id();
              p.sdp = sdp;
              p.writeTo(&piece);

              if (rtcid_wsid_map.find(conn->id()) == rtcid_wsid_map.end())
                return;

              auto ws_id = rtcid_wsid_map[conn->id()];
              ws_cake->send(ws_id, piece.data());
            },

        .on_local_ice_func =
            [&ws_cake, &rtcid_wsid_map](WebRtcConnection* conn, std::string const& ice) {
              PattyCakePieceMaker piece;

              packet::SendWebRtcIce p;
              p.ice = ice;
              p.writeTo(&piece);

              if (rtcid_wsid_map.find(conn->id()) == rtcid_wsid_map.end())
                return;

              auto ws_id = rtcid_wsid_map[conn->id()];
              ws_cake->send(ws_id, piece.data());
            },
      };


  // listen websocket
  ws_cake = PattyCake::listen(
      {
        .type = PattyCake::Type::WEB_SOCKET,
        .host = "127.0.0.1",
        .on_message_func =
            [&rtc_cake, &rtc_listen_cnf, &rtcid_wsid_map](PattyCake* cake, PattyCakePiece const& piece) {
              __onWsMessage(rtcid_wsid_map, (WebSocketCake*)cake, (WebRtcCake*)rtc_cake.get(), rtc_listen_cnf, piece);

              //// broadcast echo
              //for (auto& itr : cake->client_info_map()) {
              //  auto& id = itr.first;
              //  cake->send(id, piece.data);
              //}
            },
      }
  );


  // listen webrtc
  rtc_cake = PattyCake::listen(
      {
        .type = PattyCake::Type::WEB_RTC,
      }
  );


  while (ws_cake->state() == PattyCake::State::LISTENING) {
  }

  return 0;
}
