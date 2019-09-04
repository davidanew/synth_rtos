#include "Voice.h"

//Calculates frequency for given note number
float Voice::get_freq_for_note_number(uint8_t note_number) {
	//Formula from: newt.phys.unsw.edu.au/jw/notes.html
	//TODO: check these limits
	if(note_number >= 21 && note_number <= 108) {
		const float freq = (float) 440 * powf((float) 2, ((float) note_number - (float) 69) / (float) 12);
		return freq;
	}
	else
		while(1); //error
}	

void Voice::turn_on(const Global_parameters& global_parameters, uint8_t note_number, const float &velocity, const uint64_t sample_number) {
	start_sample_number = sample_number;
	this->global_parameters = global_parameters;
	this->note_number = note_number;
	freq_1 = freq_2 = get_freq_for_note_number(note_number);
	this->velocity = velocity;	
	state = on;
	//TODO: These frequencies need to be seperate or have offsets
	phase_rel_per_sample_1 = freq_1 / global_parameters.sample_tick_freq;
	phase_rel_per_sample_2 = freq_2 / global_parameters.sample_tick_freq;
}

void Voice::turn_off() {
	state = off;
	//TODO comment that note_number means 0 means off, look at what else this means
	note_number = 0;
}

//This function updates the phase and sample tick variables to the latest situation
//(function has side effects)
//The current sample is then calculated
float Voice::get_next_sample(const uint64_t sample_number) {
	if (state == off)
		return 0;
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
	switch (global_parameters.wave_1_type) {
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
	//TODO: if amplitude is zero and end of adsr envelope then turn off?
	//does on or off even matter?

	return (float)(wave_1_value * global_parameters.ampl_1 + 
					wave_2_value * global_parameters.ampl_2)
	                * velocity;
}	
