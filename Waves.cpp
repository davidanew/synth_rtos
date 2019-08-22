#include "Waves.h"


float Waves::sine_array[NUM_SAMPLES_PER_WAVE] {};


	
void Waves::init() {
	uint32_t i = 0;
	for (i = 0; i < NUM_SAMPLES_PER_WAVE; i++) {
		float phase_radians = (float)i / (float) NUM_SAMPLES_PER_WAVE * (float)  6.28318530718;
		float value = (float) sin(phase_radians);
		sine_array[i] = value;	
	}
}

float Waves::get_sample_with_sample_number_sine(uint32_t sample_number) {
	if (sample_number < NUM_SAMPLES_PER_WAVE)
		return sine_array[sample_number];
	else
		return 0;	
}