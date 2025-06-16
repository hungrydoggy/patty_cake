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

  void poll () override;


private: // inner types
  struct ClientInfo_ {
    uint id;
    std::weak_ptr<ix::ConnectionState> conn_state;
    ix::WebSocket* socket;

    ClientInfo_ (
        uint id,
        std::weak_ptr<ix::ConnectionState> const& conn_state,
        ix::WebSocket* socket
    );

    inline bool expired () const { return conn_state.expired(); }
  };


private: // vars
  std::shared_ptr<ix::WebSocket> socket_;
  std::shared_ptr<ix::WebSocketServer> server_;

  std::unordered_map<uint, std::shared_ptr<ClientInfo_>> client_info_map_;


private: // methods
  WebSocketCake (ConnectConfig const& cnf);
  WebSocketCake (ListenConfig const& cnf);
};


}
#endif  // ifndef PATTY_CAKE_CAKES_WEB_SOCKET_CAKE_H_
