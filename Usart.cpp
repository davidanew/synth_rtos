#include "Usart.h"
//TODO: move this
#include "FreeRTOSConfig.h"

void Usart::transmit_byte(const UART_HandleTypeDef& huart, uint8_t byte) {
	HAL_UART_Transmit((UART_HandleTypeDef *) &huart,  &byte, 1, HAL_MAX_DELAY) ;
}

UART_HandleTypeDef Usart_1::huart1 = {};
uint8_t data;

void Usart_1::init(void)
{
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct {}	;
	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 31250;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	NVIC_SetPriority(USART1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(USART1_IRQn);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		//Error_Handler();
		while(1);
	}
}

void Usart_1::transmit_byte(uint8_t byte) {
	Usart::transmit_byte(huart1, byte);
}

uint32_t Usart_1::read_dr() {
	return (uint32_t) USART1->DR;
}

void Usart_1::receive(uint8_t* buffer_ptr, uint16_t size) {
	HAL_UART_Receive(&huart1, buffer_ptr, size, HAL_MAX_DELAY);
}

bool Usart_1::is_flag_set(const uint32_t flag) {
	return __HAL_UART_GET_FLAG(&Usart_1::huart1, flag);
}

UART_HandleTypeDef Usart_2::huart2 = { };

//Init code from CubeMX
void Usart_2::init(void)
{
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct {}	;
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	NVIC_SetPriority(USART2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(USART2_IRQn);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);

	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		//Error_Handler();
		while(1);
	}

}

void Usart_2::transmit_byte(uint8_t byte) {
	Usart::transmit_byte(huart2, byte);
}

uint32_t Usart_2::read_dr() {
	return (uint32_t) USART2->DR;
	
}

bool Usart_2::is_flag_set(const uint32_t flag) {
	return __HAL_UART_GET_FLAG(&Usart_2::huart2, flag);
}

//First step in midi read is to get the status byte
Midi_state Midi_in::state = { wait_status_byte };

Note_on_struct Midi_in::note_on_struct = { };
Controller_change_struct Midi_in::controller_change_struct = { };

//Handle the recieved midi byte. FSM state is updated and appropriate midi data holding structs
//When the full message is recieved (3 bytes) then one of the supplied funtions is run
void Midi_in::handle_midi_byte(uint8_t midi_byte, void(*note_on_handler)(Note_on_struct), void(*controller_change_handler)(Controller_change_struct)) {
	switch (state) {
	case (wait_status_byte): 
		if (is_status_byte(midi_byte))
			//Setting next state
			state = get_next_state_from_status_byte(midi_byte);
		else 
			//do nothing
			state = wait_status_byte;
		break;
	case (wait_note_number):
		//We can handle a new status message from any state 
		if(is_status_byte(midi_byte))
			state = get_next_state_from_status_byte(midi_byte);
		else 
			//If it isn't a stutus byte assume it is valid dats
			note_on_struct.note_number = midi_byte;
		state = wait_velocity;
		break;
	case (wait_controller_number):
		if (is_status_byte(midi_byte))
			state = get_next_state_from_status_byte(midi_byte);
		else 
			controller_change_struct.controller_number = midi_byte;
		state = wait_controller_data;
		break;
	case (wait_velocity):
		if (is_status_byte(midi_byte))
			state = get_next_state_from_status_byte(midi_byte);
		else {
			//The final part of the message for Note On
			note_on_struct.velocity = midi_byte;
			note_on_handler(note_on_struct);
			state = wait_status_byte;			
		}
		break;
	case (wait_controller_data):
		if (is_status_byte(midi_byte))
			state = get_next_state_from_status_byte(midi_byte);
		else {
			//The final part of the message for Controller Change
			controller_change_struct.controller_data = midi_byte;
			controller_change_handler(controller_change_struct);
			state = wait_status_byte;
		}
		break;
	default:
		state = wait_status_byte;
		break;
	}		
}

Midi_state Midi_in::get_next_state_from_status_byte(uint8_t midi_byte) {
	if (is_status_byte(midi_byte)) {
		if ((midi_byte & 0xF0)  == 0x90) return wait_note_number; //it was note on
		else if((midi_byte & 0xB0) == 0xB0) return wait_controller_number;  //it was a controller change
		else return wait_status_byte;  //unsupported status byte
	}
	else return wait_status_byte;
}

bool Midi_in::is_status_byte(uint8_t midi_byte) {
	return ((midi_byte & 1 << 7) == (1 << 7));
}


