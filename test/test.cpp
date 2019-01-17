#include "test.h"

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "patty_cake.h"

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::thread;
using std::vector;
using namespace patty_cake;


shared_ptr<PattyCake> cake;

volatile bool is_listening = true;
static void _listen () {
    while (is_listening) {
        auto piece = cake->receive();


        // FIXME test
        piece->data.resize(piece->data.size() + 1);
        piece->data[piece->data.size() - 1] = 0;
        cout << (int)piece->from.sin_addr.S_un.S_un_b.s_b1 << "."
            << (int)piece->from.sin_addr.S_un.S_un_b.s_b2 << "."
            << (int)piece->from.sin_addr.S_un.S_un_b.s_b3 << "."
            << (int)piece->from.sin_addr.S_un.S_un_b.s_b4 << ":"
            << (int)piece->from.sin_port << " - "
            << piece->data.data() << endl;


        // echo
        cake->send(piece->from, piece->data);
    }
}

void main () {
	cout << "test" << endl;

    cake = make_shared<PattyCake>();
    cake->initSocket();
    cake->bindSocket(9854);

    thread listening_thread(_listen);

    listening_thread.join();

}