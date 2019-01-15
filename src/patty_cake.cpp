#include "patty_cake.h"

#include <iostream>


using std::cout;
using std::endl;

namespace patty_cake {


int PattyCake::stt_instance_count_ = 0;


PattyCake::PattyCake ()
:socket_(INVALID_SOCKET) {
    _init();
    ++stt_instance_count_;
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

    WSACleanup();
}

#if defined(_WINDOWS)
void PattyCake::_init () {
    if (stt_instance_count_ > 0)
        return;

    WORD winsock_version = 0x202;
    WSADATA winsock_data;
    if (WSAStartup(winsock_version, &winsock_data)) {
        cout << "WSAStartup failed: " << WSAGetLastError() << endl;
        return;
    }
}

bool PattyCake::initSocket (int port) {

    // sock 
    int address_family = AF_INET;
    int type = SOCK_DGRAM;
    int protocol = IPPROTO_UDP;
    socket_ = socket(address_family, type, protocol);
    if (socket_ == INVALID_SOCKET) {
        cout << "socket failed: " << WSAGetLastError() << endl;
        return false;
    }
    
    // bind
    SOCKADDR_IN local_address;
    local_address.sin_family = AF_INET;
    local_address.sin_port = htons(port);
    local_address.sin_addr.s_addr = INADDR_ANY;
    if(bind(socket_, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR) {
        cout << "bind failed: " << WSAGetLastError();
        return false;
    }

    return true;
}

int PattyCake::receive () {
    if (isSocketReady() == false)
        return -1;

    const int SOCKET_BUFFER_SIZE = 1024;
    char buffer[SOCKET_BUFFER_SIZE];
    int flags = 0;
    SOCKADDR_IN from;
    int from_size = sizeof(from);
    int bytes_received = recvfrom(socket_, buffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);

    if (bytes_received == SOCKET_ERROR) {
        cout << "recvfrom returned SOCKET_ERROR, WSAGetLastError() " << WSAGetLastError() << endl;
        return -1;
    }


    buffer[bytes_received] = 0;
    cout << from.sin_addr.S_un.S_un_b.s_b1 << "."
         << from.sin_addr.S_un.S_un_b.s_b2 << "."
         << from.sin_addr.S_un.S_un_b.s_b3 << "."
         << from.sin_addr.S_un.S_un_b.s_b4 << ":"
         << from.sin_port << " - "
         << buffer;

    return bytes_received;
}
#endif


}