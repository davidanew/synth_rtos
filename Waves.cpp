#include "Waves.h"

//Holds data for standard waveforms and provides getter methods

//Arrays hold one cycle of waveform
float Waves::sine_array[NUM_SAMPLES_PER_WAVE] {};
//init needs to be run to set the data	
void Waves::init() {
	uint32_t i = 0;
	for (i = 0; i < NUM_SAMPLES_PER_WAVE; i++) {
		const float phase_radians = (float)((float)i / (float) NUM_SAMPLES_PER_WAVE * (float)  6.28318530718);
		const float value = (float) sin(phase_radians);
		sine_array[i] = value;	
	}
}

//Use the sample number to index the data
float Waves::get_sample_with_sample_number_sine(uint32_t location) {
	//Original implementation used abstact classes which meant that we didn't need a new
	//function for every waveform type
	if (location < NUM_SAMPLES_PER_WAVE)
		return sine_array[location];
	else
		//TODO: this should be error as in _rel version 
		return 0;	
}

//Use phase (value range 0 to 1) to index the data
float Waves::get_sample_with_phase_rel_sine(float phase_rel){
	uint32_t location = uint32_t(phase_rel * (float)(NUM_SAMPLES_PER_WAVE - 1));
	if (location < NUM_SAMPLES_PER_WAVE) {
		const float value = sine_array[location];
		return value;
	}
	else //error
		while(1);
}