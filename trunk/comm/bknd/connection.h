/*
 * connection.h
 *
 *  Created on: 2010-7-16
 *      Author: pat
 */

#pragma once

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "common.h"
#include "IConnection.h"

namespace va {

class Connection : public IConnection {
private:
	int client_fd;
	struct sockaddr_in client_addr;
	bool open;

	static int sock_fd;
	static struct sockaddr_in sock_addr;
	static bool listening;

	Connection() {}
	void sendDirect(const char* content, unsigned int size);
	void receiveDirect(char* content, unsigned int size);

	static void setSocketOptions(int socket);
	static void listen();
	static void closeListener();
	static const int BACKLOG = 10;
	static const unsigned int SEND_THRESHOLD = 2048;
	static const unsigned int RECEIVE_THRESHOLD = 2048;

	class StaticDestroyer;
	friend class StaticDestroyer;
	class StaticDestroyer {
	public:
		StaticDestroyer() {
		}
		~StaticDestroyer() {
			Connection::closeListener();
		}
	};
	static StaticDestroyer destroyer;
public:
	static Connection* acceptConnection();
	virtual ~Connection();

	virtual void send(const char* content, unsigned int size);
	virtual void receive(char* content, unsigned int size);
	void close();
};

class ConnectionException: public std::runtime_error {
public:
	ConnectionException(std::string what) :
		runtime_error(what) {
	}
};

} //namespace va

#endif /* CONNECTION_H_ */
