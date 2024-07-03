#include "JsonRequestPacketDeserializer.h"

/*
This is the deserializer function of the ip scan request in the db.
input:
buffer - data of the sacn req.
output: struct of the scan req.
*/
IPScanRequest JsonRequestPacketDeserializer::deserializeIPScanRequest(vector<uint8_t> buffer) {
	IPScanRequest iPScanRequest;
	json jsn = json::parse(buffer);
	iPScanRequest.IPAddress = jsn["ip"].get<string>();

	return iPScanRequest;
}

/*
This is the deserializer function of the hash scan request in the db.
input:
buffer - data of the sacn req.
output: struct of the scan req.
*/
HashScanRequest JsonRequestPacketDeserializer::deserializeHashScanRequest(vector<uint8_t> buffer) {
	HashScanRequest hashScanRequest;
	json jsn = json::parse(buffer);
	hashScanRequest.hashes = jsn["hashes"].get<std::list<string>>();

	return hashScanRequest;
}

/*
This is the deserializer function of the ip save request in the db.
input:
buffer - data of the save ip req.
output: struct of the save ip req.
*/
SaveIpRequest JsonRequestPacketDeserializer::deserializeSaveIpRequest(vector<uint8_t> buffer)
{
	SaveIpRequest saveIpRequest;
	json jsn = json::parse(buffer);
	saveIpRequest.IPAddress = jsn["ip"].get<string>();
	saveIpRequest.DateAdded = jsn["DateAdded"].get<string>();
	saveIpRequest.Source = jsn["source"].get<string>();

	return saveIpRequest;
}

/*
This is the deserializer function of the hash save request in the db.
input:
buffer - data of the save hash req.
output: struct of the save hash req.
*/
SaveHashRequest JsonRequestPacketDeserializer::deserializeSaveHashRequest(vector<uint8_t> buffer)
{
	SaveHashRequest saveHashRequest;
	json jsn = json::parse(buffer);
	saveHashRequest.DateAdded = jsn["DateAdded"].get<string>();
	saveHashRequest.FileHash_SHA256 = jsn["hash"].get<string>();
	saveHashRequest.FileName = jsn["fileName"].get<string>();
	saveHashRequest.FilePath = jsn["filepath"].get<string>();
	saveHashRequest.SourceDetection = jsn["source"].get<string>();

	return saveHashRequest;
}
