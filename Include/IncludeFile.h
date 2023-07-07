#pragma once

#include <iostream>
#include "GetLidarData.cpp"

#define GetLidarData_CH1W_MACRO												//Predefinition, fill in the type of lidar to be used in respond to the lidars below

//create class of HS1
#ifdef GetLidarData_HS1_MACRO
#include "GetLidarData_HS1.h"
#include "GetLidarData_HS1.cpp"													/ *.vcxproj  doesnt add .cpp file； if added here, then there will be repeative definition
GetLidarData* m_GetLidarData = new GetLidarData_HS1;							//create class of HS1
#endif

//create class of CH16
#ifdef GetLidarData_CH16_MACRO
#include "GetLidarData_CH16.h"
#include "GetLidarData_CH16.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH16;						//create class of CH16
#endif

//create class of CH32 
#ifdef GetLidarData_CH32_MACRO
#include "GetLidarData_CH32.h"
#include "GetLidarData_CH32.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH32;						//create class of CH32 
#endif

//create class of _CH32_WideAngle
#ifdef GetLidarData_CH32_WideAngle_MACRO
#include "GetLidarData_CH32_WideAngle.h"
#include "GetLidarData_CH32_WideAngle.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH32_WideAngle;			//create class of CH32_WideAngle
#endif

//create class of CH64
#ifdef GetLidarData_CH64_MACRO
#include "GetLidarData_CH64.h"
#include "GetLidarData_CH64.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH64;						//create class of CH64
#endif

//create class of CH120
#ifdef GetLidarData_CH120_MACRO
#include "GetLidarData_CH120.h"
#include "GetLidarData_CH120.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH120;						//create class of CH120
#endif

//create class of CH128
#ifdef GetLidarData_CH128_MACRO
#include "GetLidarData_CH128.h"
#include "GetLidarData_CH128.cpp"
	 GetLidarData* m_GetLidarData = new GetLidarData_CH128;						//create class of CH128
#endif

//create class of CH128x1
#ifdef GetLidarData_CH128x1_MACRO
#include "GetLidarData_CH128x1.h"
#include "GetLidarData_CH128x1.cpp"
		 GetLidarData* m_GetLidarData = new GetLidarData_CH128x1;				//create class of CH128x1
#endif

// create class of CH16x1
#ifdef GetLidarData_CH16x1_MACRO
#include "GetLidarData_CH16x1.h"
#include "GetLidarData_CH16x1.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CH16x1;			// create class of CH16x1
#endif

// create class of CH128S1
#ifdef GetLidarData_CH128S1_MACRO
#include "GetLidarData_CH128S1.h"
#include "GetLidarData_CH128S1.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CH128S1;			//  create class of CH128S1
#endif

			 // create class of CX128S2
#ifdef GetLidarData_CX128S2_MACRO
#include "GetLidarData_CX128S2.h"
#include "GetLidarData_CX128S2.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CX128S2;			// create class of CX128S2
#endif

			 // create class of CX126S3
#ifdef GetLidarData_CX126S3_MACRO
#include "GetLidarData_CX126S3.h"
#include "GetLidarData_CX126S3.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CX126S3;			// create class of CX126S3
#endif

			 //create class of CX126S3_6， only display the data of the 6 lines in the middle of S3 
#ifdef GetLidarData_CX126S3_6_MACRO
#include "GetLidarData_CX126S3_6.h"
#include "GetLidarData_CX126S3_6.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CX126S3_6;			// create class of CX126S3_6
#endif

// create class of CH64w
#ifdef GetLidarData_CH64w_MACRO
#include "GetLidarData_CH64w.h"
#include "GetLidarData_CH64w.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CH64w;				// create class of CH64w
#endif

			 // create class of CH64w_A
#ifdef GetLidarData_CH64w_A_MACRO
#include "GetLidarData_CH64w_A.h"
#include "GetLidarData_CH64w_A.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CH64w_A;				//create class of CH64w_A
#endif

