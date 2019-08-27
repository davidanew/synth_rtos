#pragma once

#include <stm32f4xx_hal.h>
#include <math.h>

#define NUM_SAMPLES_PER_WAVE 4096

class Waves {
	static float sine_array[NUM_SAMPLES_PER_WAVE];
public:
	static void init(void);
	static float get_sample_with_sample_number_sine(uint32_t);
	static float get_sample_with_phase_rel_sine(float);
	//experimental
	//float sine_array_public[NUM_SAMPLES_PER_WAVE];
};
