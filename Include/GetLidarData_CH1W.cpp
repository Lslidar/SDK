#include "GetLidarData_CH1W.h"

GetLidarData_CH1W::GetLidarData_CH1W() {
    for (int i = 0; i < 4; i++) {
        prismAngle[i] = i * 0;
    }

    for (int i = 0; i < 8; i++) {
     
        sinTheta_1[i] = sin(0);
        sinTheta_2[i] = sin(0);
        cosTheta_1[i] = cos(0);
        cosTheta_2[i] = cos(0);
        
    }
    
    count = 0;
}

GetLidarData_CH1W::~GetLidarData_CH1W() {
}

void GetLidarData_CH1W::LidarRun() {
    int count = 0;
    while (true) {
        if (isQuit) {
            clearQueue(allDataValue);
            break;
        }
        if (!allDataValue.empty()) {
            unsigned char data[1206] = {0};
            m_mutex.lock();
            memcpy(data, allDataValue.front(), sizeof(data));
            delete allDataValue.front();
            allDataValue.pop();
            m_mutex.unlock();

            if (data[0] == 0xa5 && data[1] == 0xff && data[2] == 0x00 && data[3] == 0x5a) {
                if (data[44] == 0x00) {   //gps授时
                    time_service_mode = "gps";
                } else if (data[44] == 0x01) { //ptp授时
                    time_service_mode = "gptp";
                }
    
                m_UTC_Time.year = data[52] + 2000;
                m_UTC_Time.month = data[53];
                m_UTC_Time.day = data[54];
                m_UTC_Time.hour = data[55];
                m_UTC_Time.minute = data[56];
                m_UTC_Time.second = data[57];
    
                std::vector<int> anglePrism;
                for (int i = 0; i < 5; i++) {
                    short int angleP = data[240 + i * 2] * 256 + data[241 + i * 2];
                    anglePrism.push_back(angleP);
                }
                getOffsetAngle(anglePrism);
                continue;
            }
    
            if (time_service_mode == "gps") {
                timestamp_nsce = (data[1200] * pow(2, 24) + data[1201] * pow(2, 16) + data[1202] * pow(2, 8) +
                                  data[1203] * pow(2, 0)) * pow(10, 3);
            } else if (time_service_mode == "gptp") {
                timestamp_nsce = (data[1200] * pow(2, 24) + data[1201] * pow(2, 16) + data[1202] * pow(2, 8) +
                                  data[1203] * pow(2, 0));
            }
            int lidar_EchoModel = data[1205];
            //double echo
            if (2 == lidar_EchoModel) {
                handleDoubleEcho(data);
            }
                //single echo
            else {
                handleSingleEcho(data);
            }
            lastAllTimestamp = allTimestamp;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void GetLidarData_CH1W::getOffsetAngle(std::vector<int> OffsetAngleValue) {
    double prismAngle[4];
    float PrismOffsetVAngle = 0.0f;

    if (abs(OffsetAngleValue[0]) != 0) {
        PrismOffsetVAngle = OffsetAngleValue[0] / 100.0;
    
        for (int i = 0; i < 8; i++) {
            //右边
            if (i / 4 % 2 == 0) {
                sinTheta_1[i] = sin(0);
                cosTheta_1[i] = cos(0);
            }
                //左边
            else {
                sinTheta_1[i] = sin(0 + PrismOffsetVAngle);
    
                cosTheta_1[i] = cos(0 + PrismOffsetVAngle);
            }
        }
    }
    
    if (abs(OffsetAngleValue[1]) == 0 && abs(OffsetAngleValue[2]) == 0 && abs(OffsetAngleValue[3]) == 0 &&
        abs(OffsetAngleValue[4]) == 0) {
        for (int i = 0; i < 4; i++) {
            prismAngle[i] = i * 0;
        }
        for (int i = 0; i < 8; i++) {
            sinTheta_2[i] = sin((prismAngle[i % 4]) * PI / 180);
            cosTheta_2[i] = cos((prismAngle[i % 4]) * PI / 180);
        }
    } else {
        for (int i = 0; i < 4; i++) {
            prismAngle[i] = OffsetAngleValue[i + 1] / 100.0;
        }
        for (int i = 0; i < 8; i++) {
            sinTheta_2[i] = sin((prismAngle[i % 4]) * PI / 180);
            cosTheta_2[i] = cos((prismAngle[i % 4]) * PI / 180);
        }
    }
    return;
}

void GetLidarData_CH1W::handleSingleEcho(unsigned char *data) {
    m_UTC_Time.hour = data[1197];
    m_UTC_Time.minute = data[1198];
    m_UTC_Time.second = data[1199];
    struct tm t;
    t.tm_sec = m_UTC_Time.second;
    t.tm_min = m_UTC_Time.minute;
    t.tm_hour = m_UTC_Time.hour;
    t.tm_mday = m_UTC_Time.day;
    t.tm_mon = m_UTC_Time.month - 1;
    t.tm_year = m_UTC_Time.year - 1900;
    t.tm_isdst = 0;
    time_t _t = static_cast<uint64_t>(timegm(&t)); //s//mktime(&t);
    if (-1 == _t) {
        perror("parse error");
    }

    allTimestamp = _t * pow(10, 9) + timestamp_nsce;
    PointIntervalT_ns = (allTimestamp - lastAllTimestamp) * 1.0 / 171.0;
    
    for (int i = 0; i < 1197; i = i + 7) {
        if (data[i] == 0xff && data[i + 1] == 0xaa && data[i + 2] == 0xbb && data[i + 3] == 0xcc &&
            data[i + 4] == 0xdd) {
            count++;
        }
        if (count == 1) {
            sendLidarData();
    
            count = 0;
            continue;
        }
        if (count == 0) {
            if (data[i] < 128) {
                float m_Distance = (data[i + 3] * 65536 + data[i + 4] * 256 + data[i + 5]) / 256.0 / ConversionUnit;
                int m_ID = data[i];
                float m_H_angle = (data[i + 1] * 256 + data[i + 2]) / 100.f;
                int m_Intensity = data[i + 6];
    
    			if (m_H_angle < 0 || m_H_angle > 180)
    			{
    				continue;
    			}
                m_PointXYZ m_point_1 = XYZ_calculate(m_ID, m_H_angle, m_Distance, -1);
                MuchLidarData m_DataT;
                m_DataT.X = m_point_1.x1;
                m_DataT.Y = m_point_1.y1;
                m_DataT.Z = m_point_1.z1;
                m_DataT.ID = m_ID;
                m_DataT.H_angle = m_H_angle;
                m_DataT.Distance = m_Distance;
                m_DataT.Intensity = m_Intensity;
                m_DataT.Mtimestamp_nsce = allTimestamp - PointIntervalT_ns * (170 - i / 7);
    			LidarPerFrameDatePrt_Get->push_back(m_DataT);
            }
        }
    }
}

void GetLidarData_CH1W::handleDoubleEcho(unsigned char *data) {
    m_UTC_Time.second = data[1199];
    struct tm t;
    t.tm_sec = m_UTC_Time.second;
    t.tm_min = m_UTC_Time.minute;
    t.tm_hour = m_UTC_Time.hour;
    t.tm_mday = m_UTC_Time.day;
    t.tm_mon = m_UTC_Time.month - 1;
    t.tm_year = m_UTC_Time.year - 1900;
    t.tm_isdst = 0;
    time_t _t = static_cast<uint64_t>(timegm(&t)); //s//mktime(&t);
    if (-1 == _t) {
        perror("parse error");
    }

    allTimestamp = _t * pow(10, 9) + timestamp_nsce;
    PointIntervalT_ns = (allTimestamp - lastAllTimestamp) * 1.0 / 171.0;
    
    for (int i = 0; i < 1199; i = i + 11) {
        if (data[i] == 0xff && data[i + 1] == 0xaa && data[i + 2] == 0xbb && data[i + 3] == 0xcc &&
            data[i + 4] == 0xdd) {
            count++;
        }
        if (count == 1) {
            sendLidarData();
    
            count = 0;
            continue;
        }
        if (count == 0) {
            if (data[i] < 128) {
                float m_Distance = (data[i + 3] * 65536 + data[i + 4] * 256 + data[i + 5]) / 256.0 / ConversionUnit;
                int m_ID = data[i];
                float m_H_angle = (data[i + 1] * 256 + data[i + 2]) / 100.f;
                int m_Intensity = data[i + 6];
    
    			float m_Distance_2 = (data[i + 7] * 65536 + data[i + 8] * 256 + data[i + 9]) / 256.0 / ConversionUnit;
    			int m_Intensity_2 = (data[i + 10]);
    
    			if (m_H_angle < 0 || m_H_angle > 180)
    			{
    				continue;
    			}
    
    			m_PointXYZ m_point = XYZ_calculate(m_ID, m_H_angle, m_Distance, m_Distance_2);
                MuchLidarData m_DataT;
    			m_DataT.X = m_point.x1;
    			m_DataT.Y = m_point.y1;
    			m_DataT.Z = m_point.z1;
                m_DataT.ID = m_ID;
                m_DataT.H_angle = m_H_angle;
                m_DataT.Distance = m_Distance;
                m_DataT.Intensity = m_Intensity;
                m_DataT.Mtimestamp_nsce = allTimestamp - PointIntervalT_ns * (108 - i / 11);
    			LidarPerFrameDatePrt_Get->push_back(m_DataT);


                MuchLidarData m_DataT_2;
    			m_DataT_2.X = m_point.x2;
    			m_DataT_2.Y = m_point.y2;
    			m_DataT_2.Z = m_point.z2;
                m_DataT_2.ID = m_ID;
                m_DataT_2.H_angle = m_H_angle;
                m_DataT_2.Distance = m_Distance_2;
                m_DataT_2.Intensity = m_Intensity_2;
                m_DataT_2.Mtimestamp_nsce = allTimestamp - PointIntervalT_ns * (108 - i / 11);
    			LidarPerFrameDatePrt_Get->push_back(m_DataT_2);
            }
        }
    }
}

m_PointXYZ GetLidarData_CH1W::XYZ_calculate(int ID, float H_angle, float Distance, float Distance2) {
    m_PointXYZ point;

    double cos_xita;
    double sin_xita;


    //right
    if (ID / 4 % 2 == 0) {
        cos_xita = cos((H_angle / 2.0 + 22.5) * PI / 180);
        sin_xita = sin((H_angle / 2.0 + 22.5) * PI / 180);
    } else {
        cos_xita = cos((-H_angle / 2.0 + 112.5) * PI / 180);
        sin_xita = sin((-H_angle / 2.0 + 112.5) * PI / 180);
    }
    
    //intermediate variable
    double _R_ = cosTheta_2[ID] * cosTheta_1[ID] * cos_xita - sinTheta_2[ID] * sinTheta_1[ID];
    
    //vertical angle
    double sin_Theta = sinTheta_1[ID] + 2 * _R_ * sinTheta_2[ID];
    double cos_Theta = sqrt(1 - pow(sin_Theta, 2));
    
    //horizontal angle
    double cos_H_xita = (2 * _R_ * cosTheta_2[ID] * cos_xita - cosTheta_1[ID]) / cos_Theta;
    
    //double sin_H_xita = sqrt(1 - pow(cos_H_xita, 2));
    
    double sin_H_xita = (2 * _R_ * cosTheta_2[ID] * sin_xita) / cos_Theta;
    
    double cos_xita_F = 0;
    double sin_xita_F = 0;
    
    float addDis = 0;
    //right
    if (ID / 4 % 2 == 0) {
        cos_xita_F = (cos_H_xita + sin_H_xita) * sqrt(0.5);
        sin_xita_F = (sin_H_xita - cos_H_xita) * sqrt(0.5);
    
        if (cos_xita_F > 1) {
            cos_xita_F = 1;
        }
    
        double xita_Hangle = acos(cos_xita_F) * 180 / PI;
        double xita_Hangle_new = -3.6636 * pow(10, -7) * pow(xita_Hangle, 3)
                                 + 5.2766 * pow(10, -5) * pow(xita_Hangle, 2)
                                 + 0.9885 * pow(xita_Hangle, 1)
                                 + 0.5894;
    
        cos_xita_F = cos(xita_Hangle_new * PI / 180);
        sin_xita_F = sin(xita_Hangle_new * PI / 180);
    
        addDis = 0.017;
    } else {
        cos_xita_F = (cos_H_xita + sin_H_xita) * (-sqrt(0.5));
        sin_xita_F = (sin_H_xita - cos_H_xita) * sqrt(0.5);
    
        if (cos_xita_F < -1) {
            cos_xita_F = -1;
        }
    
        double xita_Hangle = acos(cos_xita_F) * 180 / PI;
        double xita_Hangle_new = -3.6636 * pow(10, -7) * pow(xita_Hangle, 3)
                                 + 1.4507 * pow(10, -4) * pow(xita_Hangle, 2)
                                 + 0.9719 * pow(xita_Hangle, 1)
                                 + 1.9003;
    
        cos_xita_F = cos(xita_Hangle_new * PI / 180);
        sin_xita_F = sin(xita_Hangle_new * PI / 180);


        addDis = -0.017;
    }
    
    point.x1 = float(Distance * cos_Theta * cos_xita_F + addDis);
    point.y1= float(Distance * cos_Theta * sin_xita_F);
    point.z1= float(Distance * sin_Theta);
    
    if (Distance2 > 0)
    {
        point.x2 = float(Distance2 * cos_Theta * cos_xita_F + addDis);
        point.y2 = float(Distance2 * cos_Theta * sin_xita_F);
        point.z2 = float(Distance2 * sin_Theta);
    }
    
    return point;
}

#pragma region //set lidar paramters

bool GetLidarData_CH1W::setLidarRotateState(int RotateState) {
    if (setLidarParam()) {
        if (RotateState != 0 && RotateState != 1) {
            std::string str = "RotateState can only be equal to 0 or 1, please check the input parameters";
            messFunction(str, 0);
            return false;
        }
        //set port number of device package
        Rest_UCWP_buff[40] = 0x00;
        Rest_UCWP_buff[41] = RotateState;
        return true;
    } else {
        std::string str = "Equipment package is not update!!!";
        messFunction(str, 0);
        return false;
    }
}


bool GetLidarData_CH1W::setLidarSoureSelection(int StateValue) {
    if (setLidarParam()) {
        if (StateValue != 0 && StateValue != 1) {
            std::string str = "StateValue can only be equal to 0 or 1, please check the input parameters";
            messFunction(str, 0);
            return false;
        }
        Rest_UCWP_buff[44] = StateValue;
        return true;
    } else {
        std::string str = "Equipment package is not update!!!";
        messFunction(str, 0);
        return false;
    }
}

bool GetLidarData_CH1W::setLidarWorkState(int LidarState) {
    if (setLidarParam()) {
        if (LidarState != 0 && LidarState != 1) {
            std::string str = "LidarState can only be equal to 0 or 1, please check the input parameters";
            messFunction(str, 0);
            return false;
        }
        Rest_UCWP_buff[45] = LidarState;
        return true;
    } else {
        std::string str = "Equipment package is not update!!!";
        messFunction(str, 0);
        return false;
    }
}

#pragma endregion 
