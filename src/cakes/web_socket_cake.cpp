#include "./web_socket_cake.h"

#include <iostream>

#include "./_piece.h"



namespace patty_cake {


using std::cout;
using std::endl;
using std::make_shared;
using std::shared_ptr;


WebSocketCake::WebSocketCake (ConnectConfig const& cnf) {

  state(State::CONNECTING);

  socket_ = make_shared<ix::WebSocket>();
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
                    this,
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

  server_ = make_shared<ix::WebSocketServer>(cnf.port, cnf.host);

  server_->setOnClientMessageCallback(
      [this](shared_ptr<ix::ConnectionState> conn_state, ix::WebSocket& socket, ix::WebSocketMessagePtr const& msg) {
        cout << "[patty_cake] from " << conn_state->getId() << " --- ";

        switch (msg->type) {
          case ix::WebSocketMessageType::Open: {
            cout << "New Connection" << endl;
            auto client =
                shared_ptr<PattyClientInfo>(
                  new WebSocketClientInfo(
                    conn_state->getId(),
                    conn_state,
                    &socket
                  )
                );
            _addClientInfo(client);
            break;
          }

          case ix::WebSocketMessageType::Error:
            cout << endl << "  #Error " << msg->errorInfo.reason << endl;
            state(State::FAILED);
            break;

          case ix::WebSocketMessageType::Close:
            _removeClientInfo(conn_state->getId());
            break;

          case ix::WebSocketMessageType::Message: {
            cout << "message received" << endl;
            auto callback = on_message_func();
            if (callback != nullptr) {
                const auto& data = msg->str;
                callback(
                    this,
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

  cout << "[patty_cake] listening" << endl;
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

  state(State::DISCONNECTED);
}


bool WebSocketCake::send (std::vector<uint8_t> const& data) {
  if (state() != State::CONNECTED)
    return false;

  socket_->sendBinary(data);
  return true;
}


bool WebSocketCake::send (std::string const& id, std::vector<uint8_t> const& data) {
  auto client_info = _findClientInfo(id);
  if (client_info == nullptr) {
    cout << "[patty_cake Error] cannot find client_info --- id: " << id << endl;
    return false;
  }

  if (client_info->is_alive() == false) {
    cout << "[patty_cake Error] client_info is not alive --- id: " << id << endl;
    return false;
  }

  auto socket = ((WebSocketClientInfo*)client_info.get())->socket;
  socket->sendBinary(data);
  return true;
}


void WebSocketCake::poll () {
  // ixwebsocket is fully async; no-op here
}




WebSocketClientInfo::WebSocketClientInfo (
    std::string const& id,
    std::weak_ptr<ix::ConnectionState> const& conn_state,
    ix::WebSocket* socket
)
:super(id),
 conn_state(conn_state),
 socket(socket)
{
}


bool WebSocketClientInfo::is_alive () const { 
  return conn_state.expired() == false;
}




}
