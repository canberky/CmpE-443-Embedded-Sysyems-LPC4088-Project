#include "LPC407x_8x_177x_8x.h"
#include "Library/GPIO.h"
#include "Library/PWM.h"
#include "Library/Joystick.h"
#include "Library/Timer.h"
#include "Library/Parameters.h"
#include "Library/Ultrasonic.h" 
#include "Library/ADC.h"
#include "Library/Serial.h"
#include "Library/Wait.h"

uint32_t temp;
uint32_t led;
uint32_t cap;
uint32_t counter;
uint32_t CW;
uint32_t CCW;
uint32_t S;
uint32_t value;
uint32_t trimpot;
uint32_t turn;
uint32_t left_ldr;
uint32_t right_ldr;
uint32_t leftspeed;
uint32_t rightspeed;


void init() {
	turn=0;
	CW=0;
	CCW=1;
	S=2;
	Ultrasonic_Trigger_Timer_Init();
	Ultrasonic_Capture_Timer_Init();
	Timer_Init();
	Joystick_Init();
	ADC_Init();
	Serial_Init();
	//Direction Pins of MotorLeft
	GPIO_DIR_Write(PORT2, 1<<10, 1);//P23
	GPIO_DIR_Write(PORT1, 1<<12, 1);//P24
	//Direction Pins of MotorRight
	GPIO_DIR_Write(PORT1, 1<<31, 1);//P20
	GPIO_DIR_Write(PORT1, 1<<30, 1);//P19
	//Direction Pins of LEDs
	
	GPIO_DIR_Write(PORT1, 1<<6, 1);//P27 for front left
	GPIO_DIR_Write(PORT1, 1<<5, 1);//P28 for front right
	GPIO_DIR_Write(PORT1, 1<<11, 1);//P25 for back left
	GPIO_DIR_Write(PORT1, 1<<7, 1);//P26 for back right
	//P30 for channel1
	//P29 for channel2
	PWM_Init();
	PWM_Cycle_Rate(50);
	PWM_Write(0,0);
	//Parameters
	
}

int charToInt(char c) {
	int value;
	value = c - '0';
	return value;
}

void motorLeft(uint32_t direction){
	if(direction==0){
		GPIO_PIN_Write(PORT2, 1<<10, 1);
		GPIO_PIN_Write(PORT1, 1<<12, 0);
	}else if(direction==1){
		GPIO_PIN_Write(PORT2, 1<<10, 0);
		GPIO_PIN_Write(PORT1, 1<<12, 1);
	}else{
		GPIO_PIN_Write(PORT2, 1<<10, 0);
		GPIO_PIN_Write(PORT1, 1<<12, 0);
	}
}

void motorRight(uint32_t direction){
	if(direction==0){
		GPIO_PIN_Write(PORT1, 1<<31, 1);
		GPIO_PIN_Write(PORT1, 1<<30, 0);
	}else if(direction==1){
		GPIO_PIN_Write(PORT1, 1<<31, 0);
		GPIO_PIN_Write(PORT1, 1<<30, 1);
	}else{
		GPIO_PIN_Write(PORT1, 1<<31, 0);
		GPIO_PIN_Write(PORT1, 1<<30, 0);
	}
}

int pressed() {
	if(Joystick_Left_Pressed()){
		return 1;
	}else if(Joystick_Up_Pressed()){
		return 2;
	}else if(Joystick_Down_Pressed()){
		return 3;
	}else if(Joystick_Center_Pressed()){
		return 4;
	}else if(Joystick_Right_Pressed()){
		return 5;
	}else{
		return 0;
	}
}

void LED(uint32_t corner, uint32_t on_off){
	if(corner==1){
		GPIO_PIN_Write(PORT1, 1<<6, on_off);//frontleft
	}else if(corner==2){
		GPIO_PIN_Write(PORT1, 1<<5, on_off);//frontright
	}else if(corner==3){
		GPIO_PIN_Write(PORT1, 1<<11, on_off);//backleft
	}else{
		GPIO_PIN_Write(PORT1, 1<<7, on_off);//backright
	}
}

void left(){
	PWM_Cycle_Rate(50);
	PWM_Write(ROBOT_SPEED,ROBOT_SPEED);  
	//directions
	motorLeft(CW);
	motorRight(CCW);
	//flags
	TURN_LEFT_FLAG = 1;
	TURN_RIGHT_FLAG = 0;
	FORWARD_FLAG = 0;
	BACKWARD_FLAG = 0;
}

void right(){
	PWM_Cycle_Rate(50);
	PWM_Write(ROBOT_SPEED,ROBOT_SPEED);
	//directions
	motorLeft(CCW);
	motorRight(CW);
	//flags
	TURN_LEFT_FLAG = 0;
	TURN_RIGHT_FLAG = 1;
	FORWARD_FLAG = 0;
	BACKWARD_FLAG = 0;
}

