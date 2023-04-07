#pragma once

#include "GetLidarData.h"

class GetLidarData_C1_v4_0 : public GetLidarData
{
public:
	GetLidarData_C1_v4_0();
	~GetLidarData_C1_v4_0();

	void LidarRun();
	m_PointXYZ XYZ_calculate(int, double, double);	

	bool setLidarRotateState(int StateValue);							//Lidar rotate/stop
	bool setLidarSoureSelection(int StateValue);						//time source selection
	bool setLidarWorkState(int StateValue);								//Lidar state, low power mode or not

private:
	double cosTheta[8];
	double sinTheta[8];
	MuchLidarData m_DataT[32];
	MuchLidarData m_DataT_d[32];

	double endFrameAngle = 0;										//angle of the end frame which is also the starting angle
	std::vector<float>BlockAngle;									//angle of each block
	std::vector<float>all_BlockAngle;								//angles of all blocks in each frame

	void handleSingleEcho(unsigned char* data);						//single echo handling
	void handleDoubleEcho(unsigned char* data);						//double echo handling
};

