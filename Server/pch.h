#pragma once

#include <cstdint>
#include <string.h>
#include <sstream>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <ctime>   
#include <iostream> 
#include <fstream> 
#include <exception>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdexcept>
#include <cstring>
#include <bitset>
#include <regex>
#include <sys/socket.h>
#include <list>
#include <string>
#include <algorithm>
#include <sys/ioctl.h>

//client requests
#define SCAN_REQUEST 100 
#define SUSPICIOUS_PROCESSES_LIST_REQUEST 101 
#define SUSPENDED_PROCESSES_LIST_REQUEST 102 
#define UPDATE_BLOCKED_TABLE_REQUEST 103
#define UPDATE_VIRUS_LIST_REQUEST 104
#define FREE_PROCESS_REQUEST 105
#define BLOCK_PROCESS_REQUEST 106 
#define KILL_PROCESS_REQUEST 107

#define PERIODIC_SCAN_TIME_REQUEST 108
#define PERIODIC_SCAN_FOLDER_REQUEST 109
#define GET_CURRENT_SCAN_FREQUENCY_REQUEST 110
#define GET_CURRENT_SCAN_FOLDER_REQUEST 111

//client request to server
#define IP_SCAN_REQUEST 112
#define HASH_SCAN_REQUEST 113 
#define SAVING_IP_REQUEST 114
#define SAVING_HASH_REQUEST 115 

//backend response
#define SCAN_RESPONSE 200 
#define SUSPICIOUS_PROCESSES_LIST_RESPONSE 201 
#define SUSPENDED_PROCESSES_LIST_REESPONSE 202 
#define FREE_PROCESS_RESPONSE 205 
#define BLOCK_PROCESS_RESPONSE 206 
#define KILL_PROCESS_RESPONSE 207
#define PERIODIC_SCAN_TIME_RESPONSE 208
#define PERIODIC_SCAN_FOLDER_RESPONSE 209
#define UPDATE_BLOCKED_TABLE_RESPONSE 203
#define UPDATE_VIRUS_TABLE_RESPONSE 204
#define GET_CURRENT_SCAN_FREQUENCY_RESPONSE 210
#define GET_CURRENT_SCAN_FOLDER_RESPONSE  211

//server response
#define IP_SCAN_RESPONSE 212
#define HASH_SCAN_RESPONSE 213 
#define SAVING_IP_RESPONSE 214 
#define SAVING_HASH_RESPONSE 215 

#define ERROR 1

#define ERROR 1

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::thread;
