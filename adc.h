#ifndef _ADC_H_
#define _ADC_H_

void     adc_init(void);
uint16_t adc_read(uint8_t channel);
int8_t   adc_value2Temp(uint16_t adcValue);
uint8_t  adc_tempInRange(uint16_t value);


#endif
