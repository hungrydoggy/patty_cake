#include "patty_cake.h"

#include <fcntl.h>
#include <iostream>

#if defined(_WINDOWS)
#elif defined(_LINUX)
  #include <arpa/inet.h>
  #include <cstring>
  #include <errno.h>
#endif

#if defined(_WINDOWS)
  #define socklen_t int
#elif defined(_LINUX)
  #define closesocket close
#endif


using std::cout;
using std::endl;
using std::make_shared;
using std::shared_ptr;
using std::vector;

namespace patty_cake {


int PattyCake::stt_instance_count_ = 0;


PattyCake::PattyCake (int piece_size)
:socket_(INVALID_SOCKET),
 piece_size_(piece_size),
 is_closed_(false)
{
  _init();
  ++stt_instance_count_;
}

PattyCake::PattyCake (SOCKET socket, int piece_size) {
  _init();
  ++stt_instance_count_;

  socket_   = socket;
  piece_size_ = piece_size;
  is_closed_  = false;
}

PattyCake::~PattyCake () {
  closesocket(socket_);

  --stt_instance_count_;
  _shutdown();
}

bool PattyCake::isSocketReady () {
  return socket_ != INVALID_SOCKET;
}

void PattyCake::_shutdown () {
  if (stt_instance_count_ > 0)
    return;

#if defined(_WINDOWS)
  WSACleanup();
#endif
}

void PattyCake::_init () {
  if (stt_instance_count_ > 0)
    return;

#if defined(_WINDOWS)
  cout << "winsock startup" << endl;
  WORD winsock_version = 0x202;
  WSADATA winsock_data;
  if (WSAStartup(winsock_version, &winsock_data)) {
    cout << "WSAStartup failed: " << WSAGetLastError() << endl;
    return;
  }
#endif
}

bool PattyCake::initSocket (SocketType sock_type/*= TCP*/) {
  if (isSocketReady() == true)
    closesocket(socket_);

  // sock
  switch (sock_type) {
  case TCP: socket_ = socket(PF_INET, SOCK_STREAM, 0      ); break;
  case UDP: socket_ = socket(PF_INET, SOCK_DGRAM , IPPROTO_UDP); break;
  default: cout << "err: unknown socket type - " << sock_type << endl; return false;
  }

  if (socket_ == INVALID_SOCKET) {
    cout << "err: socket opening failed - " << GET_SOCK_ERR << endl;
    return false;
  }

  is_closed_ = false;

  return true;
}

bool PattyCake::bindSocket (int port) {
  SOCKADDR_IN local_address;
  memset((char*)&local_address, 0, sizeof(local_address));
  local_address.sin_family = AF_INET;
  local_address.sin_port = htons(port);
  local_address.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if (bind(socket_, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR) {
    cout << "err: socket binding failed - " << GET_SOCK_ERR << endl;
    return false;
  }

  return true;
}

bool PattyCake::setNonblock () {
  // set nonblock mode
#if defined(_WINDOWS)
  unsigned long flag=1;
  if (ioctlsocket(socket_, FIONBIO, &flag) != 0) {
#else
  if (fcntl(socket_, F_SETFL, fcntl(socket_, F_GETFL) | O_NONBLOCK) < 0) {
#endif
    cout << "err: cannot set nonblock mode - " << GET_SOCK_ERR << endl;
    return false;
  }

  return true;
}

bool PattyCake::listen (int backlog/*= 20*/) {
  if (isSocketReady() == false) {
    cout << "err: socket must be ready before listen." << endl;
    return false;
  }

  if (::listen(socket_, backlog) == -1) {
    cout << "err: listen failed - " << GET_SOCK_ERR << endl;
    return false;
  }
  return true;
}

shared_ptr<PattyCake> PattyCake::accept () {
  if (isSocketReady() == false) {
    cout << "err: socket must be ready before accept." << endl;
    return null;
  }
  
  SOCKADDR_IN client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  auto new_sock = ::accept(socket_, (SOCKADDR*)&client_addr, &client_addr_len);
  if (new_sock == SOCKET_ERROR) {
    cout << "err: accept failed - " << GET_SOCK_ERR << endl;
    return null;
  }

  return shared_ptr<PattyCake>(new PattyCake(new_sock, piece_size_));
}

bool PattyCake::connect (const std::string &ip_address, int port) {
  if (isSocketReady() == false) {
    cout << "err: socket must be ready before connect." << endl;
    return false;
  }

  SOCKADDR_IN server_addr;
  makeSockAddrIn(server_addr, ip_address, port);

  if (::connect(socket_, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
    cout << "err: connect failed - " << GET_SOCK_ERR << endl;
    return false;
  } 

  return true;
}

bool PattyCake::send (void *data, size_t size) {
  if (isSocketReady() == false) {
    cout << "err: socket must be ready before send." << endl;
    return false;
  }

  auto n = ::send(socket_, (const char*)data, size, 0);
  if (n < 0) {
    cout << "err: send failed - " << GET_SOCK_ERR << endl;
    return false;
  }

  return true;
}

std::shared_ptr<PattyCakePiece> PattyCake::receive () {
  if (isSocketReady() == false) {
    cout << "err: socket must be ready before receive." << endl;
    return 0;
  }

  auto piece = make_shared<PattyCakePiece>();
  piece->data.resize(piece_size_);

  // receive
  int bytes_received = ::recv(socket_, piece->data.data(), piece_size_, 0);

  if (bytes_received == 0) {
    is_closed_ = true;
    return 0;
  }else if (bytes_received == SOCKET_ERROR) {
    switch (ERR_NO) {
      case WOULDBLOCK: return 0;
#ifdef _WINDOWS
      case WSAECONNRESET:
        is_closed_ = true;
        return 0;
#endif
    }
    cout << "err: receive failed - " << GET_SOCK_ERR << endl;
    return 0;
  }
  piece->data.resize(bytes_received);

  return piece;
}

void PattyCake::makeSockAddrIn (SOCKADDR_IN &output, const std::string &ip_address, int port) {
  memset((char*)&output, 0, sizeof(output));
  output.sin_family = AF_INET;
  output.sin_port = htons(port);
#if defined(_WINDOWS)
  output.sin_addr.s_addr = inet_addr(ip_address.c_str());
  //inet_pton(AF_INET, ip_address.c_str(), &output.sin_addr.S_un.S_addr);
#else
  inet_pton(AF_INET, ip_address.c_str(), &output.sin_addr);
#endif
}

bool PattyCake::sendTo (const std::string &ip_address, int port, void *data, size_t size) {
  SOCKADDR_IN to_address;
  makeSockAddrIn(to_address, ip_address, port);
  return sendTo(to_address, data, size);
}

bool PattyCake::sendTo (const SOCKADDR_IN &address, void *data, size_t size) {
  if (isSocketReady() == false) {
    cout << "err: socket must be ready before sendTo." << endl;
    return false;
  }

  int flags = 0;
  if (::sendto(socket_, (const char*)data, size, flags, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR) {
    cout << "err: sendto failed - " << GET_SOCK_ERR << endl;
    return false;
  }

  return true;
}

shared_ptr<PattyCakePiece> PattyCake::receiveFrom () {
  if (isSocketReady() == false) {
    cout << "err: socket must be ready before receiveFrom." << endl;
    return 0;
  }

  auto piece = make_shared<PattyCakePiece>();
  piece->data.resize(piece_size_);

  // receive
  int flags = 0;
  socklen_t from_size = sizeof(piece->udp_from);
  int bytes_received = recvfrom(
      socket_,
      piece->data.data(),
      piece_size_,
      flags,
      (SOCKADDR*)&piece->udp_from,
      &from_size);

  if (bytes_received == SOCKET_ERROR) {
    cout << "err: recvfrom failed - " << GET_SOCK_ERR << endl;
    return 0;
  }
  piece->data.resize(bytes_received);

  return piece;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PattyCakePiece



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PattyCakePieceSlicer
PattyCakePieceSlicer::PattyCakePieceSlicer (const shared_ptr<PattyCakePiece> &piece)
:idx_(0)
{
  piece_ = piece;
}

}
