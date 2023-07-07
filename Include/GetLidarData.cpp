#include "GetLidarData.h"

GetLidarData::GetLidarData() {

	LidarPerFrameDatePrt_Get = std::make_shared<std::vector<MuchLidarData>>();
}
void GetLidarData::setPortAndIP(int mdataPort, int mdevPort, std::string mComputerIP)
{
	dataPort = mdataPort;
	devPort = mdevPort;
	computerIP = mComputerIP;
}

GetLidarData::~GetLidarData() {

}

void GetLidarData::setCallbackFunction(FunDataPrt *callbackValue) {
    callback = callbackValue;
}

void GetLidarData::LidarStart() {
    isQuit = false;
    std::thread t1(&GetLidarData::LidarRun, this);
    t1.detach();

	std::thread m_DataSockT(&GetLidarData::getDataSock, this);
	m_DataSockT.detach();
	
	std::thread m_DevSockT(&GetLidarData::getDevSock, this);
	m_DevSockT.detach();
}

void GetLidarData::LidarOfflineDataStar()
{
	isQuit = false;
	std::thread t1(&GetLidarData::LidarRun, this);
	t1.detach();
}
//get port number of  lidar's data package
void GetLidarData::getDataSock()
{
#ifdef LINUX
#else
	WORD wVerRequest = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(wVerRequest, &wsaData);
#endif

	//create socket
	int m_sock = socket(2, 2, 0);			//createsock
	//initialize UDP communication
	//create socket
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	//define address
	struct sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(dataPort);
	//sockAddr.sin_addr.s_addr = inet_addr(computerIP.c_str());
	inet_pton(AF_INET, computerIP.c_str(), &sockAddr.sin_addr);
	
	int value = 10 * 1024 * 1024;
	int tmpCode = 0;
	tmpCode = ::setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&value, sizeof(value));
	
	//int result=0;
	//int lenValue=4;
	//tmpCode = ::getsockopt((SOCKET)m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&result, &lenValue);
	//printf("SO_RCVBUF=%d, tmpCode=%d\n", result, tmpCode);


	//bind socket
	int retVal = ::bind(m_sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
	struct sockaddr_in addrFrom;

#ifdef LINUX
	socklen_t len = sizeof(sockaddr_in);
#else
	int len = sizeof(sockaddr_in);
#endif
	//receive data
	char recvBuf[1212] = { 0 };
	int recvLen;

	while (true)
	{
		//get socket
#ifdef LINUX
		recvLen = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrFrom, &len);
#else
		recvLen = ::recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&addrFrom, &len);
#endif
		if (recvLen > 0)
		{
			u_char data[1212] = { 0 };
			memcpy(data, recvBuf, recvLen);
			CollectionDataArrive(data, recvLen);												//send data to class
		}
	}
}

//get port number of device package
void GetLidarData::getDevSock()
{
#ifdef LINUX
#else
	WORD wVerRequest = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(wVerRequest, &wsaData);
#endif
	//create socket
	int m_sock = socket(2, 2, 0);			//create  sock
	//initialize UDP communication //
	//create socket
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//define address
	struct sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(devPort);											//get port number of device package
	//sockAddr.sin_addr.s_addr = inet_addr(computerIP.c_str());
	inet_pton(AF_INET, computerIP.c_str(), &sockAddr.sin_addr);
	//bind socket
	int retVal = ::bind(m_sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
	struct sockaddr_in addrFrom;
#ifdef LINUX
	socklen_t len = sizeof(sockaddr_in);
#else
	int len = sizeof(sockaddr_in);
#endif
	//receive data
	char recvBuf[1212] = { 0 };
	int recvLen;

	while (true)
	{
		//get socket data
#ifdef LINUX
		recvLen = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrFrom, &len);
#else
		recvLen = ::recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&addrFrom, &len);
#endif
		if (recvLen > 0)
		{
			u_char data[1212] = { 0 };
			memcpy(data, recvBuf, recvLen);
			CollectionDataArrive(data, recvLen);												//send data to class
		}
	}
}

void GetLidarData::LidarStop() {
    isQuit = true;
}


