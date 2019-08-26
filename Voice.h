#pragma once
#include <stm32f4xx_hal.h>
#include "Global_parameters.h"
#include "Waves.h"
#include <optional>


enum Voice_state{ on , off };

class Voice {
	
	Global_parameters global_parameters;
	wave_type wave_1 {sine_wave};
	wave_type wave_2 {sine_wave};
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
public:		
	//Never contructed with this one?
	//what about global parameters?
	//Voice(const Global_parameters& global_parameters, const uint32_t& freq, const float &velocity);
	void turn_on(const Global_parameters& global_parameters, const uint32_t& freq, const float &velocity);
	float get_next_sample(const uint64_t sample_number);
	//exprimental
	std::optional<uint64_t> start_sample = std::nullopt;
	wave_type wave_1_type {sine_wave}	;
	wave_type wave_2_type {sine_wave}	;
	float velocity_public {0};

};
