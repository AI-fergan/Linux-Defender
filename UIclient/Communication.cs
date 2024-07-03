using System;
using System.Collections;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.IO;
using System.Security.Cryptography;
using Tmds.DBus.Protocol;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Newtonsoft.Json;
using System.Collections.Generic;



namespace UIclient2
{
    class Communication
    {
        private const int SIZE_OF_BYTE = 8;
        private const int CODE_SIZE = 1;
        private const int LEN_SIZE = 4;
        private const int PORT = 8876;
        public TcpClient client;

        public enum scanTypes {
            file_scan,
            folder_scan,
            quick_scan,
            full_scan
        }
        public Communication()
        {
            client = new TcpClient();
            IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), PORT);
            try {
                client.Connect(serverEndPoint);
                Console.WriteLine("[+] Connected to server");
            }
            catch (Exception ex) {
                Console.WriteLine($"[+] Failed to connect to server: {ex.Message}");
            }
        }

        public void SendMSG(int code, string json)
        {
            Structs.message msg;
            msg.code = code;
            msg.data = json;
            msg.length = msg.data.Length;
            msg.msg = new byte[msg.length + CODE_SIZE + LEN_SIZE];
            
            byte[] codeBytes = BitConverter.GetBytes(code);
            byte[] lengthBytes = BitConverter.GetBytes(msg.length);
            byte[] dataBytes = Encoding.UTF8.GetBytes(msg.data);

            Array.Copy(codeBytes, 0, msg.msg, 0, CODE_SIZE);
            Array.Copy(lengthBytes, 0, msg.msg, CODE_SIZE, LEN_SIZE);
            Array.Copy(dataBytes, 0, msg.msg, CODE_SIZE + LEN_SIZE, dataBytes.Length);

            NetworkStream clientStream = client.GetStream();
            clientStream.Write(msg.msg, 0, msg.length + CODE_SIZE + LEN_SIZE);
            clientStream.Flush();
        }

        public byte[] GetPartFromSocket(int bytesNum)
        {
            NetworkStream clientStream = client.GetStream();
            byte[] data = new byte[bytesNum];

            int bytesRead = clientStream.Read(data, 0, bytesNum);

            if (bytesRead != bytesNum)
            {
                string errorMsg = $"Error while receiving from socket: {clientStream}";
                throw new InvalidOperationException(errorMsg);
            }

            return data;
        }

        public string GetMSG()
        {
            Structs.message msg;
            byte[] code = new byte[4];
            byte[] length = new byte[4];
            Array.Copy(GetPartFromSocket(CODE_SIZE), 0, code, 0, CODE_SIZE);
            Array.Copy(GetPartFromSocket(LEN_SIZE), 0, length, 0, LEN_SIZE);
            msg.code = BitConverter.ToInt32(code);
            msg.length = BitConverter.ToInt32(length);
            byte[] data = new byte[msg.length];
            Array.Copy(GetPartFromSocket(msg.length), 0, data, 0, msg.length);
            msg.data = Encoding.ASCII.GetString(data);

            return msg.data;
        }
        public string[][] scan(scanTypes scanType, string path) {
            List<string> filesResults = new List<string>();
            List<string> tablesResults = new List<string>();
            List<string> networkResults = new List<string>();
            Structs.ScanRequest scanReq;

            scanReq.scanType = (int)scanType;
            scanReq.path = path;
            string json = JsonConvert.SerializeObject(scanReq);
            SendMSG(100, json);
            string respJson = GetMSG();

            var respStruct = JsonConvert.DeserializeObject<Structs.ScanResponse>(respJson);

            switch(scanType){  
                case scanTypes.full_scan:
                case scanTypes.quick_scan:
                    //yara & hashes
                    foreach (var entry in respStruct.yaraResults) {
                        string data = "";
                        data += "File: " + entry.Key + "\n";
                        data += "is Malicious: " + respStruct.hashesResults[entry.Key].ToString() + "\n";
                        data += "File details: " + String.Join(", ", entry.Value);
                        filesResults.Add(data);
                    }

                    //sct & idt
                    if(respStruct.sctResults == -1){
                        tablesResults.Add("SCT: Hook not found\nin sct table");
                    }
                    else {
                        tablesResults.Add("SCT: Hook found\nin sct table\n" + "syscall entry: " + respStruct.sctResults.ToString());
                    }

                    if(respStruct.idtResults == -1){
                        tablesResults.Add("IDT: Hook not found\nin idt table");
                    }
                    else {
                        tablesResults.Add("IDT: Hook found in\nidt table\n" + "interrupt entry: " + respStruct.idtResults.ToString());
                    }
                    
                    //open ports
                    foreach(var entry in respStruct.openPortsResults){
                        if(entry.Value){
                            networkResults.Add("port " + entry.Key + " is open");
                        }
                        else{
                            networkResults.Add("port " + entry.Key + " is close");
                        }
                    }

                    foreach(var entry in respStruct.packetsResults){
                        networkResults.Add(entry);
                    }

                    foreach(var entry in respStruct.spofingResults){
                        networkResults.Add("ip " + entry.Key + "is spofing port " + entry.Value);
                    }
                    break;         
                case scanTypes.file_scan:
                case scanTypes.folder_scan:
                    foreach (var entry in respStruct.yaraResults) {
                        string data = "";
                        data += "File: " + entry.Key + "\n";
                        data += "is Malicious: " + respStruct.hashesResults[entry.Key].ToString() + "\n";
                        data += "File details: " + String.Join(", ", entry.Value);
                        filesResults.Add(data);
                    }
                    break;
            }

                        
            string[][] results = {filesResults.ToArray(), tablesResults.ToArray(), networkResults.ToArray()};
            return results;
        }
        public void close(){
            //client.Close();
        }

    }    
}
