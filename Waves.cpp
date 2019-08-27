#include "Waves.h"

float Waves::sine_array[NUM_SAMPLES_PER_WAVE] {};
	
void Waves::init() {
	uint32_t i = 0;
	for (i = 0; i < NUM_SAMPLES_PER_WAVE; i++) {
		float phase_radians = (float)((float)i / (float) NUM_SAMPLES_PER_WAVE * (float)  6.28318530718);
		//float float_i = (float) i;
		//float float_num_samples = (float) NUM_SAMPLES_PER_WAVE;
		//float float_pi = (float)  6.28318530718;
		
		//float phase_radians = (float) 0.0;
		float value = (float) sin(phase_radians);
		//float value = (float) 0.0;

		sine_array[i] = value;	
	}
}

float Waves::get_sample_with_sample_number_sine(uint32_t location) {
	if (location < NUM_SAMPLES_PER_WAVE)
		return sine_array[location];
	else
		return 0;	
}

float Waves::get_sample_with_phase_rel_sine(float phase_rel){
	uint32_t location = uint32_t(phase_rel * (float)(NUM_SAMPLES_PER_WAVE - 1));
//const uint32_t location = 0;
	float value {0};
	if (location < NUM_SAMPLES_PER_WAVE) {
		value = sine_array[location];
		return value;
	}
	else //error
		while(1);
}