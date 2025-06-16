#include "./web_socket_cake.h"

#include <iostream>

#include "./_piece.h"



namespace patty_cake {


using std::cout;
using std::endl;


WebSocketCake::WebSocketCake (Config const& cnf) {

  state(State::CONNECTING);
  socket.setUrl(cnf.url);

  socket.setOnMessageCallback(
      [this](ix::WebSocketMessagePtr const& msg) {
        switch (msg->type) {
          case ix::WebSocketMessageType::Open:
            state(State::CONNECTED);
            break;

          case ix::WebSocketMessageType::Error:
            cout << "[patty_cake Error] " << msg->errorInfo.reason << std::endl;
            state(State::FAILED);
            break;

          case ix::WebSocketMessageType::Close:
            state(State::DISCONNECTED);
            break;

          case ix::WebSocketMessageType::Message: {
            auto callback = on_message_func();
            if (callback != nullptr) {
                const auto& data = msg->str;
                callback(
                    PattyCakePiece{
                      std::vector<uint8_t>(data.begin(), data.end()),
                    }
                );
            }
            break;
          }

          default:
            break;
        }
      }
  );


  socket.start();
}


WebSocketCake::~WebSocketCake () {
  disconnect();
}


PattyCake::Type WebSocketCake::type () const {
  return Type::WEB_SOCKET;
}


void WebSocketCake::disconnect () {
  socket.stop();
}


bool WebSocketCake::send (std::vector<uint8_t> const& data) {
  if (state() != State::CONNECTED)
    return false;

  socket.sendBinary(data);
  return true;
}


void WebSocketCake::poll () {
  // ixwebsocket is fully async; no-op here
}



}
