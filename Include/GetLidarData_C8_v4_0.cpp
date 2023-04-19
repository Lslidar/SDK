#include "GetLidarData_C8_v4_0.h"

#define ConversionDistance_C_v4_0 	3.61 / 100.0 		//convert the optical center to structural center, convert the unit of centimeter to meter
#define ConversionAngle_C_v4_0	    20.25 				//convert the optical center to structural center, unit is degree
#define IDistanceACC_C_v4_0			0.4 / 100.0			//distance accuracy conversion, convert 0.4cm to 1m, data "1" means 0.4cm

GetLidarData_C8_v4_0::GetLidarData_C8_v4_0()
{
	float theta[8] = { -12, 4, -8, 8, -4, 10, 0, 12 };
	for (int i = 0; i < 8; i++)
	{
		cosTheta[i] = cos(theta[i] * PI / 180);
		sinTheta[i] = sin(theta[i] * PI / 180);
	}

	all_lidardata.clear();
	all_lidardata_d.clear();
	BlockAngle.clear();
	all_BlockAngle.clear();
	endFrameAngle = 90.0;					//Fix at 90 degree to analyze the data
	Model = 0;
}

GetLidarData_C8_v4_0::~GetLidarData_C8_v4_0()
{
}

void GetLidarData_C8_v4_0::LidarRun()
{
	while (true)
	{
		if (isQuit)
		{
			clearQueue(allDataValue);
			break;
		}
		if (!allDataValue.empty())
		{
			unsigned char data[1212] = { 0 };
			m_mutex.lock();
			memcpy(data, allDataValue.front(), 1212);
			delete allDataValue.front();
			allDataValue.pop();
			m_mutex.unlock();

			if (data[0] == 0xa5 && data[1] == 0xff && data[2] == 0x00 && data[3] == 0x5a)
			{
				continue;
			}

			timestamp_nsce = (data[1209] * pow(2, 24) + data[1208] * pow(2, 16) + data[1207] * pow(2, 8) + data[1206] * pow(2, 0));
            if (0xff == data[1200]) {    //ptp timing
                //std::cout << "ptp";
                uint64_t _t = (data[1201] * pow(2, 32) + data[1202] * pow(2, 24) +
                               data[1203] * pow(2, 16) +
                               data[1204] * pow(2, 8) + data[1205] * pow(2, 0));


                allTimestamp = _t * pow(10, 9) + timestamp_nsce;
            } else {          //gps timing
                m_UTC_Time.year = data[1200] + 2000;
                m_UTC_Time.month = data[1201];
                m_UTC_Time.day = data[1202];
                m_UTC_Time.hour = data[1203];
                m_UTC_Time.minute = data[1204];
                m_UTC_Time.second = data[1205];

                struct tm t;
                t.tm_sec = m_UTC_Time.second;
                t.tm_min = m_UTC_Time.minute;
                t.tm_hour = m_UTC_Time.hour;
                t.tm_mday = m_UTC_Time.day;
                t.tm_mon = m_UTC_Time.month - 1;
                t.tm_year = m_UTC_Time.year - 1900;
                t.tm_isdst = 0;
                time_t _t = static_cast<uint64_t>(timegm(&t));//mktime(&t);
                if (-1 == _t){
                    perror("parse error");
                }
                allTimestamp = _t * pow(10, 9) + timestamp_nsce;
            }

			int lidar_EchoModel = data[1210];
#pragma region	//judge echo mode analysis
			if (0x37 == lidar_EchoModel)
			{
				handleSingleEcho(data);
			}
			else if (0x39 == lidar_EchoModel)
			{
				handleDoubleEcho(data);
			}
			lastAllTimestamp = allTimestamp;
#pragma endregion
		}
		else
		{
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}


void GetLidarData_C8_v4_0::handleSingleEcho(unsigned char* data)
{
	if (data[0] == 0xff && data[1] == 0xee)
	{
		std::vector<MuchLidarData> lidardata_tmp;												//data point analysis structure
		lidardata_tmp.clear();
		bool isOneFrame = false;

		PointIntervalT_ns = (allTimestamp - lastAllTimestamp) * 1.0 / 384;
		for (int i = 0; i < 12; i++)
		{
			//Extract the value of azimuth of one circle
			float AngleBlock = (data[3 + 100 * i] * 256 + data[2 + 100 * i]) / 100.f;
			if (AngleBlock >= 360.0)
			{
				BlockAngle.push_back(AngleBlock - 360.0);
				all_BlockAngle.push_back(AngleBlock - 360.0);
			}
			else
			{
				BlockAngle.push_back(AngleBlock);
				all_BlockAngle.push_back(AngleBlock);
			}

			if (BlockAngle.size() >= 2)
			{
				for (int j = 0; j < 32; j++)
				{
					float mdiff = (BlockAngle.back() - BlockAngle[BlockAngle.size() - 2]);
					mdiff = mdiff >= 0 ? mdiff : mdiff + 360;

					double m_angle = (BlockAngle[BlockAngle.size() - 2] + (mdiff / 32) * j);
					if (m_angle < 360.0)
						m_DataT[j].H_angle = m_angle;
					else
						m_DataT[j].H_angle = m_angle - 360.0;

					m_PointXYZ m_point_1 = XYZ_calculate(m_DataT[j].ID, m_DataT[j].H_angle, m_DataT[j].Distance);
					m_DataT[j].X = m_point_1.x;
					m_DataT[j].Y = m_point_1.y;
					m_DataT[j].Z = m_point_1.z;

					if (all_BlockAngle.size() > 100)
					{
						if ((m_DataT[j].H_angle - endFrameAngle > 0) && abs(m_DataT[j].H_angle - endFrameAngle) < 20)
						{
							lidardata_tmp.push_back(m_DataT[j]);
							isOneFrame = true;
						}
						else
						{
							all_lidardata.push_back(m_DataT[j]);
						}
					}
					else
					{
						all_lidardata.push_back(m_DataT[j]);
					}
				}
			}

			for (int j = 0; j < 32; j++)
			{
				m_DataT[j].ID = j % 8;
				m_DataT[j].Distance = (data[5 + 3 * j + 100 * i] * 256 + data[4 + 3 * j + 100 * i]) * IDistanceACC_C_v4_0;
				m_DataT[j].Intensity = data[6 + 3 * j + 100 * i];
				m_DataT[j].Mtimestamp_nsce = lastAllTimestamp + (32 * i + j + 1) * PointIntervalT_ns;
			}

			if (i == 11)
			{
				float mdiff = (BlockAngle.back() - BlockAngle[BlockAngle.size() - 2]);
				mdiff = mdiff >= 0 ? mdiff : mdiff + 360;
				for (int j = 0; j < 32; j++)
				{
					double m_angle = (BlockAngle.back() + (mdiff / 32) * j);
					if (m_angle < 360.0)
						m_DataT[j].H_angle = m_angle;
					else
						m_DataT[j].H_angle = m_angle - 360.0;

					m_PointXYZ m_point_1 = XYZ_calculate(m_DataT[j].ID, m_DataT[j].H_angle, m_DataT[j].Distance);
					m_DataT[j].X = m_point_1.x;
					m_DataT[j].Y = m_point_1.y;
					m_DataT[j].Z = m_point_1.z;

					if (all_BlockAngle.size() > 100)
					{
						if ((m_DataT[j].H_angle - endFrameAngle > 0) && abs(m_DataT[j].H_angle - endFrameAngle) < 20)
						{
							lidardata_tmp.push_back(m_DataT[j]);
							isOneFrame = true;
						}
						else
						{
							all_lidardata.push_back(m_DataT[j]);
						}
					}
					else
					{
						all_lidardata.push_back(m_DataT[j]);
					}
				}
				BlockAngle.clear();
			}
		}


		if (isOneFrame)
		{
			sendLidarData();
			all_BlockAngle.clear();

			if (lidardata_tmp.size() > 0)
			{
				for (size_t SF = 0; SF < lidardata_tmp.size(); SF++)
				{
					all_lidardata.push_back(lidardata_tmp[SF]);
				}
			}
		}
	}
}


void GetLidarData_C8_v4_0::handleDoubleEcho(unsigned char* data)
{
	if (data[0] == 0xff && data[1] == 0xee)
	{
		std::vector<MuchLidarData> lidardata_tmp, lidardata_tmp_d;												//data point analysis structure	
		lidardata_tmp.clear();
		lidardata_tmp_d.clear();

		bool isOneFrame = false;
		PointIntervalT_ns = (allTimestamp - lastAllTimestamp) * 1.0 / 384 / 2;

		for (int i = 0; i < 12; i += 2)
		{
			//Extract the value of azimuth of one circle
			float AngleBlock = (data[3 + 100 * i] * 256 + data[2 + 100 * i]) / 100.f;
			if (AngleBlock >= 360.0)
			{
				BlockAngle.push_back(AngleBlock - 360.0);
				all_BlockAngle.push_back(AngleBlock - 360.0);
			}
			else
			{
				BlockAngle.push_back(AngleBlock);
				all_BlockAngle.push_back(AngleBlock);
			}

			if (BlockAngle.size() >= 2)
			{
				for (int j = 0; j < 32; j++)
				{
					float mdiff = (BlockAngle.back() - BlockAngle[BlockAngle.size() - 2]);
					mdiff = mdiff >= 0 ? mdiff : mdiff + 360;

					double m_angle = (BlockAngle[BlockAngle.size() - 2] + (mdiff / 32) * j);
					if (m_angle < 360.0)
					{
						m_DataT[j].H_angle = m_angle;
						m_DataT_d[j].H_angle = m_angle;
					}
					else
					{
						m_DataT[j].H_angle = m_angle - 360.0;
						m_DataT_d[j].H_angle = m_angle - 360.0;
					}


					m_PointXYZ m_point_1 = XYZ_calculate(m_DataT[j].ID, m_DataT[j].H_angle, m_DataT[j].Distance);
					m_DataT[j].X = m_point_1.x;
					m_DataT[j].Y = m_point_1.y;
					m_DataT[j].Z = m_point_1.z;

					m_PointXYZ m_point_d = XYZ_calculate(m_DataT_d[j].ID, m_DataT_d[j].H_angle, m_DataT_d[j].Distance);
					m_DataT_d[j].X = m_point_d.x;
					m_DataT_d[j].Y = m_point_d.y;
					m_DataT_d[j].Z = m_point_d.z;


					if (all_BlockAngle.size() > 100)
					{
						if ((m_DataT[j].H_angle - endFrameAngle > 0) && abs(m_DataT[j].H_angle - endFrameAngle) < 20)
						{
							lidardata_tmp.push_back(m_DataT[j]);
							lidardata_tmp_d.push_back(m_DataT_d[j]);
							isOneFrame = true;
						}
						else
						{
							all_lidardata.push_back(m_DataT[j]);
							all_lidardata_d.push_back(m_DataT_d[j]);
						}
					}
					else
					{
						all_lidardata.push_back(m_DataT[j]);
						all_lidardata_d.push_back(m_DataT_d[j]);
					}
				}
			}

			for (int j = 0; j < 32; j++)
			{
				m_DataT[j].ID = j % 8;
				m_DataT[j].Distance = (data[5 + 3 * j + 100 * i] * 256 + data[4 + 3 * j + 100 * i]) * IDistanceACC_C_v4_0;
				m_DataT[j].Intensity = data[6 + 3 * j + 100 * i];
				m_DataT[j].Mtimestamp_nsce = lastAllTimestamp + (32 * (i / 2) + j + 1) * PointIntervalT_ns;

				m_DataT_d[j].ID = j % 8;
				m_DataT_d[j].Distance = (data[5 + 3 * j + 100 * (i + 1)] * 256 + data[4 + 3 * j + 100 * (i + 1)]) * IDistanceACC_C_v4_0;
				m_DataT_d[j].Intensity = data[6 + 3 * j + 100 * (i + 1)];
				m_DataT_d[j].Mtimestamp_nsce = lastAllTimestamp + (32 * (i / 2) + j + 1) * PointIntervalT_ns;
			}


			if (i == 10)
			{
				float mdiff = (BlockAngle.back() - BlockAngle[BlockAngle.size() - 2]);
				mdiff = mdiff >= 0 ? mdiff : mdiff + 360;
				for (int j = 0; j < 32; j++)
				{
					double m_angle = (BlockAngle.back() + (mdiff / 32) * j);
					if (m_angle < 360.0)
						m_DataT[j].H_angle = m_angle;
					else
						m_DataT[j].H_angle = m_angle - 360.0;

					m_PointXYZ m_point_1 = XYZ_calculate(m_DataT[j].ID, m_DataT[j].H_angle, m_DataT[j].Distance);
					m_DataT[j].X = m_point_1.x;
					m_DataT[j].Y = m_point_1.y;
					m_DataT[j].Z = m_point_1.z;

					m_PointXYZ m_point_d = XYZ_calculate(m_DataT_d[j].ID, m_DataT_d[j].H_angle, m_DataT_d[j].Distance);
					m_DataT_d[j].X = m_point_d.x;
					m_DataT_d[j].Y = m_point_d.y;
					m_DataT_d[j].Z = m_point_d.z;

					if (all_BlockAngle.size() > 100)
					{
						if ((m_DataT[j].H_angle - endFrameAngle > 0) && abs(m_DataT[j].H_angle - endFrameAngle) < 20)
						{
							lidardata_tmp.push_back(m_DataT[j]);
							lidardata_tmp_d.push_back(m_DataT_d[j]);
							isOneFrame = true;
						}
						else
						{
							all_lidardata.push_back(m_DataT[j]);
							all_lidardata_d.push_back(m_DataT_d[j]);
						}
					}
					else
					{
						all_lidardata.push_back(m_DataT[j]);
						all_lidardata_d.push_back(m_DataT_d[j]);
					}

				}
				BlockAngle.clear();
			}
		}

		if (isOneFrame)
		{
			sendLidarData();
			all_BlockAngle.clear();

			if (lidardata_tmp.size() > 0)
			{
				for (size_t SF = 0; SF < lidardata_tmp.size(); SF++)
				{
					all_lidardata.push_back(lidardata_tmp[SF]);
					all_lidardata_d.push_back(lidardata_tmp_d[SF]);
				}
			}
		}
	}
}

m_PointXYZ GetLidarData_C8_v4_0::XYZ_calculate(int ID, double H_angle, double Distance)
{
	m_PointXYZ point;

	point.x = float(Distance * cosTheta[ID] * sin((H_angle)* PI / 180) +
		ConversionDistance_C_v4_0 * sin((H_angle - ConversionAngle_C_v4_0) * PI / 180));

	point.y = float(Distance * cosTheta[ID] * cos(H_angle * PI / 180) +
		ConversionDistance_C_v4_0 * cos((H_angle - ConversionAngle_C_v4_0) * PI / 180));

	point.z = float(Distance * sinTheta[ID]);

	return point;
}


#pragma region //set Lidar parameters

bool GetLidarData_C8_v4_0::setLidarRotateState(int RotateState)
{
	if (setLidarParam())
	{
        if (RotateState !=0 && RotateState !=1) {
            std::string str = "RotateState can only be equal to 0 or 1, please check the input parameters";
            messFunction(str, 0);
            return false;
        }
		//set the port number of device package 
		Rest_UCWP_buff[40] = 0x00;
		Rest_UCWP_buff[41] = RotateState;
		return true;
	}
	else
	{
		std::string str = "Equipment package is not update!!!";
		messFunction(str, 0);
		return false;
	}
}


bool GetLidarData_C8_v4_0::setLidarSoureSelection(int StateValue)
{
	if (setLidarParam())
	{
        if (StateValue !=0 && StateValue !=1 && StateValue !=2) {
            std::string str = "StateValue can only be equal to 0 or 1, please check the input parameters";
            messFunction(str, 0);
            return false;
        }
		Rest_UCWP_buff[44] = 0x00;
		Rest_UCWP_buff[45] = StateValue;
		return true;
	}
	else
	{
		std::string str = "Equipment package is not update!!!";
		messFunction(str, 0);
		return false;
	}
}

bool GetLidarData_C8_v4_0::setLidarWorkState(int LidarState)
{
	if (setLidarParam())
	{
		Rest_UCWP_buff[50] = LidarState;
		return true;
	}
	else
	{
		std::string str = "Equipment package is not update!!!";
		messFunction(str, 0);
		return false;
	}
}

#pragma endregion 