#include "Serial.h"

char* serialTransmitData = 0;
uint8_t serialTransmitCompleted = 0;
char serialReceivedCharacter = 0;

void Serial_Init() {
	//Change the function of TX and RX pins for UART.
	uint32_t func = Serial_UART_TX_PIN;
	func |= 1<<0;
	func &= ~((1<<1) | (1<<2));
	Serial_UART_TX_PIN = func;
	func = Serial_UART_RX_PIN;
	func |= 1<<0;
	func &= ~((1<<1) | (1<<2));
	Serial_UART_RX_PIN = func;
	
	//Turn on UART0.
	PCONP |= (1<<3);
	
	//Enable FIFO for UART0.
	Serial_UART->FCR |= (1<<0);
	
	//In order to change the DLM, DLL and FDR values, Write correct code for enabling the access to Divisor Latches.
	Serial_UART->LCR |= (1<<7);
	
	//Write correct DLM, DLL and FDR values for 9600 baudrate
	Serial_UART->DLM = (0x01<<0);
	Serial_UART->DLL = (0x25<<0);
	Serial_UART->FDR = ((0x01<<0) | (0x03<<4));

	//Write correct code for disabling the access to Divisor Latches.
	Serial_UART->LCR &= ~(1<<7);
	
	//Change LCR register value for 8-bit character transfer, 1 stop bits and Even Parity.
	Serial_UART->LCR = (3<<0) | (0<<2) | (1<<3) | (1<<4);
}

void Serial_WriteData(char data) {
	//Wait until Transmit Holding Register become empty.
	while (!(Serial_UART->LSR & 1<<5)) {};
	
	//Write data to Transmit Holding Register
	Serial_UART->THR = data;
	
}

void Serial_Write(char* data) {
	while(*data > 0)  {
		Serial_WriteData(*data++);
	}
}

char Serial_ReadData() {
	char data;
	//Wait until Receiver Data Ready.
	while (!(Serial_UART->LSR & 1<<0)) {};
	
	//Read data from Receiver Buffer Register.
	data =  Serial_UART->RBR;
	
	return data;
}
