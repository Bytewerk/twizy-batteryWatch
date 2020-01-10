#ifndef __setup_H__
#define __setup_H__

#define TEMP_GRAD_MUL (-11)
#define TEMP_GRAD_DIV (10)

#define TRUE  (0xA5)
#define FALSE (0x5A)



enum msgIds_e {
	// TX
	eMsgId_reportInputStatus = 0x18000011,
	eMsgId_reportVersion     = 0x18000012,
	eMsgId_reportState       = 0x18000013,
	eMsgId_marker            = 0x18000020,

	// RX
	eMsgId_forceState     = 0x18000001,
	eMsgId_forceReboot    = 0x180000FF,
	eMsgId_batteryTempMsg = 0x0554,
//	eWakeBusMsg     = 0x423,
};



enum timings_e {
	eTimeout_heaterUseage      = 90000, // ms
	eDelay_inputStatusMsgCycle = 1000,
	eDelay_versionMsgCycle     = 10000,
	eDelay_stateMsgCycle       = 10000,
};




enum {
	eBlueLed   = 1, // blue led
	eGreenLed  = 0, // green led

	eOut_off = 0,
	eOut_on  = 1,
};

enum channels_e {
	eIn_heaterTemp = 6, // adc channel
	eOut_relais1   = 0,
	eOut_relais2   = 1,
};

enum temperatures_e {
	eTempLowLimit    = 0x80,
	eTempLowThresh   = 0x1D1, // 50c
	eTempHighThresh  = 0x1FC, // 73c
	eTempHighLimit   = 0x212, // 80c
	eTempTarget      = 0x1DE, // 60c (estimation)
};



enum algoConstants_e {
	eTempFilterIterations = 3,
};



enum states_e {
	eState_init          = 0x00,
	eState_heating       = 0x11,
	eState_cooling       = 0x10,
	eState_emergencyOff  = 0x80,
	eState_forceOn       = 0x41,
	eState_forceOff      = 0x40,
};




/*
# 12V
110k : 25c
105k : 26c
103k : 28c
 98k : 29c
 95k : 30c
 90k : 33c
 86k : 35c
 80k : 37c
 74k : 40c
 66k : 43c
 55k : 46c

# 230V
106k : 24
100k : 33
 90k : 40
 80k : 50
 70k : 60
 60k : 73
 50k : 83
 40k : 98
*/



#else
#error "double include of setup.h"
#endif
