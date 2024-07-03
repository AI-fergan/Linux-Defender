#include "Helper.h"

/*
This function get part of data from socket and return it.
input:
sc - the socket to get the data from.
byteNum - the number of the bytes that the function need to return from the socket.
output: part of data from the socket.
*/
char* Helper::getPartFromSocket(int sc, int bytesNum) {
	//check if the part of the socket should be empty
    if (bytesNum == 0)
        return (char*)"";

    char* data = new char[bytesNum];

	//recv the data
    ssize_t res = recv(sc, data, bytesNum, NO_FLAGS);

	//check if the data recv successfully
    if (res == -1) {
        std::string s = "Error while receiving from socket: ";
        s += std::to_string(sc);
        throw std::runtime_error(s.c_str());
    }

    return data;
}

/*
This function send given data via the socket.
input:
sc - socket for sending data.
msg - data structure of the data.
output: NULL.
*/
void Helper::sendData(int sc, message msg) {
	//check if the sending ended successfully
    if (send(sc, msg.msg, msg.length + CODE_SIZE + LEN_SIZE, NO_FLAGS) == -1)
        throw std::runtime_error("Error while sending message to client");
}

/*
This function send given data via the socket.
input:
sc - socket for sending data.
message - string data to send.
output: NULL.
*/
void Helper::sendData(int sc, std::string message) {
    const char* data = message.c_str();
    
    //send the data
    if (send(sc, data, message.size(), NO_FLAGS) == -1)
    {
        throw std::runtime_error("Error while sending message to client");
    }
}

/*
This function return all the data from socket by the protocol:
1-bytes of msg code,
4-bytes of data length,
{length}-bytes of the msg data.
input:
sc - socket for recving data.
output: msg data structure of the msg details and data.
*/
message Helper::getAllTheSocket(int sc) {
    message msg;
    msg.code = 0;
	//get the msg code
    std::memcpy(&msg.code, Helper::getPartFromSocket(sc, CODE_SIZE), CODE_SIZE);
	//get the msg length
    std::memcpy(&msg.length, Helper::getPartFromSocket(sc, LEN_SIZE), LEN_SIZE);

	//get the msg data by the msg length
    std::vector<char> data(msg.length);
    std::memcpy(data.data(), Helper::getPartFromSocket(sc, msg.length), msg.length);
    msg.data.assign(data.begin(), data.end());

    msg.msg = new char[msg.length + LEN_SIZE + CODE_SIZE];
    
	//init the msg data struct with the msg data
    std::memcpy(msg.msg, &msg.code, CODE_SIZE);
    std::memcpy(msg.msg + CODE_SIZE, &msg.length, LEN_SIZE);
    std::memcpy(msg.msg + CODE_SIZE + LEN_SIZE, msg.data.c_str(), msg.data.length());

    return msg;
}

/*
This function update the viruses table with new virus name.
input:
name - the name of the virus.
output: if the saving process is ok.
*/
bool Helper::updateVirusTables(std::string name) {
    sqlite3* db;

    //get the curr date time
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);
    char buffer[20];
    
    //set the date time format
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %I:%M %p", timeinfo);

    //save the virus in the db using the sql query
    std::string dateTime(buffer);
    std::string query = "INSERT INTO " + VIRUSES_TABLE + "(Name, DateAdded) VALUES('" + name  + "', '" + dateTime + "');";
	char** errMessage = nullptr;
    sqlite3_open(DB_NAME, &db);
    
    return sqlite3_exec(db, query.c_str(), nullptr, nullptr, errMessage) == SQLITE_OK;
}