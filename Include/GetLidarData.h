#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <cmath>
#include <chrono>
#include <cstring>
#include <functional>
#include <string>
#include <regex>

#ifdef LINUX
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
typedef uint64_t u_int64;
#else
#include <WinSock2.h>
#include <WS2tcpip.h>
typedef unsigned __int64 u_int64;

#define timegm _mkgmtime
#endif

#define PI 3.1415926
#define ConversionUnit	100.0			//unit concersion: convert the centimeter to meter

typedef struct _Point_XYZ
{
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
}m_PointXYZ;

typedef struct _MuchLidarData
{
	float X = 0.0;
	float Y = 0.0;
	float Z = 0.0;
	int ID = 0;
	float H_angle = 0.0;
	float V_angle = 0.0;
	float Distance = 0.0;
	int Intensity = 0.0;
	u_int64 Mtimestamp_nsce = 0.0;
}MuchLidarData;

typedef struct _UTC_Time
{
	int year = 2000;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;
}UTC_Time;

typedef std::function<void(std::vector<MuchLidarData>, int)> Fun;


class GetLidarData
{
public:
	GetLidarData();
	~GetLidarData();

	bool isFrameOK = false;
	std::vector<MuchLidarData> getLidarPerFrameDate();


	void setCallbackFunction(Fun*);																//set the callback of point cloud data 
	bool setEcho_SingleOrDouble(int value);														//set the echo mode, single echo or double echo

	void LidarStar();
	void LidarStop();
	virtual void LidarRun() = 0;
 	void CollectionDataArrive(void * pData, uint16_t len);										//collect the UDP data and send to here

public:

#pragma region //set the parameters of the Lidar 
	 unsigned char dataDev[1206];															//receive device package
	 unsigned char Rest_UCWP_buff[1206];												    // modify configuration package
	 std::string ip_sa;																		//send the Lidar IP 

	 /*
	 Function: modify rotate speed 
	 Parameter: SpeedValue				speed value， support 300， 600，1200
	 Returned value: true: operation succeeded; false: operation failed
	 */
	virtual bool setLidarRotateSpeed(int SpeedValue);												// set rotate speed

	/*
	Function: modify the Lidar IP
	Parameter: IPString	,input the IP address to be changed，such as "192.168.1.200"
	Returned value: true: operation succeeded; false: operation failed
	*/
	virtual bool setLidarIP(std::string IPString);													//set the Lidar IP

	/*
	Function:modify the destination IP, which is the computer's IP address
	Parameter：IPString, input the IPv4 address to be changed，such as 192.168.1.102
	Returned value: true: operation succeeded; false: operation failed
	*/
	virtual bool setComputerIP(std::string IPString);												//set the computer IP
	
	/*
	Function:modify the port number of the data package
	Parameter: PortNum, input the number of Lidar's data port,0~65536 can be input by default（a value larger than 2000 is recommended）
	Returned value: true: operation succeeded; false: operation failed
	*/
	virtual bool setDataPort(int PortNum);															//set the port number of the data package
	
	/*
	Function:modify the port number of the device package
	Parameter: PortNum, input the number of Lidar's device port,0~65536 can be input by default(a value larger than 2000 is recommended)
	Returned value: true: operation succeeded; false: operation failed
	*/
	virtual bool setDevPort(int PortNum);															//the port number of the device package
	
	/*
	Function:run or stop the Lidar's moto
	Parameter: StateValue, input the mode of Lidar's moto,by default, 0 means run , 1 means stop
	Returned value: true: operation succeeded; false: operation failed
	*/
	virtual bool setLidarRotateState(int StateValue);												//Lidar run/stop

	/*
	Function:modify Lidar's timing source selection
	Parameter:selectValue, input the source selection, by default, 0 means GPS, 1 means PTP, 2 means NTP
	Returned value: true: operation succeeded; false: operation failed
	*/
	virtual bool setLidarSoureSelection(int StateValue);											//modify Lidar's timing source selection

	/*
	Function:modify Lidar's low power mode, which means the lidar only sends device package without data package)
	Parameter:selectValue, input the mode, by default, 0 means normal mode, 1 means low power mode
	Returned value: true: operation succeeded; false: operation failed
	*/
	virtual bool  setLidarWorkState(int StateValue);												//Lidar state: low power mode or not

	/*
	Function:send UDP package, Lidar data
	Parameter:none
	Returned value: true: operation succeeded; false: operation failed
	*/
	virtual bool sendPackUDP();																		//send UDP package

	bool setLidarParam();																			//set Lidar parameters
	bool sendPacketToLidar(unsigned char* packet, const char* ip_data, u_short portdata);			//send the package via socket
#pragma endregion 

protected:
	bool isQuit = false;																		//The mark means Lidar quits
	int Model = 0;																				//Output juadement：0 means single echo and double echo data; 1 means single echo data; 2 means single echo and double echo data
	std::queue<unsigned char *> allDataValue;													//Data cache queue
	std::mutex m_mutex;																			//Lock

	u_int64 timestamp_nsce;																		//timestamp under 1 second in the package(unit: nanosecond)
	u_int64 allTimestamp;																		//save the timestamp in the package
	u_int64 lastAllTimestamp = 0;																//save the timestamp of the previous package
	double PointIntervalT_ns;																	//time interval between each point in the package, unit is nanosecond

	std::vector<MuchLidarData> all_lidardata, all_lidardata_d;									//obtain data of each frame
	UTC_Time m_UTC_Time;																		//obtain the GPS information of device package
	void clearQueue(std::queue<unsigned char *>& m_queue);										//clear queue
	void sendLidarData();																		//packaging and send data
	Fun *callback = NULL;																		//callback pointer--callback data package
    std::string time_service_mode = "gps";                                                     //timing method

	void messFunction(std::string strValue, int gValue);
	//Check whether the source IP matches or not
	bool checkDefaultIP(std::vector<std::string> m_DefaultIP);
	//Check whether the destination IP matches or not
	bool checkDestIP(std::vector<std::string> m_DestIP);
private:
	std::vector<MuchLidarData> LidarPerFrameDate;
	bool islidarDevCome = false;														//Check whether the device package is obtained or not
	bool isSendUDP = true;



};
