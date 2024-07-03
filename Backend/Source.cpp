#include "Backend.h"
#include "SqliteHelper.h"
#include "pch.h"

int main()
{
    try {
        //open the Backend db for read / write
        SqliteHelper* db = new SqliteHelper(DB_NAME);

        //starts periodic scan
        std::thread autoScan(&PeriodicScanHandler::periodicScan);
        autoScan.detach();

        //starts serving clients in port 8876
        Backend myBackend(db);
        myBackend.serve(8876);
    }
    catch (std::exception& e) {
        std::cout << "Error occurred: " << e.what() << std::endl;
    }
    return 0;
}