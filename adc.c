#include <stdint.h>
#include <avr/io.h>

#include "setup.h"
#include "adc.h"



void adc_init(void) {
  ADMUX |= (1<<REFS0); // select AVcc as vref
  ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // sample at F_CPU/128
	ADCSRA |= (1<<ADEN); // enable ADC
}


uint16_t adc_read(uint8_t channel) {
  ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // select channel
  ADCSRA |= (1<<ADSC);       // convert value only once
  while(ADCSRA & (1<<ADSC)); // wait for completion
  return ADC;
}


int8_t adc_value2Temp(uint16_t adcValue) {
	// Compute the temperature based on the voltage measured
	// over the voltage divider of the NTC resistor

	// convert to voltage
	// convert to resistance
	// convert to temperature
	return 0;
}



uint8_t adc_tempInRange(uint16_t value) {
	if(eTempLowLimit > value) {
		return FALSE; // probable sensor disconnect
	}

	if(eTempHighLimit < value) {
		return FALSE; // probable heater issue
	}

	return TRUE;
}
