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
    msg.msg = (char*)malloc(msg.length + LEN_SIZE + CODE_SIZE);

    //copy all the data into the msg struct.
    std::memcpy(msg.msg, (void*)&code, CODE_SIZE);
    std::memcpy(msg.msg + CODE_SIZE, &msg.length, LEN_SIZE);
    std::memcpy(msg.msg + CODE_SIZE + LEN_SIZE, msg.data.c_str(), msg.length);

    return msg;
}

/*
This is the serialize function of the ip scan response.
input:
ipScanResp - struct of the scan results.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(IpScanResponse ipScanResp) {
    json j;
    j["status"] = ipScanResp.status;
    j["res"] = ipScanResp.res;

    return JsonResponsePacketSerializer::serializeResponse(j, ipScanResp.status);
}

/*
This is the serialize function of the hash scan response.
input:
hashScanResp - struct of the scan results.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(HashScanResponse hashScanResp) {
    json j;
    j["status"] = hashScanResp.status;
    j["res"] = hashScanResp.res;

    return JsonResponsePacketSerializer::serializeResponse(j, hashScanResp.status);
}

/*
This is the serialize function of the save ip response.
input:
saveIpResp - struct of the ip save results.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(SaveIpResponse saveIpResp) {
    json j;
    j["status"] = saveIpResp.status;
    j["isWorked"] = saveIpResp.isWorked;

    return JsonResponsePacketSerializer::serializeResponse(j, saveIpResp.status);
}

/*
This is the serialize function of the save hash response.
input:
saveIpResp - struct of the hash save results.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(SaveHashResponse saveHashResp)
{
    json j;
    j["status"] = saveHashResp.status;
    j["isWorked"] = saveHashResp.isWorked;

    return JsonResponsePacketSerializer::serializeResponse(j, saveHashResp.status);
}

/*
This is the serialize function of the ERROR response.
input:
errResp - struct of the error msg results.
output: msg struct of the response.
*/
message JsonResponsePacketSerializer::serializeResponse(ErrorResponse errResp) {
    json j = { {"message", errResp.message} };
    return JsonResponsePacketSerializer::serializeResponse(j, ERROR);
}
