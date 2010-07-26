/*
 * MessageConnection.cpp
 *
 *  Created on: 2010-7-19
 *      Author: pat
 */

#include "MessageConnection.h"

using namespace va;

MessageConnection::MessageConnection(IConnection* conn) {
	connection = conn;
}

MessageConnection::~MessageConnection() {
	delete connection;
}

template <class T>
static T max(T a, T b) {
	return a > b ? a : b;
}

struct msg_struct {
	unsigned int msg_header;
	unsigned int msg_length;
};

const Message MessageConnection::readMessage() {
	msg_struct msg;
	connection->receive((char*)&msg, sizeof(msg));
	char* content = new char[max(msg.msg_length, (unsigned int)1)];
	if (msg.msg_length > 0) {
		connection->receive(content, msg.msg_length);
	}
	return Message(msg.msg_header, msg.msg_length, content);
}

void MessageConnection::writeMessage(const Message& mess) {
	msg_struct msg;
	msg.msg_header = mess.getHeader();
	msg.msg_length = mess.getLength();
	connection->send((const char*)&msg, sizeof(msg));
	if (msg.msg_length > 0)
		connection->send(mess.getContentPtr(), msg.msg_length);
}

void MessageConnection::flush() {
	connection->flush();
}
