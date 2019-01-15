#ifndef PATTY_CAKE_H_
#define PATTY_CAKE_H_

#include <functional>
#include <string>
#include <vector>

#if defined(_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif



namespace patty_cake {

class PattyCakePiece;


class PattyCake {
public:
    PattyCake (int piece_size = 512);
    ~PattyCake ();

    bool initSocket ();
    bool bindSocket (int port);
    std::shared_ptr<PattyCakePiece> receive ();
    bool send (const std::string &ip_address, int port, const std::vector<char> &data);
    bool isSocketReady ();

private:
    int piece_size_;

#if defined(_WINDOWS)
    SOCKET socket_;
#endif

    void _init ();
    void _shutdown ();


    static int stt_instance_count_;
};


class PattyCakePiece {
public:
    unsigned char b1, b2, b3, b4;
    int port;
    std::vector<char> data;

private:
};


}


#endif