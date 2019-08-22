
#include "main.h"

static void thread1(void *);
static void thread2(void *);

void SystemClock_Config(void);



enum wave_type { sine_wave };
struct Global_parameters {
	//Wave type for each waveform (oscillator in synth terms)
	wave_type wave_1_type {sine_wave};
	wave_type wave_2_type {sine_wave};
	//Amplitute for each waveform
	float ampl_1 {0.5};
	float ampl_2 {0.5};
	//Sample rate currently set to 40K, 25ms
	float sample_tick_freq {40000};
	//TODO: This appears to not be used anymore?
	uint64_t period_in_ticks {1000};
};


class Voice {
	Global_parameters global_parameters;
	wave_type wave_1 {sine_wave};
	wave_type wave_2 {sine_wave};
	//Frequencies for both voices
	float freq_1 {1000}	;
	float freq_2 {1000}	;
	//Shared velocity set by keypress
	float velocity {0}	;
	//Class needs to store previous sample number
	//The get sample funtion gets passed the current sample number
	//So then the correct sample can be calculated
	uint64_t previous_sample_number {0}	;
	//Used in sample calculation
	//TODO Comment about these holding prev/current phase
	float phase_rel_1 {0};
	float phase_rel_2 {0};
	//How much the phase is updated per sample
	float phase_rel_per_sample_1 {0};
	float phase_rel_per_sample_2 {0};
public:		
	Voice(const Global_parameters& global_parameters, const uint32_t& freq, const float &velocity);
	//Voice() = default;
	////automatic variables used for pointers, these three definitions left in for debugging
	//~Voice() = default;
	//Voice(const Voice &source) = default;
	//Voice& operator=(const Voice& rhs) = default;
	float get_next_sample(const uint64_t sample_number);
};

//Construct new voice
//This will be placed in the voice map in main
Voice::Voice(const Global_parameters& global_parameters, const uint32_t& freq, const float &velocity)
	: global_parameters(global_parameters)
	, freq_1 (freq)
	, freq_2 (freq)
	, velocity(velocity)
{
	//TODO: These frequencies need to be seperate or have offsets

	phase_rel_per_sample_1 = freq_1 / global_parameters.sample_tick_freq;
	phase_rel_per_sample_2 = freq_2 / global_parameters.sample_tick_freq;
	//try {
		////TODO: These will also need to be set with dial
		//switch(global_parameters.wave_1_type) {
		//case sine_wave :
			//wave_1 = std::shared_ptr<Wave> { new Sine() };
			//break;       
		//default:
			//wave_1 = std::shared_ptr<Wave> { new Square() };
			//break;
		//}
		//switch (global_parameters.wave_2_type) {
		//case sine_wave :
			//wave_2 = std::shared_ptr<Wave> { new Sine() };
			//break;       
		//default:
			//wave_2 = std::shared_ptr<Wave> { new Square() };
			//break;
		//}
	//}
	//catch (...) {
		//while (1) ;
	//}
}

//This function updates the phase and sample tick variables to the latest situation
//(function has side effects)
//The current sample is then calculated
float Voice::get_next_sample(const uint64_t sample_number) {
	//coded to prevent costly int to float conversion and fmod
	//allows recovery from missed sample ticks
	//may not be necessary in this implementation
	if(previous_sample_number == 0) {
		//Set this so there is not a long delay due to the first sample
		previous_sample_number = sample_number;
	}
	//Get to current sample number,
	//updating phase each time
	//current phase being considered is held in
	//phase_rel_*
	while(sample_number > previous_sample_number) {
		phase_rel_1 += phase_rel_per_sample_1;
		if (phase_rel_1 > (float) 1.0) 
			phase_rel_1 -= 1;
		phase_rel_2 += phase_rel_per_sample_2;
		if (phase_rel_2 > (float) 1.0) 
			phase_rel_2 -= 1;
		previous_sample_number += 1;
	}
	//Previous_sample_number will now be set to the current sample number
	//The function should not be called until the next sample, but it doesn't matter if
	//it is called earlier
	//Calculate the sample from the two waveforms and return value
	
	float wave_1_value {0};
	float wave_2_value {0};
	
	switch(global_parameters.wave_1_type) {
	case sine_wave :
		wave_1_value = Waves::get_sample_with_phase_rel_sine(phase_rel_1);
		break;       
	default:
		wave_1_value = Waves::get_sample_with_phase_rel_sine(phase_rel_1);
		break;
	}
	switch (global_parameters.wave_2_type) {
	case sine_wave :
		wave_2_value = Waves::get_sample_with_phase_rel_sine(phase_rel_1);
		break;       
	default:
		wave_2_value = Waves::get_sample_with_phase_rel_sine(phase_rel_1);
		break;
	}
	return (float)(wave_1_value * global_parameters.ampl_1 + 
					wave_2_value * global_parameters.ampl_2)
	                * velocity;
}	



int main(void)
{
	HAL_Init(); 
	SystemClock_Config();
	Dac_1::init();
	Usart_2::init();
	Tim::init();
	Waves::init();
	//Tests::output_sine();
	xTaskCreate(thread1, "thread1", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(thread2, "dummy_test", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

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

static void thread1(void *argument)
{
	(void) argument;
	
	uint32_t test_sample_number {0}	;
	float test_sample {0};
	float test_sample_rel {0};
	bool buffer_add_success {false};
	
	while (1) {
		test_sample = Waves::get_sample_with_sample_number_sine(test_sample_number);
		test_sample_rel = test_sample * (float) 0.5 + (float) 0.5; 	
		buffer_add_success = Sample_buffer::add_sample(test_sample_rel);
		if (!buffer_add_success)
			taskYIELD()
		else
			test_sample_number++;
		if (test_sample_number == NUM_SAMPLES_PER_WAVE)
			test_sample_number = 0;
	}
}

static void thread2(void *argument)
{
	(void) argument;
	while (1) ;
}

//Mainly autogenerated code
//180MHz clock
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct {}
	;
	RCC_ClkInitTypeDef RCC_ClkInitStruct {}
	;

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

