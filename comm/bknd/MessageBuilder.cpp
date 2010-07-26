/*
 * MessageBuilder.cpp
 *
 *  Created on: 2010-7-19
 *      Author: pat
 */

#include <memory.h>
#include "MessageBuilder.h"

using namespace va;

MessageBuilder::MessageBuilder(int header, unsigned int init_cap) {
	this->header = header;
	this->init_cap = capacity = init_cap;
	content = new char[capacity];
	size = 0;
}


// Ensure ZERO-Copy construction of Message instance
const Message MessageBuilder::toMessage() {
	char* orgcontent = content;
	unsigned int orgsize = size;
	capacity = init_cap;
	content = new char[capacity];
	size = 0;
	return Message(header, orgsize, orgcontent);
}

void MessageBuilder::reserve(unsigned int size) {
	unsigned int capacity = this->capacity;
	if (capacity < size) {
		capacity <<= 1;
		if (capacity < size)
			capacity = size;
	} else {
		return;
	}

	// New capacity got, allocate & copy memory here
	char* new_content = new char[capacity];
	memcpy(new_content, content, this->size);
	delete [] content;
	content = new_content;
	this->capacity = capacity;
}

void MessageBuilder::write(char value) {
	reserve(size + sizeof(value));
	*(content + size) = value;
	size += sizeof(value);
}

void MessageBuilder::write(unsigned char value) {
	reserve(size + sizeof(value));
	*(unsigned char*)(content + size) = value;
	size += sizeof(value);
}

void MessageBuilder::write(short value) {
	reserve(size + sizeof(value));
	*(short*)(content + size) = value;
	size += sizeof(value);
}

void MessageBuilder::write(unsigned short value) {
	reserve(size + sizeof(value));
	*(unsigned short*)(content + size) = value;
	size += sizeof(value);
}

void MessageBuilder::write(int value) {
	reserve(size + sizeof(value));
	*(int*)(content + size) = value;
	size += sizeof(value);
}

void MessageBuilder::write(unsigned int value) {
	reserve(size + sizeof(value));
	*(unsigned int*)(content + size) = value;
	size += sizeof(value);
}

void MessageBuilder::write(int64 value) {
	reserve(size + sizeof(value));
	*(int64*)(content + size) = value;
	size += sizeof(value);
}
void MessageBuilder::write(uint64 value) {
	reserve(size + sizeof(value));
	*(uint64*)(content + size) = value;
	size += sizeof(value);
}

void MessageBuilder::write(float value) {
	reserve(size + sizeof(value));
	*(float*)(content + size) = value;
	size += sizeof(value);
}

void MessageBuilder::write(double value) {
	reserve(size + sizeof(value));
	*(double*)(content + size) = value;
	size += sizeof(value);
}

void MessageBuilder::write(const std::string& value) {
	reserve(size + sizeof(unsigned int) + value.length());
	*(unsigned int*)(content + size) = value.length();
	size += sizeof(unsigned int);
	memcpy(content + size, value.c_str(), value.length());
	size += value.length();
}

void MessageBuilder::write(const char* bytes, unsigned int size) {
	reserve(this->size + size);
	memcpy(content + this->size, bytes, size);
	this->size += size;
}

MessageBuilder::~MessageBuilder() {
	delete [] content;
}

void MessageBuilder::setHeader(int header) {
	this->header = header;
}

int MessageBuilder::getHeader() const {
	return header;
}

MessageBuilder::MessageBuilder() {
	this->header = 0;
	this->init_cap = capacity = DEFAULT_CAP;
	content = new char[capacity];
	size = 0;
}