void forward(uint32_t sleft,uint32_t sright){
	PWM_Cycle_Rate(50);
	PWM_Write(sleft,sright);
	//directions
	motorLeft(CCW);
	motorRight(CCW);
	//LEDs
	/*
	LED(1,1);
	LED(2,1);
	LED(3,0);
	LED(4,0);
	*/
	//flags
	TURN_LEFT_FLAG = 0;
	TURN_RIGHT_FLAG = 0;
	FORWARD_FLAG = 1;
	BACKWARD_FLAG = 0;
	
	
	
}

void backward(uint32_t sleft, uint32_t sright){
	PWM_Cycle_Rate(50);
	PWM_Write(sleft,sright);
	//directions
	motorLeft(CW);
	motorRight(CW);
	//LEDs
	LED(1,0);
	LED(2,0);
	LED(3,1);
	LED(4,1);
	//flags
	TURN_LEFT_FLAG = 0;
	TURN_RIGHT_FLAG = 0;
	FORWARD_FLAG = 0;
	BACKWARD_FLAG = 1;
}

void stop(){
	motorLeft(S);
	motorRight(S);
	//LEDs
	LED(1,0);
	LED(2,0);
	LED(3,0);
	LED(4,0);
	//flags
	TURN_LEFT_FLAG = 0;
	TURN_RIGHT_FLAG = 0;
	FORWARD_FLAG = 0;
	BACKWARD_FLAG = 0;
	turn=0;
}

//When Joystick Left button is pressed, your robot should start to rotate 90 degree in
//counter-clockwise direction (Point Turn is necessary). After the turn is completed, the car
//should not move.

//When Joystick Up button is pressed, your robot should start to travel in forward direction.

//When Joystick Down button is pressed, your robot should start to travel in backward direction.

//When Joystick Center button is pressed, your robot should stop.

//When Joystick Right button is pressed, your robot should rotate 90 degree in clockwise
//direction (Point Turn is necessary). After the turn is completed, the car should not move.

