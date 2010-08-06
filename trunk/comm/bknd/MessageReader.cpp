/*
 * MessageReader.cpp
 *
 *  Created on: 2010-7-19
 *      Author: pat
 */

#include <memory.h>
#include "MessageReader.h"

using namespace va;

MessageReader::MessageReader(const Message& mess) : message(mess) {
	seek_pos = 0;
	content = message.getContentPtr();
}

unsigned int MessageReader::getSize() const {
	return message.getLength();
}

unsigned int MessageReader::getSeekPos() const {
	return seek_pos;
}

void MessageReader::seek(unsigned int pos) {
	if (pos > getSize())
		throw ReaderException("Seek out of range.");
	seek_pos = pos;
}

unsigned int MessageReader::tell() {
	return seek_pos;
}

void MessageReader::assureSize(unsigned int size_to_read) {
	if (seek_pos + size_to_read > getSize())
		throw ReaderException("Not enough data to read.");
}

char MessageReader::readSByte() {
	assureSize(sizeof(char));
	char result = *(content + seek_pos);
	seek_pos += sizeof(char);
	return result;
}

unsigned char MessageReader::readByte() {
	assureSize(sizeof(unsigned char));
	unsigned char result = *(unsigned char*)(content + seek_pos);
	seek_pos += sizeof(unsigned char);
	return result;
}

short MessageReader::readInt16() {
	assureSize(sizeof(short));
	short result = *(short*)(content + seek_pos);
	seek_pos += sizeof(short);
	return result;
}

unsigned short MessageReader::readUInt16() {
	assureSize(sizeof(unsigned short));
	unsigned short result = *(unsigned short*)(content + seek_pos);
	seek_pos += sizeof(unsigned short);
	return result;
}

int MessageReader::readInt32() {
	assureSize(sizeof(int));
	int result = *(int*)(content + seek_pos);
	seek_pos += sizeof(int);
	return result;
}

unsigned int MessageReader::readUInt32() {
	assureSize(sizeof(unsigned int));
	unsigned int result = *(unsigned int*)(content + seek_pos);
	seek_pos += sizeof(unsigned int);
	return result;
}

int64 MessageReader::readInt64() {
	assureSize(sizeof(int64));
	int64 result = *(int64*)(content + seek_pos);
	seek_pos += sizeof(int64);
	return result;
}

uint64 MessageReader::readUInt64() {
	assureSize(sizeof(uint64));
	uint64 result = *(uint64*)(content + seek_pos);
	seek_pos += sizeof(uint64);
	return result;
}

const std::string MessageReader::readString() {
	unsigned int size = readUInt32();
	assureSize(size);
	unsigned int org_seek_pos = seek_pos;
	seek_pos += size;
	return std::string(content + org_seek_pos, size);
}

void MessageReader::readBytes(char* bytes, unsigned int size) {
	assureSize(size);
	memcpy(bytes, content + seek_pos, size);
	seek_pos += size;
}

MessageReader::~MessageReader() {

}
