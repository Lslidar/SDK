#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define UsageMethod_1 1


#include <IncludeFile.h>
int data_ip = 2368;																//数据包端口号
int dev_ip = 2369;																//设备包端口号
std::string computer_ip = "192.168.1.102";										//雷达的目的IP (电脑IP)

//获取数据包的端口号
void getDataSock()
{
#ifdef LINUX
#else
	WORD wVerRequest = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(wVerRequest, &wsaData);
#endif

	//创建socket
	int m_sock = socket(2, 2, 0);			//构建sock
	//UDP通信初始化
	//创建socket
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//定义地址
	struct sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(data_ip);
	//sockAddr.sin_addr.s_addr = inet_addr(computer_ip.c_str());
	inet_pton(AF_INET, computer_ip.c_str(), &sockAddr.sin_addr);

	int value = 1 * 1024 * 1024;
	int tmpCode = 0;
	tmpCode = ::setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&value, sizeof(value));

	//int result=0;
	//int lenValue=4;
	//tmpCode = ::getsockopt((SOCKET)m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&result, &lenValue);
	//printf("SO_RCVBUF=%d, tmpCode=%d\n", result, tmpCode);


	//绑定套接字
	int retVal = bind(m_sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
	struct sockaddr_in addrFrom;

#ifdef LINUX
	socklen_t len = sizeof(sockaddr_in);
#else
	int len = sizeof(sockaddr_in);
#endif
	//接收数据
	char recvBuf[1212] = { 0 };
	int recvLen;

	while (true)
	{
		//获取套接字接收内容
#ifdef LINUX
		recvLen = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrFrom, &len);
#else
		recvLen = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&addrFrom, &len);
#endif
		if (recvLen > 0)
		{
			u_char data[1212] = { 0 };
			memcpy(data, recvBuf, recvLen);
			m_GetLidarData->CollectionDataArrive(data, recvLen);												//把数据闯入到类内
		}
	}
}

//获取设备包的端口号
void getDevSock()
{
#ifdef LINUX
#else
	WORD wVerRequest = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(wVerRequest, &wsaData);
#endif
	//创建socket
	int m_sock = socket(2, 2, 0);			//构建sock
	//UDP通信初始化//
	//创建socket
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//定义地址
	struct sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(dev_ip);											//获取设备包的端口号
	//sockAddr.sin_addr.s_addr = inet_addr(computer_ip.c_str());
	inet_pton(AF_INET, computer_ip.c_str(), &sockAddr.sin_addr);
	//绑定套接字
	int retVal = bind(m_sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
	struct sockaddr_in addrFrom;
#ifdef LINUX
	socklen_t len = sizeof(sockaddr_in);
#else
	int len = sizeof(sockaddr_in);
#endif
	//接收数据
	char recvBuf[1212] = { 0 };
	int recvLen;

	while (true)
	{
		//获取套接字接收内容
#ifdef LINUX
		recvLen = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrFrom, &len);
#else
		recvLen = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&addrFrom, &len);
#endif
		if (recvLen > 0)
		{
			u_char data[1212] = { 0 };
			memcpy(data, recvBuf, recvLen);
			m_GetLidarData->CollectionDataArrive(data, recvLen);												//把数据闯入到类内
		}
	}
}

#if UsageMethod_1

