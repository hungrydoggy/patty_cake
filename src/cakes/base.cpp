#include "./base.h"

#include <iostream>

#include "./web_socket_cake.h"



using std::cout;
using std::endl;
using std::shared_ptr;



namespace patty_cake {


// stt
shared_ptr<PattyCake> PattyCake::connect (Config const& cnf) {
  switch (cnf.type) {
    case Type::WEB_SOCKET:
      return shared_ptr<PattyCake>(new WebSocketCake(cnf));
    //case Type::WEB_RTC:
    default:
      cout << "[patty_cake error] unhandled Type --- " << (uint32_t)cnf.type << endl;
      return nullptr;
  }

  return nullptr;
}


PattyCake::PattyCake () {
}


void PattyCake::state (State v) {
  state_ = v;

  if (on_state_change_func_ != nullptr)
    on_state_change_func_(v);
}



}
