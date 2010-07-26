/*
 * bandwidth.cpp
 *
 *  Created on: 2010-7-20
 *      Author: pat
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sys/time.h>
#include "MessageBuilder.h"
#include "MessageConnection.h"
#include "connection.h"
#include "bandwidth.h"

using namespace std;
using namespace va;

namespace va {

const char* const FILE = "bandwidth.txt";
const int MSG_HEADER_SIZE = 8;
const int REPEAT = 100;
const int TEST_CASES[] = {
		1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32,
		40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256,
		320, 384, 448, 512, 640, 768, 896, 1024,
		1280, 1536, 1792, 2048, 2560, 3072, 3584, 4096,
		5120, 6144, 7168, 8192, 10240, 12288, 14336, 16384,
		20480, 24576, 28672, 32768, 40960, 49152, 57344, 65536,
		81920, 98304, 114688, 131072, 163840, 196608, 229376, 262144,
		327680, 393216, 458752, 524288, 655360, 786432, 917504, 1048576
};
const int TEST_COUNT = sizeof(TEST_CASES) / sizeof(int);

timeval start;

void tickStart() {
	gettimeofday(&start, NULL);
}

int tickEnd() {
	timeval now;
	gettimeofday(&now, NULL);
	return (now.tv_sec - start.tv_sec) * 1000000 + (now.tv_usec - start.tv_usec);
}

// avg_latency is measured in microseconds
// bandwidth is measured in bytes per second
void testCase(unsigned int msg_len, int repeat, MessageConnection& conn,
		int& avg_latency, int64& bandwidth) {
	char* bytes = new char[msg_len];
	memset(bytes, 0, msg_len);
	Message msg(0, msg_len, bytes);

	int64 totalTime = 0;
	for (int i = 0; i < repeat; i++) {
		tickStart();
		conn.writeMessage(msg);
		conn.readMessage();
		totalTime += tickEnd();
	}

	avg_latency = totalTime / repeat;
	bandwidth = (int64)(msg_len + MSG_HEADER_SIZE) * 1000000 / avg_latency;
}

void printHeader(ostream& os) {
	os << "Message Length\tLatency\t\tBandwidth" << endl;
}

void print(ostream& os, int test, int latency, int64 bandwidth) {
	os << test << "\t\t" << setiosflags(ios::fixed) << setprecision(3)
			<< (latency / 1000.0) << "ms\t" << (bandwidth / 1000000.0) << "MB/s"
			<< endl;
}

void testBandwidthCommon(IConnection* baseConnection) {
	MessageConnection conn(baseConnection);
	ofstream os(FILE);
	int latency;
	int64 bandwidth;
	printHeader(os);
	printHeader(cout);
	for (int i = 0; i < TEST_COUNT; i++) {
		int thisCase = TEST_CASES[i] * 10;
		testCase(thisCase, REPEAT, conn, latency, bandwidth);
		print(os, thisCase, latency, bandwidth);
		print(cout, thisCase, latency, bandwidth);
	}
	os.close();
}

void testBandwidth() {
	testBandwidthCommon(Connection::acceptConnection());
}

}
