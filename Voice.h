#pragma once
#include <stm32f4xx_hal.h>
#include "Global_parameters.h"
#include "Waves.h"

//This class holds hold one one voice, which has two waveform generators

enum Voice_state{ on , off };

class Voice {
	//Patremeters that are shared between voices
	Global_parameters global_parameters;
	Wave_type wave_1 {sine_wave};
	Wave_type wave_2 {sine_wave};
	//Frequencies for both voices
	float freq_1 {1000};
	float freq_2 {1000};
	Voice_state state {off}	;
	//Shared velocity set by keypress
	float velocity {0};
	//Class needs to store previous sample number
	//The get sample funtion gets passed the current sample number
	//So then the correct sample can be calculated
	uint64_t previous_sample_number {0};
	//Used in sample calculation
	//TODO Comment about these holding prev/current phase
	float phase_rel_1 {0};
	float phase_rel_2 {0};
	//How much the phase is updated per sample
	float phase_rel_per_sample_1 {0};
	float phase_rel_per_sample_2 {0};
	float get_freq_for_note_number(uint8_t note_number);

public:		
	uint32_t note_number {0};
	uint64_t start_sample_number {0};
	//Default constructor is used. Voice is initialised off so will output 0
	void turn_on(const Global_parameters& global_parameters, uint8_t note_number, const float &velocity, const uint64_t sample_number);
	void turn_off();

	float get_next_sample(const uint64_t sample_number);
};
