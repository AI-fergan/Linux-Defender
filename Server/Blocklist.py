from requests import get
from sqlite3 import connect
from datetime import date
import schedule
from time import sleep

IPS_TABLE = "BlackListIPs"
INSERT_QUERY = "INSERT INTO " + IPS_TABLE + " (IPAddress, DateAdded) VALUES ('%s', '%s');"
SELECT_QUERY = "SELECT * FROM " + IPS_TABLE + " WHERE IPAddress='%s';"
SOURCE = "https://lists.blocklist.de/lists/all.txt"
DB_NAME = "ServerData.db"
UPDATE_TIME = "00:00"


def getIP():
    """
    This function return single IP from the ip's SOURCE
    """

    IPlist = get(SOURCE).text.split('\n')
    for ip in IPlist:
        yield ip


def isExists(ip, crsr):
    """
    This function get ip and the db crsr,
    return if the ip is already exusts in the db.
    """

    crsr.execute(SELECT_QUERY % ip)
    ans = crsr.fetchall()
    return len(ans) != 0


def blocklistTask():
    """
    This is the main task if the ip downloading of the server.
    """
    connection = connect(DB_NAME)
    crsr = connection.cursor()
    DateAdded = date.today()

    #get ip's for the server db
    for ip in getIP():
        #check if the ip is already exists
        if not isExists(ip, crsr):
            crsr.executescript(INSERT_QUERY % (ip, DateAdded))

    connection.close()