void GetLidarData::sendLidarData() {

	if (callback) {
		LidarPerFrameDatePrt_Send = LidarPerFrameDatePrt_Get;
		(*callback)(LidarPerFrameDatePrt_Send, 0);
	}
	
	LidarPerFrameDatePer = LidarPerFrameDatePrt_Get;
	LidarPerFrameDatePrt_Get.reset(new std::vector<MuchLidarData>); 
	isFrameOK = true;

}

void GetLidarData::CollectionDataArrive(void *pData, uint16_t len) {
    if (len >= 1206) {
        unsigned char *dataV = new unsigned char[1212];
        memset(dataV, 0, 1212);
        memcpy(dataV, pData, len);
        m_mutex.lock();
        allDataValue.push(dataV);
        m_mutex.unlock();

        if (dataV[0] == 0xa5 && dataV[1] == 0xff && dataV[2] == 0x00 && dataV[3] == 0x5a) {
            memcpy(dataDev, pData, 1206);
    
            ip_sa = std::to_string(dataDev[10]) + "." +
                    std::to_string(dataDev[11]) + "." +
                    std::to_string(dataDev[12]) + "." +
                    std::to_string(dataDev[13]);
    
            islidarDevCome = true;
        }
    }
}

void GetLidarData::clearQueue(std::queue<unsigned char *> &m_queue) {
    std::queue<unsigned char *> empty;
    swap(empty, m_queue);
}

std::shared_ptr<std::vector<MuchLidarData>> GetLidarData::getLidarPerFrameDate() {
    isFrameOK = false;
	return LidarPerFrameDatePer;
}

#pragma region // set lidar parameters, send device package

//set rotate speed
bool GetLidarData::setLidarRotateSpeed(int SpeedValue) {
    if (setLidarParam()) {
        ///set rotate speed
		Rest_UCWP_buff[8] = SpeedValue / 256;	
		Rest_UCWP_buff[9] = SpeedValue % 256;
    } else {
        std::string str = "Equipment package is not update!!!";
        messFunction(str, 0);
        return false;
    }
    return true;
}

//set lidar IP
bool GetLidarData::setLidarIP(std::string LidarIPValue) {
    std::regex ipv4(
            "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if (!regex_match(LidarIPValue, ipv4)) {
        std::string str = "The IP format entered is incorrect, please check the input parameters";
        messFunction(str, 0);
        return false;
    }
    if (setLidarParam()) {
        //set lidar IP
        std::string::size_type defailtIP_pos;
        std::vector<std::string> IP_result;
        IP_result.clear();
        LidarIPValue = LidarIPValue + ".";                                                     //easily get the last part of data
        for (size_t i = 0; i < LidarIPValue.size(); i++)                                     //cut defaultIP lineedit
        {
            defailtIP_pos = LidarIPValue.find(".", i);
            if (defailtIP_pos < LidarIPValue.size()) {
                std::string s = LidarIPValue.substr(i, defailtIP_pos - i);
                IP_result.push_back(s);
                i = defailtIP_pos;// +pattern.size() - 1;
            }
        }

        if (IP_result.size() < 4) {
            std::string str = "Please enter the full Lidar IP address!!!Failed to set the Lidar IP address!!!";
            messFunction(str, 0);
            return false;
        } else if (IP_result.size() == 4 && IP_result[3] == "") {
            std::string str = "Please enter the full Lidar IP address!!!Failed to set the Lidar IP address!!!";
            messFunction(str, 0);
            return false;
        }
    
        if (!checkDefaultIP(IP_result)) {
            std::string str = "Failed to set the Lidar IP address!!!";
            messFunction(str, 0);
            return false;
        }
    
        Rest_UCWP_buff[10] = atoi(IP_result[0].c_str());
        Rest_UCWP_buff[11] = atoi(IP_result[1].c_str());
        Rest_UCWP_buff[12] = atoi(IP_result[2].c_str());
        Rest_UCWP_buff[13] = atoi(IP_result[3].c_str());
    
        return true;
    } else {
        std::string str = "Equipment package is not update!!!";
        messFunction(str, 0);
        return false;
    }
}

