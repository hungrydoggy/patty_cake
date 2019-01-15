#include "patty_cake.h"

#include <iostream>


using std::cout;
using std::endl;
using std::make_shared;
using std::shared_ptr;
using std::vector;

namespace patty_cake {


int PattyCake::stt_instance_count_ = 0;


PattyCake::PattyCake (int piece_size)
:socket_(INVALID_SOCKET),
 piece_size_(piece_size) {

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

bool PattyCake::initSocket () {
    if (isSocketReady() == true)
        closesocket(socket_);

    // sock
    socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_ == INVALID_SOCKET) {
        cout << "socket failed: " << WSAGetLastError() << endl;
        return false;
    }

    return true;
}

bool PattyCake::bindSocket (int port) {
    SOCKADDR_IN local_address;
    local_address.sin_family = AF_INET;
    local_address.sin_port = htons(port);
    local_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(socket_, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR) {
        cout << "bind failed: " << WSAGetLastError();
        return false;
    }

    return true;
}

bool PattyCake::send (const std::string &ip_address, int port, const vector<char> &data) {
    SOCKADDR_IN to_address;
    to_address.sin_family = AF_INET;
    to_address.sin_port = htons(port);
    InetPton(AF_INET, ip_address.c_str(), &to_address.sin_addr.S_un.S_addr);

    int flags = 0;
    if (sendto(socket_, data.data(), data.size(), flags, (SOCKADDR*)&to_address, sizeof(to_address)) == SOCKET_ERROR) {
        cout << "sendto failed: " << WSAGetLastError() << endl;
        return false;
    }

    return true;
}

shared_ptr<PattyCakePiece> PattyCake::receive () {
    if (isSocketReady() == false) {
        cout << "socket must be ready before receive." << endl;
        return 0;
    }

    auto piece = make_shared<PattyCakePiece>();
    piece->data.resize(piece_size_);

    // receive
    int flags = 0;
    SOCKADDR_IN from;
    int from_size = sizeof(from);
    int bytes_received = recvfrom(
            socket_,
            piece->data.data(),
            piece_size_,
            flags,
            (SOCKADDR*)&from,
            &from_size);

    if (bytes_received == SOCKET_ERROR) {
        cout << "recvfrom returned SOCKET_ERROR, WSAGetLastError() " << WSAGetLastError() << endl;
        return 0;
    }
    piece->data.resize(bytes_received);

    piece->b1   = from.sin_addr.S_un.S_un_b.s_b1;
    piece->b2   = from.sin_addr.S_un.S_un_b.s_b2;
    piece->b3   = from.sin_addr.S_un.S_un_b.s_b3;
    piece->b4   = from.sin_addr.S_un.S_un_b.s_b4;
    piece->port = from.sin_port;

    return piece;
}
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PattyCakePiece




}