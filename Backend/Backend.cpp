#include "Backend.h"

/*
This c'tor create the backend socket with the client & server, 
and init the db field.
input:
db - pointer to the db.
*/
Backend::Backend(SqliteHelper* db) : _db(db) {
    //create the client & server socket
    _BackendSocket = socket(AF_INET, SOCK_STREAM, 0);
    _ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    //check if the socket is valid
    if (_BackendSocket == -1 || _ServerSocket == -1)
        throw std::runtime_error("Backend::Backend() - socket");

    int enable = 1;
    if (setsockopt(_BackendSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        throw std::runtime_error("Backend::Backend() - setsockopt(SO_REUSEADDR) failed");

}

/*
This d'tor clode the client and server sockets.
*/
Backend::~Backend() {
    try {
        close(_BackendSocket);
        close(_ServerSocket);
    }
    catch (...) {}
}

/*
This function start serving clients in given port.
input:
port - port to serving with.
output: NULL.
*/
void Backend::serve(int port) {
    struct sockaddr_in sa = { 0 };

    //set the socket settings
    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;

    //bind the port
    if (bind(_BackendSocket, (struct sockaddr*)&sa, sizeof(sa)) == -1)
        throw std::runtime_error("Backend::serve() - bind");

    //start listening with the port
    if (listen(_BackendSocket, SOMAXCONN) == -1)
        throw std::runtime_error("Backend::serve() - listen");

    std::cout << "Listening on port " << port << std::endl;

    //loop to accepting clients
    while (true) {
        try {
            std::cout << "Waiting for client connection request" << std::endl;
            acceptClient();
        }
        catch(...) {
            
        }
    }
}

/*
This function accept new clients and start serving them.
output: NULL.
*/
void Backend::acceptClient() {
    //accept client and create socket
    int client_socket = accept(_BackendSocket, NULL, NULL);

    //check if the socket is valid
    if (client_socket == -1)
        throw std::runtime_error("Backend::acceptClient() - accept");

    std::cout << "Client accepted" << std::endl;

    //create thread of client handler
    std::thread client(&Backend::clientHandler, this, client_socket);
    client.detach();
}

/*
This function handle all the client requests.
input:
clientSocket - socket to communicate with the client.
output: NULL.
*/
void Backend::clientHandler(int clientSocket) {
    try {
        //loop over all the clien requests
        while (true) {
            //get client request into msg struct
            message packet = Helper::getAllTheSocket(clientSocket);
            RequestInfo info(packet);

            //create req handler for the request
            RequestHandler* requestHandler = new RequestHandler(_db);

            //check if the request is relevant
            if (!requestHandler->isRequestRelevant(info)) {
                ErrorResponse errResponse{ "Request is not relevant." };
                Helper::sendData(clientSocket, JsonResponsePacketSerializer::serializeResponse(errResponse));
            }
            else {
                //client - backend requests 
                if (info.id >= 100 && info.id <= 107) {
                    message result = requestHandler->handleRequest(info);
                    std::cout << "Data sent: " << result.msg << std::endl;
                    Helper::sendData(clientSocket, result);
                }
                else if(info.id >= 108 && info.id <= 111) {
                    message result = PeriodicScanHandler::handleRequest(info);
                    cout << "Data sent: " << result.msg << endl;
                    Helper::sendData(clientSocket, result);
                }
                else //client - server requests
                    handleServerRequest(clientSocket, packet);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Client existed: " << std::endl;
        close(clientSocket);
    }
}

/*
This function handle the server requests.
input:
clientSocket - socket to communicate with the server.
clientRequest - request of the server.
output: NULL.
*/
void Backend::handleServerRequest(int clientSocket, message clientRequest) {
    //create the socket to communicate with the server
    _ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sa = { 0 };

    sa.sin_port = htons(9000);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(_ServerSocket, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        std::cerr << "Error connecting to the server" << std::endl;
        close(_ServerSocket);
        return;
    }

    //communicate between the client and the server (not the backend)
    try {
        Helper::sendData(_ServerSocket, clientRequest);
        message serverResponse = Helper::getAllTheSocket(_ServerSocket);
        Helper::sendData(clientSocket, serverResponse);
    }
    catch (const std::exception& e) {
        std::cerr << "Error handling server request: " << e.what() << std::endl;
    }
    close(_ServerSocket);
}