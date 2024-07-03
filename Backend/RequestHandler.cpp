#include "RequestHandler.h"
RequestHandler::RequestHandler(SqliteHelper *db) : _db(db) {}

/*
This function check if that request is valid by the req id.
input:
reqInfo - request details.
output: if that request is relevant.
*/
bool RequestHandler::isRequestRelevant(RequestInfo reqInfo) {
	switch (reqInfo.id) {
	case SCAN_REQUEST:
	case SUSPENDED_PROCESSES_LIST_REQUEST:
	case SUSPICIOUS_PROCESSES_LIST_REQUEST:
	case FREE_PROCESS_REQUEST:
	case BLOCK_PROCESS_REQUEST:
	case IP_SCAN_REQUEST:
	case HASH_SCAN_REQUEST:
	case SAVING_HASH_REQUEST:
	case SAVING_IP_REQUEST:
	case KILL_PROCESS_REQUEST:
	case PERIODIC_SCAN_TIME_REQUEST:
	case UPDATE_BLOCKED_TABLE_REQUEST:
	case UPDATE_VIRUS_LIST_REQUEST:
	case GET_CURRENT_SCAN_FREQUENCY_REQUEST:
		return true;

	default:
		return false;

	}
}

/*
This function get the request then handle it and return the result.
input:
reqInfo - request details.
output: the handle result.
*/
message RequestHandler::handleRequest(RequestInfo reqInfo) {
	message result;
	if (reqInfo.id == SCAN_REQUEST)
		result = scan(reqInfo);
	else if (reqInfo.id == SUSPENDED_PROCESSES_LIST_REQUEST)
		result = getSuspendedProcessesList(reqInfo);
	else if (reqInfo.id == SUSPICIOUS_PROCESSES_LIST_REQUEST)
		result = getSuspiciousProcessesList(reqInfo);
	else if (reqInfo.id == FREE_PROCESS_REQUEST)
		result = freeProcess(reqInfo);
	else if (reqInfo.id == BLOCK_PROCESS_REQUEST)
		result = blockProcess(reqInfo);
	else if(reqInfo.id == KILL_PROCESS_REQUEST)
		result = killProcess(reqInfo);
	else if(reqInfo.id == UPDATE_BLOCKED_TABLE_REQUEST)
		result = updateBlockedTable(reqInfo);
	else if(reqInfo.id == UPDATE_VIRUS_LIST_REQUEST)
		result = updateVirusTable(reqInfo);
	return result;
}

/*
This function doing the scan process by the client request,
scan types: file, folder, quick, full.
input:
reqInfo - request details.
output: the scan handle result.
*/
message RequestHandler::scan(RequestInfo reqInfo) {
	//get the project config folders
	config::parseConfigData();
	
	ScanRequest scanRequest = JsonRequestPacketDeserializer::deserializeScanRequest(reqInfo.buffer);
	ScanResponse scanResp;
	vector<string> path = {scanRequest.path};

	//check which scan the client ask for
	switch(scanRequest.scanType){
		case file_scan:
			scanResp.yaraResults = PeriodicScanHandler::yaraScan(path, false, false);
			scanResp.hashesResults = PeriodicScanHandler::hashScan(path, false, false);
			break;	
		case folder_scan:
			scanResp.yaraResults = PeriodicScanHandler::yaraScan(path, true, false);
			scanResp.hashesResults = PeriodicScanHandler::hashScan(path, true, false);
			break;
		case quick_scan:
			scanResp.yaraResults = PeriodicScanHandler::yaraScan(config::_quickPathsForScan, true, false);
			scanResp.hashesResults = PeriodicScanHandler::hashScan(config::_quickPathsForScan, true, false);
			scanResp.idtResults = PeriodicScanHandler::idtScan();
			scanResp.openPortsResults = PeriodicScanHandler::openPortsScan();
			scanResp.packetsResults = PeriodicScanHandler::_packetScanResult;
			scanResp.sctResults = PeriodicScanHandler::sctScan();
			scanResp.spofingResults = PeriodicScanHandler::_spofingScanResult;
			PeriodicScanHandler::_packetScanResult.clear();
			PeriodicScanHandler::_spofingScanResult.clear();
			break;
		case full_scan:
			scanResp.yaraResults = PeriodicScanHandler::yaraScan(config::_fullPathsForScan, true, true);
			scanResp.hashesResults = PeriodicScanHandler::hashScan(config::_fullPathsForScan, true, true);
			scanResp.idtResults = PeriodicScanHandler::idtScan();
			scanResp.openPortsResults = PeriodicScanHandler::openPortsScan();
			scanResp.packetsResults = PeriodicScanHandler::_packetScanResult;
			scanResp.sctResults = PeriodicScanHandler::sctScan();
			scanResp.spofingResults = PeriodicScanHandler::_spofingScanResult;
			PeriodicScanHandler::_packetScanResult.clear();
			PeriodicScanHandler::_spofingScanResult.clear();
			break;
	}
	
	scanResp.status = SCAN_RESPONSE;
	return JsonResponsePacketSerializer::serializeResponse(scanResp);
}

