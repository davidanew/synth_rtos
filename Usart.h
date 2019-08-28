#pragma once
#include <stm32f4xx_hal.h>

class Usart {
protected :
	static void transmit_byte(const UART_HandleTypeDef&, uint8_t);
};

/* Usart2 is connected to the virtual com port
 * So this class is used to send messages that can be 
 * seen on a PC terminal*/

class Usart_2 : public Usart {
public:
	static UART_HandleTypeDef huart2;

	static void init();
	static void transmit_byte(uint8_t);	
	static uint8_t receive_byte();
};

