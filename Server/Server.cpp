#include "Server.h"

/*
This c'tor build the server main object and the server socket.
input:
db - pointer to the server db object.
*/
Server::Server(SqliteHelper* db) {
    this->_db = db;
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    //check if the socket created successfully.
    if (_serverSocket == -1)
        throw std::runtime_error("Server::Server() - socket");

    //start the db updater task script
    std::thread updater(system, "python3 ./Updater.py");
    updater.detach();
}

/*
This d'tor close the server socket.
*/
Server::~Server() {
    try {
        close(_serverSocket);
    }
    catch(...) {}
}

/*
This function start serve clients with given port.
inpit:
port - port to serve with.
output: NULL.
*/
void Server::serve(int port) {
    struct sockaddr_in sa = { 0 };

    //init socket settings
    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;

    //binding the port
    if (bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == -1)
        throw std::runtime_error("Server::serve() - bind");

    //start listen on that port
    if (listen(_serverSocket, SOMAXCONN) == -1)
        throw std::runtime_error("Server::serve() - listen");

    std::cout << "Listening on port " << port << std::endl;

    //start clients accepting loop
    while (true) {
        std::cout << "Waiting for client connection request" << std::endl;
        acceptBackend();
    }
}

/*
This function using for accept backend connection.
output: NULL.
*/
void Server::acceptBackend() {
    //serve the backend requestes.
    while(true) {
        //accept backend and start socket
        int backend_socket = accept(_serverSocket, nullptr, nullptr);

        //check if the socket is ok
        if (backend_socket == -1)
            throw std::runtime_error(__FUNCTION__);

        //start thread for serve the backend
        std::cout << "Client accepted" << std::endl;
        std::thread backend(&Server::backendHandler, this, backend_socket);
        backend.detach();
    }
}

/*
This function handle all the backend requests.
inpit:
backendSocket - socket to communicate with the backend.
output: NULL.
*/
void Server::backendHandler(int backendSocket) {
    try {
        //loop to get all the backend requests
        while (true) {
            //get backend request
            message packet = Helper::getAllTheSocket(backendSocket);
            cout << packet.data << endl;
            RequestInfo info(packet);

            //create request handler
            RequestHandler* requestHandler = new RequestHandler(this->_db);

            //check if the request of the backend is relevant
            if (!requestHandler->isRequestRelevant(info)) {
                ErrorResponse errResponse{ "Request is not relevant." };
                Helper::sendData(backendSocket, JsonResponsePacketSerializer::serializeResponse(errResponse));
            }
            else {
                message result = requestHandler->handleRequest(info);
                std::cout << "Data sent: " << result.data << std::endl;
                Helper::sendData(backendSocket, result);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        close(backendSocket);
    }
}