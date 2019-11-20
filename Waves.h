#pragma once

//Holds data for standard waveforms and provides getter methods

#include <stm32f4xx_hal.h>
#include <math.h>

//Chose 4096 samples as this is the same as the resolution of the DAC
//No point going much above this
#define NUM_SAMPLES_PER_WAVE 4096

class Waves {
	static float sine_array[NUM_SAMPLES_PER_WAVE];
public:
	static void init(void);
	static float get_sample_with_sample_number_sine(uint32_t);
	static float get_sample_with_phase_rel_sine(float);
};
