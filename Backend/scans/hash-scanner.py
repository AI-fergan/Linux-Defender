import hashlib
import sys
import struct
import socket
import os

LOG_FILE = "hash-scan.log"

def get_dir_files(path, getSubFiles = False):
    """
    This function get all the folders / files in given diractory.
    """
    results = []

    #loop over all the files / folders in the diractory
    for root, dirs, files in os.walk(path):
        for file in files:
            results.append(os.path.join(root, file))

        #check if needed also sub dirs and sub files
        if not getSubFiles:
            return results
        
    return results

def connect_to_server():
    """
    This function conect to the server and return the connection socket.
    """
    conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serv_addr = ('127.0.0.1', 9000)
    conn.connect(serv_addr)
    
    return conn

def sendPacket(conn, hashes):
    """
    This function send packet with data to the server.
    """

    #check if this is single hash in the data or list of hashes
    if type(hashes) == str:
        lst = []
        lst.append(hashes)
        hashes = lst

    #parse the data into the packet data format
    hashes = '[' + ', '.join(f'"{hash}"' for hash in hashes) + ']'
    msg = '{"hashes":' + hashes + '}'
    data = chr(113).encode() + struct.pack('<I', len(msg)) + msg.encode()
    conn.sendall(data)

def recvPacket(conn):
    """
    This function recv packets from the server by the project protocol.
    """
    packet = {}
    packet["code"] = conn.recv(1)
    packet["length"] = struct.unpack('<I', conn.recv(4))[0]
    packet["data"] = eval(conn.recv(packet["length"]).decode())

    return packet

def is_malicious_hashes(hashes):
    """
    This function communicate with the server to check if some of hashes are malicious.
    """
    conn = connect_to_server()
    sendPacket(conn, hashes)
    return recvPacket(conn)["data"]["res"]


def calc_sha256(file_path):
    """
    This function calc the hash of given file.
    """
    with open(file_path, "rb") as f:
        file_data = f.read()
    return hashlib.sha256(file_data).hexdigest()

def scan_file(path):
    """
    This function get path to given file 
    and using the other functions in the scannig process of single file.
    """
    results = {}
    results[path] = is_malicious_hashes(calc_sha256(path))[0]
    return results

def scan_dir(path, sub_files):
    """
    This function get path to given dir 
    and using the other functions in the scannig process of all the dir.
    """
    hashes = []
    results = {}
    data = []
    files = get_dir_files(path, sub_files)

    #loop over all the files in the dir / sub dirs
    for file in files:
        hashes.append(calc_sha256(file))
    data = is_malicious_hashes(hashes)

    #get all the data into results object
    for status, file in zip(data, files):
        results[file] = status

    return results

def main():
    params = sys.argv
    results = {}

    #usage helper of the scan tool
    if len(params) < 3:
        print("Usage: (-f / -d for file or directory) (path to the file / dir) (-a if diractory it will scan also sub directorys files)")
        exit(1)

    #scan single file
    if params[1] == "-f":
        results = scan_file(params[2])

    #scan dir
    elif params[1] == "-d":
        scanSubFiles = False

        #scan sub dirs
        if len(params) == 4:
            scanSubFiles = params[3] == "-a"
        results = scan_dir(params[2], scanSubFiles)

    #save the log data in the log file
    with open(LOG_FILE, "w") as file:
        for file_path, status in results.items():
            file.write(file_path + '@' + str(status) + "\n")

if __name__ == "__main__":
    main()
