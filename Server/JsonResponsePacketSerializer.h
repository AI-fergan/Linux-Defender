#pragma once
#include "json.hpp"
#include "pch.h"
#include "Helper.h"

using json = nlohmann::json;

struct IpScanResponse {
	unsigned int status;
	int res; //o - clear, 1 - malicious
};

struct HashScanResponse
{
	unsigned int status;
	std::list<int> res; //o - clear, 1 - malicious
};

struct SaveIpResponse {
	unsigned int status;
	int isWorked;
};

struct SaveHashResponse {
	unsigned int status;
	int isWorked;
};

struct ErrorResponse {
	string message;
};

/*
This class is using for serialize all the given buffers in the server.
*/
class JsonResponsePacketSerializer {
private:
	static message serializeResponse(json j, int code);

public:
	static message serializeResponse(IpScanResponse ipScanResp);
	static message serializeResponse(HashScanResponse hashScanResp);
	static message serializeResponse(SaveIpResponse saveIpResp);
	static message serializeResponse(SaveHashResponse saveHashResp);
	static message serializeResponse(ErrorResponse errResp);
};

