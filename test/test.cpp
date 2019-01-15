#include "test.h"

#include <iostream>

#include "patty_cake.h"

using std::cout;
using std::endl;
using std::make_shared;
using namespace patty_cake;

int main (void) {

	cout << "test" << endl;

    auto pc = make_shared<PattyCake>();

    pc->initSocket(9854);
    pc->receive();

	return 0;
}