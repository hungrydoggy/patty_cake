#ifndef PATTY_CAKE_PATTY_CAKE_H_
#define PATTY_CAKE_PATTY_CAKE_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#if defined(_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(_LINUX)
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "../util/defines.h"


#if defined(_WINDOWS)
  #define WOULDBLOCK WSAEWOULDBLOCK
  #define GET_SOCK_ERR WSAGetLastError()
  #define ERR_NO WSAGetLastError()
#elif defined(_LINUX)
  #define SOCKET    int
  #define SOCKADDR_IN sockaddr_in
  #define SOCKADDR  sockaddr
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR   -1
  #define WOULDBLOCK EWOULDBLOCK
  #define GET_SOCK_ERR strerror(errno)
  #define ERR_NO errno
#endif


namespace patty_cake {

class PattyCakePiece;


class APPETIZER_API PattyCake {
public:
  enum SocketType {
    TCP,
    UDP,
  };


  static void makeSockAddrIn (SOCKADDR_IN &output, const std::string &ip_address, int port);


  PattyCake (int piece_size = 512);
  ~PattyCake ();


  GETTER(bool, is_closed)


  bool isSocketReady ();

  bool initSocket (SocketType sock_type = TCP);
  bool bindSocket (int port);
  bool setNonblock ();


  // for tcp
  bool listen (int backlog = 20);
  std::shared_ptr<PattyCake> accept ();
  bool connect (const std::string &ip_address, int port);

  std::shared_ptr<PattyCakePiece> receive ();
  inline bool send (const std::vector<char> &data) {
    return send((void*)data.data(), data.size());
  }
  bool send (void *data, size_t size);


  // for udp
  std::shared_ptr<PattyCakePiece> receiveFrom ();
  inline bool sendTo (const std::string &ip_address, int port, const std::vector<char> &data) {
    return sendTo(ip_address, port, (void*)data.data(), data.size());
  }
  bool sendTo (const std::string &ip_address, int port, void *data, size_t size);
  inline bool sendTo (const SOCKADDR_IN &address, const std::vector<char> &data) {
    return sendTo(address, (void*)data.data(), data.size());
  }
  bool sendTo (const SOCKADDR_IN &address, void *data, size_t size);


private:
  SOCKET socket_;
  int piece_size_;
  bool is_closed_;


  PattyCake (SOCKET socket, int piece_size);


  void _init ();
  void _shutdown ();


  static int stt_instance_count_;
};


class APPETIZER_API PattyCakePiece {
public:
  SOCKADDR_IN udp_from;
  std::vector<char> data;

};

class APPETIZER_API PattyCakePieceSlicer {
public:
  PattyCakePieceSlicer (const std::shared_ptr<PattyCakePiece> &piece);

  template <class T>
  T slice () {
    auto cur_idx = idx_;
    idx_ += sizeof(T);
    return *((T*)(piece_->data[cur_idx]));
  }

private:
  std::shared_ptr<PattyCakePiece> piece_;
  int idx_;
};


class APPETIZER_API PattyCakePieceMaker {
public:
  template <class T>
  void append (const T &input_data) {
    auto sz = sizeof(T);
    auto idx = data_.size();
    data_.resize(idx + sz);

    memcpy(&data_[idx], &input_data, sz);
  }

private:
  std::vector<char> data_;
};

}


#endif  // ifndef PATTY_CAKE_PATTY_CAKE_H_
