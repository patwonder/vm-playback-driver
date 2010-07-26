/*
 * UdpConnection.h
 *
 *  Created on: 2010-7-22
 *      Author: pat
 */

#ifndef UDPCONNECTION_H_
#define UDPCONNECTION_H_

#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include "lib\udt.h"

#include "IConnection.h"

namespace va {

class UdpConnection: public IConnection {
public:
	UDTSOCKET sock_fd;
	struct sockaddr_in sock_addr;
	bool open;

	static bool initialized;
	static void initialize();

	UdpConnection() {}
	void sendDirect(const char* content, unsigned int size);
	void receiveDirect(char* content, unsigned int size);

	static const unsigned int SEND_THRESHOLD = 50000;
	static const unsigned int RECEIVE_THRESHOLD = 50000;

	class StaticDestroyer;
	friend class StaticDestroyer;
	class StaticDestroyer {
	public:
		StaticDestroyer() {
		}
		~StaticDestroyer() {
			UDT::cleanup();
		}
	};
	static StaticDestroyer destroyer;
public:
	static UdpConnection* newConnection();
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