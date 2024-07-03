#include "config.h"

//init the static fields
std::vector<std::string> config::_fullPathsForScan = {};
std::vector<std::string> config::_quickPathsForScan = {};

/*
This function parse the config file into the static params.
output: NULL.
*/
void config::parseConfigData() {
    //get the config data from the file
    std::ifstream inputFile("config.json");
    json data;
    inputFile >> data;

    //parse the data and set the static params
    config::_quickPathsForScan = data["quick"];
    config::_fullPathsForScan = data["full"];
    config::_fullPathsForScan.insert(config::_fullPathsForScan.end(), config::_quickPathsForScan.begin(), config::_quickPathsForScan.end());
}