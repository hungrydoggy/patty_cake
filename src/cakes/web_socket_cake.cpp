#include "./web_socket_cake.h"

#include <iostream>

#include "./_piece.h"



namespace patty_cake {


using std::cout;
using std::endl;


WebSocketCake::WebSocketCake (ConnectConfig const& cnf) {

  state(State::CONNECTING);

  socket_ = std::make_shared<ix::WebSocket>();
  socket_->setUrl(cnf.url);

  socket_->setOnMessageCallback(
      [this](ix::WebSocketMessagePtr const& msg) {
        switch (msg->type) {
          case ix::WebSocketMessageType::Open:
            state(State::CONNECTED);
            break;

          case ix::WebSocketMessageType::Error:
            cout << "[patty_cake Error] " << msg->errorInfo.reason << endl;
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
                      "",
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


  socket_->start();
}


WebSocketCake::WebSocketCake (ListenConfig const& cnf) {

  server_ = std::make_shared<ix::WebSocketServer>(cnf.host, cnf.port);

  server_->setOnClientMessageCallback(
      [this](std::shared_ptr<ix::ConnectionState> conn_state, ix::WebSocket& socket, ix::WebSocketMessagePtr const& msg) {
        cout << "[patty_cake] Remote ip: " << conn_state->getRemoteIp() << endl;

        switch (msg->type) {
          case ix::WebSocketMessageType::Open: {
            cout << "[patty_cake] New Connection" << endl;
            cout << "id: " << conn_state->getId() << endl;
            auto client =
                make_shared<ClientInfo_>(
                  conn_state->getId(),
                  conn_state,
                  &socket
                );
            client_info_map_[client->id] = client;
            break;
          }

          case ix::WebSocketMessageType::Error:
            cout << "[patty_cake Error] " << msg->errorInfo.reason << endl;
            state(State::FAILED);
            break;

          case ix::WebSocketMessageType::Close:
            state(State::DISCONNECTED);
            break;

          case ix::WebSocketMessageType::Message: {
            cout << "[patty_cake] Received: " << msg->str << endl;
            auto callback = on_message_func();
            if (callback != nullptr) {
                const auto& data = msg->str;
                callback(
                    PattyCakePiece{
                      conn_state->getId(),
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


  auto res = server_->listen();
  if (res.first == false) {
    cout << "[patty_cake error] cannot listen" << endl;
    state(State::FAILED);
    return;
  }


  server_->disablePerMessageDeflate();
  server_->start();


  state(State::LISTENING);
}


WebSocketCake::~WebSocketCake () {
  disconnect();
}


PattyCake::Type WebSocketCake::type () const {
  return Type::WEB_SOCKET;
}


void WebSocketCake::disconnect () {
  if (socket_ != nullptr)
    socket_->stop();
}


bool WebSocketCake::send (std::vector<uint8_t> const& data) {
  if (state() != State::CONNECTED)
    return false;

  socket_->sendBinary(data);
  return true;
}


void WebSocketCake::poll () {
  // ixwebsocket is fully async; no-op here
}






WebSocketCake::ClientInfo_::ClientInfo_ (
    uint id,
    std::weak_ptr<ix::ConnectionState> const& conn_state,
    ix::WebSocket* socket
)
:id(id),
 conn_state(conn_state),
 socket(socket)
{
}



}
