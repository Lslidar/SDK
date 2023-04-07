#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define UsageMethod_1 1

#include <IncludeFile.h>

#if  UsageMethod_1
#pragma region 		//����һ ͨ�����λ ��ȡһ֡�״����ݵķ���

#include <IncludeFile.h>

//���߲��ŵ� Pcap��
#include <pcap.h>
#pragma comment(lib, "packet.lib")
#pragma comment(lib, "wpcap.lib")
//���߲���
const int DataPacketLen = 1206;										//���ݰ�����
pcap_t *pfile;														//�ļ����
bool openPcapFile(const char *fileName);							//���ļ��� ��ȡ�ļ��ľ��
void readPcapFile();												//����Pcap�ļ����� ������ļ���
bool isFinish = false;												//���Ž�������˳�
bool isWait = false;												//���ŵȴ��ı�ǣ� ��ֹ����������

int main()
{
	m_GetLidarData->LidarStar();										//��ʼ�����״�����

	//��pcap�ļ�
	if (!openPcapFile("E:/test.pcap"))
	{
		std::cout << "Pcap File Open Failed!" << std::endl;
		return 0;
	}
	isFinish = false;
	isWait = false;
	std::thread m_ReadPcapT(&readPcapFile);
	m_ReadPcapT.detach();

#pragma region   //��ȡ�����������㷨�Ĵ���  �� ������ʾ

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
			std::cout << "File Playing completed������" << std::endl;
			break;

		}
	}
#pragma endregion  

	m_GetLidarData->LidarStop();

	char m_getWait = getchar();
}

//�� pcap�� ��ȡ���
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

//�߳̽��� pcap��,  ����������н���
void readPcapFile()
{
	pcap_pkthdr *pkthdr = 0;
	const u_char *pktdata = 0;
	unsigned char databuf[DataPacketLen];
	while (pcap_next_ex(pfile, &pkthdr, &pktdata) > 0)
	{
		if (pkthdr->caplen == (DataPacketLen + 42)
			&& !(pktdata[42] == 0xaa && pktdata[43] == 0x00 && pktdata[44] == 0xff && pktdata[45] == 0x11))					//�������ð�
		{
			pktdata = pktdata + 42;
			memcpy(databuf, pktdata, DataPacketLen);
			m_GetLidarData->CollectionDataArrive(databuf, DataPacketLen);												//�����ݴ��뵽����
			if (isWait)
			{
				Sleep(80);
				isWait = false;
			}
		}
	}
	isFinish = true;
	//����ֹͣ
	std::cout << "Pcap parsing Pcap file completed. Close file" << std::endl;
	pcap_close(pfile);
}

#pragma endregion 	

#else

#pragma region 		//������ ͨ���ص�������ȡһ֡�״����ݵķ���


std::mutex m_mutex;
std::vector<MuchLidarData> m_LidarData;								//һ֡������

void callbackFunction_123(std::vector<MuchLidarData> LidarDataValue, int);	//�ص���������ȡÿһ֡������

//���߲��ŵ� Pcap��
#include <pcap.h>
#pragma comment(lib, "packet.lib")
#pragma comment(lib, "wpcap.lib")
//���߲���
const int DataPacketLen = 1206;										//���ݰ�����
pcap_t *pfile;														//�ļ����
bool openPcapFile(const char *fileName);							//���ļ��� ��ȡ�ļ��ľ��
void readPcapFile();												//����Pcap�ļ����� ������ļ���
bool isFinish = false;												//���Ž�������˳�
bool isWait = false;												//���ŵȴ��ı�ǣ� ��ֹ����������

int main()
{
	//��ͨ�ص�
	Fun fun = std::bind(callbackFunction_123, std::placeholders::_1, std::placeholders::_2);
	m_GetLidarData->setCallbackFunction(&fun);							//���ûص�����
	m_GetLidarData->LidarStar();										//��ʼ�����״�����

	//��pcap�ļ�
	if (!openPcapFile("E:/test.pcap"))
	{
		std::cout << "Pcap File Open Failed!" << std::endl;
		return 0;
	}
	isFinish = false;
	isWait = false;
	std::thread m_ReadPcapT(&readPcapFile);
	m_ReadPcapT.detach();

#pragma region   //��ȡ�����������㷨�Ĵ���  �� ������ʾ
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
			std::cout << "File Playing completed������" << std::endl;
			break;

		}
	}
#pragma endregion  

	m_GetLidarData->LidarStop();

	char m_getWait = getchar();
}

// �ص�����ָ��   ��ȡÿһ֡��������
void callbackFunction_123(std::vector<MuchLidarData> LidarDataValue, int Type)
{
	m_mutex.lock();
	m_LidarData = LidarDataValue;
	isWait = true;
	m_mutex.unlock();
}

//�� pcap�� ��ȡ���
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

//�߳̽��� pcap��,  ����������н���
void readPcapFile()
{
	pcap_pkthdr *pkthdr = 0;
	const u_char *pktdata = 0;
	unsigned char databuf[DataPacketLen];
	while (pcap_next_ex(pfile, &pkthdr, &pktdata) > 0)
	{
		if (pkthdr->caplen == (DataPacketLen + 42)
			&& !(pktdata[42] == 0xaa && pktdata[43] == 0x00 && pktdata[44] == 0xff && pktdata[45] == 0x11))					//�������ð�
		{
			pktdata = pktdata + 42;
			memcpy(databuf, pktdata, DataPacketLen);
			m_GetLidarData->CollectionDataArrive(databuf, DataPacketLen);												//�����ݴ��뵽����
			if (isWait)
			{
				Sleep(80);
				isWait = false;
			}
		}
	}
	isFinish = true;
	//����ֹͣ
	std::cout << "Pcap parsing Pcap file completed. Close file" << std::endl;
	pcap_close(pfile);
}

#pragma endregion 	

#endif