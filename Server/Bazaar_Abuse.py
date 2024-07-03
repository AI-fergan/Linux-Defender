from requests import get
from sqlite3 import connect
from datetime import date
import schedule
from time import sleep

IPS_TABLE = "BlackListHashes"
INSERT_QUERY = "INSERT INTO " + IPS_TABLE + " (SHA256, DateAdded) VALUES ('%s', '%s');"
SELECT_QUERY = "SELECT * FROM " + IPS_TABLE + " WHERE SHA256='%s';"
SOURCE = "https://bazaar.abuse.ch/export/txt/sha256/recent/"
DB_NAME = "ServerData.db"
UPDATE_TIME = "00:00"

def getHash():
    """
    This function return the single hash evrey call from the hashes SOURCR.
    """
    hashesList = get(SOURCE).text.split('\n')
    #loop over all the hashes
    for hash in hashesList:
        #check if the curr line is just comment
        if not hash.startswith("#"):
            print(hash)
            yield hash


def isExists(hash, crsr):
    """
    This function get hash and the db crsr,
    return if the hash is already exists in the db.
    """
    crsr.execute(SELECT_QUERY % hash)
    ans = crsr.fetchall()

    return len(ans) != 0

def BazzarTask():
    """
    This is the main task of all the server hashes proccess downloading.
    """
    connection = connect(DB_NAME)
    crsr = connection.cursor()
    DateAdded = date.today()

    #get hashes for the db
    for hash in getHash():
        #check if the hash is already exists
        if not isExists(hash, crsr):
            crsr.executescript(INSERT_QUERY % (hash, DateAdded))

    connection.close()

"""
hash = "0c8049d7fd83e24d08e94f5c92f9e42694341a472564d78d521637936b15aa54"
connection = connect(DB_NAME)
crsr = connection.cursor()
DateAdded = date.today()
crsr.executescript(INSERT_QUERY % (hash, DateAdded))
connection.close()
"""