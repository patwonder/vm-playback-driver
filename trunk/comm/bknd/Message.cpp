/*
 * Message.cpp
 *
 *  Created on: 2010-7-16
 *      Author: pat
 */

#include "Message.h"

using namespace va;

namespace va {

// Buffer with reference count
class Refbuf {
	const char* content;
	unsigned int refcount;
public:
	Refbuf(const char* buf) {
		content = buf;
		refcount = 1;
	}
	const char* getContentPtr() {
		return content;
	}
	void inc() {
		++refcount;
	}
	void dec() {
		if (--refcount == 0) {
			delete [] content;
		}
	}
};

} // namespace va

Message::Message(int header, unsigned int length, const char* buf) {
	// WARNING: buf should always be allocated on the HEAP!!!
	this->header = header;
	this->length = length;
	this->content = new Refbuf(buf);
}

const char* Message::getContentPtr() const {
	return content->getContentPtr();
}

Message::Message(const Message& other) {
	this->header = other.header;
	this->length = other.length;
	this->content = other.content;
	this->content->inc();
}

Message& Message::operator=(const Message& other) {
	if (this == &other)
		return *this;

	this->content->dec();
	this->header = other.header;
	this->length = other.length;
	this->content = other.content;
	this->content->inc();

	return *this;
}

Message::~Message() {
	content->dec();
}
