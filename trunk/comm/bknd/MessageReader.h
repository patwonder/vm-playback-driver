/*
 * MessageReader.h
 *
 *  Created on: 2010-7-19
 *      Author: pat
 */

#pragma once

#ifndef MESSAGEREADER_H_
#define MESSAGEREADER_H_

#include <string>
#include <stdexcept>
#include "Message.h"
#include "MessageTypes.h"

namespace va {

class MessageReader {
private:
	Message message;
	const char* content;
	unsigned int seek_pos;
	void assureSize(unsigned int size_to_read);
public:
	MessageReader(const Message& mess);
	unsigned int getSize() const;
	unsigned int getSeekPos() const;
	void seek(unsigned int pos);
	char readSByte();
	unsigned char readByte();
	short readInt16();
	unsigned short readUInt16();
	int readInt32();
	unsigned int readUInt32();
	int64 readInt64();
	uint64 readUInt64();
	const std::string readString();
	void readBytes(char* bytes, unsigned int size);
	~MessageReader();
};

class ReaderException: public std::runtime_error {
public:
	ReaderException(const std::string& what) :
		runtime_error(what) {

	}
};

}

#endif /* MESSAGEREADER_H_ */
