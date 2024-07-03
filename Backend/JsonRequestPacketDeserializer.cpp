#include "JsonRequestPacketDeserializer.h"
#include "Helper.h"

/*
This is the deserializer function of the scan request.
input:
buffer - data of the sacn req.
output: struct of the scan req.
*/
ScanRequest JsonRequestPacketDeserializer::deserializeScanRequest(vector<uint8_t> buffer)
{
	ScanRequest request;
	json jsn = json::parse(buffer);
	request.scanType = jsn["scanType"].get<int>();
	request.path = jsn["path"].get<string>();
	return request;
}

/*
This is the deserializer function of the free process request.
input:
buffer - data of the req.
output: struct of the process data req.
*/
FreeRequest JsonRequestPacketDeserializer::desrializeFreeRequest(vector<uint8_t> buffer)
{
	FreeRequest request;
	json jsn = json::parse(buffer);
	request.pid = jsn["pid"].get<int>();
	return request;
}

/*
This is the deserializer function of the block process request.
input:
buffer - data of the req.
output: struct of the process data req.
*/
BlockRequest JsonRequestPacketDeserializer::desirializeBlockRequest(vector<uint8_t> buffer)
{
	BlockRequest request;
	json jsn = json::parse(buffer);
	request.pid = jsn["pid"].get<int>();
	return request;
}

/*
This is the deserializer function of the kill process request.
input:
buffer - data of the req.
output: struct of the process data req.
*/
KillRequest JsonRequestPacketDeserializer::desirializeKillRequest(vector<uint8_t> buffer)
{
	KillRequest request;
	json jsn = json::parse(buffer);
	request.pid = jsn["pid"].get<int>();
	return request;
}

/*
This is the deserializer function of the change scan time request.
input:
buffer - data of the req.
output: struct of the data req.
*/
ChangeScanTimeRequest JsonRequestPacketDeserializer::desirializeChangeScanTimeRequest(std::vector<uint8_t> buffer)
{
	ChangeScanTimeRequest request;
	json jsn = json::parse(buffer);
	request.time = jsn["time"].get<int>();
	return request;
}

/*
This is the deserializer function of the change blocked processes table request.
input:
buffer - data of the req.
output: struct of the data req.
*/
UpdateBlockedTableRequest JsonRequestPacketDeserializer::deserializeUpdateBlockedTbaleRequest(std::vector<uint8_t> buffer)
{
	UpdateBlockedTableRequest request;
	json jsn = json::parse(buffer);

	//process data
	request.pid = jsn["pid"].get<int>();
	request.dateTime = jsn["dateTime"].get<string>();
	request.action = jsn["action"].get<string>();
	return request;
}

/*
This is the deserializer function of the change virus files table request.
input:
buffer - data of the req.
output: struct of the data req.
*/
UpdateVirusTableRequest JsonRequestPacketDeserializer::deserializeUpdateVirusTableRequest(std::vector<uint8_t> buffer)
{
	UpdateVirusTableRequest request;
	json jsn = json::parse(buffer);

	//virus data
	request.name = jsn["name"].get<string>();
	request.dateTime = jsn["dateTime"].get<string>();
	return request;
}


