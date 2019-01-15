#ifndef PATTY_CAKE_H_
#define PATTY_CAKE_H_

#if defined(_WINDOWS)
#include <winsock2.h>
#endif


namespace patty_cake {

class PattyCake {
public:
    PattyCake ();
    ~PattyCake ();

    bool initSocket (int port);
    int receive ();
    bool isSocketReady ();

private:
#if defined(_WINDOWS)
    SOCKET socket_;
#endif

    void _init ();
    void _shutdown ();


    static int stt_instance_count_;
};

}


#endif