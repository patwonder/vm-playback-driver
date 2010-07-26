/*
 * IConnection.h
 *
 *  Created on: 2010-7-19
 *      Author: pat
 */

#pragma once

#ifndef ICONNECTION_H_
#define ICONNECTION_H_

namespace va {

// Block-mode send/receive interface
class IConnection /* interface */ {
public:
	virtual void send(const char* content, unsigned int size) = 0;
	virtual void receive(char* content, unsigned int size) = 0;
	virtual void flush();
	virtual ~IConnection() = 0;
};

}

#endif /* ICONNECTION_H_ */