#pragma region 		//方法一 通过标记位 获取一帧雷达数据的方法
int main()
{
	m_GetLidarData->LidarStar();										//开始解析雷达数据

	//m_GetLidarData->CollectionDataArrive(data, 1212);					//把获取的UDP 包  放进类的队列中依此解析（在Socket获取数据包后， 调用这个方法）

	//开启线程  获取雷达 数据包和 设备包  将每个包的数据传入到对象中
	std::thread m_DataSockT(&getDataSock);
	m_DataSockT.detach();

	std::thread m_DevSockT(&getDevSock);
	m_DevSockT.detach();

#pragma region //修改雷达参数 可单独修改某个参数 
	//std::this_thread::sleep_for(std::chrono::milliseconds(2000));			//先等待一段时间 获取设备包
	//m_GetLidarData->setLidarRotateSpeed(600);
	//m_GetLidarData->setLidarIP("192.168.1.200");
	//m_GetLidarData->setComputerIP("192.168.1.102");
	//m_GetLidarData->setDataPort(2368);
	//m_GetLidarData->setDevPort(2369);

	//m_GetLidarData->setLidarRotateState(0);
	//m_GetLidarData->setLidarSoureSelection(0);
	//m_GetLidarData->setLidarWorkState(0);

	//m_GetLidarData->sendPackUDP();										//设置完参数后，必须要调用 sendPackUDP 发送UDP包
#pragma endregion 


	//获取点云进行算法处理    //循环处理
#pragma region   //获取点云做其他算法的处理  或 点云显示
	while (true)
	{
		//方法一 获取一帧雷达数据的方法
		if (m_GetLidarData->isFrameOK)
		{
			std::vector<MuchLidarData> m_LidarData_temp = m_GetLidarData->getLidarPerFrameDate();
                for (u_int m_FF = 0; m_FF < int(m_LidarData_temp.size()); m_FF++)
                {
                    std::cout.width(10);
                    std::cout << m_LidarData_temp[m_FF].X << " 	";
                    std::cout.width(10);
                    std::cout << m_LidarData_temp[m_FF].Y << " 	";
                    std::cout.width(10);
                    std::cout << m_LidarData_temp[m_FF].Z << " 	";
                    std::cout.width(10);
                    std::cout << m_LidarData_temp[m_FF].ID << " 	";
                    std::cout.width(10);
                    std::cout << m_LidarData_temp[m_FF].Intensity << " 	";
                    std::cout.width(20);
                    std::cout << m_LidarData_temp[m_FF].Mtimestamp_nsce;
                    std::cout.width(20);
                    std::cout << m_LidarData_temp[m_FF].H_angle;
                    std::cout << std::endl;
                }
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
#pragma endregion

	m_GetLidarData->LidarStop();
}

#else

#pragma region 		//方法二 通过回调函数获取一帧雷达数据的方法
std::mutex m_mutex;
std::vector<MuchLidarData> m_LidarData;									//一帧的数据

//普通的回调函数
void callbackFunction_123(std::vector<MuchLidarData>, int);				//回调函数，获取每一帧的数据

//类内方法做回调函数
class A
{
public:
	void callbackFunction(std::vector<MuchLidarData> LidarDataValue, int);	//回调函数，获取每一帧的数据
	Fun fun;

	Fun* bind()
	{
		fun = std::bind(&A::callbackFunction, this, std::placeholders::_1, std::placeholders::_2);
		return &fun;
	}
};

int main()
{
	//类成员函数作为回调函数
	A m_a;
	Fun fun = std::bind(&A::callbackFunction, m_a, std::placeholders::_1, std::placeholders::_2);

	//普通回调
	//Fun fun = std::bind(callbackFunction_123, std::placeholders::_1, std::placeholders::_2);

	m_GetLidarData->setCallbackFunction(&fun);							//设置回调函数
	m_GetLidarData->LidarStar();										//开始解析雷达数据

	//m_GetLidarData->CollectionDataArrive(data, 1212);					//把获取的UDP 包  放进类的队列中依此解析（在Socket获取数据包后， 调用这个方法）

	//开启线程  获取雷达 数据包和 设备包  将每个包的数据传入到对象中
	std::thread m_DataSockT(&getDataSock);
	m_DataSockT.detach();

	std::thread m_DevSockT(&getDevSock);
	m_DevSockT.detach();

#pragma region //修改雷达参数 可单独修改某个参数 
	//std::this_thread::sleep_for(std::chrono::milliseconds(2000));			//先等待一段时间 获取设备包
	//m_GetLidarData->setLidarRotateSpeed(600);
	//m_GetLidarData->setLidarIP("192.168.1.200");
	//m_GetLidarData->setComputerIP("192.168.1.102");
	//m_GetLidarData->setDataPort(2368);
	//m_GetLidarData->setDevPort(2369);

	//m_GetLidarData->setLidarRotateState(0);
	//m_GetLidarData->setLidarSoureSelection(0);
	//m_GetLidarData->setLidarWorkState(0);

	//m_GetLidarData->sendPackUDP();										//设置完参数后，必须要调用 sendPackUDP 发送UDP包
#pragma endregion 

#pragma region   //获取点云做其他算法的处理  或 点云显示
	while (true)
	{
		if (!m_LidarData.empty())
		{
			m_mutex.lock();
			std::vector<MuchLidarData> m_LidarData_temp;
			m_LidarData_temp = m_LidarData;
			m_LidarData.clear();
			m_mutex.unlock();

			for (u_int m_FF = 0; m_FF < int(m_LidarData_temp.size()); m_FF++)
			{
				std::cout.width(10);
				std::cout << m_LidarData_temp[m_FF].X << " 	";
				std::cout.width(10);
				std::cout << m_LidarData_temp[m_FF].Y << " 	";
				std::cout.width(10);
				std::cout << m_LidarData_temp[m_FF].Z << " 	";
				std::cout.width(10);
				std::cout << m_LidarData_temp[m_FF].ID << " 	";
				std::cout.width(10);
				std::cout << m_LidarData_temp[m_FF].Intensity << " 	";
				std::cout.width(20);
				std::cout << m_LidarData_temp[m_FF].Mtimestamp_nsce;
				std::cout << std::endl;
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
#pragma endregion

	m_GetLidarData->LidarStop();
}

// 回调函数指针   获取每一帧点云数据
void A::callbackFunction(std::vector<MuchLidarData> LidarDataValue, int Type)
{
	m_mutex.lock();
	m_LidarData = LidarDataValue;
	m_mutex.unlock();
}

void callbackFunction_123(std::vector<MuchLidarData> LidarDataValue, int Type)
{
	m_mutex.lock();
	m_LidarData = LidarDataValue;
	m_mutex.unlock();
}
#pragma endregion 

#endif


