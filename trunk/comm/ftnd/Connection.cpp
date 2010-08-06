/*
 * Connection.cpp
 *
 *  Created on: 2010-7-19
 *      Author: pat
 */

#include <ws2tcpip.h>
#include <memory.h>
#include <sstream>
#include <cstdio>
#include "Connection.h"
#include "Common.h"

using namespace va;
using namespace std;

Connection::StaticDestroyer Connection::destroyer;

Connection::StaticDestroyer::~StaticDestroyer() {
	if (initialized) {
		WSACleanup();
	}
}

bool Connection::initialized = false;

void Connection::initialize() {
	if (initialized) return;
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	int hr = WSAStartup(wVersionRequested, &wsaData);
	if (hr != 0 || LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		throw ConnectionException("WSAStartup failed: cannot find a suitable version of winsock.dll.");
	}
	initialized = true;
}

Connection::Connection() {

}

Connection::~Connection() {
	close();
}

void Connection::close() {
	if (!open) return;
	::closesocket(sock_fd);
	open = false;
}

void Connection::setSocketOptions(SOCKET socket) {
	int window_size = TCP_WINDOW_SIZE;
	setsockopt(socket, SOL_SOCKET, SO_SNDBUF,
		(char*)&window_size, sizeof(window_size));
	setsockopt(socket, SOL_SOCKET, SO_RCVBUF,
		(char*)&window_size, sizeof(window_size));
	int nodelay = 1;
	setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay));
}

Connection* Connection::newConnection(int prt) {
	if (!initialized)
		initialize();

	addrinfo hints;
	addrinfo* res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	char port[10];
	sprintf(port, "%d", prt);

	if (0 != getaddrinfo(HOST_ADDR, port, &hints, &res)) {
		throw ConnectionException("Incorrect network address.");
	}

	SOCKET sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock_fd == INVALID_SOCKET) {
		throw ConnectionException("Cannot create socket.");
	}

	setSocketOptions(sock_fd);

	int hr = connect(sock_fd, res->ai_addr, res->ai_addrlen);
	if (hr == SOCKET_ERROR) {
		throw ConnectionException("Cannot connect to va host.");
	}

	Connection* conn = new Connection();
	conn->sock_fd = sock_fd;
	conn->sock_addr = *(sockaddr_in*)res->ai_addr;
	conn->open = true;
	return conn;
}

void Connection::send(const char* content, unsigned int size) {
	if (!open)
		throw ConnectionException("Connection closed.");
	//while (size > SEND_THRESHOLD) {
	//	sendDirect(content, SEND_THRESHOLD);
	//	content += SEND_THRESHOLD;
	//	size -= SEND_THRESHOLD;
	//}
	sendDirect(content, size);
}

void Connection::receive(char* content, unsigned int size) {
	if (!open)
		throw ConnectionException("Connection closed.");
	//while (size > RECEIVE_THRESHOLD) {
	//	receiveDirect(content, RECEIVE_THRESHOLD);
	//	content += RECEIVE_THRESHOLD;
	//	size -= RECEIVE_THRESHOLD;
	//}
	receiveDirect(content, size);
}

void Connection::sendDirect(const char* content, unsigned int size) {
	int bytes_send = ::send(sock_fd, content, size, 0);
	if (bytes_send == SOCKET_ERROR || ((unsigned int)bytes_send) < size) {
		close();
		throw ConnectionException("Send failed.");
	}
}

void Connection::receiveDirect(char* content, unsigned int size) {
	int bytes_recv = ::recv(sock_fd, content, size, 0);
	while (bytes_recv > 0 && ((unsigned int)bytes_recv) < size) {
		content += bytes_recv;
		size -= bytes_recv;
		bytes_recv = ::recv(sock_fd, content, size, 0);
	}
	if (bytes_recv <= 0) {
		close();
		stringstream ss;
		ss << "Receive failed: content = 0x" << hex << (int)content << ", size = " << dec << size
			<< ", bytes_recv = " << bytes_recv;
		throw ConnectionException(ss.str());
	}
}