//check if the lidar ip is set properly
bool GetLidarData::checkDefaultIP(std::vector<std::string> m_DefaultIP) {
    int HeadDefaultIPValue = stoi(m_DefaultIP[0]);
    int endDefaultIPValue = stoi(m_DefaultIP[3]);
    if (HeadDefaultIPValue == 0 || HeadDefaultIPValue == 127 ||
        (HeadDefaultIPValue >= 224 && HeadDefaultIPValue <= 255)
            ) {
        std::string str =
                "The Lidar IP cannot be set to " + std::to_string(HeadDefaultIPValue) + std::string(".x.x.x!!!");
        messFunction(str, 0);
        return false;
    } else if (endDefaultIPValue == 255) {
        std::string str = "The Lidar IP cannot be set to broadcast(x.x.x.255)!!!";
        messFunction(str, 0);
        return false;
    }
    return true;
}

//check if the destination ip is set properly
bool GetLidarData::checkDestIP(std::vector<std::string> m_DestIP) {
    int HeadDefaultIPValue = stoi(m_DestIP[0]);
    int endDefaultIPValue = stoi(m_DestIP[3]);
    if (HeadDefaultIPValue == 0 || HeadDefaultIPValue == 127 ||
        (HeadDefaultIPValue >= 240 && HeadDefaultIPValue <= 255)
            ) {
        std::string str =
                "The Dest IP cannot be set to " + std::to_string(HeadDefaultIPValue) + std::string(".x.x.x!!!");
        messFunction(str, 0);
        return false;
    }
    return true;
}

//set computer ip
bool GetLidarData::setComputerIP(std::string ComputerIPValue) {
    std::regex ipv4(
            "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if (!regex_match(ComputerIPValue, ipv4)) {
        std::string str = "The IP format entered is incorrect, please check the input parameters";
        messFunction(str, 0);
        return false;
    }

    if (setLidarParam()) {
        //set computer ip
        std::vector<std::string> IP_result;
        IP_result.clear();
        std::string::size_type DestIP_pos;
        ComputerIPValue = ComputerIPValue + ".";
        for (size_t i = 0; i < ComputerIPValue.size(); i++)                                     //cut out destIP lineedit
        {
            DestIP_pos = ComputerIPValue.find(".", i);
            if (DestIP_pos < ComputerIPValue.size()) {
                std::string s = ComputerIPValue.substr(i, DestIP_pos - i);
                IP_result.push_back(s);
                i = DestIP_pos;
            }
        }
    
        if (IP_result.size() < 4) {
            std::string str = "Please enter the full Dest IP address!!! Failed to set the Dest(Computer) IP address!!!";
            messFunction(str, 0);
            return false;
    
        } else if (IP_result.size() == 4 && IP_result[3] == "") {
            std::string str = "Please enter the full Dest IP address!!!Failed to set the Dest(Computer) IP address!!!";
            messFunction(str, 0);
            return false;
        }
        if (!checkDestIP(IP_result)) {
            std::string str = "Failed to set the computer IP address!!!";
            messFunction(str, 0);
            return false;
        }
        Rest_UCWP_buff[14] = atoi(IP_result[0].c_str());
        Rest_UCWP_buff[15] = atoi(IP_result[1].c_str());
        Rest_UCWP_buff[16] = atoi(IP_result[2].c_str());
        Rest_UCWP_buff[17] = atoi(IP_result[3].c_str());
    
        return true;
    } else {
        std::string str = "Equipment package is not update!!!";
        messFunction(str, 0);
        return false;
    }
}

//set port number of data package
bool GetLidarData::setDataPort(int DataPort) {
    if (setLidarParam()) {
        //check port
        int devPort = Rest_UCWP_buff[26] * 256 + Rest_UCWP_buff[27];
        if (DataPort < 1025 || DataPort > 65535 || DataPort == devPort) {
            std::string str = "DataPort range 1025-65535 and DataPort and devport cannot be equal, please check the input parameters";
            messFunction(str, 0);
            return false;
        } else {
            //set port number of data package
            Rest_UCWP_buff[24] = DataPort / 256;
            Rest_UCWP_buff[25] = DataPort % 256;
            return true;
        }

    } else {
        std::string str = "Equipment package is not update!!!";
        messFunction(str, 0);
        return false;
    }
}

