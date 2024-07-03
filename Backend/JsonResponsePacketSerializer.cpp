#include "JsonResponsePacketSerializer.h"

/*
This is the main serialize function of all the server responses.
input:
j - json of the response data.
code - response msg code.
output: msg struct with all of the msg details.
*/
message JsonResponsePacketSerializer::serializeResponse(json j, int code) {
    message msg;
    msg.code = code;
    msg.data = j.dump();
    msg.length = msg.data.length();
    msg.msg = new char[msg.length + LEN_SIZE + CODE_SIZE];
    
    //copy all the data into the msg struct.
    std::memcpy(msg.msg, (void*)&code, CODE_SIZE);
    std::memcpy(msg.msg + CODE_SIZE, &msg.length, LEN_SIZE);
    std::memcpy(msg.msg + CODE_SIZE + LEN_SIZE, msg.data.c_str(), msg.data.length());

    return msg;
}

/*
This is the serialize function of the scan response.
input:
scanResp - struct of the scan results.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(ScanResponse scanResp) {
    json j;
    j["status"] = scanResp.status;
    j["hashesResults"] = scanResp.hashesResults;
    j["idtResults"] = scanResp.idtResults;
    j["openPortsResults"] = scanResp.openPortsResults;
    j["packetsResults"] = scanResp.packetsResults;
    j["sctResults"] = scanResp.sctResults;
    j["spofingResults"] = scanResp.spofingResults;
    j["yaraResults"] = scanResp.yaraResults;
    return JsonResponsePacketSerializer::serializeResponse(j, scanResp.status);
}

/*
This is the serialize function of the suspend process response.
input:
suspendedResp - struct of the results data.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(SuspendedListResponse suspendedResp) {
    json j;
    j["status"] = suspendedResp.status;
    j["procPids"] = suspendedResp.procPids;
    return JsonResponsePacketSerializer::serializeResponse(j, suspendedResp.status);
}

/*
This is the serialize function of the suspend process response.
input:
suspendedResp - struct of the results data.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(SuspiciousListResponse suspiciousResp) {
    json j;
    j["status"] = suspiciousResp.status;
    j["procNames"] = suspiciousResp.procNames;
    return JsonResponsePacketSerializer::serializeResponse(j, suspiciousResp.status);
}

/*
This is the serialize function of the free process response.
input:
freeResp - struct of the results data.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(FreeResponse freeResp) {
    json j = { {"status", freeResp.status}, {"isWorked", freeResp.isWorked}};
    return JsonResponsePacketSerializer::serializeResponse(j, freeResp.status);
}

/*
This is the serialize function of the block process response.
input:
freeResp - struct of the results data.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(BlockResponse blockResp) {
    json j = { {"status", blockResp.status}, {"isWorked", blockResp.isWorked} };
    return JsonResponsePacketSerializer::serializeResponse(j, blockResp.status);
}

/*
This is the serialize function of the kill process response.
input:
freeResp - struct of the results data.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(KillResponse killResp) {
    json j = { {"status", killResp.status}, {"isWorked", killResp.isWorked} };
    return JsonResponsePacketSerializer::serializeResponse(j, killResp.status);
}

/*
This is the serialize function of the error response.
input:
errResp - struct of the error data.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(ErrorResponse errResp) {
    json j = { {"message", errResp.message} };
    return JsonResponsePacketSerializer::serializeResponse(j, ERROR);
}

/*
This is the serialize function of the change scan time response.
input:
changeTimeResp - struct of the results data.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(ChangeScanTimeResponse changeTimeResp) {
    json j = { {"status", changeTimeResp.status}, {"isWorked", changeTimeResp.isWorked} };
    return JsonResponsePacketSerializer::serializeResponse(j, changeTimeResp.status);
}

/*
This is the serialize function of the block process response.
input:
blockedTbaleResp - struct of the results data.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(UpdateBlockedTableResponse blockedTbaleResp) {
    json j = { {"status", blockedTbaleResp.status}, {"isWorked", blockedTbaleResp.isWorked} };
    return JsonResponsePacketSerializer::serializeResponse(j, blockedTbaleResp.status);
}

/*
This is the serialize function of the update virus table response.
input:
blockedTbaleResp - struct of the results data.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(UpdateVirusTableResponse virusTableResp) {
    json j = { {"status", virusTableResp.status}, {"isWorked", virusTableResp.isWorked} };
    return JsonResponsePacketSerializer::serializeResponse(j, virusTableResp.status);    
}

/*
This is the serialize function of the get scan time response.
input:
blockedTbaleResp - struct of the results data.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(GetTimeResponse timeResp) {
    json j = {{"status", timeResp.status}, {"time", timeResp.time}};
    return JsonResponsePacketSerializer::serializeResponse(j, timeResp.status);
}