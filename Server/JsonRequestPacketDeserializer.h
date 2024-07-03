#pragma once
#include "json.hpp"
#include "pch.h"
#include "Helper.h"

using json = nlohmann::json;

struct IPScanRequest {
	string IPAddress;
};

struct HashScanRequest
{
	std::list<string> hashes;
};

struct  SaveIpRequest {
	string IPAddress;
	string DateAdded;
	string Source;
};

struct SaveHashRequest {
	string FileName;
	string FilePath;
	string FileHash_SHA256;
	string DateAdded;
	string SourceDetection;
};

/*
This class is using for deserialize all the given buffers in the server.
*/
class JsonRequestPacketDeserializer
{
public:
	static IPScanRequest deserializeIPScanRequest(vector<uint8_t> buffer);
	static HashScanRequest deserializeHashScanRequest(vector<uint8_t> buffer);
	static SaveIpRequest deserializeSaveIpRequest(vector<uint8_t> buffer);
	static SaveHashRequest deserializeSaveHashRequest(vector<uint8_t> buffer);
};

