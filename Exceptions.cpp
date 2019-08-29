#include "Exceptions.h"

extern "C" {
	
	void USART1_IRQHandler(void)
	{
		BaseType_t higherPriorityTaskWoken {pdFALSE};
		if (Usart_1::is_flag_set(UART_FLAG_RXNE)) {
			const uint8_t byte = Usart_1::read_dr();
			//for logging
			Usart_2::transmit_byte(byte);
			uint8_t message {0};
			if (uart_byte_queue_handle != nullptr) {
				const BaseType_t queueSendReturn = xQueueSendToFrontFromISR(uart_byte_queue_handle, &byte, &higherPriorityTaskWoken);	
				if (queueSendReturn != pdPASS)
					//error
					while(1);
			}
		}
		else if (Usart_1::is_flag_set(UART_FLAG_ORE)) {
			Usart_1::read_dr();
			//while (1) ;
		}
		else {
			while (1) ;
		}
		if (higherPriorityTaskWoken == pdTRUE) {
			taskYIELD();
		}
	}
	
	
	
	
	void USART2_IRQHandler(void)
	{
		BaseType_t higherPriorityTaskWoken {pdFALSE};
		if (Usart_2::is_flag_set(UART_FLAG_RXNE)) {
			const uint8_t byte = Usart_2::read_dr();
			Usart_2::transmit_byte(byte);
			uint8_t message {0}
			;
			if (uart_byte_queue_handle != nullptr) {
				const BaseType_t queueSendReturn = xQueueSendToFrontFromISR(uart_byte_queue_handle, &byte, &higherPriorityTaskWoken);	
				if (queueSendReturn != pdPASS)
					//error
					while(1);
			}

		}
		else if (Usart_2::is_flag_set(UART_FLAG_ORE)) {
			Usart_2::read_dr();
			//while (1) ;
		}
		else {
			while (1) ;
		}
		if (higherPriorityTaskWoken == pdTRUE) {
			taskYIELD();
		}
	}
}



void *malloc(size_t size)
{
	//no dynamic memory allocation
	while(1);
	return NULL;
}

extern "C" {
	void SysTick_Handler(void)
	{
		HAL_IncTick();
		osSystickHandler();
	}
}

void TIM2_IRQHandler_cpp(void) {
	const float sample = Sample_buffer::get_next_sample().value;
	//TODO: Check optional
	Dac_1::set_value_rel((float) sample);
}

extern "C" {
	void TIM2_IRQHandler(void)
	{
		TIM2_IRQHandler_cpp();
		HAL_TIM_IRQHandler(&(Tim::htim2));
	}
}

extern "C" {
	void NMI_Handler()			{while (1) ;}
	void HardFault_Handler()    {while (1) ;}
	void MemManage_Handler()    {while (1) ;}
	void BusFault_Handler()     {while (1) ;}
	void UsageFault_Handler()   {while (1) ;}
}

extern "C" {
	
	void vApplicationMallocFailedHook(void)
	{
		__set_PRIMASK(1);
		for (;;) ;
	}

	void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
	{
		__set_PRIMASK(1);
		for (;;) ;
	}
}


