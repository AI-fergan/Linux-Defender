from scapy.all import sniff, TCP, IP, Ether
import time
import subprocess
import os
import sys
MAX_CONNS = 5  # For each connection
MAX_TIME = 6  # Time in seconds

Telnet = {23}
FTP = {21}
SSH = {22, 2222}
RDP = {3389}

LOCMAC = "00:00:00:00:00:00"

LOG_FILE = "spofing.log"

# Ports counter for each connection
Ports = {"Telnet": {}, "FTP": {}, "SSH": {}, "RDP": {}}

def get_mac_address():
    """
    This function get the mac address of the curr machaine.
    """
    #try to run the ifconfig command
    try:
        command = "ifconfig | grep -o -E '([0-9a-fA-F]{2}:){5}([0-9a-fA-F]{2})'"
        result = subprocess.run(command, shell=True, capture_output=True, text=True)

        #check if the result of the command is valid
        if result.returncode == 0:
            return result.stdout.strip().split('\n')
        else:
            print("Error:", result.stderr.strip())
    except Exception as e:
        print("Error:", e)


def packet_callback(packet):
    """
    This function filter packets and count the number of packets 
    in each port in each connection.
    """
    global LOCMAC, Ports
    proto = ""

    #check if the layers in the packes are valid
    if Ether in packet and packet[Ether].dst in LOCMAC and TCP in packet and IP in packet:
        dport = packet[TCP].dport
        if dport in Telnet:
            proto = "Telnet"
        elif dport in FTP:
            proto = "FTP"
        elif dport in SSH:
            proto = "SSH"
        elif dport in RDP:
            proto = "RDP"

        #count the number of the packets
        if proto and packet[IP].src in Ports[proto]:
            Ports[proto][packet[IP].src] += 1
        elif proto:
            Ports[proto][packet[IP].src] = 1


def main():
    global LOCMAC, Ports

    LOCMAC = get_mac_address()
    sniff_duration = MAX_TIME

    #start sniffing
    packets = sniff(prn=packet_callback, timeout=sniff_duration)

    suspicious_connections = []

    #loop over all the log data and fix it into the log format
    for port, data in Ports.items():
        for ip, conn in data.items():
            if conn >= MAX_CONNS:
                suspicious_connections.append(f"{ip}-{port}")

    #write the log data into the log file
    with open(LOG_FILE, "w") as file:
        if suspicious_connections:
            file.write("\n".join(suspicious_connections))
    

if __name__ == "__main__":
    main()
