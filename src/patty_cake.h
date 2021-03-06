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
    bool send (const SOCKADDR_IN &address, const std::vector<char> &data);
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
    SOCKADDR_IN from;
    std::vector<char> data;

private:
};

class PattyCakePieceSlicer {
public:
    PattyCakePieceSlicer (const std::shared_ptr<PattyCakePiece> &piece);

    template <class T>
    T slice () {
        auto cur_idx = idx_;
        idx_ += sizeof(T);
        return (*((T*)piece_->data[cur_idx]));
    }

private:
    std::shared_ptr<PattyCakePiece> piece_;
    int idx_;
};


}


#endif