#include "ADC.h"

uint32_t ADC_Last = 0;
uint8_t ADC_New_Data_Available = 0;

void ADC_Init() {
	//Change the function value of pin to ADC.
	ANALOG_PIN_IOCON |= 1;
	ANALOG_PIN_IOCON2 |= 1;
	TRIMPOT_IOCON |= 1;
	//Change the mode value of pin to mode which should be selected if Analog mode is used.
	ANALOG_PIN_IOCON &= ~((1<<3)|(1<<4));
	ANALOG_PIN_IOCON2 &= ~((1<<3)|(1<<4));
	TRIMPOT_IOCON &= ~((1<<3)|(1<<4));
	//Change Analog/Digital mode of pin to Analog.
	ANALOG_PIN_IOCON &= ~(1<<7);
	ANALOG_PIN_IOCON2 &= ~(1<<7);
	TRIMPOT_IOCON &= ~(1<<7);
	//Turn on ADC.
	PCONP |= 1<<12;
	//Set the CLKDIV and make the A/D converter operational without Burst mode.
	ADC->CR |= ADC_CLKDIV<<8;
	ADC->CR |= (1<<21);
}

void ADC_Start() {
	//Write a code for starting A/D conversion
	ADC->CR |= 1<<24;
}

void ADC_Stop() {
	//Write a code for stopping A/D conversion
	ADC->CR &= ~(1<<24);
}

uint32_t ADC_Read() {
	uint32_t data;
	
	//Configure CR SEL bits for sampled and converting corresponding pin
	ADC->CR &= ~(0xFF);
	ADC->CR |= 1<<2;
	ADC_Start();
	
	//Wait until A/D conversion completed
	while(!(ADC->DR[2]>>31&1));
	//Convert the data RESULT to 0 - 1000 range and return the ADC data
	data=1000*(ADC->DR[2]>>4&ADC_MAX_VALUE)/ADC_MAX_VALUE;
	ADC_Stop();
	
	return data;
}

uint32_t ADC_Read2() {
	uint32_t data;
	
	//Configure CR SEL bits for sampled and converting corresponding pin
	ADC->CR &= ~(0xFF);
	ADC->CR |= 1<<3;
	ADC_Start();
	
	//Wait until A/D conversion completed
	while(!(ADC->DR[3]>>31&1));
	//Convert the data RESULT to 0 - 1000 range and return the ADC data
	data=1000*(ADC->DR[3]>>4&ADC_MAX_VALUE)/ADC_MAX_VALUE;
	ADC_Stop();
	
	return data;
}

uint32_t ADC_Read_Trim() {
	uint32_t data;
	
	//Configure CR SEL bits for sampled and converting corresponding pin
	ADC->CR &= ~(0xFF);
	ADC->CR |= 1<<0;
	ADC_Start();
	
	//Wait until A/D conversion completed
	while(!(ADC->DR[0]>>31&1));
	//Convert the data RESULT to 0 - 1000 range and return the ADC data
	data=1000*(ADC->DR[0]>>4&ADC_MAX_VALUE)/ADC_MAX_VALUE;
	ADC_Stop();
	
	return data;
}

