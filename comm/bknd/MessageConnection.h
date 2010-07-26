/*
 * MessageConnection.h
 *
 *  Created on: 2010-7-19
 *      Author: pat
 */

#pragma once

#ifndef MESSAGECONNECTION_H_
#define MESSAGECONNECTION_H_

#include "IConnection.h"
#include "Message.h"

namespace va {

class MessageConnection {
private:
	IConnection* connection;

	MessageConnection(const MessageConnection&);
	MessageConnection& operator=(const MessageConnection&);
public:
	MessageConnection(IConnection* conn);
	const Message readMessage();
	void writeMessage(const Message& mess);
	void flush();
	~MessageConnection();
};

}

#endif /* MESSAGECONNECTION_H_ */
