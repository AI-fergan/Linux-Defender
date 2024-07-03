#pragma once
#include "sqlite3.h"
#include "pch.h"

/*
This class manage all the db tasks.
*/
class SqliteHelper {
public:
    SqliteHelper(const char* db);
    ~SqliteHelper();

    vector<string> getBlockedPorcesses();
    vector<string> getViruses();

    bool updateBlockedTable(string action, int pid, string dateTime); //action - remove, add
    bool updateViruses(string name, string dateTime);

private:
    sqlite3* _db;
};

