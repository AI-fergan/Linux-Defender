#pragma once
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include "RequestHandler.h"
#include "Helper.h"
#include "pch.h"
#include "SqliteHelper.h"

#define DB_NAME "ServerData.db"

/*
This class have the server manage functions.
*/

class Server
{
public:
    Server(SqliteHelper* db);
    ~Server();

    void serve(int port);

private:
    void acceptBackend();
    void backendHandler(int backendSocket);  

    SqliteHelper* _db;
    int _serverSocket;  
};

