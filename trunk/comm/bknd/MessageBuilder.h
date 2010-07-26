/*
 * MessageBuilder.h
 *
 *  Created on: 2010-7-19
 *      Author: pat
 */

#pragma once

#ifndef MESSAGEBUILDER_H_
#define MESSAGEBUILDER_H_

#include <string>
#include "Message.h"
#include "MessageTypes.h"

namespace va {

class MessageBuilder {
private:
	char* content;
	unsigned int init_cap;
	unsigned int capacity;
	unsigned int size;
	int header;
	static const unsigned int DEFAULT_CAP = 10;

	MessageBuilder(const MessageBuilder&);
	MessageBuilder& operator=(const MessageBuilder&);
public:
	MessageBuilder();
	MessageBuilder(int header, unsigned int init_cap = DEFAULT_CAP);
	const Message toMessage();
	void setHeader(int header);
	int getHeader() const;
	void reserve(unsigned int size);
	void write(char value);
	void write(unsigned char value);
	void write(short value);
	void write(unsigned short value);
	void write(int value);
	void write(unsigned int value);
	void write(int64 value);
	void write(uint64 value);
	void write(float value);
	void write(double value);
	void write(const std::string& value);
	void write(const char* bytes, unsigned int size);
	~MessageBuilder();
};

}

#endif /* MESSAGEBUILDER_H_ */
