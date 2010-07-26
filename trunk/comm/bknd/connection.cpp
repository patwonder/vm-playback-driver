/*
 * connection.cpp
 *
 *  Created on: 2010-7-16
 *      Author: pat
 */

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <iostream>
#include <sstream>
#include "connection.h"

using namespace va;
using namespace std;

int Connection::sock_fd;
struct sockaddr_in Connection::sock_addr;
bool Connection::listening = false;

Connection::StaticDestroyer Connection::destroyer;

void Connection::setSocketOptions(int socket) {
	int window_size = TCP_WINDOW_SIZE;
	setsockopt(socket, SOL_SOCKET, SO_SNDBUF,
		(char*)&window_size, sizeof(window_size));
	setsockopt(socket, SOL_SOCKET, SO_RCVBUF,
		(char*)&window_size, sizeof(window_size));
	int nodelay = 1;
	setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay));
}

// Start listening on port PORT
void Connection::listen() {
	if (listening)
		closeListener();
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		throw ConnectionException("Unable to create socket!");
	}
	bzero(&sock_addr, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(PORT);
	sock_addr.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1"); // Only accept connections from localhost
	if (bind(sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
		stringstream ss;
		ss << "Unable to bind on port " << PORT;
		throw ConnectionException(ss.str());
	}
	if (::listen(sock_fd, BACKLOG) == -1) {
		stringstream ss;
		ss << "Unable to listen on port " << PORT;
		throw ConnectionException(ss.str());
	}
	setSocketOptions(sock_fd);
	listening = true;
}

void Connection::closeListener() {
	if (!listening) return;
	::close(sock_fd);
	listening = false;
}

// Accepts a connection from vm, this function may block
Connection* Connection::acceptConnection() {
	if (!listening)
		listen();
	struct sockaddr_in remote_addr;
	socklen_t sin_size = sizeof(remote_addr);
	int remote_fd;
	if ((remote_fd = accept(sock_fd, (struct sockaddr*)&remote_addr, &sin_size)) == -1) {
		throw ConnectionException("Accept failed.");
	}
	cout << "Received connection from " << inet_ntoa(remote_addr.sin_addr)
			<< ":" << ntohs(remote_addr.sin_port) << endl;
	Connection* conn = new Connection();
	conn->client_fd = remote_fd;
	conn->client_addr = remote_addr;
	conn->open = true;
	return conn;
}

void Connection::close() {
	if (!open) return;
	::close(client_fd);
	open = false;
}

Connection::~Connection() {
	close();
}

void Connection::send(const char* content, unsigned int size) {
	if (!open)
		throw ConnectionException("Connection closed.");
//	while (size > SEND_THRESHOLD) {
//		sendDirect(content, SEND_THRESHOLD);
//		content += SEND_THRESHOLD;
//		size -= SEND_THRESHOLD;
//	}
	sendDirect(content, size);
}

void Connection::receive(char* content, unsigned int size) {
	if (!open)
		throw ConnectionException("Connection closed.");
//	while (size > RECEIVE_THRESHOLD) {
//		receiveDirect(content, RECEIVE_THRESHOLD);
//		content += RECEIVE_THRESHOLD;
//		size -= RECEIVE_THRESHOLD;
//	}
	receiveDirect(content, size);
}

void Connection::sendDirect(const char* content, unsigned int size) {
    ssize_t bytes_send = ::send(client_fd, content, size, 0);
    if (bytes_send == -1 || ((unsigned int)bytes_send) < size) {
    	close();
		throw ConnectionException("Send failed.");
	}
}

void Connection::receiveDirect(char* content, unsigned int size) {
    ssize_t bytes_recv = ::recv(client_fd, content, size, MSG_WAITALL);
    if (bytes_recv == -1 || ((unsigned int)bytes_recv) < size) {
    	close();
		throw ConnectionException("Receive failed.");
	}
}
