#include "server.h"

#include <iostream>

#include "../../src/patty_cake.h"


using namespace patty_cake;

using std::cout;
using std::endl;


int main (int argc, char** argv) {

  auto cake = PattyCake::listen(
      {
        .type = PattyCake::Type::WEB_SOCKET,
        .host = "127.0.0.1",
        .on_message_func =
            [](PattyCakePiece const& piece) {
              cout << "[on msg]" << std::string(piece.data.begin(), piece.data.end()) << endl;
            },
      }
  );




  while (cake->state() == PattyCake::State::LISTENING) {
  }



  return 0;
}
