#pragma once

#include "Helper.h"
#include "pch.h"

//client
struct ScanResponse {
	unsigned int status;
	int sctResults;
	int idtResults;
	vector<string> packetsResults;
	map<string, bool> openPortsResults;
	map<string, string> spofingResults;
	map<string, vector<string>> yaraResults;
	map<string, bool> hashesResults;
};

struct SuspiciousListResponse {
	unsigned int status;
	vector<string> procNames;
};

struct SuspendedListResponse {
	unsigned int status;
	vector<string> procPids;
};

struct FreeResponse {
	unsigned int status;
	int isWorked;
};

struct BlockResponse {
	unsigned int status;
	int isWorked;
};

struct KillResponse {
	unsigned int status;
	int isWorked;
};

struct ChangeScanTimeResponse{
	unsigned int status;
	int isWorked;
};

struct UpdateBlockedTableResponse
{
	unsigned int status;
	int isWorked;
};

struct UpdateVirusTableResponse{
	unsigned int status;
	int isWorked;
};

struct GetTimeResponse{
	unsigned int status;
	int time;
};

//err
struct ErrorResponse {
	string message;
};

/*
This class is using for serialize all the given buffers in the backend.
*/
class JsonResponsePacketSerializer {
public:
	//client 
	static message serializeResponse(ScanResponse scanResp);
	static message serializeResponse(SuspendedListResponse suspendedResp);
	static message serializeResponse(SuspiciousListResponse suspiciousResp);
	static message serializeResponse(FreeResponse freeResp);
	static message serializeResponse(BlockResponse blockResp);
	static message serializeResponse(KillResponse killResp);
	static message serializeResponse(ChangeScanTimeResponse changeTimeResp);
	static message serializeResponse(UpdateBlockedTableResponse blockedTbaleResp);
	static message serializeResponse(UpdateVirusTableResponse virusTableResp);
	static message serializeResponse(GetTimeResponse timeResp);

	//Error
	static message serializeResponse(ErrorResponse errResp);

private:
	static message serializeResponse(json j, int code);
};