void update() {
	Ultrasonic_Start_Trigger();
	for(int i=0;i<1000;i++);
	char resultString[12];
	int result = 0;
	int num0 = 0;
	int num1 = 0;
	char op;
	int isSecond = 0;
	
	for(int i=0; i<2; i++) {
		char c = Serial_ReadData();
		
		
			//sprintf(resultString, "\r\n%d\r\n", result);
			
			Serial_WriteData(c);
			Serial_Write(resultString);
			break;
		
		Serial_WriteData(c);
	}
	
	wait(1000);
	
	if(Joystick_Left_Pressed()){
		//for döngüsü lazim galiba
		
		cap = TIMER3->CR0;
		counter=0;
		while((counter<ROTATION_NUMBER_FOR_90_DEGREE)&&(pressed()==0||pressed()==1)){
			if(cap!=TIMER3->CR0){
				counter++;
				cap = TIMER3->CR0;
			}
			trimpot = ADC_Read_Trim();
			ROBOT_SPEED = trimpot/10;
			left();
			led = (((TIMER3->TC)/100000)%5);
			if(led<1||led>4){
				LED(1,1);
				LED(2,0);
				LED(3,1);
				LED(4,0);
			}else{
				LED(1,0);
				LED(2,0);
				LED(3,0);
				LED(4,0);
			}
		}
		stop();
	}
	
	else if(Joystick_Right_Pressed()){
		cap = TIMER3->CR0;
		counter=0;
		while((counter<ROTATION_NUMBER_FOR_90_DEGREE)&&(pressed()==0||pressed()==5)){
			if(cap!=TIMER3->CR0){
				counter++;
				cap = TIMER3->CR0;
			}
			trimpot = ADC_Read_Trim();
			ROBOT_SPEED = trimpot/10;
			right();
			led = (((TIMER3->TC)/100000)%5);
			if(led<1||led>4){
				LED(1,0);
				LED(2,1);
				LED(3,0);
				LED(4,1);
			}else{
				LED(1,0);
				LED(2,0);
				LED(3,0);
				LED(4,0);
			}
			
		}
		stop();
	}
	
	else if(Joystick_Up_Pressed()||turn==1){
		int aa = 0; 
		
		while((pressed()==0||pressed()==2)){
			Ultrasonic_Start_Trigger();
			for(int i=0;i<1000000;i++);
			trimpot = ADC_Read_Trim();
			turn=1;
			temp = ultrasonicSensorFallingTime-ultrasonicSensorRisingTime;
		 /*
			if((ultrasonicSensorFallingTime-ultrasonicSensorRisingTime)/50<OBSTACLE_DISTANCE){
				turn = 2;
				aa = 1;
				break;
			}
			*/
			ROBOT_SPEED = trimpot/10;
			if(ROBOT_SPEED>10){
				ROBOT_SPEED -= 10;
			}
			left_ldr = ADC_Read();
			right_ldr = ADC_Read2();
			
			if(left_ldr>right_ldr){
				leftspeed = ROBOT_SPEED - (left_ldr-right_ldr)*3/4;
				leftspeed = 0;
				if(ROBOT_SPEED<(left_ldr-right_ldr)*3/4){
					leftspeed=0;
				}
				rightspeed = ROBOT_SPEED + (left_ldr-right_ldr)*1/4;
				if((ROBOT_SPEED + (left_ldr-right_ldr)*1/4)>90){
					rightspeed=ROBOT_SPEED;
				}
				led = (((TIMER3->TC)/100000)%5);
				if(led<1||led>4){
					LED(1,0);
					LED(2,1);
					LED(3,0);
					LED(4,1);
				}else{
					LED(1,0);
					LED(2,0);
					LED(3,0);
					LED(4,0);
				}
			}
			else if (left_ldr<right_ldr){
				leftspeed = ROBOT_SPEED + (right_ldr-left_ldr)*1/4;
				if((ROBOT_SPEED + (right_ldr-left_ldr)*1/4)>90){
					leftspeed=ROBOT_SPEED;
				}
				rightspeed = ROBOT_SPEED - (right_ldr-left_ldr)*3/4;
				rightspeed = 0;
				if(ROBOT_SPEED<(right_ldr-left_ldr)*3/4){
					rightspeed=0;
				}
				led = (((TIMER3->TC)/100000)%5);
				if(led<1||led>4){
					LED(1,1);
					LED(2,0);
					LED(3,1);
					LED(4,0);
				}else{
					LED(1,0);
					LED(2,0);
					LED(3,0);
					LED(4,0);
				}
			}else{
				leftspeed = ROBOT_SPEED/2;
				rightspeed = ROBOT_SPEED/2;
			}
			if(leftspeed<1){
				leftspeed=0;
			}
			if(rightspeed<1){
				rightspeed=0;
			}
			
			//leftspeed = (ROBOT_SPEED-10)*(right_ldr-300)/250+10;
			//rightspeed = (ROBOT_SPEED-10)*(left_ldr-300)/250+10;
			forward(leftspeed,rightspeed);
		}
		if(aa==0){
			stop();
		}
	}
	
	else if(Joystick_Down_Pressed()||turn==2){
		int aa = 0; 
		
		while((pressed()==0||pressed()==3)){
			Ultrasonic_Start_Trigger();
			for(int i=0;i<1000000;i++);
			trimpot = ADC_Read_Trim();
			turn = 2;
			/*
			if((ultrasonicSensorFallingTime-ultrasonicSensorRisingTime)/50+5>OBSTACLE_ESCAPE_DISTANCE+10&&(ultrasonicSensorFallingTime-ultrasonicSensorRisingTime)/50+5<OBSTACLE_ESCAPE_DISTANCE+25){
				turn=1;
				aa = 1;
				break;
			}
			*/
			ROBOT_SPEED = trimpot/10;
			if(ROBOT_SPEED>15){
				ROBOT_SPEED -= 15;
			}
			left_ldr = ADC_Read();
			right_ldr = ADC_Read2();
			
			if(left_ldr>right_ldr){
				leftspeed = ROBOT_SPEED - (left_ldr-right_ldr)*3/4;
				leftspeed = 0;
				if(ROBOT_SPEED<(left_ldr-right_ldr)*3/4){
					leftspeed=0;
				}
				rightspeed = ROBOT_SPEED + (left_ldr-right_ldr)*1/4;
				if((ROBOT_SPEED + (left_ldr-right_ldr)*1/4)>90){
					rightspeed=ROBOT_SPEED;
				}
			}
			else if (left_ldr<right_ldr){
				leftspeed = ROBOT_SPEED + (right_ldr-left_ldr)*1/4;
				if((ROBOT_SPEED + (right_ldr-left_ldr)*1/4)>90){
					leftspeed=ROBOT_SPEED;
				}
				rightspeed = ROBOT_SPEED - (right_ldr-left_ldr)*3/4;
				rightspeed = 0;
				if(ROBOT_SPEED<(right_ldr-left_ldr)*3/4){
					rightspeed=0;
				}
			}else{
				leftspeed = ROBOT_SPEED/2;
				rightspeed = ROBOT_SPEED/2;
			}
			leftspeed -= 10;
			if(leftspeed<1){
				leftspeed=0;
			}
			leftspeed -= 10;
			if(rightspeed<1){
				rightspeed=0;
			}
			backward(rightspeed,leftspeed);
		}
		if(aa==0){
			stop();
		}
	}
	
	else if(Joystick_Center_Pressed()){
		stop();
		turn=0;
	}else{
		
	}
	
}
 
int main() {
    init();
		
    while(1) {
        update();
    }
}
