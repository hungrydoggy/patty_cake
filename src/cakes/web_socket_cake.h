#ifndef PATTY_CAKE_CAKES_WEB_SOCKET_CAKE_H_
#define PATTY_CAKE_CAKES_WEB_SOCKET_CAKE_H_

#include "./base.h"

#if defined(_EMSCRIPTEN)
//TODO
#else
#include <ixwebsocket/IXWebSocket.h>
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


private: // vars
  ix::WebSocket socket;


private: // methods
  WebSocketCake (Config const& cnf);
};


}
#endif  // ifndef PATTY_CAKE_CAKES_WEB_SOCKET_CAKE_H_
