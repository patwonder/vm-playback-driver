/*
 * UdpConnection.cpp
 *
 *  Created on: 2010-7-22
 *      Author: pat
 */

#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <memory.h>

#include "UdpConnection.h"
#include "common.h"

using namespace va;
using namespace std;

bool UdpConnection::initialized = false;

UdpConnection::StaticDestroyer UdpConnection::destroyer;

void UdpConnection::initialize() {
	if (initialized)
		return;
	UDT::startup();
	initialized = true;
}

UdpConnection* UdpConnection::newConnection() {
	if (!initialized)
		initialize();

	addrinfo hints;
	addrinfo* res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	char port[10];
	sprintf(port, "%d", PORT);

	if (0 != getaddrinfo(HOST_ADDR, port, &hints, &res)) {
		throw UdpException("Incorrect network address.");
	}

	int remote_fd = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (UDT::ERROR == UDT::connect(remote_fd, res->ai_addr, res->ai_addrlen)) {
		throw UdpException("Connect failed.");
	}

	sockaddr_in remote_addr = *(sockaddr_in*)res->ai_addr;
	cout << "Received connection from " << inet_ntoa(remote_addr.sin_addr)
			<< ":" << ntohs(remote_addr.sin_port) << endl;
	
	freeaddrinfo(res);

	UdpConnection* conn = new UdpConnection();
	conn->sock_fd = remote_fd;
	conn->sock_addr = remote_addr;
	conn->open = true;
	return conn;
}

void UdpConnection::close() {
	if (!open) return;
	UDT::close(sock_fd);
	open = false;
}

UdpConnection::~UdpConnection() {
	close();
}

void UdpConnection::send(const char* content, unsigned int size) {
	if (!open)
		throw UdpException("UdpConnection closed.");
	//while (size > SEND_THRESHOLD) {
	//	sendDirect(content, SEND_THRESHOLD);
	//	content += SEND_THRESHOLD;
	//	size -= SEND_THRESHOLD;
	//}
	sendDirect(content, size);
}

void UdpConnection::receive(char* content, unsigned int size) {
	if (!open)
		throw UdpException("UdpConnection closed.");
	//while (size > RECEIVE_THRESHOLD) {
	//	receiveDirect(content, RECEIVE_THRESHOLD);
	//	content += RECEIVE_THRESHOLD;
	//	size -= RECEIVE_THRESHOLD;
	//}
	receiveDirect(content, size);
}

void UdpConnection::sendDirect(const char* content, unsigned int size) {
	int bytes_send = UDT::send(sock_fd, content, size, 0);
    if (bytes_send == UDT::ERROR || ((unsigned int)bytes_send) < size) {
    	close();
		throw UdpException("Send failed.");
	}
}

void UdpConnection::receiveDirect(char* content, unsigned int size) {
	int bytes_recv = UDT::recv(sock_fd, content, size, 0);
	while (bytes_recv > 0 && ((unsigned int)bytes_recv) < size) {
		content += bytes_recv;
		size -= bytes_recv;
		bytes_recv = UDT::recv(sock_fd, content, size, 0);
	}
	if (bytes_recv <= 0) {
		close();
		stringstream ss;
		ss << "Receive failed: content = 0x" << hex << (int)content << ", size = " << dec << size
			<< ", bytes_recv = " << bytes_recv << "\n" << UDT::getlasterror().getErrorMessage();
		throw UdpException(ss.str());
	}
}

void UdpConnection::flush() {

}
