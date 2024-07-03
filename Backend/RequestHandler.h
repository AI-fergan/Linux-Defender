#pragma once
#include "pch.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "SqliteHelper.h"
#include "Helper.h"
#include "PeriodicScanHandler.h"
//needed to communicate with driver
#include "config.h"

#define SPYBOT_IOC_MAGIC 'k'
#define SPYBOT_IOC_SIGNAL _IOWR(SPYBOT_IOC_MAGIC, 1, int)

class PeriodicScanHandler;

enum scanTypes{
    file_scan,
    folder_scan,
    quick_scan,
    full_scan
};


/*
This class handle all the backend scans operations.
*/
class RequestHandler {
public:
	RequestHandler(SqliteHelper* db);
	virtual bool isRequestRelevant(RequestInfo reqInfo);
	virtual message handleRequest(RequestInfo reqInfo);
	int communicateWithDriver(int action, int target_pid);

private:
	SqliteHelper* _db;
	
	message scan(RequestInfo reqInfo);
	message getSuspiciousProcessesList(RequestInfo reqInfo);
	message getSuspendedProcessesList(RequestInfo reqInfo);
	message freeProcess(RequestInfo reqInfo);
	message blockProcess(RequestInfo reqInfo);
	message killProcess(RequestInfo reqInfo);
	message updateBlockedTable(RequestInfo reqInfo);
	message updateVirusTable(RequestInfo reqInfo);

};

