#pragma once
#include "sqlite3.h"
#include "pch.h"

#define IPS_TABLE "BlackListIPs"
#define HASHES_TABLE "BlackListHashes"

/*
This class manage all the db tasks.
*/
class SqliteHelper {
public:
    SqliteHelper(const char* db);
    ~SqliteHelper();
    bool IPisMalicious(string ip);
    bool HASHisMalicious(string hash);
    bool addIP(string IPAddress, string DateAdded, string Source);
    bool addHASH(string Name, string Path, string SHA256, string DateAdded, string Source);
    
private:
    sqlite3* _db;
};
