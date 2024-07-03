using System.Collections.Generic;
using System.Numerics;

namespace UIclient2 {
    class Structs {
        public struct message {
            public int code;
            public int length;
            public string data;
            public byte[] msg;
        };
        public struct ScanRequest {
            public int scanType;
            public string path;
        }

        public struct FreeRequest {
            public int pid;
        }

        public struct BlockRequest {
            public int pid;
        }

        public struct KillRequest {
            int pid;
        }

        public struct IPScanRequest {
            public string ip;
        }

        public struct HashScanRequest {
            public List<string> hashes;
        }

        public struct IPSaveRequest {
            public string ip;
        }

        public struct HashSaveRequest {
            public string file_name;
            public string file_path;
            public string hash;
            public string date_added;
            public string source;
        }
        public struct ScanResponse {
            public int status;
            public int sctResults;
            public int idtResults;
            public string[] packetsResults;
            public Dictionary<string, bool> openPortsResults;
            public Dictionary<string, string> spofingResults;
            public Dictionary<string, List<string>> yaraResults;
            public Dictionary<string, bool> hashesResults;
        }

        public struct SuspiciousListResponse {
            public int status;
            public List<string> procNames;
        }
        public struct SuspendedListResponse {
            public int status;
            public List<string> procPids;
        }

        public struct FreeResponse {
            public int status;
            public int isWorked;
        }
        public struct BlockResponse {
            public int status;
            public int isWorked;
        }
        public struct KillResponse {
            public int status;
            public int isWorked;
        }
        public struct IpScanResponse {
            public int status;
            public int res;
        }
        public struct HashScanResponse {
            public int status;
            public List<int> res;
        }
        public struct SaveIpResponse {
            public int status;
            public int isWorked;
        }
        public struct SaveHashResponse {
            public int status;
            public int isWorked;
        }

        public struct ChangeScanTimeResponse {
            public int status;
            public int isWorked;
        }

        public struct UpdateBlockedTableRequest {
            public int pid;
            public string dateTime;
            public string action; //add, remove
        }; 

        public struct UpdateBlockedTableResponse { 
            public int status;
            public int isWorked;
        }

        public struct UpdateVirusTableResponse {
            public int status;
            public int isWorked;
        }

        public struct GetTimeResponse {
            public int status;
            public int time;
        }
    }
}
