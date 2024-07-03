#include "SqliteHelper.h"

/*
This c'tor create the ip & hash tables in the db.
input:
db - pointer to the database
*/
SqliteHelper::SqliteHelper(const char* db) {
	string query_1 = "", query_2 = "";

	//open the database
	if (sqlite3_open(db, &_db) != SQLITE_OK) {
		_db = nullptr;
		std::cout << "Failed to open DB\n";
		exit(1);
	}
	// Iniate Relevant SQL Tables
	char** errMessage = nullptr;

	//creater the ip & hash tables in the db
	query_1 = query_1 + "CREATE TABLE IF NOT EXISTS " + IPS_TABLE + " (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, IPAddress TEXT NOT NULL, DateAdded TEXT NOT NULL, Source TEXT DEFAULT 'Blocklist.de' NOT NULL);";
	query_2 = query_2 + "CREATE TABLE IF NOT EXISTS " + HASHES_TABLE + " (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, Name TEXT, Path TEXT, SHA256 TEXT NOT NULL, DateAdded TEXT NOT NULL, Source TEXT DEFAULT 'bazaar.abuse.ch' NOT NULL);";

	sqlite3_exec(_db, query_1.c_str(), nullptr, nullptr, errMessage);
	sqlite3_exec(_db, query_2.c_str(), nullptr, nullptr, errMessage);
}

/*
This d'tor close the db and erase it.
*/
SqliteHelper::~SqliteHelper() {
	sqlite3_close(_db);
	_db = nullptr;
}

/*
This function check if given ip is in the malicious ip's db.
input:
ip - ip for check.
output: if the ip is in the malicious database.
*/
bool SqliteHelper::IPisMalicious(string ip) {
	string query = "";

	//query to check if the ip is in the db
	query = query + "SELECT COUNT(*) FROM " + IPS_TABLE + " WHERE IPAddress = '" + ip + "'; ";

	char** errMessage = nullptr;
	int ans;

	//exec the query
	sqlite3_exec(_db, query.c_str(), [](void* data, int argc, char** argv, char** azColName) {
		if (argc != 0) {
			int* existRes = static_cast<int*>(data);
			*existRes = atoi(argv[0]);
		}
		return 0;
		}, &ans, errMessage);

	return ans >= 1;
}

/*
This function check if given hash is in the malicious hashes db.
input:
hash - hash for check.
output: if the hash is in the malicious database.
*/
bool SqliteHelper::HASHisMalicious(string hash) {
	string query = "";

	//query to check if the hash is in the db
	query = query + "SELECT COUNT(*) FROM " + HASHES_TABLE + " WHERE SHA256 = '" + hash + "'; ";

	char** errMessage = nullptr;
	int ans;

	//exec the query
	sqlite3_exec(_db, query.c_str(), [](void* data, int argc, char** argv, char** azColName) {
		if (argc != 0) {
			int* existRes = static_cast<int*>(data);
			*existRes = atoi(argv[0]);
		}
		return 0;
		}, &ans, errMessage);

	return ans >= 1;
}

/*
This function add new ip into the db.
input:
ip - ip to add.
DateAdded - the added date of the ip.
Source - who add that ip.
output: if the query exec successfully
*/
bool SqliteHelper::addIP(string IPAddress, string DateAdded, string Source) {
	string query = "";
	query = "INSERT INTO " + string(IPS_TABLE) + "(IPAddress, DateAdded, Source) VALUES('" + IPAddress + "', '" + DateAdded + "', '" + Source + "');";
	char** errMessage = nullptr;
	return sqlite3_exec(_db, query.c_str(), nullptr, nullptr, errMessage) == SQLITE_OK;
}

/*
This function add new hash into the db.
input:
FileName - the file name.
FilePath - path to that file.
FileHash_SHA256 - hash of the file.
DateAdded - the added date of the hash.
SourceDetection - who add that ip.
output: if the query exec successfully
*/
bool SqliteHelper::addHASH(string Name, string Path, string SHA256, string DateAdded, string Source) {
	//insert into db query
	std::string query = std::string("INSERT INTO ") + HASHES_TABLE + std::string("(Name, Path, SHA256, DateAdded, Source) VALUES(?, ?, ?, ?, ?);");
    sqlite3_stmt* stmt = nullptr;
    
	//check if the query exec successfully
    if (sqlite3_prepare_v2(_db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, Name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, Path.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, SHA256.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, DateAdded.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, Source.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return true;
        }
    }
    
    if (stmt) {
        sqlite3_finalize(stmt);
    }
    
    return false;
}
