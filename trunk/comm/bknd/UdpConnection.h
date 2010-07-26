/*
 * UdpConnection.h
 *
 *  Created on: 2010-7-22
 *      Author: pat
 */

#ifndef UDPCONNECTION_H_
#define UDPCONNECTION_H_

#include <stdexcept>

#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include "lib/udt.h"

#include "IConnection.h"

namespace va {

class UdpConnection: public IConnection {
public:
	UDTSOCKET client_fd;
	struct sockaddr_in client_addr;
	bool open;

	static UDTSOCKET sock_fd;
	static struct sockaddr_in sock_addr;
	static bool listening;

	UdpConnection() {}
	void sendDirect(const char* content, unsigned int size);
	void receiveDirect(char* content, unsigned int size);
	static void listen();
	static void closeListener();
	static const int BACKLOG = 10;
	static const unsigned int SEND_THRESHOLD = 50000;
	static const unsigned int RECEIVE_THRESHOLD = 50000;

	class StaticDestroyer;
	friend class StaticDestroyer;
	class StaticDestroyer {
	public:
		StaticDestroyer() {
		}
		~StaticDestroyer() {
			UdpConnection::closeListener();
			UDT::cleanup();
		}
	};
	static StaticDestroyer destroyer;
public:
	static UdpConnection* acceptConnection();
	virtual ~UdpConnection();

	virtual void send(const char* content, unsigned int size);
	virtual void receive(char* content, unsigned int size);
	virtual void flush();
	void close();
};

class UdpException: public std::runtime_error {
public:
	UdpException(std::string what) :
		runtime_error(what) {
	}
};


}

#endif /* UDPCONNECTION_H_ */
