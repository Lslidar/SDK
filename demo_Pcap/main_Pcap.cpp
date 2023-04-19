#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define UsageMethod_1 1

#include <IncludeFile.h>

#if  UsageMethod_1
#pragma region 		//方法一 通过标记位 获取一帧雷达数据的方法

#include <IncludeFile.h>

//离线播放的 Pcap库
#include <pcap.h>
#pragma comment(lib, "packet.lib")
#pragma comment(lib, "wpcap.lib")
//离线播放
const int DataPacketLen = 1206;										//数据包长度
pcap_t *pfile;														//文件句柄
bool openPcapFile(const char *fileName);							//打开文件， 获取文件的句柄
void readPcapFile();												//解析Pcap文件包， 传入库文件中
bool isFinish = false;												//播放结束标记退出
bool isWait = false;												//播放等待的标记， 防止包解析过快

int main()
{
	m_GetLidarData->LidarStar();										//开始解析雷达数据

	//打开pcap文件
	if (!openPcapFile("E:/test.pcap"))
	{
		std::cout << "Pcap File Open Failed!" << std::endl;
		return 0;
	}
	isFinish = false;
	isWait = false;
	std::thread m_ReadPcapT(&readPcapFile);
	m_ReadPcapT.detach();

#pragma region   //获取点云做其他算法的处理  或 点云显示

	while (true)
	{
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
				std::cout << std::endl;
			}
			isWait = true;
		}
		else
		{
			Sleep(1);
		}

		if (isFinish && isWait)
		{
			std::cout << "File Playing completed！！！" << std::endl;
			break;

		}
	}
#pragma endregion  

	m_GetLidarData->LidarStop();

	char m_getWait = getchar();
}

//打开 pcap包 获取句柄
bool openPcapFile(const char *fileName)
{
	char errbuf[100];
	pfile = pcap_open_offline(fileName, errbuf);
	if (NULL == pfile) {
		std::cout << errbuf;
		return false;
	}
	return true;
}

//线程解析 pcap包,  传给对象进行解析
void readPcapFile()
{
	pcap_pkthdr *pkthdr = 0;
	const u_char *pktdata = 0;
	unsigned char databuf[DataPacketLen];
	while (pcap_next_ex(pfile, &pkthdr, &pktdata) > 0)
	{
		if (pkthdr->caplen == (DataPacketLen + 42)
			&& !(pktdata[42] == 0xaa && pktdata[43] == 0x00 && pktdata[44] == 0xff && pktdata[45] == 0x11))					//过滤配置包
		{
			pktdata = pktdata + 42;
			memcpy(databuf, pktdata, DataPacketLen);
			m_GetLidarData->CollectionDataArrive(databuf, DataPacketLen);												//把数据传入到类内
			if (isWait)
			{
				Sleep(80);
				isWait = false;
			}
		}
	}
	isFinish = true;
	//结束停止
	std::cout << "Pcap parsing Pcap file completed. Close file" << std::endl;
	pcap_close(pfile);
}

#pragma endregion 	

#else

#pragma region 		//方法二 通过回调函数获取一帧雷达数据的方法


std::mutex m_mutex;
std::vector<MuchLidarData> m_LidarData;								//一帧的数据

void callbackFunction_123(std::vector<MuchLidarData> LidarDataValue, int);	//回调函数，获取每一帧的数据

//离线播放的 Pcap库
#include <pcap.h>
#pragma comment(lib, "packet.lib")
#pragma comment(lib, "wpcap.lib")
//离线播放
const int DataPacketLen = 1206;										//数据包长度
pcap_t *pfile;														//文件句柄
bool openPcapFile(const char *fileName);							//打开文件， 获取文件的句柄
void readPcapFile();												//解析Pcap文件包， 传入库文件中
bool isFinish = false;												//播放结束标记退出
bool isWait = false;												//播放等待的标记， 防止包解析过快

int main()
{
	//普通回调
	Fun fun = std::bind(callbackFunction_123, std::placeholders::_1, std::placeholders::_2);
	m_GetLidarData->setCallbackFunction(&fun);							//设置回调函数
	m_GetLidarData->LidarStar();										//开始解析雷达数据

	//打开pcap文件
	if (!openPcapFile("E:/test.pcap"))
	{
		std::cout << "Pcap File Open Failed!" << std::endl;
		return 0;
	}
	isFinish = false;
	isWait = false;
	std::thread m_ReadPcapT(&readPcapFile);
	m_ReadPcapT.detach();

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

			for (u_int m_FF = 0; m_FF < m_LidarData_temp.size(); m_FF++)
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
			Sleep(1);
		}

		if (isFinish && isWait)
		{
			std::cout << "File Playing completed！！！" << std::endl;
			break;

		}
	}
#pragma endregion  

	m_GetLidarData->LidarStop();

	char m_getWait = getchar();
}

// 回调函数指针   获取每一帧点云数据
void callbackFunction_123(std::vector<MuchLidarData> LidarDataValue, int Type)
{
	m_mutex.lock();
	m_LidarData = LidarDataValue;
	isWait = true;
	m_mutex.unlock();
}

//打开 pcap包 获取句柄
bool openPcapFile(const char *fileName)
{
	char errbuf[100];
	pfile = pcap_open_offline(fileName, errbuf);
	if (NULL == pfile) {
		std::cout << errbuf;
		return false;
	}
	return true;
}

//线程解析 pcap包,  传给对象进行解析
void readPcapFile()
{
	pcap_pkthdr *pkthdr = 0;
	const u_char *pktdata = 0;
	unsigned char databuf[DataPacketLen];
	while (pcap_next_ex(pfile, &pkthdr, &pktdata) > 0)
	{
		if (pkthdr->caplen == (DataPacketLen + 42)
			&& !(pktdata[42] == 0xaa && pktdata[43] == 0x00 && pktdata[44] == 0xff && pktdata[45] == 0x11))					//过滤配置包
		{
			pktdata = pktdata + 42;
			memcpy(databuf, pktdata, DataPacketLen);
			m_GetLidarData->CollectionDataArrive(databuf, DataPacketLen);												//把数据传入到类内
			if (isWait)
			{
				Sleep(80);
				isWait = false;
			}
		}
	}
	isFinish = true;
	//结束停止
	std::cout << "Pcap parsing Pcap file completed. Close file" << std::endl;
	pcap_close(pfile);
}

#pragma endregion 	

#endif