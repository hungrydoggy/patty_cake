#include "client.h"

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
volatile bool is_running = true;
static void _listen () {
    while (is_listening) {

        auto piece = cake->receive();
        if (piece == 0) {
            cout << "error: piece is null" << endl;
            is_running = false;
            return;
        }

        // FIXME test
        piece->data.resize(piece->data.size() + 1);
        piece->data[piece->data.size() - 1] = 0;
        cout << (int)piece->from.sin_addr.S_un.S_un_b.s_b1 << "."
            << (int)piece->from.sin_addr.S_un.S_un_b.s_b2 << "."
            << (int)piece->from.sin_addr.S_un.S_un_b.s_b3 << "."
            << (int)piece->from.sin_addr.S_un.S_un_b.s_b4 << ":"
            << (int)piece->from.sin_port << " - "
            << piece->data.data() << endl;
    }
}

int main () {

	cout << "test client" << endl;

    cake = make_shared<PattyCake>();
    cake->initSocket();

    string str;
    vector<char> buffer;
    str = "connect to server";
    buffer.resize(str.length() + 1);
    memcpy(buffer.data(), str.data(), str.length());
    buffer[buffer.size() - 1] = 0;
    cake->send("127.0.0.1", 9854, buffer);

    thread listening_thread(_listen);

    while (is_running) {
        getline(cin, str);
        if (str == "exit")
            break;

        cout << "sending: " << str << endl;

        buffer.resize(str.length() + 1);
        memcpy(buffer.data(), str.data(), str.length());
        buffer[buffer.size() - 1] = 0;
        cake->send("127.0.0.1", 9854, buffer);

    }

    is_listening = false;
    listening_thread.join();

	return 0;
}