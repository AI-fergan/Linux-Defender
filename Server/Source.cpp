#include "SqliteHelper.h"
#include "Server.h"
#include "pch.h"

int main() {
    //try to create the database and start serving clients
    try {
        SqliteHelper* db = new SqliteHelper(DB_NAME);
        Server myServer(db);

        myServer.serve(9000);
    }
    catch (std::exception& e) {
        std::cout << "Error occurred: " << e.what() << std::endl;
    }
    return 0;
}
