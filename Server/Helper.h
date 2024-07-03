#pragma once
#include "pch.h"

#define NO_FLAGS 0
#define SIZE_OF_BYTE 8
#define CODE_SIZE 1
#define LEN_SIZE 4

struct message{
	int code;
	int length;
	string data;
	char* msg;
};

/*
This class contains the socket communication helper functions.
*/

class Helper {
public:
    static char* getPartFromSocket(int sc, int bytesNum);  // Change SOCKET to int
    static message getAllTheSocket(int sc);  // Change SOCKET to int
    static void sendData(int sc, message msg);  // Change SOCKET to int
};