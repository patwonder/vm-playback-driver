// ftndtest.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include "MessageConnection.h"
#include "MessageReader.h"
#include "Connection.h"
#include "echo.h"

using namespace va;
using namespace std;

int main(int argc, char* argv[]) {
	//try {
	//	MessageConnection msgconn(UdpConnection::newConnection());
	//	Message msg = msgconn.readMessage();
	//	string content = MessageReader(msg).readString();
	//	while (content != "quit") {
	//		cout << content << endl;
	//		msg = msgconn.readMessage();
	//		content = MessageReader(msg).readString();
	//	}
	//} catch (std::exception& e) {
	//	cout << e.what() << endl;
	//}
	echo();
}
