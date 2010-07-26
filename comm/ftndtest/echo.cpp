#include <iostream>
#include "MessageConnection.h"
#include "Connection.h"
#include "echo.h"

using namespace va;
using namespace std;

namespace va {

void echoCommon(IConnection* base) {
	try {
		MessageConnection conn(base);
		Message ack(0, 0, new char[1]);
		while (true) {
			Message msg = conn.readMessage();
			conn.writeMessage(ack);
		}
	} catch (std::exception& e) {
		cerr << e.what() << endl;
	}
}

void echo() {
	echoCommon(Connection::newConnection());
}

}
