#include "Exceptions.h"
//This file handles various types of processor exceptions
//Note that C++ exceptions are not used at the moment

extern "C" {
	
	//triggered by UART1 when data is ready
	void USART1_IRQHandler(void)
	{
		BaseType_t higherPriorityTaskWoken {pdFALSE};
		//If data is ready
		if (Usart_1::is_flag_set(UART_FLAG_RXNE)) {
			//Read data from uart, this clears UART state
			const uint8_t byte = Usart_1::read_dr();
			//for logging, may cause timing issues so be careful
			Usart_2::transmit_byte(byte);
			uint8_t message {0};
			//Make sure queue is initialised
			if (uart_byte_queue_handle != nullptr) {
				//send data to processing task
				const BaseType_t queueSendReturn = xQueueSendToFrontFromISR(uart_byte_queue_handle, &byte, &higherPriorityTaskWoken);	
				if (queueSendReturn != pdPASS)
					//error
					while(1);
			}
		}
		else if (Usart_1::is_flag_set(UART_FLAG_ORE)) {
			//clear overun error
			Usart_1::read_dr();
		}
		else {
			//Should never get here
			while (1) ;
		}
		if (higherPriorityTaskWoken == pdTRUE) {
			taskYIELD();
		}
	}
	
	//Triggered bu UART2 when data is ready, similar to UART1 task
	//This is from PC virtial com port, so is for testing
	void USART2_IRQHandler(void)
	{
		BaseType_t higherPriorityTaskWoken {pdFALSE};
		if (Usart_2::is_flag_set(UART_FLAG_RXNE)) {
			const uint8_t byte = Usart_2::read_dr();
			Usart_2::transmit_byte(byte);
			uint8_t message {0}			;
			if (uart_byte_queue_handle != nullptr) {
				const BaseType_t queueSendReturn = xQueueSendToFrontFromISR(uart_byte_queue_handle, &byte, &higherPriorityTaskWoken);	
				if (queueSendReturn != pdPASS)
					//error
					while(1);
			}

		}
		else if (Usart_2::is_flag_set(UART_FLAG_ORE)) {
			Usart_2::read_dr();
		}
		else {
			while (1) ;
		}
		if (higherPriorityTaskWoken == pdTRUE) {
			taskYIELD();
		}
	}
}

//Memory allocation should be handled by freeRTOS
//For now just disable dynamic memory allocation
//If any functions use memory allocation, this funtion is included
//which causes a compiler error
void *malloc(size_t size)
{
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

//Takes a sample off the sample buffer and outputs to DAC
//May be able to include this in the main irq handler
void TIM2_IRQHandler_cpp(void) {
	const float sample = Sample_buffer::get_next_sample().value;
	Dac_1::set_value_rel((float) sample);
}

extern "C" {
	void TIM2_IRQHandler(void)
	{
		TIM2_IRQHandler_cpp();
		//TODO: check what this actually does, may not be necessary
		//may just need to clear EXTI interrupt
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
	//FreeRTOS hooks
	//May cause some performance degredation is enabled
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


