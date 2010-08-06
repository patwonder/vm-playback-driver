/*
 * main.cpp
 *
 *  Created on: 2010-7-21
 *      Author: pat
 */

#include "connection.h"
#include "MessageConnection.h"
#include "MessageReader.h"
#include <syscall.h>
#include <iostream>

using namespace std;
using namespace va;

void service(Connection* conn) {
	MessageConnection msg_conn(conn);
	try {
		while (true) {
			Message msg = msg_conn.readMessage();
//			cout << "Received message, header = 0x" << hex << msg.getHeader()
//					<< ", length = " << msg.getLength();
//			if (msg.getLength() > 0) {
//				cout << ", content:" << endl;
//				for (const char* ptr = msg.getContentPtr(); ptr
//						< msg.getContentPtr() + msg.getLength(); ++ptr) {
//					cout << " " << "0x" << hex << (int) (*ptr);
//				}
//			}
//			cout << endl;
		}
	} catch (exception& e) {
		cerr << e.what() << endl;
	}
}

int main() {
	try {
		while (true) {
			Connection* conn = Connection::acceptConnection();
			if (fork() == 0) {
				service(conn);
				conn->close();
				delete conn;
				break;
			}
			conn->close();
			delete conn;
		}
	} catch (exception& e) {
		cerr << e.what() << endl;
	}
}
