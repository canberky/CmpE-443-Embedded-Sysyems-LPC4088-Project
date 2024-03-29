#ifndef ADC_H
#define ADC_H

#include "LPC407x_8x_177x_8x.h"

#include "SystemStructures.h"

typedef struct {
  volatile	uint32_t CR;
  volatile	uint32_t GDR;
						uint32_t RESERVED0;
  volatile	uint32_t INTEN;
  volatile	uint32_t DR[8];
  volatile	uint32_t STAT;
  volatile	uint32_t TRM;
} ADC_TypeDef;

#define ADC_CLOCK_FREQUENCY 1000000

//Write the IOCON address of Analog Pin
#define	ANALOG_PIN_IOCON_ADDRESS	0x4002C064
#define ANALOG_PIN_IOCON	*((volatile uint32_t*)(ANALOG_PIN_IOCON_ADDRESS))
	
#define	ANALOG_PIN_IOCON_ADDRESS2	0x4002C068
#define ANALOG_PIN_IOCON2	*((volatile uint32_t*)(ANALOG_PIN_IOCON_ADDRESS2))
	
#define	TRIMPOT_ADDRESS	0x4002C05C
#define TRIMPOT_IOCON	*((volatile uint32_t*)(TRIMPOT_ADDRESS))

//Write the CLKDIV of ADC for given ADC_CLOCK_FREQUENCY.
#define ADC_CLKDIV 0x03B

//Write the max value of ADC.
#define ADC_MAX_VALUE 0xFFF

#define ADC_BASE	0x40034000
#define ADC	((ADC_TypeDef*) ADC_BASE)

void ADC_Init(void);
void ADC_Start(void);
void ADC_Stop(void);
uint32_t ADC_Read(void);
uint32_t ADC_Read2(void);
uint32_t ADC_Read_Trim(void);

#endif
