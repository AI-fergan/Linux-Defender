#include "RequestHandler.h"

/*
This c'tor build the project request handler.
input:
db - project database for db tasks.
output: class object.
*/
RequestHandler::RequestHandler(SqliteHelper* db) : _db(db)
{ /**/ }

/*
This function check if the request is valid and exist in the request codes.
input:
reqInfo - data structure of the req.
output: if the request is relevant.
*/
bool RequestHandler::isRequestRelevant(RequestInfo reqInfo) {
    //check the req code id
    switch (reqInfo.id) {
    case IP_SCAN_REQUEST:
    case HASH_SCAN_REQUEST:
    case SAVING_IP_REQUEST:
    case SAVING_HASH_REQUEST:
        return true;

    default:
        return false;
    }
}

/*
This function return the msg struct of the handle by the req code.
input:
reqInfo - data structure of the req.
output: msg struct of the handle result.
*/
message RequestHandler::handleRequest(RequestInfo reqInfo) {
    message result;
    if (reqInfo.id == IP_SCAN_REQUEST)
        result = IPScan(reqInfo);
    else if (reqInfo.id == HASH_SCAN_REQUEST)
        result = hashScan(reqInfo);
    else if (reqInfo.id == SAVING_IP_REQUEST)
        result = saveIp(reqInfo);
    else if (reqInfo.id == SAVING_HASH_REQUEST)
        result = saveHash(reqInfo);

    return result;
}

/*
This function return if the ip is exists in the ip malicious ip's table in the db.
input:
reqInfo - data structure of the req.
output: msg struct of the result.
*/
message RequestHandler::IPScan(RequestInfo reqInfo) {
    IpScanResponse ipScanResponse;
    string ip = JsonRequestPacketDeserializer::deserializeIPScanRequest(reqInfo.buffer).IPAddress;
    ipScanResponse.status = IP_SCAN_RESPONSE;
    ipScanResponse.res = _db->IPisMalicious(ip);

    return JsonResponsePacketSerializer::serializeResponse(ipScanResponse);
}

/*
This function return if the ip is exists in the hash malicious hashes table in the db.
input:
reqInfo - data structure of the req.
output: msg struct of the result.
*/
message RequestHandler::hashScan(RequestInfo reqInfo) {
    HashScanResponse hashScanResponse;
    std::list<string> hashes = JsonRequestPacketDeserializer::deserializeHashScanRequest(reqInfo.buffer).hashes;
    hashScanResponse.status = HASH_SCAN_RESPONSE;
    for(auto& hash : hashes)
        hashScanResponse.res.push_back(_db->HASHisMalicious(hash));
    return JsonResponsePacketSerializer::serializeResponse(hashScanResponse);
}

/*
This function return the results of svaing ip in the malicious ip's table in the db.
input:
reqInfo - data structure of the req.
output: msg struct of the result.
*/
message RequestHandler::saveIp(RequestInfo reqInfo) {
    SaveIpResponse saveIpResponse;
    SaveIpRequest saveIpRequest = JsonRequestPacketDeserializer::deserializeSaveIpRequest(reqInfo.buffer);

    //get the msg details from the reqInfo
    string IPAddress = saveIpRequest.IPAddress;
    string DateAdded = saveIpRequest.DateAdded;
    string Source = saveIpRequest.Source;

    //insert the ip into the db
    bool isWorked = _db->addIP(IPAddress, DateAdded, Source);
    saveIpResponse.status = SAVING_IP_RESPONSE;
    saveIpResponse.isWorked = isWorked;

    return JsonResponsePacketSerializer::serializeResponse(saveIpResponse);
}

/*
This function return the results of svaing hash in the malicious hashes table in the db.
input:
reqInfo - data structure of the req.
output: msg struct of the result.
*/
message RequestHandler::saveHash(RequestInfo reqInfo) {
    SaveHashResponse saveHashResponse;
    SaveHashRequest saveHashRequest = JsonRequestPacketDeserializer::deserializeSaveHashRequest(reqInfo.buffer);

    //get the msg details from the reqInfo
    string FileName = saveHashRequest.FileName;
    string FilePath = saveHashRequest.FilePath;
    string FileHash_SHA256 = saveHashRequest.FileHash_SHA256;
    string DateAdded = saveHashRequest.DateAdded;
    string SourceDetection = saveHashRequest.SourceDetection;

    //insert the hash into the db
    bool isWorked = _db->addHASH(FileName, FilePath, FileHash_SHA256, DateAdded, SourceDetection);
    saveHashResponse.status = SAVING_HASH_RESPONSE;
    saveHashResponse.isWorked = isWorked;

    return JsonResponsePacketSerializer::serializeResponse(saveHashResponse);
}
