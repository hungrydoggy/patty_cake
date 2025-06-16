#include "client.h"

#include <iostream>

#include "../../src/patty_cake.h"


using namespace patty_cake;

using std::cout;
using std::endl;


int main (int argc, char** argv) {

  auto cake = PattyCake::connect(
      {
        .type = PattyCake::Type::WEB_SOCKET,
        .url = "wss://echo.websocket.org",
        .on_message_func =
            [](PattyCakePiece const& piece) {
              cout << "[on msg]" << std::string(piece.data.begin(), piece.data.end()) << endl;
            },
      }
  );




  while (cake->state() != PattyCake::State::CONNECTED) {
  }


  PattyCakePieceMaker piece;
  piece.append("hello world");

  cake->send(piece.data());

  while (cake->state() != PattyCake::State::DISCONNECTED) {
  }

  return 0;
}
