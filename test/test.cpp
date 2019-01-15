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
shared_ptr<PattyCake> sender_cake;

volatile bool is_listening = true;
void listen () {
    while (is_listening) {
        auto piece = cake->receive();


        // FIXME test
        piece->data.resize(piece->data.size() + 1);
        piece->data[piece->data.size() - 1] = 0;
        cout << (int)piece->b1 << "."
            << (int)piece->b2 << "."
            << (int)piece->b3 << "."
            << (int)piece->b4 << ":"
            << (int)piece->port << " - "
            << piece->data.data() << endl;

    }
}

int main () {

	cout << "test" << endl;

    cake = make_shared<PattyCake>();
    cake->initSocket();
    cake->bindSocket(9854);

    thread listening_thread(listen);

    sender_cake = make_shared<PattyCake>();
    sender_cake->initSocket();

    volatile bool is_running = true;
    string str;
    vector<char> buffer;
    while (is_running) {
        getline(cin, str);
        if (str == "exit")
            break;

        cout << "sending: " << str << endl;

        buffer.resize(str.length() + 1);
        memcpy(buffer.data(), str.data(), str.length());
        buffer[buffer.size() - 1] = 0;
        sender_cake->send("127.0.0.1", 9854, buffer);
    }

    is_listening = false;
    listening_thread.join();

	return 0;
}