/*
This function return list of blocked processes list.
input:
reqInfo - request details.
output: msg of the blocked processes list.
*/
message RequestHandler::getSuspendedProcessesList(RequestInfo reqInfo) {
	SuspendedListResponse susResp;
	susResp.status = SUSPENDED_PROCESSES_LIST_REESPONSE;
	susResp.procPids = _db->getBlockedPorcesses();
	return JsonResponsePacketSerializer::serializeResponse(susResp);
}

/*
This function return list of suspicious processes list.
input:
reqInfo - request details.
output: msg of the suspicious processes list.
*/
message RequestHandler::getSuspiciousProcessesList(RequestInfo reqInfo) {
	SuspiciousListResponse susResp;
	susResp.status = SUSPICIOUS_PROCESSES_LIST_RESPONSE;
	susResp.procNames = _db->getViruses();
	return JsonResponsePacketSerializer::serializeResponse(susResp);
}

/*
This function free process from block.
input:
reqInfo - request details.
output: msg of the operation results.
*/
message RequestHandler::freeProcess(RequestInfo reqInfo) {
	FreeRequest freeRequest = JsonRequestPacketDeserializer::desrializeFreeRequest(reqInfo.buffer);

	//free the process
	FreeResponse freeResp { FREE_PROCESS_RESPONSE, communicateWithDriver(2, freeRequest.pid) };
	return JsonResponsePacketSerializer::serializeResponse(freeResp);
	
}

/*
This function block process.
input:
reqInfo - request details.
output: msg of the operation results.
*/
message RequestHandler::blockProcess(RequestInfo reqInfo) {
	BlockRequest blockRequest = JsonRequestPacketDeserializer::desirializeBlockRequest(reqInfo.buffer);

	//block the process
	BlockResponse blockResp { BLOCK_PROCESS_RESPONSE, communicateWithDriver(1, blockRequest.pid) };
	return JsonResponsePacketSerializer::serializeResponse(blockResp);
}

/*
This function kill process.
input:
reqInfo - request details.
output: msg of the operation results.
*/
message RequestHandler::killProcess(RequestInfo reqInfo) {
	KillRequest killReq = JsonRequestPacketDeserializer::desirializeKillRequest(reqInfo.buffer);

	//kill the process
	KillResponse killResp { KILL_PROCESS_RESPONSE, communicateWithDriver(0, killReq.pid) };
	return JsonResponsePacketSerializer::serializeResponse(killResp);
}

/*
This function update the block list process.
input:
reqInfo - request details.
output: msg of the operation results.
*/
message RequestHandler::updateBlockedTable(RequestInfo reqInfo) {
	UpdateBlockedTableRequest request = JsonRequestPacketDeserializer::deserializeUpdateBlockedTbaleRequest(reqInfo.buffer);
	UpdateBlockedTableResponse response;
	response.isWorked = _db->updateBlockedTable(request.action, request.pid, request.dateTime);
	response.status = UPDATE_BLOCKED_TABLE_RESPONSE;
	return JsonResponsePacketSerializer::serializeResponse(response);
}

/*
This function update the virus list process.
input:
reqInfo - request details.
output: msg of the operation results.
*/
message RequestHandler::updateVirusTable(RequestInfo reqInfo) {
	UpdateVirusTableRequest request = JsonRequestPacketDeserializer::deserializeUpdateVirusTableRequest(reqInfo.buffer);
	UpdateVirusTableResponse response;
	response.isWorked = _db->updateViruses(request.name, request.dateTime);
	response.status = UPDATE_VIRUS_TABLE_RESPONSE;
	return JsonResponsePacketSerializer::serializeResponse(response);
}

/*
This function communicate with the kernel module using ioctl for sending signals to processes.
input:
action - signal action for sending to process.
target_pid - process pid for sending the signal.
output: msg of the operation results.
*/
int RequestHandler::communicateWithDriver(int action, int target_pid) {
   int fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        //perror("Failed to open the device file");
    }

	int args[2] = {action, target_pid};
	int result = 0;
	//send the action and pid
	ioctl(fd, SPYBOT_IOC_SEND, args);
	//get the result from the driver(success - 1, fail - 0);
	ioctl(fd, SPYBOT_IOC_RECV, &result);
    close(fd);

	return result;
}
