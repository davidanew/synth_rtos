#include "main.h"

static void thread1(void *);
static void thread2(void *);
void SystemClock_Config(void);
QueueHandle_t queue_handle;



void *malloc(size_t size)
{
	//no dynamic memory allocation
	while (1) ;
	return NULL;
}

extern "C" {
	
	void vApplicationMallocFailedHook(void)
	{
		__set_PRIMASK(1);
		for (;  ;) ;
	}

	void vApplicationStackOverflowHook( TaskHandle_t xTask,signed char *pcTaskName)
	{
		__set_PRIMASK(1);
		for (;  ;) ;
	}
}


int main(void)
{
	//taskENTER_CRITICAL();
	HAL_Init(); 
	SystemClock_Config();
	Dac_1::init();
	Dac_2::init();
	Usart_2::init();
	Tim::init();
	Waves::init();
	//std::array<uint32_t,3> test_vector {1, 2, 3}	;
	//uint32_t* my_ptr = (uint32_t*) malloc(sizeof(uint32_t));
	//Tests::output_sine();
	BaseType_t taskCreateReturn;
	queue_handle = xQueueCreate(10,sizeof(uint8_t));
	taskCreateReturn = xTaskCreate(thread1, "thread1", 2048, NULL, 2, NULL);
	
	//if (taskCreateReturn != pdPASS)
		////error
		//while(1);
	
	
	
	//xTaskCreate(thread2, "dummy_test", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	vTaskStartScheduler();
	 /* We should never get here as control is now taken by the scheduler */
	for (;;);
}

extern "C" {
	void SysTick_Handler(void)
	{
		HAL_IncTick();
		osSystickHandler();
	}
}

void TIM2_IRQHandler_cpp(void) {
	float sample = Sample_buffer::get_next_sample().value;
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
	void NMI_Handler()                    {while (1) ;}	;
	void HardFault_Handler()              {while (1) ;}	;
	void MemManage_Handler()              {while (1) ;}	;
	void BusFault_Handler()              {while (1) ;}	;
	void UsageFault_Handler()             {while (1) ;}	;
}



#define NUM_VOICES 1

enum Sample_value_state { calculated, invalid };

//TODO: Rename this, maybe sample_and_state
struct Sample {
	float value {0};
	Sample_value_state state {invalid};	
};

//TODO: no capital for Wave_type?
//public array needs initialisation
//
//float calc_sample_for_wave(const uint64_t sample_number, const std::optional<uint64_t>& start_sample, wave_type this_wave_type, const Waves& waves) {
	//
	//uint32_t location 
	//
	//uint32_t location = uint32_t(phase_rel * (float)(NUM_SAMPLES_PER_WAVE - 1));
	////const uint32_t location = 0;
		//float value {0}
	//;
	//if (location < NUM_SAMPLES_PER_WAVE) {
		//value = sine_array[location];
		//return value;
	//}
	//else //error
		//while(1);
	//return (float) 0.0;
//}
//
//
////voice needs start sample
//float calc_sample_for_voice(uint64_t sample_number, Voice& voice, Global_parameters& global_parameters, Waves waves) {
	////global parameters calculations should be done in here
	//const float sample_for_wave_1 = calc_sample_for_wave(sample_number, voice.start_sample, voice.wave_1_type, waves);
	//const float sample_for_wave_2 = calc_sample_for_wave(sample_number, voice.start_sample, voice.wave_2_type, waves);
	//return (float)(sample_for_wave_1 * global_parameters.ampl_1 + 
				 //sample_for_wave_2 * global_parameters.ampl_2)
				//* voice.velocity_public;	
//}



static void thread1(void *argument)
{
	(void) argument;
	
	uint64_t sample_number {0}	;


	



	Sample sample;
	
	BaseType_t xQueueReceiveReturn;
	
	uint8_t queue_message;
	



	
	
	//while (1) ;
	
	std::array<Voice, NUM_VOICES> voice_array;


	
	Global_parameters global_parameters;

	




	

	
	//All voices will be initialised off
	//Voice voice_array[NUM_VOICES];
	

	
	


	//TODO: velocity is a byte?
	//voice_array[0].turn_on(global_parameters, 1000, 1);
	
	while (1) {
		

	
		if(sample.state == invalid) {
			//we need to calculate a new sample
			uint32_t voice_index {0};
			float total {0};
			
			voice_array[0].turn_on(global_parameters, 1000, 1);

			
			*Dac_2::dac2_fast_ptr = 0xFFF;
			
			//const u_int32_t sample_number_start
			
			//std::for_each(voice_array.begin(),
				//voice_array.end(),
				//[&total, &sample_number](Voice& voice) {
//
					//total += calc_sample(sample_number,voice)
					//sample_number++;				
				//});
			//
			//
			//


			
			std::for_each(voice_array.begin(),
				voice_array.end(),
				[&total, &sample_number](Voice& voice) {

					total += voice.get_next_sample(sample_number);
					sample_number++;				
				});
			
			
			
			//float voice_sample {0};
//
			//for (voice_index = 0; voice_index < NUM_VOICES; voice_index++) {
				//voice_sample = voice_array[voice_index].get_next_sample(sample_number);
				//total += voice_sample;
				//sample_number++;
			//}	
			//
			//
			
			*Dac_2::dac2_fast_ptr = 0x000;
			
			sample.value = total;
			sample.state = calculated;
		}
		//This function should always be run as the sample should have been calculated by calculation above or in previous loop
		if (sample.state == calculated) {
			//attempt to write to buffer
			bool buffer_add_success {false};
			buffer_add_success = Sample_buffer::add_sample(sample.value * (float) 0.5 + (float) 0.5) ; 	
			if (buffer_add_success)
				sample.state = invalid;
		}
		else 
			//Should never happen
			while(1);
		
//		xQueueReceiveReturn = xQueueReceive(queue_handle,&queue_message,0);
		if (xQueueReceiveReturn == pdTRUE) {
			voice_array[0].turn_on(global_parameters, 1000, 1);

		}
		
	}
}

static void thread2(void *argument)
{
	(void) argument;
	BaseType_t xQueueSendReturn;
	uint8_t message {0};
	//TODO put in proper wait time

	//xQueueSendReturn = xQueueSendToFront(queue_handle,&message,999);
	while (1) ;
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

