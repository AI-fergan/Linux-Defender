import schedule

from Blocklist import blocklistTask
from Bazaar_Abuse import BazzarTask
from time import sleep

UPDATE_TIME = "00:00"

def Task():
    """
    This function is the main task of db tables updating.
    """
    blocklistTask()
    BazzarTask()

def main():
    schedule.every().day.at(UPDATE_TIME).do(Task)

    while True:
        #update the db every day in 00:00
        schedule.run_pending()
        sleep(1)

if __name__ == "__main__":
    main()