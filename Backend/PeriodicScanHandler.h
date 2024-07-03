#pragma once
#include "RequestHandler.h"
#include "PeriodicScanHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "Helper.h"
#include "pch.h"

enum spybot_ioctl{
    kill_proc,
    stop_proc,
    cont_proc,
    sct_scan,
    sct_fix,
    idt_scan
};

class RequestHandler;
class config;

/*
This class manage the periodic scans.
*/
class PeriodicScanHandler {
public:
    static message handleRequest(RequestInfo reqInfo);
    static void periodicScan();

    static map<string, string> _spofingScanResult;
    static vector<string> _packetScanResult;
    static int _time;

    static message changeTime(const RequestInfo& reqInfo);

    static message getTime(const RequestInfo& reqInfo);

    static map<string, bool> hashScan(vector<string> path, bool isFolder, bool all);
    static map<string, vector<string>> yaraScan(vector<string> path, bool isFolder, bool all);
    static vector<string> packetScan(); //scans for packets that speak with malicous ips
    static map<string, string>  spofingScan(); //scans if a computer on the same netwrok sends many conenction requests in a short time
    static map<string, bool> openPortsScan(); //scans for open ports on the computer
    static int sctScan(); //scan hooks in the sct 
    static int idtScan(); //scan hooks in the idt

    static int blockPid(int pid); // block pid    
};
