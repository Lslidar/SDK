#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define UsageMethod_1 1


#include <IncludeFile.h>

#if UsageMethod_1

#pragma region 		//method one, get one frame of data through flag bit
int main()
{
	m_GetLidarData->setPortAndIP(2368, 2369, "192.168.1.102");				//set the parameters of the Ethernet port which receives the lidar data（port number of data package, device package, destination IP)
	m_GetLidarData->LidarStart();											//start parsing lidar data

#pragma region //modify lidar parameters and parameters can be modified individually 
	//std::this_thread::sleep_for(std::chrono::milliseconds(2000));			//wait for a while, get device package
	//m_GetLidarData->setLidarRotateSpeed(600);
	//m_GetLidarData->setLidarIP("192.168.1.200");
	//m_GetLidarData->setComputerIP("192.168.1.102");
	//m_GetLidarData->setDataPort(2368);
	//m_GetLidarData->setDevPort(2369);

	//m_GetLidarData->setLidarRotateState(0);
	//m_GetLidarData->setLidarSoureSelection(0);
	//m_GetLidarData->setLidarWorkState(0);
	
	//m_GetLidarData->sendPackUDP();										//after setting parameters, "sendPackUDP" must be called to send UDP package
#pragma endregion 


	//get point cloud and process it with algorism   //circular process
#pragma region   //get point cloud and process it with different algorithm,or display it
	while (true)
	{
		//method one, get one frame of lidar data 
		if (m_GetLidarData->isFrameOK)
		{
			std::shared_ptr<std::vector<MuchLidarData>> m_LidarData_temp = m_GetLidarData->getLidarPerFrameDate();
			for (u_int m_FF = 0; m_FF < m_LidarData_temp->size(); m_FF++)
                {
                    std::cout.width(10);
                    std::cout <<(*m_LidarData_temp)[m_FF].X << " 	";
                    std::cout.width(10);
                    std::cout << (*m_LidarData_temp)[m_FF].Y << " 	";
                    std::cout.width(10);
                    std::cout << (*m_LidarData_temp)[m_FF].Z << " 	";
                    std::cout.width(10);
                    std::cout << (*m_LidarData_temp)[m_FF].ID << " 	";
					std::cout.width(20);
					std::cout << (*m_LidarData_temp)[m_FF].H_angle;
					std::cout.width(20);
					std::cout << (*m_LidarData_temp)[m_FF].Distance << " 	";
                    std::cout.width(20);
                    std::cout << (*m_LidarData_temp)[m_FF].Intensity << " 	";
					std::cout.width(20);
                    std::cout << (*m_LidarData_temp)[m_FF].Mtimestamp_nsce;

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

#pragma region 		//method two: get one frame data through callback function
std::mutex m_mutex;
std::shared_ptr<std::vector<MuchLidarData>> m_LidarData = nullptr;									//one frame data

//ordinary callback function
void callbackFunction_123(std::shared_ptr<std::vector<MuchLidarData>>, int);				//callback function，get data of each frame


class A
{
public:
	void callbackFunction(std::shared_ptr<std::vector<MuchLidarData>>, int);	//callback function, get data of each frame

	FunDataPrt fun;
	FunDataPrt* bind()
	{
		fun = std::bind(&A::callbackFunction, this, std::placeholders::_1, std::placeholders::_2);
		return &fun;
	}
};

int main()
{
	//class member function as callback function
	A m_a;
	FunDataPrt fun = std::bind(&A::callbackFunction, m_a, std::placeholders::_1, std::placeholders::_2);

	//normal callback function
	//FunDataPrt fun = std::bind(callbackFunction_123, std::placeholders::_1, std::placeholders::_2);
	
	m_GetLidarData->setCallbackFunction(&fun);							//set callback function
	m_GetLidarData->setPortAndIP(2368, 2369, "192.168.1.102");				//set the parameters of the Ethernet port which receives the lidar data（port number of data package, device package, destination IP)
	m_GetLidarData->LidarStart();										//start parsing lidar data

#pragma region //modify lidar parameters and parameters can be modified individually 
	//std::this_thread::sleep_for(std::chrono::milliseconds(2000));			//wait for a while, get device package
	//m_GetLidarData->setLidarRotateSpeed(600);
	//m_GetLidarData->setLidarIP("192.168.1.200");
	//m_GetLidarData->setComputerIP("192.168.1.102");
	//m_GetLidarData->setDataPort(2368);
	//m_GetLidarData->setDevPort(2369);

	//m_GetLidarData->setLidarRotateState(0);
	//m_GetLidarData->setLidarSoureSelection(0);
	//m_GetLidarData->setLidarWorkState(0);
	
	//m_GetLidarData->sendPackUDP();										//after setting parameters, "sendPackUDP" must be called to send UDP package
#pragma endregion 

#pragma region   //get point cloud and process it with different algorithm,or display it
	while (true)
	{
		if (m_LidarData != nullptr)
		{

			for (u_int m_FF = 0; m_FF < m_LidarData->size(); m_FF++)
			{
	                std::cout.width(10);
					std::cout << (*m_LidarData)[m_FF].X << " 	";
	                std::cout.width(10);
					std::cout << (*m_LidarData)[m_FF].Y << " 	";
	                std::cout.width(10);
					std::cout << (*m_LidarData)[m_FF].Z << " 	";
	                std::cout.width(10);
					std::cout << (*m_LidarData)[m_FF].ID << " 	";
					std::cout.width(20);
					std::cout << (*m_LidarData)[m_FF].H_angle;
					std::cout.width(20);
					std::cout << (*m_LidarData)[m_FF].Distance << " 	";
	                std::cout.width(20);
					std::cout << (*m_LidarData)[m_FF].Intensity << " 	";
					std::cout.width(20);
					std::cout << (*m_LidarData)[m_FF].Mtimestamp_nsce;
	   
	                std::cout << std::endl;
			}
			m_LidarData.reset();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
#pragma endregion

	m_GetLidarData->LidarStop();
}

// callback function pointer  get point cloud data of each frame
void A::callbackFunction(std::shared_ptr<std::vector<MuchLidarData>> LidarDataValue, int)
{
	m_mutex.lock();
	m_LidarData = LidarDataValue;
	m_mutex.unlock();
}

void callbackFunction_123(std::shared_ptr<std::vector<MuchLidarData>> LidarDataValue, int)
{
	m_mutex.lock();
	m_LidarData = LidarDataValue;
	m_mutex.unlock();
}
#pragma endregion 

#endif

