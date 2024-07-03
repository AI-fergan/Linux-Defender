#pragma once

#include "sqlite3.h"
#include "pch.h"

#define NO_FLAGS 0


#define CODE_SIZE 1
#define LEN_SIZE 4


typedef struct message{
	int code;
	int length;
	string data;
	char* msg;
} message;

struct RequestInfo {
	unsigned char id;
	vector<uint8_t> buffer;

	RequestInfo(message msg) {
		id = msg.code;
		buffer = vector<uint8_t>(msg.data.begin(), msg.data.end());
	}
};

/*
This class contains the socket communication helper functions.
*/

class Helper {
public:
    static char* getPartFromSocket(int sc, int bytesNum);
    static message getAllTheSocket(int sc);
    static void sendData(int sc, message msg);
    static void sendData(int sc, std::string message);
    static bool updateVirusTables(std::string name);
};