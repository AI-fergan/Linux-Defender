#pragma once
#include "pch.h"

//client
struct ScanRequest {
	int scanType;
	string path; //optional
};

struct FreeRequest {
	int pid;
};

struct BlockRequest {
	int pid;
};

struct KillRequest {
	int pid;
};


struct ChangeScanTimeRequest{
	int time;
};

struct UpdateBlockedTableRequest{
	int pid;
	string dateTime;
	string action; //add, remove
}; 

struct UpdateVirusTableRequest{
	string name; //could be path of a file or a pid of a process
	string dateTime;
};

/*
This class is using for deserialize all the given buffers in the backend.
*/
class JsonRequestPacketDeserializer {
public:
	//client
	static ScanRequest deserializeScanRequest(vector<uint8_t> buffer);
	static FreeRequest desrializeFreeRequest(vector<uint8_t> buffer);
	static BlockRequest desirializeBlockRequest(vector<uint8_t> buffer);
	static KillRequest desirializeKillRequest(vector<uint8_t> buffer);
	static ChangeScanTimeRequest desirializeChangeScanTimeRequest(vector<uint8_t> buffer);
	static UpdateBlockedTableRequest deserializeUpdateBlockedTbaleRequest(vector<uint8_t> buffer);
	static UpdateVirusTableRequest deserializeUpdateVirusTableRequest(vector<uint8_t> buffer);
};

