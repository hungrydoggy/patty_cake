#include "./base.h"

#include <iostream>

#include "./web_socket_cake.h"



using std::cout;
using std::endl;
using std::shared_ptr;



namespace patty_cake {


// stt
shared_ptr<PattyCake> PattyCake::connect (ConnectConfig const& cnf) {

  // create cake
  shared_ptr<PattyCake> cake;
  switch (cnf.type) {
    case Type::WEB_SOCKET:
      cake = shared_ptr<PattyCake>(new WebSocketCake(cnf));
      break;

    //case Type::WEB_RTC:

    default:
      cout << "[patty_cake error] unhandled Type --- " << (uint32_t)cnf.type << endl;
      return nullptr;
  }


  // set configs
  cake->on_message_func     (cnf.on_message_func     );
  cake->on_state_change_func(cnf.on_state_change_func);


  return cake;
}


// stt
std::shared_ptr<PattyCake> PattyCake::listen (ListenConfig const& cnf) {
  // create cake
  shared_ptr<PattyCake> cake;
  switch (cnf.type) {
    case Type::WEB_SOCKET:
      cake = shared_ptr<PattyCake>(new WebSocketCake(cnf));
      break;

    //case Type::WEB_RTC:

    default:
      cout << "[patty_cake error] unhandled Type --- " << (uint32_t)cnf.type << endl;
      return nullptr;
  }


  // set configs


  return cake;
}


PattyCake::PattyCake () {
}


void PattyCake::state (State v) {
  state_ = v;

  if (on_state_change_func_ != nullptr)
    on_state_change_func_(v);
}



}
