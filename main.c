#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "can/can.h"
#include "setup.h"
#include "byteworker.h"
#include "timer.h"
#include "adc.h"


#define VERSION_MAJOR (1)
#define VERSION_MINOR (7)



void can_parse_msgs(can_t *msg);
void emergencyOff(void);
void msgSendInputStatus(uint32_t now, uint16_t heaterAdcRaw);
void msgSendState(uint8_t state, uint8_t lastState, uint8_t errorCode);
void msgSendVersion(void);
void msgMarker(uint8_t id);



int main(void) {
	wdt_disable();
	// The idea here is to have simple temperature regulation of the battery.
	// There are 2 temperatures to consider. The temperature of the heating
	// element and the internal temperature of the battery.
	// The target temperature for the battery is 20 degrees however the outer shell
	// is not conducting heat very efficiently. However heat transfer can be increased
	// by raising the temperature of the heater above the target temperature of the battery.
	// Temperatures of 60 degrees on the outer shell are acceptable.
	uint32_t now            = 0;
	uint32_t timeInputStatusMsg = 0;
	uint32_t timeStateMsg   = 0;
	uint32_t timeVersionMsg = 0;
	uint32_t timeHeaterStart= 0;
	uint16_t heaterAdcRaw   = 0;
	uint8_t  inRange        = FALSE;
	uint8_t  state          = eState_init;
	uint8_t  lastState      = eState_init;
	uint8_t  maxBatteryTemp = 0;
	uint8_t  minBatteryTemp = 0;
	can_t msgRx;

	timer_init();
	adc_init();
	bw_canInit(500 /*kbit*/);
	sei();

	wdt_enable(WDTO_250MS);
	wdt_reset();

	bw_ledSet(eBlueLed,  0);
	bw_ledSet(eGreenLed, 0);

	while( 1 ) {
		wdt_reset();

		now = timer_getMs();


		if(now > 0x80000000) {
			// force reboot when timestamps are getting too big
			// this prevents overflow and related inconsistent
			// behaviour.
			emergencyOff();
			while(1);
		}


		// get can messages
		while(can_check_message()) {
			can_get_message(&msgRx);

			switch(msgRx.id) {
				case eMsgId_batteryTempMsg: {
					// TODO: update min/max temp values
					break;
				}

				case eMsgId_forceState: {
					if(eState_cooling == msgRx.data[0]) {
						state = eState_cooling;
					}
					break;
				}

				case eMsgId_forceReboot: {
					emergencyOff();
					while(1);
					break;
				}

				default: {
					break;
				}
			} // switch
		} // while


		heaterAdcRaw = adc_read(eIn_heaterTemp);
//		uint16_t heaterAdc = adc_value2Temp(heaterAdcRaw);
		inRange = adc_tempInRange(heaterAdcRaw);


		if(TRUE != inRange) {
			state = eState_emergencyOff;
		}

		// TODO: check for sudden temperature jumps (sensor damaged?)

		// select new state
		switch(state) {
			case eState_init:    // fall through
			case eState_cooling: {
				if(heaterAdcRaw < eTempLowThresh) {
					state = eState_heating;
				}
				break;
			}

			case eState_heating: {
				if(now > (timeHeaterStart + eTimeout_heaterUseage)) {
					state = eState_emergencyOff; // heating phase too long
				}

				if(heaterAdcRaw > eTempHighThresh) {
					state = eState_cooling;
				}
				break;
			}

			case eState_forceOn: {
				// TODO: manual override via CAN
				break;
			}

			case eState_forceOff: {
				// manual override via CAN
				break;
			}

			case eState_emergencyOff: // fall through
			default: {
				state = eState_emergencyOff;
				// never regenerate from this state
				break;
			}
		}


		if(lastState != state) {
			msgSendState(state, lastState, 0); // report state changes
		}


		// react on state
		switch(state) {
			default:             // fall through
			case eState_init:    // fall through
			case eState_cooling: // fall through
			case eState_emergencyOff: {
				bw_outputSet(eOut_relais1, eOut_off);
				bw_outputSet(eOut_relais2, eOut_off);
				bw_ledSet(eBlueLed, 0);
				break;
			}

			case eState_forceOn: // fall through
			case eState_heating: {
				if(lastState != state) {
					timeHeaterStart = now;
				}
				bw_outputSet(eOut_relais1, eOut_on);
				bw_outputSet(eOut_relais2, eOut_on);
				bw_ledSet(eBlueLed, 1);
				break;
			}
		}
/*
		// calc temp
		// 106k : 24
		// 100k : 33
		//  90k : 40
		//  80k : 50
		//  70k : 60
		//  60k : 73
		//  50k : 83
		//  40k : 98

		//  50k : 83 <-- 0x212(530);
		//  60k : 73 <-- 0x1FC(508)
		//  80k : 50 <-- 0x1D1(465)
*/

		if(timeInputStatusMsg < now) {
			timeInputStatusMsg = now + eDelay_inputStatusMsgCycle;
			msgSendInputStatus(now, heaterAdcRaw);
		}

		if(timeVersionMsg < now) {
			timeVersionMsg = now + eDelay_versionMsgCycle;
			msgSendVersion();
		}

		if(timeStateMsg < now) {
			timeStateMsg = now + eDelay_stateMsgCycle;
			msgSendState(state, lastState, 0);
		}

		lastState = state;
		timer_wait(100);

	} // while(1)
}



void can_wakeBus( void ) {
#ifdef AUTO_WAKE_BUS
	// 423#03.00.FF.FF.00.E0.00.00
	can_t msg = {
		.id = eWakeBusMsg,
		.flags = { .rtr = 0, .extended = 0 },
		.length = 8,
		.data = { 0x03, 0x00, 0xFF, 0xFF, 0x00, 0xE0, 0x00, 0x00 }
	};

	can_send_message( &msg );
#endif
}



void emergencyOff(void) {
	// software detected a serious problem
	// make sure the heater stays off and
	// never turns back on!
	bw_outputSet(eOut_relais1, eOut_off);
	bw_outputSet(eOut_relais2, eOut_off);
}



void msgSendInputStatus(uint32_t now, uint16_t heaterAdcRaw) {
	can_t msg = {
		.id = eMsgId_reportInputStatus,
		.flags = { .rtr = 0, .extended = 1 },
		.length = 6,
		.data = {
			(now>>24) & 0xFF, // time since boot
			(now>>16) & 0xFF, // in milliseconds
			(now>> 8) & 0xFF, // in big endian
			(now)     & 0xFF,

			((heaterAdcRaw>>8) & 0xFF), // state of relais | raw heater ADC
			msg.data[5] = heaterAdcRaw & 0xFF,

		}
	};

	can_send_message(&msg);
}



void msgSendState(uint8_t state, uint8_t lastState, uint8_t errorCode) {
	can_t msg = {
		.id = eMsgId_reportState,
		.flags = { .rtr = 0, .extended = 1 },
		.length = 3,
		.data = {
			state,
			lastState,
			errorCode
		}
	};

	can_send_message(&msg);
}



void msgSendVersion(void) {
	can_t msg = {
		.id = eMsgId_reportVersion,
		.flags = { .rtr = 0, .extended = 1 },
		.length = 2,
		.data = { VERSION_MAJOR, VERSION_MINOR }
	};

	can_send_message(&msg);
}



void msgMarker(uint8_t id) {
	can_t msg = {
		.id = eMsgId_marker,
		.flags = { .rtr = 0, .extended = 1 },
		.length = 1,
		.data = { id }
	};

	can_send_message(&msg);
}
