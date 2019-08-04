#include "Ultrasonic.h"

void Ultrasonic_Trigger_Timer_Init() {
	//Give the Correct Function Values to IOCON_TRIGGER
	IOCON_TRIGGER &= ~(7);
	IOCON_TRIGGER |= 3;
	//Enable Timer2.
	PCONP |= 1<<22;
	//Change the mode of Timer2 to Timer Mode.
	TIMER2->CTCR &= ~(3);
	//Reset Timer Counter and Prescale Counter for Timer2.
	TIMER2->TCR &= ~(1 << 0);
	TIMER2->TCR |= 1<<1;
	//Change PR Register value for 1 microsecond incrementing
	TIMER2->PR = (PERIPHERAL_CLOCK_FREQUENCY/1000000)-1;
	//Remove the reset on counters of Timer2.
	TIMER2->TCR &= ~(1<<1);
	TIMER2->TCR |= (1 << 0);
}

void Ultrasonic_Capture_Timer_Init() {
	//Give the Correct Function Values to IOCON_ECHO
	IOCON_ECHO &= ~(7);
	IOCON_ECHO |= 3;
	
	PCONP |= 1 << 23;
	
	TIMER3->CTCR = 0x00;
	
	TIMER3->TCR &= ~(1 << 0);
	
	TIMER3->TCR |= (1 << 1);
	
	TIMER3->PR = PERIPHERAL_CLOCK_FREQUENCY / 1000000 - 1;
	
	//Write the Correct Value to CCR register for getting interrupt when Rising Edge Occur
	TIMER3->CCR = (1<<3)|(1<<5);
	
	TIMER3->TCR &= ~(1 << 1);
	
	TIMER3->TCR |= (1 << 0);
	
	//Clear pendings for Timer3.
	NVIC_ClearPendingIRQ(TIMER3_IRQn);
	
	//Set Priority Timer3 IRQ as 5.
	NVIC_SetPriority(TIMER3_IRQn,5);
	
	//Enable TIMER3_IRQn (Interrupt Request).
	NVIC_EnableIRQ(TIMER3_IRQn);
}

void Ultrasonic_Start_Trigger() {
	//Give HIGH value to T2_MAT_3 pin
	TIMER2->EMR |= 1<<3;
	//Calculate the MR3 register value for giving 10 microsecond HIGH value
	TIMER2->MR3 = 10;
	//Write to correct values to Timer2 EMR register for making LOW output value of Trigger Pin when match occurs.
	TIMER2->EMR &= ~(1<<3);
	//Reset TC and Stop (TC and PC), if MR3 register matches the TC.
	TIMER2->MCR |= (1<<10)|(1<<11);
	//Enable Timer2 Counter and Prescale Counter for counting.
	TIMER2->TCR |= 1;
	ultrasonicSensorEdgeCount = 0;
}

uint8_t ultrasonicSensorEdgeCount = 0;

uint32_t ultrasonicSensorRisingTime = 0;
uint32_t ultrasonicSensorFallingTime = 0;
uint32_t ultrasonicSensorDuration = 0;
uint32_t ultrasonicSensorDistance = 0;

void TIMER3_IRQHandler() {
	if(ultrasonicSensorEdgeCount == 0) {
		
		//Store the rising time into ultrasonicSensorRisingTime variable
		ultrasonicSensorRisingTime = TIMER3->CR1;
		//Change the CCR register value for getting interrupt when falling edge event is occured.
		TIMER3->CCR |= (1<<4)|(1<<5);
		ultrasonicSensorEdgeCount = 1;
		
		NVIC_ClearPendingIRQ(TIMER3_IRQn);
	}
	else if(ultrasonicSensorEdgeCount == 1){
		
		//Store the falling time into ultrasonicSensorFallingTime variable
		ultrasonicSensorFallingTime = TIMER3->CR1;
		//Change the CCR register value for getting interrupt when rising edge event is occured.
		TIMER3->CCR |= (1<<3)|(1<<5);
		ultrasonicSensorEdgeCount = 2;
		
		NVIC_ClearPendingIRQ(TIMER3_IRQn);
	}
	NVIC_DisableIRQ(TIMER3_IRQn);
	//Clear the interrupt flag for capture channel 1 event
	TIMER3->IR |= 1<<5;
}

