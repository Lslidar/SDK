#pragma once

#include "GetLidarData.h"

class GetLidarData_CH1W : public GetLidarData
{
public:
	GetLidarData_CH1W();
	~GetLidarData_CH1W();

	void LidarRun();
	m_PointXYZ XYZ_calculate(int, float, float, float Distance2);	//Coordinate conversion formula()  tDistance2： <=0 , then not calculate the data of the 2nd echo
	
	bool setLidarRotateState(int StateValue);							//lidar rotate or stay  still
	bool setLidarSoureSelection(int StateValue);						//clock source selection
	bool setLidarWorkState(int StateValue);								//lidar operating mode, in low-power mode or not

private:
	double prismAngle[4];										// prism angle
	float sinTheta_1[8];										//vertical angle
	float sinTheta_2[8];										//vertical angle
	float cosTheta_1[8];										//vertical angle
	float cosTheta_2[8];										//vertical angle

	void getOffsetAngle(std::vector<int> OffsetAngleValue);		//get offset angle
	
	int count = 0;												//judge synchronization frame 
	void handleSingleEcho(unsigned char* data);					//single echo handling
	void handleDoubleEcho(unsigned char* data);					//double echo handling
};