//set port number of device package
bool GetLidarData::setDevPort(int DevPort) {
    if (setLidarParam()) {
        //check port
        int dataPort = Rest_UCWP_buff[24] * 256 + Rest_UCWP_buff[25];
        if (DevPort < 1025 || DevPort > 65535 || DevPort == dataPort) {
            std::string str = "DataPort range 1025-65535 and DataPort and devport cannot be equal, please check the input parameters";
            messFunction(str, 0);
            return false;
        } else {
            //set port number of device package
            Rest_UCWP_buff[26] = DevPort / 256;
            Rest_UCWP_buff[27] = DevPort % 256;
            return true;
        }
    } else {
        std::string str = "Equipment package is not update!!!";
        messFunction(str, 0);
        return false;
    }
}

bool GetLidarData::setLidarRotateState(int RotateState) {
	std::string str = "This version of Lidar does not support 'setLidarRotateState()'!!!";
	messFunction(str, 0);
	return false;
}

bool GetLidarData::setLidarSoureSelection(int StateValue) {
	std::string str = "This version of Lidar does not support 'setLidarSoureSelection()'!!!";
	messFunction(str, 0);
    return false;
}

bool GetLidarData::setLidarWorkState(int LidarState) {
	std::string str = "This version of Lidar does not support 'setLidarWorkState()'!!!";
	messFunction(str, 0);
    return false;
}

bool GetLidarData::setLidarParam() {
    if (isSendUDP == false) {
        return true;
    }

    if (true == islidarDevCome && true == isSendUDP) {
        islidarDevCome = false;
        isSendUDP = false;
    
        //发送的配置包之前，保存一波设备包
        memcpy(Rest_UCWP_buff, dataDev, 1206);
    
        for (int i = 52; i < 60; i++) {
            Rest_UCWP_buff[i] = 0x00;
        }
    
        for (int i = 160; i < 168; i++) {
            Rest_UCWP_buff[i] = 0x00;
        }
    
        Rest_UCWP_buff[0] = 0xAA;                           //merge UCWP with ACWP, the UCWP identification header is the first 8 bytes
        Rest_UCWP_buff[1] = 0x00;
        Rest_UCWP_buff[2] = 0xFF;
        Rest_UCWP_buff[3] = 0x11;
        Rest_UCWP_buff[4] = 0x22;
        Rest_UCWP_buff[5] = 0x22;
        Rest_UCWP_buff[6] = 0xAA;
        Rest_UCWP_buff[7] = 0xAA;
        return true;
    } else {
        std::string str = "Equipment package is not update!!!";
        messFunction(str, 0);
        return false;
    }

}

bool GetLidarData::sendPacketToLidar(unsigned char *packet, const char *ip_data, u_short portNum) {
#ifdef __linux__
    struct sockaddr_in addrSrv{};
    //create socket UDP
    int socketid = socket(2, 2, 0);
#define SOCKET_ERROR -1
#else
    //initialize socket
    WORD  request;
    WSADATA  ws;
    request = MAKEWORD(1, 1);
    int err = WSAStartup(request, &ws);
    if (err != 0)
    {
        return false;
    }
    if (LOBYTE(ws.wVersion) != 1 || HIBYTE(ws.wVersion) != 1)
    {
        WSACleanup();
        return false;
    }
    SOCKADDR_IN addrSrv;
    //create socket UDP
    SOCKET socketid = socket(2, 2, 0);
#endif

    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(portNum);
    inet_pton(AF_INET, ip_data, &addrSrv.sin_addr);
    
    int sd = sendto(socketid, (const char *) packet, 1206, 0, (struct sockaddr *) &addrSrv, sizeof(addrSrv));
    
    if (sd != SOCKET_ERROR) {
        printf("sent succeeded，sent %d\n characters", sd);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
#ifdef __linux__
        (void) ::close(socketid);
#else
        closesocket(socketid);
#endif
    } else {
        printf("sent failed\n");
        return false;
    }

    isSendUDP = true;
    return true;
}

bool GetLidarData::sendPackUDP() {
    return sendPacketToLidar(Rest_UCWP_buff, ip_sa.c_str(), 2368);
}

void GetLidarData::messFunction(std::string strValue, int gValue) {
    std::cout << strValue.c_str() << std::endl;
}

#pragma endregion