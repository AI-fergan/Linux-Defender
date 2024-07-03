#include "pch.h"

/*
This class help with the config data.
*/
class config{
public:
    static std::vector<std::string> _fullPathsForScan;
    static std::vector<std::string> _quickPathsForScan;
    static void parseConfigData();
};
