#pragma once
/******************** (C) COPYRIGHT 2021 Shenzhen LeiShen Intelligent System Co., Ltd. *****************

* File Name         : LSLidarDemo.h
* Author            : LSJ
* Version           : v2.1.0
* Date              : 2023/07/06
* Description       : initial verision
* Modify Record		:
* Instruction		:
*******************************************************************************/

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
#include <memory>

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
#define ConversionUnit	100.0			// unit conversion， convert centimeter to meter

typedef struct _Point_XYZ
{
	float x1 = 0.0;
	float y1 = 0.0;
	float z1 = 0.0;

	float x2 = 0.0;
	float y2 = 0.0;
	float z2 = 0.0;
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

typedef std::function<void(std::shared_ptr<std::vector<MuchLidarData>>, int)> FunDataPrt;

class GetLidarData
{
public:
	GetLidarData();
	~GetLidarData();

	bool isFrameOK = false;
	std::shared_ptr<std::vector<MuchLidarData>> getLidarPerFrameDate();
	
	void setCallbackFunction(FunDataPrt*);																// set point cloud data callback function
	
	void LidarStart();
	void LidarOfflineDataStar();
	void LidarStop();
	virtual void LidarRun() = 0;
	void CollectionDataArrive(void * pData, uint16_t len);										//send UDP data here
	
	void setPortAndIP(int, int, std::string);
public:

#pragma region //set lidar parameters
	 unsigned char dataDev[1206];															//receive device package
	 unsigned char Rest_UCWP_buff[1206];												    //modify configuration package
	 std::string ip_sa;																		// send lidar IP

	 /*
	 function: modify lidar rotate speed
	 parameters: SpeedValue				speed value, 300, 600, 1200 supported
	returned value: true:succeeded, false: failed
	 */
	virtual bool setLidarRotateSpeed(int SpeedValue);												//set lidar rotate speed
	
	/*
	 function: modify lidar IP
	parameters: IPString	input the IPv4 address，  for example: "192.168.1.200"
	returned value:  tru:succeeded, false: failed
	*/
	virtual bool setLidarIP(std::string IPString);													//set lidar IP
	
	/*
	function:modify lidar's destination IP (computer IP)
	parameter: IPString,	input the IPv4 address，  for example: "192.168.1.200"
	returned value:  tru:succeeded, false: failed
	*/
	virtual bool setComputerIP(std::string IPString);												//set computer IP
	
	/*
	function:modify lidar's data package
	port number
	parameter: PortNum input the lidar data package port number,by default 0.65536 can be input, (recommended to use ports after 2000)
	returned value:  tru:succeeded, false: failed
	*/
	virtual bool setDataPort(int PortNum);															//set port numbr of data package
	
	/*
	modify lidar's device package port number
	parameter: PortNum	input the lidar data package port number,by default 0.65536 can be input, (recommended to use ports after 2000)
	returned value:  tru:succeeded, false: failed
	*/
	virtual bool setDevPort(int PortNum);															//set port numbr of device package
	
	/*
	function:  change lidar's motor to stand still or rotate
	parameter: Enter lidar mode, default 0 is rotation, 1 is stationary
	returned value:  tru:succeeded, false: failed
	*/
	virtual bool setLidarRotateState(int StateValue);												//lidar rotation/stationary	
	
	/*
	function: Modify the clock source selection
	parameter: selectValue,  Input lidar mode, by default, 0 is GPS, 1 is PTP， 2 is NTP
	returned value:  tru:succeeded, false: failed
	*/
	virtual bool setLidarSoureSelection(int StateValue);											//set clock source 
	
	/*
	function: Modify the lidar low-power operating mode （only send device package, no data package)
	parameter: selectValue	Enter the lidar mode,default 0 is normal mode, 1 is low power mode (no packets are sent at this time)
	returned value:  tru:succeeded, false: failed
	*/
	virtual bool  setLidarWorkState(int StateValue);												//lidar's operating mode, whether it is in low-power operating mode
	
	/*
	function: Sending UDP packets of lidar data
	parameter：none
	returned value:  tru:succeeded, false: failed
	*/
	virtual bool sendPackUDP();																		//send UDP package
	
	bool setLidarParam();																			//set package
	bool sendPacketToLidar(unsigned char* packet, const char* ip_data, u_short portdata);			//send package throught socket
#pragma endregion 

protected:
	bool isQuit = false;																		//lidar quit mark
	std::queue<unsigned char *> allDataValue;													// data cache queue
	std::mutex m_mutex;																			//lock

	u_int64 timestamp_nsce;																		//timestamp under 1 second in the package(unit: nanosecond)
	u_int64 allTimestamp;																		//save the timestamp in the package
	u_int64 lastAllTimestamp = 0;																//save the timestamp of the previous package
	double PointIntervalT_ns;																	//time interval between each point in the package, unit is nanosecond
	
	std::shared_ptr<std::vector<MuchLidarData>> LidarPerFrameDatePrt_Get;						// get data
	std::shared_ptr<std::vector<MuchLidarData>> LidarPerFrameDatePrt_Send;						// send data
	
	UTC_Time m_UTC_Time;																		//get the GPS information of device package
	void clearQueue(std::queue<unsigned char *>& m_queue);										//clear queue
	void sendLidarData();																		//packaging and send data
	FunDataPrt *callback = NULL;																//callback function pointer--callback function data package
	std::string time_service_mode = "gps";                                                     //timing method
	
	void messFunction(std::string strValue, int gValue);
	//Check whether the source IP matches or not
	bool checkDefaultIP(std::vector<std::string> m_DefaultIP);
	//Check whether the destination IP matches or not
	bool checkDestIP(std::vector<std::string> m_DestIP);
	
	//start the thread to get data package
	void getDataSock();		
	//start the thread to get device package
	void getDevSock();

private:
	std::shared_ptr<std::vector<MuchLidarData>> LidarPerFrameDatePer;
	bool islidarDevCome = false;														//Check whether get the device package  or not
	bool isSendUDP = true;

	int dataPort = 2368;																//port number of data package
	int devPort = 2369;																//port number of device package
	std::string computerIP = "192.168.1.102";										//destination IP of lidar (computer IP)
};
