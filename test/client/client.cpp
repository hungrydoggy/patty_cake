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
        .url = "ws://127.0.0.1:9991",
        .on_message_func =
            [](PattyCake* cake, PattyCakePiece const& piece) {
              auto slicer = PattyCakePieceSlicer(&piece);
              std::string str;
              slicer.slice(str);
              cout << "[on msg]" << str << endl;
            },
      }
  );




  while (cake->state() != PattyCake::State::CONNECTED) {
  }


  PattyCakePieceMaker piece;
  piece.append("hello world");

  cake->send(piece.data());



  /// Display a prompt
  std::cout << "> " << std::flush;

  std::string text;
  // Read text from the console and send messages in text mode.
  // Exit with Ctrl-D on Unix or Ctrl-Z on Windows.
  while (cake->state() != PattyCake::State::DISCONNECTED && std::getline(std::cin, text))
  {
    piece.clear();
    piece.append(text);
    cake->send(piece.data());
    std::cout << "> " << std::flush;
  }

  return 0;
}
