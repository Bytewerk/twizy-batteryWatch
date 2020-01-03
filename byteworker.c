#include <stdint.h>
#include <avr/io.h>
#include "can/can.h"
#include "byteworker.h"



//
// I/O
//
void bw_ledSet(uint8_t ledId, uint8_t state) {
	switch(ledId) {
		case 0: {
			PORTC &= ~(1<<PC5);
			DDRC  |=  (1<<PC5);

			if(state) {
				PORTC |= (1<<PC5);
			}
			break;
		}

		case 1: {
			PORTC &= ~(1<<PC6);
			DDRC  |=  (1<<PC6);

			if(state) {
				PORTC |= (1<<PC6);
			}
			break;
		}

		default: {
			break;
		}
	}
}



void bw_ledToggle(uint8_t ledId) {
	switch(ledId) {
		case 0: {
			PORTC ^= (1<<PC5);
			break;
		}

		case 1: {
			PORTC ^= (1<<PC6);
			break;
		}

		default: {
			break;
		}
	}
}



void bw_outputSet(uint8_t channel, uint8_t state) {
	switch(channel) {
		case 0: {
			PORTB &= ~(1<<PB6);
			DDRB  |=  (1<<PB6);

			if(state) {
				PORTB |= (1<<PB6);
			}
			break;
		}

		case 1: {
			PORTC &= ~(1<<PC1);
			DDRC  |=  (1<<PC1);

			if(state) {
				PORTC |= (1<<PC1);
			}
			break;
		}

		case 2: {
			PORTD &= ~(1<<PD7);
			DDRD  |=  (1<<PD7);

			if(state) {
				PORTD |= (1<<PD7);
			}
			break;
		}

		default: {
			break;
		}
	}
}



//
// CAN
//
int8_t bw_canInit( uint16_t bitrate /* kbits */ ) {
	switch( bitrate ) {
		case 125: {
			can_init( BITRATE_125_KBPS );
			break;
		}

		case 500: {
			can_init( BITRATE_500_KBPS );
			break;
		}

		case 1000: {
			can_init( BITRATE_1_MBPS );
			break;
		}

		default: {
			return -1;
			break;
		}
	}

	can_filter_t can_filter = { .id = 0, .mask = 0, .flags = { .rtr = 0, .extended = 0 } };
	can_set_filter( 0, &can_filter );
	can_set_filter( 1, &can_filter );
	can_set_filter( 2, &can_filter );

	// This is the TJA1043's standby and enable line.
	DDRB  |= (1<<PB3) | (1<<PB4); // !STB! | EN
	PORTB |= (1<<PB3) | (1<<PB4); // ready for sending data

	return 0;
}
