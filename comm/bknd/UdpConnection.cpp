/*
 * UdpConnection.cpp
 *
 *  Created on: 2010-7-22
 *      Author: pat
 */

#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <sstream>

#include "UdpConnection.h"
#include "common.h"

using namespace va;
using namespace std;

int UdpConnection::sock_fd;
struct sockaddr_in UdpConnection::sock_addr;
bool UdpConnection::listening = false;

UdpConnection::StaticDestroyer UdpConnection::destroyer;

// Start listening on port PORT
void UdpConnection::listen() {
	if (listening)
		closeListener();

	UDT::startup();
	addrinfo hints;
	addrinfo* res;

	memset(&hints, 0, sizeof(hints));

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	char port[10];
	sprintf(port, "%d", PORT);

	if (0 != getaddrinfo(NULL, port, &hints, &res)) {
		throw UdpException("Illegal port number or port is busy.");
	}

	bzero(&sock_addr, sizeof(sock_addr));

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(PORT);
	sock_addr.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1");

	sock_fd = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (UDT::ERROR == UDT::bind(sock_fd, (sockaddr*)&sock_addr, sizeof(sock_addr))) {
		stringstream ss;
		ss << "Unable to bind on port " << PORT << "\n" << UDT::getlasterror().getErrorMessage();
		throw UdpException(ss.str());
	}

	freeaddrinfo(res);

	if (UDT::ERROR == UDT::listen(sock_fd, BACKLOG)) {
		stringstream ss;
		ss << "Unable to listen on port " << PORT;
		throw UdpException(ss.str());
	}
	listening = true;
}

void UdpConnection::closeListener() {
	if (!listening) return;
	UDT::close(sock_fd);
	listening = false;
}

// Accepts a connection from vm, this function may block
UdpConnection* UdpConnection::acceptConnection() {
	if (!listening)
		listen();
	struct sockaddr_in remote_addr;
	int sin_size = sizeof(remote_addr);
	int remote_fd;
	if (UDT::INVALID_SOCK == (remote_fd = UDT::accept(sock_fd, (struct sockaddr*)&remote_addr, &sin_size))) {
		throw UdpException("Accept failed.");
	}
	cout << "Received connection from " << inet_ntoa(remote_addr.sin_addr)
			<< ":" << ntohs(remote_addr.sin_port) << endl;
	UdpConnection* conn = new UdpConnection();
	conn->client_fd = remote_fd;
	conn->client_addr = remote_addr;
	conn->open = true;
	return conn;
}

void UdpConnection::close() {
	if (!open) return;
	UDT::close(client_fd);
	open = false;
}

UdpConnection::~UdpConnection() {
	close();
}

void UdpConnection::send(const char* content, unsigned int size) {
	if (!open)
		throw UdpException("UdpConnection closed.");
//	while (size > SEND_THRESHOLD) {
//		sendDirect(content, SEND_THRESHOLD);
//		content += SEND_THRESHOLD;
//		size -= SEND_THRESHOLD;
//	}
	sendDirect(content, size);
}

void UdpConnection::receive(char* content, unsigned int size) {
	if (!open)
		throw UdpException("UdpConnection closed.");
//	while (size > RECEIVE_THRESHOLD) {
//		receiveDirect(content, RECEIVE_THRESHOLD);
//		content += RECEIVE_THRESHOLD;
//		size -= RECEIVE_THRESHOLD;
//	}
	receiveDirect(content, size);
}

void UdpConnection::sendDirect(const char* content, unsigned int size) {
	int bytes_send = UDT::send(client_fd, content, size, 0);
    if (bytes_send == UDT::ERROR || ((unsigned int)bytes_send) < size) {
    	close();
		throw UdpException("Send failed.");
	}
}

void UdpConnection::receiveDirect(char* content, unsigned int size) {
	int bytes_recv = UDT::recv(client_fd, content, size, 0);
	while (bytes_recv > 0 && ((unsigned int)bytes_recv) < size) {
		content += bytes_recv;
		size -= bytes_recv;
		bytes_recv = UDT::recv(client_fd, content, size, 0);
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
