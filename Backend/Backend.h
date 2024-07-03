#pragma once
#define closesocket close
typedef int SOCKET;

#include "pch.h"
#include "Helper.h"
#include "RequestHandler.h"
#include "PeriodicScanHandler.h"
#include "SqliteHelper.h"

/*
This class have all the main managment functions 
of the backend & server & client requests process.
*/
class Backend {
public:
    Backend(SqliteHelper* db);
    ~Backend();
    void serve(int port);

private:
    void acceptClient();
    void clientHandler(SOCKET clientSocket);
    void handleServerRequest(SOCKET clientSocket, message clientRequest);

    SOCKET _BackendSocket;
    SOCKET _ServerSocket;

    PeriodicScanHandler* _periodicHandler;
    SqliteHelper* _db;
};