// create class of CH1W
#ifdef GetLidarData_CH1W_MACRO
#include "GetLidarData_CH1W.h"
#include "GetLidarData_CH1W.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_CH1W;				// create class of CH1W
#endif

//create class of C16_v3_0
#ifdef GetLidarData_C16_v3_0_MACRO
#include "GetLidarData_C16_v3_0.h"
#include "GetLidarData_C16_v3_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C16_v3_0;			// create class of C16_v3_0
#endif

// create class of C32_v3_0
#ifdef GetLidarData_C32_v3_0_MACRO
#include "GetLidarData_C32_v3_0.h"
#include "GetLidarData_C32_v3_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C32_v3_0;			//  create class of C32_v3_0
#endif

//  create class of C16_v4_0
#ifdef GetLidarData_C16_v4_0_MACRO
#include "GetLidarData_C16_v4_0.h"
#include "GetLidarData_C16_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C16_v4_0;			// create class of C16_v4_0
#endif

// create class of C32_v4_0
#ifdef GetLidarData_C32_v4_0_MACRO
#include "GetLidarData_C32_v4_0.h"
#include "GetLidarData_C32_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C32_v4_0;			// create class of C32_v4_0
#endif

// create class of C8_v4_0
#ifdef GetLidarData_C8_v4_0_MACRO
#include "GetLidarData_C8_v4_0.h"
#include "GetLidarData_C8_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C8_v4_0;			// create class of C8_v4_0
#endif

// create class of C1_v4_0
#ifdef GetLidarData_C1_v4_0_MACRO
#include "GetLidarData_C1_v4_0.h"
#include "GetLidarData_C1_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C1_v4_0;			// create class of C1_v4_0
#endif

//  create class of C32_70D_v4_0
#ifdef GetLidarData_C32_70D_v4_0_MACRO
#include "GetLidarData_C32_70D_v4_0.h"
#include "GetLidarData_C32_70D_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C32_70D_v4_0;		//  create class of C32_70D_v4_0
#endif

// create class of C32W_v4_0
#ifdef GetLidarData_C32W_v4_0_MACRO
#include "GetLidarData_C32W_v4_0.h"
#include "GetLidarData_C32W_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C32W_v4_0;		// create class of C32W_v4_0
#endif

//  create class of C32_90D_v4_0
#ifdef GetLidarData_C32_90D_v4_0_MACRO
#include "GetLidarData_C32_90D_v4_0.h"
#include "GetLidarData_C32_90D_v4_0.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_C32_90D_v4_0;		// create class of C32_90D_v4_0
#endif

// create class of LS
#ifdef GetLidarData_LS_MACRO
#include "GetLidarData_LS.h"
#include "GetLidarData_LS.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_LS;				// create class of LS
#endif

			 // create class of LS
#ifdef GetLidarData_MS06_MACRO
#include "GetLidarData_MS06.h"
#include "GetLidarData_MS06.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_MS06;				// create class of LS
#endif

//create class of N301 //v 6.0  protocol 1.7
#ifdef GetLidarData_N301_L6_v1_7MACRO      
#include "GetLidarData_N301.h"
#include "GetLidarData_N301.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_N301(4, 4, 7);						//create class of N301 //v 6.0  protocol 1.7
#endif

//create class of N301 //v 5.0  protocol 1.7
#ifdef GetLidarData_N301_L5_v1_7MACRO     
#include "GetLidarData_N301.h"
#include "GetLidarData_N301.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_N301(4, 1, 7);					//create class of N301 //v 5.0  protocol 1.7
#endif

//create class of N301 //v 5.0  protocol 1.6
#ifdef GetLidarData_N301_L5_v1_6MACRO      
#include "GetLidarData_N301.h"
#include "GetLidarData_N301.cpp"
			 GetLidarData* m_GetLidarData = new GetLidarData_N301(2, 1, 6);						//create class of N301 //v 5.0  protocol 1.6
#endif

#ifdef LINUX
#else
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif