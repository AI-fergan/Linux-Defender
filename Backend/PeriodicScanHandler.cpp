#include "PeriodicScanHandler.h"

map<string, string> PeriodicScanHandler::_spofingScanResult;
vector<string> PeriodicScanHandler::_packetScanResult;
int PeriodicScanHandler::_time = 1; // Default value for time

/*
This function using for handle the update scan settings.
input:
reqInfo - struct of the data info.
output: msg with the results data.
*/
message PeriodicScanHandler::handleRequest(RequestInfo reqInfo) {
    message msg;
    char data[7] = "error\0";
    msg.msg = data;
    msg.length = 6;
    msg.code = 100;
    msg.data = data;

    //check what is the request
    if(reqInfo.id == PERIODIC_SCAN_TIME_REQUEST)
        return changeTime(reqInfo);
    else if(reqInfo.id == GET_CURRENT_SCAN_FREQUENCY_REQUEST)
        return getTime(reqInfo);
    else
        return msg;
}

/*
This function using for change the scan time settings.
input:
reqInfo - struct of the data info.
output: msg with the results data.
*/
message PeriodicScanHandler::changeTime(const RequestInfo& reqInfo) {
    ChangeScanTimeRequest request = JsonRequestPacketDeserializer::desirializeChangeScanTimeRequest(reqInfo.buffer);
    _time = request.time;

    ChangeScanTimeResponse resp;
    resp.isWorked = 1;
    resp.status = PERIODIC_SCAN_TIME_RESPONSE;
    
    return JsonResponsePacketSerializer::serializeResponse(resp);
}

/*
This function using for get the scan time.
input:
reqInfo - struct of the data info.
output: msg with the results data.
*/
message PeriodicScanHandler::getTime(const RequestInfo &reqInfo) {
    GetTimeResponse resp{GET_CURRENT_SCAN_FREQUENCY_RESPONSE, _time};
    return JsonResponsePacketSerializer::serializeResponse(resp);
}

/*
This function using for handle the periodic scans.
output: NULL.
*/
void PeriodicScanHandler::periodicScan() {
    vector<string> packetScanTMP;
    map<string, string> spofingScanTMP;
    
    //start every (_time) seconds the periodic network scans
    while (true) {        
        try {
            std::this_thread::sleep_for(std::chrono::seconds(_time));           

            //scan communicating with ip's in the network
            packetScanTMP = packetScan();
            for(string data : packetScanTMP)
                _packetScanResult.push_back(data);

            //scan port spoffing
            spofingScanTMP = spofingScan();
            _spofingScanResult.insert(spofingScanTMP.begin(), spofingScanTMP.end());

        } catch (const std::exception& e) {
            std::cout << "Error during periodic scan: " + string(e.what()) << std::endl;
        }
    }
}

/*
This function using for communicate with the server for the hash scan.
input:
paths - vector of all the paths with the files to scan.
isFolder - if the paths var contain path to single file or folders.
all - of the isFolder contain 'yes' this var says if to scan also the sub folders.
output: map of all the fike names / paths and if they are malicious.
*/
map<string, bool> PeriodicScanHandler::hashScan(vector<string> paths, bool isFolder, bool all) {
    string command;
    string name, isMalicious;

    map<string, bool> results;

    //start loop over all the paths
    for(string path : paths){
        command = "python3 ./scans/hash-scanner.py ";

        if(isFolder)
            command += "-d ";
        else
            command += "-f ";
        command += path;
        if(all)
            command += " -a";

        //scan the files with python communicator with server db
        system(command.c_str());

        std::ifstream inputFile("hash-scan.log");
        
        //get log data
        while (getline(inputFile, name, '@'), getline(inputFile, isMalicious, '\n')) { 

            //is malicious file?
            if(isMalicious == "1"){
                results[name] = true;
                Helper::updateVirusTables(name);
            }
            else{
                results[name] = false;
            }
        } 
        
        // Close the file 
        inputFile.close();
    }

    return results;
}

/*
This function using for scan patterns in files with yara rules.
input:
paths - vector of all the paths with the files to scan.
isFolder - if the paths var contain path to single file or folders.
all - of the isFolder contain 'yes' this var says if to scan also the sub folders.
output: map of all the fike names / paths and the rules that found.
*/
map<string, vector<string>> PeriodicScanHandler::yaraScan(vector<string> paths, bool isFolder, bool all) {
    string command;
    string name, data, rule;
    map<string, vector<string>> results;    

    //loop over all the paths
    for(string path : paths){
        command = "python3 ./scanner.py ";

        if(isFolder)
            command += "-d ";
        else
            command += "-f ";
        command += path;
        if(all)
            command += " -a";
        try{
            //scan files with yara-python scanner
            system(command.c_str());
        }
        catch(...) {

        }
        std::ifstream inputFile("yara.log");

        //get log data
        while (getline(inputFile, name, '@'), getline(inputFile, data, '\n')) { 
            cout << name << data << endl;
            vector<string> rules;

            //rules found in that file?
            if (data.length() == 0){
                results[name] = rules;
                continue;
            }
            std::stringstream ss(data);
            while (std::getline(ss, rule, '-')) {
                rules.push_back(rule);
            }    
            
            results[name] = rules;
        } 
        
        // Close the file 
        inputFile.close();
    }

    return results;
}

/*
This function using for scan the network communication with ip's.
output: vector of all the data about the malicious processes.
*/
vector<string> PeriodicScanHandler::packetScan() {
    // Run the Python script
    system("python3 ./network_scans/net-scanner.py");
    
    // Open the log file
    std::ifstream inputFile("malicious.log");
    std::string output;
    vector<string> results;

    // Define variables to hold the data
    std::string name, pid, active, port, ip;

    // Read the data from the file
    while (getline(inputFile, name, '-') && 
           getline(inputFile, pid, '-') && 
           getline(inputFile, active, '-') && 
           getline(inputFile, port, '-') && 
           getline(inputFile, ip, '\n')) {
        // Append the current line to the stringstream
        output += "process name: " + name + "\npid: " + pid + "\nis active: " + active + "\nport: " + port + "\nip: " + ip + "\nis blocked: ";
        if (PeriodicScanHandler::blockPid(std::stoi(pid))) {
            output += "yes";
        } else {
            output += "fail";
        }

        results.push_back(output);
    }

    // Close the file
    inputFile.close();

    // Return the collected output as a string
    return results;
}

/*
This function block given process by pid.
input:
pid - process id.
output: if the operation success.
*/
int PeriodicScanHandler::blockPid(int pid){
    int fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
    }

	int args[2] = {kill_proc, pid};
	int result = 0;
	//send the action and pid
	ioctl(fd, SPYBOT_IOC_SEND, args);
	//get the result from the driver(success - 1, fail - 0);
	ioctl(fd, SPYBOT_IOC_RECV, &result);
    close(fd);

    return result;
}

/*
This function scan the sct table for searching syscall hooking.
output: number of the syscall entry that hooked and if not found (-1).
*/
int PeriodicScanHandler::sctScan(){
    //open the device file for ioctl communication
    int fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
    }

	int args[2] = {sct_scan, 0};
	int result = 0;
	//send the action and pid
	ioctl(fd, SPYBOT_IOC_SEND, args);
	//get the result from the driver(success - 1, fail - 0);
	ioctl(fd, SPYBOT_IOC_RECV, &result);

    close(fd);

    return result;
}

/*
This function scan the idt table for searching interrupt hooking.
output: number of the interrupt entry that hooked and if not found (-1).
*/
int PeriodicScanHandler::idtScan(){
    //open the device file for ioctl communication
    int fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
    }

	int args[2] = {idt_scan, 0};
	int result = 0;
	//send the action and pid
	ioctl(fd, SPYBOT_IOC_SEND, args);
	//get the result from the driver(success - 1, fail - 0);
	ioctl(fd, SPYBOT_IOC_RECV, &result);
    close(fd);

    return result;
}

/*
This function scan and search for some given ports if they are open or not.
output: map of the ports number and if they are open or not.
*/
map<string, bool> PeriodicScanHandler::openPortsScan() {
    //start the python scan with psutil
    system("python3 ./network_scans/open-port-scanner.py");
    
    std::ifstream inputFile("open-ports.log");
    
    string port, isOpen;
    string message = "";
    map<string, bool> results;

    //loop over the log data
    while (getline(inputFile, port, '-'), getline(inputFile, isOpen, '\n')) { 
        //is open port?
        if(isOpen == "open"){
            results[port] = true;
        }
        else {
            results[port] = false;
        }

        message += "port: ";
        message += port;
        message += " is ";
        message += isOpen;
        message += "\n";
    }

    return results;
}

/*
This function scan the network communication and check if any ip try to spofing our network.
output: map with strings of the ip's and the ports that they are spofing.
*/
map<string, string> PeriodicScanHandler::spofingScan(){
    //run the python scapy network scanner
    system("python3 ./network_scans/spofing-scanner.py");
    
    std::ifstream inputFile("spofing.log");
    
    string ip, port;
    string message = "";
    map<string, string> results;

    //loop over the log data
    while (getline(inputFile, ip, '-'), getline(inputFile, port, '\n')) { 
        results[ip] = port;
        message += "ip: ";
        message += ip;
        message += " - port: ";
        message += port;
        message += "\n";
    }

    return results;
}
