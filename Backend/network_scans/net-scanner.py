import struct
import subprocess
from scapy.all import sniff, TCP, IP, UDP, ICMP
import psutil
import socket
import os
import sys
LOG_FILE = "malicious.log"
LOG_DATA = {}
PORT_DATA = {}
def filter(packet):
    """
    This function filter only TCP & UDP packets with IP layer.
    """
    return TCP in packet or UDP in packet or ICMP in packet or packet.haslayer(IP)

def print_packet(packet):
    """
    This function uses to manage between the packets
    """
    global LOG_DATA
    #check if the packet has ip layer
    if IP in packet:
        src, dst = packet[IP].src, packet[IP].dst
        #check if the dst ip of the packet is malicious
        if is_malicious_ip(dst):
            try:
                #check if the packet transfer layer is TCP / UDP
                if TCP in packet:
                    LOG_DATA[dst][packet[TCP].dport].append("active")
                elif UDP in packet:
                    LOG_DATA[dst][packet[UDP].dport].append("active")
            except:
                pass

def connect_to_server():
    """
    This function create connection with the server
    """
    conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serv_addr = ('127.0.0.1', 9000)
    conn.connect(serv_addr)
    
    return conn

def sendPacket(conn, ip = "127.0.0.1"):
    """
    This function send data to the server.
    """
    msg = '{"ip":"' + ip + '"}'
    data = chr(112).encode() + struct.pack('<I', len(msg)) + msg.encode()
    conn.sendall(data)

def recvPacket(conn):
    """
    This function recv data from the server
    """
    packet = {}
    packet["code"] = conn.recv(1)
    packet["length"] = struct.unpack('<I', conn.recv(4))[0]
    packet["data"] = eval(conn.recv(packet["length"]).decode())

    return packet

def is_malicious_ip(ip):
    """
    This function check if ip is malicious by communicate with the server.
    """
    conn = connect_to_server()
    sendPacket(conn, ip)

    return recvPacket(conn)["data"]["res"]

def identify_process():
    """
    This function get the pid of the processes.
    """
    global LOG_DATA

    result = subprocess.run('sudo ss -tnup', shell=True, stdout=subprocess.PIPE, text=True)
    
    lines = result.stdout.splitlines()
    ip = ""
    ans = []

    #loop over all the processes details
    for line in lines:
        fields = line.split()

        #try to get the procces pid
        try:
            data = fields[6]
            ip = fields[5].split(":")[0]
            port = fields[5].split(":")[1]
        except:
            continue
        
        #check if the process is malicious
        if data and is_malicious_ip(ip):
            cleaned_str = data[len('users:(('):-2]
            parts = cleaned_str.split(',')
            process_name = parts[0].strip('"')
            pid = parts[1].split('=')[1]

            #add the process if it isn't already exists in the log data
            if ip not in LOG_DATA.keys():
                LOG_DATA[ip] = {}
            
            LOG_DATA[ip][port] = [process_name, pid]

    return ans

def main():
    identify_process()
    
    #start sniffing packets
    sniff(lfilter=filter, prn=print_packet, timeout=5)
    with open(LOG_FILE, 'w') as log:
        if not bool(LOG_DATA):
            log.close()
            return
        
        #saving log data in the log file
        for ip in LOG_DATA:
            #loop over all the log data
            for port in LOG_DATA[ip]:
                proc = LOG_DATA[ip][port]
                if len(proc) != 3:
                    proc.append("not active")
                proc.append(port)
                proc.append(ip)
                log.write('-'.join(proc) + "\n")
                        
if __name__ == "__main__":
    main()

