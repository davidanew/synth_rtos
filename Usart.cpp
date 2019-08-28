#include "Usart.h"

#include "FreeRTOSConfig.h";

void Usart::transmit_byte(const UART_HandleTypeDef& huart, uint8_t byte) {
	HAL_UART_Transmit((UART_HandleTypeDef *) &huart,  &byte, 1, HAL_MAX_DELAY) ;
}

UART_HandleTypeDef Usart_2::huart2 = { };
uint8_t data;

//Init code from CubeMX
void Usart_2::init(void)
{
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct {}
	;
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
	//const uint32_t configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY {5}; // as per FreeRTOSConfig
	NVIC_SetPriority(USART2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		//Error_Handler();
		while(1);
	}
	
	//TODO: Use better function
	HAL_UART_Receive_IT(&huart2, &data, 1);

}

void Usart_2::transmit_byte(uint8_t byte) {
	Usart::transmit_byte(huart2, byte);
}


uint32_t Usart_2::read_dr() {
	//uint8_t buffer;
	//HAL_UART_Receive(&huart2, &buffer, 1, HAL_MAX_DELAY);
	//return buffer;
	return (uint32_t) USART2->DR;
	
}

bool Usart_2::is_flag_set(const uint32_t flag) {
	return __HAL_UART_GET_FLAG(&Usart_2::huart2, flag);
}


