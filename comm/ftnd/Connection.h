/*
 * Connection.h
 *
 *  Created on: 2010-7-19
 *      Author: pat
 */

#pragma once

#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <winsock2.h>
#include <stdexcept>
#include "IConnection.h"
#include "Common.h"

namespace va {

class Connection : public IConnection {
private:
	Connection();

	SOCKET sock_fd;
	sockaddr_in sock_addr;
	bool open;

	void sendDirect(const char* content, unsigned int size);
	void receiveDirect(char* content, unsigned int size);

	//static const unsigned int SEND_THRESHOLD = 2048;
	//static const unsigned int RECEIVE_THRESHOLD = 2048;

	static bool initialized;
	static void initialize();
	static void setSocketOptions(SOCKET socket);
	
	class StaticDestroyer;
	friend class StaticDestroyer;
	class StaticDestroyer {
	public:
		~StaticDestroyer();
	};
	static StaticDestroyer destroyer;
public:
	static Connection* newConnection(int port = va::PORT);
	virtual void send(const char* content, unsigned int size);
	virtual void receive(char* content, unsigned int size);
	void close();
	~Connection();
};

class ConnectionException: public std::runtime_error {
public:
	ConnectionException(std::string what) :
		runtime_error(what) {
	}
};

}

#endif /* _CONNECTION_H */
