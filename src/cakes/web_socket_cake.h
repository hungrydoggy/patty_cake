#ifndef PATTY_CAKE_CAKES_WEB_SOCKET_CAKE_H_
#define PATTY_CAKE_CAKES_WEB_SOCKET_CAKE_H_

#include "./base.h"

#if defined(_EMSCRIPTEN)
//TODO
#else
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>
#endif



namespace patty_cake {


class WebSocketCake : public PattyCake {
  friend PattyCake;

public: // inner types
  using super = PattyCake;


public: // getter/setter
  Type type () const override;


public: // methods
  ~WebSocketCake ();

  void disconnect () override;

  bool send (std::vector<uint8_t> const& data) override;

  bool send (std::string const& id, std::vector<uint8_t> const& data) override;

  void poll () override;


private: // vars
  std::shared_ptr<ix::WebSocket> socket_;
  std::shared_ptr<ix::WebSocketServer> server_;


private: // methods
  WebSocketCake (ConnectConfig const& cnf);
  WebSocketCake (ListenConfig const& cnf);
};



struct WebSocketClientInfo : public PattyClientInfo {
  using super = PattyClientInfo;

  std::weak_ptr<ix::ConnectionState> conn_state;
  ix::WebSocket* socket;

  WebSocketClientInfo (
      std::string const& id,
      std::weak_ptr<ix::ConnectionState> const& conn_state,
      ix::WebSocket* socket
  );

  bool is_alive () const override;
};



}
#endif  // ifndef PATTY_CAKE_CAKES_WEB_SOCKET_CAKE_H_
