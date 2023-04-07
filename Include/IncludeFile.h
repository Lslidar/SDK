#pragma once

#include <iostream>
#include "GetLidarData.cpp"

#define GetLidarData_C1_v4_0_MACRO

#ifdef GetLidarData_CH16_MACRO
#include "GetLidarData_CH16.h"
#include "GetLidarData_CH16.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH16;						//create category of CH16
#endif

#ifdef GetLidarData_CH32_MACRO
#include "GetLidarData_CH32.h"
#include "GetLidarData_CH32.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH32;						//create category of CH32
#endif

#ifdef GetLidarData_CH32_WideAngle_MACRO
#include "GetLidarData_CH32_WideAngle.h"
#include "GetLidarData_CH32_WideAngle.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH32_WideAngle;			//create category of CH32_WideAngle
#endif

#ifdef GetLidarData_CH64_MACRO
#include "GetLidarData_CH64.h"
#include "GetLidarData_CH64.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH64;						//create category of CH64
#endif

#ifdef GetLidarData_CH120_MACRO
#include "GetLidarData_CH120.h"
#include "GetLidarData_CH120.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH120;						//create category of CH120
#endif

#ifdef GetLidarData_CH128_MACRO
#include "GetLidarData_CH128.h"
#include "GetLidarData_CH128.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH128;						//create category of CH128
#endif


#ifdef GetLidarData_CH128x1_MACRO
#include "GetLidarData_CH128x1.h"
#include "GetLidarData_CH128x1.cpp"
		 GetLidarData* m_GetLidarData = new GetLidarData_CH128x1;				//create category of CH128x1
#endif

#ifdef GetLidarData_CH16x1_MACRO
#include "GetLidarData_CH16x1.h"
#include "GetLidarData_CH16x1.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CH16x1;			// create category of CH16x1
#endif

#ifdef GetLidarData_CH128S1_MACRO
#include "GetLidarData_CH128S1.h"
#include "GetLidarData_CH128S1.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CH128S1;			// create category of CH128S1
#endif

#ifdef GetLidarData_CH64w_MACRO
#include "GetLidarData_CH64w.h"
#include "GetLidarData_CH64w.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CH64w;				// create category of CH64w
#endif


#ifdef GetLidarData_C16_v3_0_MACRO
#include "GetLidarData_C16_v3_0.h"
#include "GetLidarData_C16_v3_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C16_v3_0;			// create category of C16_v3_0
#endif

#ifdef GetLidarData_C32_v3_0_MACRO
#include "GetLidarData_C32_v3_0.h"
#include "GetLidarData_C32_v3_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C32_v3_0;			// create category of C32_v3_0
#endif

#ifdef GetLidarData_C16_v4_0_MACRO
#include "GetLidarData_C16_v4_0.h"
#include "GetLidarData_C16_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C16_v4_0;			// create category of C16_v4_0
#endif

#ifdef GetLidarData_C32_v4_0_MACRO
#include "GetLidarData_C32_v4_0.h"
#include "GetLidarData_C32_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C32_v4_0;			// create category of C32_v4_0
#endif

#ifdef GetLidarData_C8_v4_0_MACRO
#include "GetLidarData_C8_v4_0.h"
#include "GetLidarData_C8_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C8_v4_0;			// create category of C8_v4_0
#endif

#ifdef GetLidarData_C1_v4_0_MACRO
#include "GetLidarData_C1_v4_0.h"
#include "GetLidarData_C1_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C1_v4_0;			// create category of C1_v4_0
#endif

#ifdef GetLidarData_C32_70D_v4_0_MACRO
#include "GetLidarData_C32_70D_v4_0.h"
#include "GetLidarData_C32_70D_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C32_70D_v4_0;		// create category of C32_70D_v4_0
#endif

#ifdef GetLidarData_C32_90D_v4_0_MACRO
#include "GetLidarData_C32_90D_v4_0.h"
#include "GetLidarData_C32_90D_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C32_90D_v4_0;		// create category of C32_90D_v4_0
#endif

#ifdef GetLidarData_MS06_MACRO
#include "GetLidarData_MS06.h"
#include "GetLidarData_MS06.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_MS06;				// create category of MS06
#endif

#ifdef GetLidarData_LS128_MACRO
#include "GetLidarData_LS128.h"
#include "GetLidarData_LS128.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_LS128;				// create category of LS128
#endif

#ifdef GetLidarData_N301_L6_v1_7MACRO      
#include "GetLidarData_N301.h"
#include "GetLidarData_N301.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_N301(4, 4, 7);						//create category of N301 //6.0 lidar, 1.7 protocol
#endif

#ifdef GetLidarData_N301_L5_v1_7MACRO     
#include "GetLidarData_N301.h"
#include "GetLidarData_N301.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_N301(4, 1, 7);					//create category of N301 //5.0 lidar, 1.7 protocol
#endif

#ifdef GetLidarData_N301_L5_v1_6MACRO      
#include "GetLidarData_N301.h"
#include "GetLidarData_N301.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_N301(2, 1, 6);						//create category of N301 //5.0 lidar, 1.6 protocol
#endif

#ifdef LINUX
#else
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif