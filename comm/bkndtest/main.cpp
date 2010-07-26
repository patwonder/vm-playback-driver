/*
 * main.cpp
 *
 *  Created on: 2010-7-16
 *      Author: pat
 */

#include <iostream>
#include <cassert>
#include "connection.h"
#include "UdpConnection.h"
#include "MessageConnection.h"
#include "MessageBuilder.h"
#include "MessageReader.h"
#include "bandwidth.h"

using namespace va;
using namespace std;

void testMsg() {
	MessageBuilder mb(0);
	mb.write('c');
	mb.write((unsigned char)'u');
	mb.write((short)-20);
	mb.write((unsigned short)30);
	mb.write(-100);
	mb.write(100u);
	mb.write((int64)-305555556777770ll);
	mb.write((uint64)305555556777770ull);
	mb.write("halahala");
	mb.write("hoho", 5);
	Message mess = mb.toMessage();
	MessageReader mr(mess);
	assert(mr.readSByte() == 'c');
	assert(mr.readByte() == 'u');
	assert(mr.readInt16() == -20);
	assert(mr.readUInt16() == 30);
	assert(mr.readInt32() == -100);
	assert(mr.readUInt32() == 100u);
	assert(mr.readInt64() == -305555556777770ll);
	assert(mr.readUInt64() == 305555556777770ull);
	assert(mr.readString() == "halahala");
	char bytes[5]; mr.readBytes(bytes, 5);
	assert(std::string(bytes) == "hoho");
}

string mygetline() {
	char line[1024];
	cin.getline(line, 1024);
	return string(line);
}

int main() {
//	testMsg();
//	while (true) {
//		MessageConnection msgconn(UdpConnection::acceptConnection());
//		MessageBuilder mb(0);
//		string str;
//		do {
//			str = mygetline();
//			mb.write(str);
//			msgconn.writeMessage(mb.toMessage());
//		} while (str != "quit");
//	}
	testBandwidth();
	return EXIT_SUCCESS;
}
