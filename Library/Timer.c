#include "Timer.h"

uint32_t currentTime = 0;
uint32_t previousTime = 0;
uint32_t capturedTime = 0;

void Timer_Init() {
	//Give the Correct Function Values to IOCON_OUT
	IOCON_OUT &= ~(7);
	IOCON_OUT |= 3;
	//Enable Timer3
	PCONP |= 1<<23;
	//Change the mode of Timer3 to Timer Mode.
	TIMER3->CTCR &= ~(3);
	//Disable Timer Counter and Prescale Counter for Timer3.
	TIMER3->TCR &= ~(1);
	//Reset Timer Counter and Prescale Counter for Timer3.
	TIMER3->TCR |= 1<<1;
	//Change PR Register value for 1 microsecond incrementing
	TIMER3->PR = (PERIPHERAL_CLOCK_FREQUENCY/1000000)-1;
	//Capture Rising and Falling Edge on CAP1
	TIMER3->CCR |= (1<<0);
	//Remove the reset on counters of Timer3.
	TIMER3->TCR &= ~(2);
	//Enable Timer3 Counter and Prescale Counter for counting.	
	TIMER3->TCR |= 1<<0;
}
