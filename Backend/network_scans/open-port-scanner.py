import psutil

LOG_FILE = "open-ports.log"

#The ports for scanning.
ports = { 21: "FTP",
	      22: "SSH",
	      23: "Telnet",
	      25: "SMTP"}

def is_open(port):
    """
    This function check if port is open
    """
    #loop over all the exists connections and ports
    for conn in psutil.net_connections(kind='inet'):
        if conn.laddr.port == port and conn.status == 'LISTEN':
            return True
    return False

if __name__ == "__main__":
    with open(LOG_FILE, "w") as file:
        #loop over the open ports and write into the log file
        for port, name in ports.items():
            file.write(str(port) + '-') 
            if is_open(port):
                file.write('open\n')
            else:
                file.write("close\n")