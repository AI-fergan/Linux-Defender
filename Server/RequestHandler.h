#pragma once
#include "pch.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "SqliteHelper.h"

/*
This is the requests object for recving data.
input:
msg - the msg object of the data
*/
struct RequestInfo {
	unsigned char id;
	vector<uint8_t> buffer;

	RequestInfo(message msg)
	{
		id = msg.code;
		buffer = vector<uint8_t>(msg.data.begin(), msg.data.end());
	}
};

/*
This c'tor help us to manage the project handlers.
*/
class RequestHandler {
public:
	RequestHandler(SqliteHelper* db);
	bool isRequestRelevant(RequestInfo reqInfo);
	message handleRequest(RequestInfo reqInfo);

private:
//pointer to the db object
	SqliteHelper* _db;

	message IPScan(RequestInfo reqInfo);
	message hashScan(RequestInfo reqInfo);
	message saveIp(RequestInfo reqInfo);
	message saveHash(RequestInfo reqInfo);
};

