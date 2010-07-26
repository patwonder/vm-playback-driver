/*
 * Message.h
 *
 *  Created on: 2010-7-16
 *      Author: pat
 */

#pragma once

#ifndef MESSAGE_H_
#define MESSAGE_H_

namespace va {

class Refbuf;

class Message {
private:
	int header;
	unsigned int length;
	Refbuf* content;
public:
	Message(int header, unsigned int length, const char* content);
	Message(const Message& other);
	Message& operator=(const Message& other);
	int getHeader() const {
		return header;
	}
	int getLength() const {
		return length;
	}
	const char* getContentPtr() const;
	~Message();
};

} // namespace va

#endif /* MESSAGE_H_ */
