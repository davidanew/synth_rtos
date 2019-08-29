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
	huart1.Init.BaudRate = 115200;
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

bool Usart_1::is_flag_set(const uint32_t flag) {
	return __HAL_UART_GET_FLAG(&Usart_1::huart1, flag);
}

UART_HandleTypeDef Usart_2::huart2 = { };
//uint8_t data;

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


