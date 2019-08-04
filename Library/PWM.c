#include "PWM.h"

void PWM_Init() {
	//Change the function of the pin in here:
	IOCON_MOTOR_SPEED_1 &= ~(7);
	IOCON_MOTOR_SPEED_1 |= 3;
	IOCON_MOTOR_SPEED_2 &= ~(7);
	IOCON_MOTOR_SPEED_2 |= 3;
	
	PCONP |= 1 << 5;
	
	//Enable PWM output for corresponding pin.
	PWM0->PCR |= (1<<9)|(1<<10);
	//Reset The PWM Timer Counter and The PWM Prescale Counter on the Next Positive Edge of PCLK
	PWM0->TCR = 1 << 1;
	
	PWM0->MR0 = (PERIPHERAL_CLOCK_FREQUENCY / 1000000) * 20 * 1000;
	
	//Reset TC, when MR0 matches TC.
	PWM0->MCR = 1 << 1;
	//Enable PWM Match 0 Latch.
	PWM0->LER |= 1 << 0;
	//Enable Counter and PWM and Clear Reset on the PWM
	PWM0->TCR = (1 << 0 | 1 << 3);
	PWM_Write(0,0);
}

void PWM_Cycle_Rate(uint32_t period_In_Cycles) {
	PWM0->MR0 = (PERIPHERAL_CLOCK_FREQUENCY / 1000000) * period_In_Cycles * 1000;
	
	//Enable PWM Match 0 Latch.
	PWM0->LER |= 1 << 0;
}

void PWM_Write(uint32_t T_ON_1,uint32_t T_ON_2) {	
	if(T_ON_1 > 100) {
		T_ON_1 = 100;
	}
	
	T_ON_1 = (uint32_t)(((PWM0->MR0) * T_ON_1) / 100);
	
	if (T_ON_1 == PWM0->MR0) {
		T_ON_1++;
	}
	PWM0->MR1 = T_ON_1;
	
	/////////////////////
	
	if(T_ON_2 > 100) {
		T_ON_2 = 100;
	}
	
	T_ON_2 = (uint32_t)(((PWM0->MR0) * T_ON_2) / 100);
	
	if (T_ON_2 == PWM0->MR0) {
		T_ON_2++;
	}
	PWM0->MR2 = T_ON_2;
	
	//Enable PWM Match Register Latch.
	PWM0->LER |= (1 << 1)| (1<<2);
}
