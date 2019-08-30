#include "main.h"

//Currently thread 1 takes midi command messages from thread 2 and updates the voice array. The voice array is used to construct the waveform and this is sent to the sample buffer
static void thread1(void *);
//Thread 2 recieves UART data and processes this to send midi messages to task 1
static void thread2(void *);
void SystemClock_Config(void);
//TODO: rename this
//Queue from task 1 to task to. At the moment it is just a trigger (no information)
QueueHandle_t queue_handle {nullptr};
QueueHandle_t uart_byte_queue_handle {nullptr};

int main(void)
{
	HAL_Init(); 
	SystemClock_Config();
	//Dac1 used to output the waveform
	Dac_1::init();
	//Dac2 used for debug/measurement
	Dac_2::init();
	//Usart 1 used for MIDI input
	Usart_1::init();
	//Usart2 used for virtual com port on PC
	Usart_2::init();
	//Tim2 triggers systick which outputs the sample to Dac1
	Tim::init();
	//Waves holds precalculated waveform information. The calculation needs to be done before the main tasks start
	Waves::init();
	//For checking success of task creation
	BaseType_t taskCreateReturn;
	Usart_2::transmit_byte('x');
	//See queue descriptions above
	queue_handle = xQueueCreate(10,sizeof(uint8_t));
	uart_byte_queue_handle = xQueueCreate(10, sizeof(uint8_t));
	taskCreateReturn = xTaskCreate(thread1, "Add Voice and Calculate Sample", 2048, NULL, 1, NULL);	
	if (taskCreateReturn != pdPASS)
		//error
		while(1);
	taskCreateReturn = xTaskCreate(thread2, "Process Midi", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	if (taskCreateReturn != pdPASS)
		//error
		while(1);
	vTaskStartScheduler();
	 /* We should never get here as control is now taken by the scheduler */
	for (;;);
}


#define NUM_VOICES 1

//Thread 1 needs to know the state of the sample calculation to allow operations on 2 independet message queues (see below for explanation). 
enum Sample_value_state { calculated, invalid };

//TODO: Rename this, maybe sample_and_state
//maybe use std::optional
struct Sample {
	float value {0};
	Sample_value_state state {invalid};	
};

//TODO: Capital for Wave_type?
static void thread1(void *argument)
{
	(void) argument;
	uint64_t sample_number {0}	;
	//All voices will be initialised off
	std::array<Voice, NUM_VOICES> voice_array;
	//This holds information which is common to all voices
	Global_parameters global_parameters;
	//Hold sample value and valididty if sample
	Sample sample;
	while (1) {
		uint8_t queue_message;
		//Revieve message from thread 2
		//TODO: define this
		const BaseType_t xQueueReceiveReturn = xQueueReceive(queue_handle, &queue_message, 0);
		if (xQueueReceiveReturn == pdTRUE) {
			//At the moment this just turns one voive on
			voice_array[0].turn_on(global_parameters, 1000, 1);
		}	
		//Sample may still be valid from last loop, but if not calculate another one
		if(sample.state == invalid) {
			//Running total for the sample value
			float total {0};
			*Dac_2::dac2_fast_ptr = 0xFFF;
			//Loop through voices adding sample value to total
			for (Voice& voice : voice_array) {
				total += voice.get_next_sample(sample_number);
			}
			//We can now move to the next sample
			sample_number++;				
			*Dac_2::dac2_fast_ptr = 0x000;
			sample.value = total;
			sample.state = calculated;
		}
		//This function should always be run as the sample should have been calculated by calculation above or in previous loop
		//This buffer send can't block as a message from the input queue may be waiting
		//If the send is unsuccessful we still have a valid sample for the next loop
		if (sample.state == calculated) {
			//This is a buffer to the ISR that outputs the sample
			const bool buffer_add_success = Sample_buffer::add_sample(sample.value * (float) 0.5 + (float) 0.5) ; 	
			if (buffer_add_success)
				//Need to calcute a new sample
				sample.state = invalid;
		}
		else 
			//Should never happen
			while(1);
	}
}

//These two funtions are called by Midi_in on success of recieving a full midi message
//Originally they were implemented as lambdas but std::funtional need dynamic memory allocation
//These two links have a potential workaround
//https : //blog.demofox.org/2015/02/25/avoiding-the-performance-hazzards-of-stdfunction/
//https : //www.quora.com/C++-How-to-pass-a-lambda-function-as-a-parameter-of-another-function

void handle_note_on(Note_on_struct note_on_struct) {
	uint8_t message {'x'};
	xQueueSendToFront(queue_handle, &message, portMAX_DELAY);
} 

void handle_controller_change(Controller_change_struct controller_change_struct) {
	uint8_t message {'x'}	;
	xQueueSendToFront(queue_handle, &message, portMAX_DELAY);
} 

//This thread looks for messages from the UART1 IRQ and handles the data in the Midi_in class
static void thread2(void *argument)
{
	(void) argument;
	uint8_t message {0}	;
	while (1) {
		message = 0;
		const BaseType_t xQueueReceiveReturn = xQueueReceive(uart_byte_queue_handle, &message, portMAX_DELAY);
		if (message != 0) {
			Midi_in::handle_midi_byte(message, handle_note_on, handle_controller_change);
		}
	}
}

//Mainly autogenerated code
//180MHz clock
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct {}	;
	RCC_ClkInitTypeDef RCC_ClkInitStruct {}	;

	/** Configure the main internal regulator output voltage 
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	//Tuned for my microcontroller
	RCC_OscInitStruct.PLL.PLLN = 374;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		while (1) ;
	}
	/** Activate the Over-Drive mode 
	*/
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	{
		while (1) ;
	}
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		while (1) ;
	}
}

