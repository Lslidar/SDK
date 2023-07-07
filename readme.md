# Instructions

LSlidar  linux and windows  c++ demo

## Dependence

pcl ： sudo apt-get install libpcl-dev

refer to :https://github.com/PointCloudLibrary/pcl

## Compiling and Running

compiling process under Linux system

switch to the directory that include driver files
mkdir build && cd build
cmake ..
make -j4

get data demo： ./demo

visualize demo： ./demo_viewer



compile process under Windows system：
use Visual Studio to compile

##create multiple lidars
//e.g. create multiple LS series lidars
	GetLidarData* m_GetLidarData_1 = new GetLidarData_LS;				//  create the 1st LS class
	m_GetLidarData_1->setPortAndIP(2368, 2369, “192.168.1.102”);		//set the parameters of the Ethernet port that receives lidar data
	m_GetLidarData_1->LidarStar();										//start the program to receive data
	

	GetLidarData* m_GetLidarData_2 = new GetLidarData_LS;				// create the 2nd LS class
	m_GetLidarData_2->setPortAndIP(2370, 2371, “192.168.1.102”);		//set the parameters of the Ethernet port that receives lidar data
	m_GetLidarData_2->LidarStar();										//start the program to receive data
	
	GetLidarData* m_GetLidarData_3 = new GetLidarData_LS;				//  create the 3rd LS class
	m_GetLidarData_3->setPortAndIP(2380, 2381, “192.168.1.102”);		//et the parameters of the Ethernet port that receives lidar data
	m_GetLidarData_3->LidarStar();										//start the program to receive data


​	
​	
	//Stop at the end of use
	m_GetLidarData_1->LidarStop()；
	m_GetLidarData_2->LidarStop();
	m_GetLidarData_3->LidarStop();