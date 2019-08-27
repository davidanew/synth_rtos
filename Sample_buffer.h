#pragma once

extern "C" {
	#include <stm32f4xx_hal.h>
}
#include <array>


struct Float_optional {
	float value;
	bool valid;
};

#define SAMPLE_BUFFER_LENGTH 256
class Sample_buffer {
	//static float buffer[SAMPLE_BUFFER_LENGTH];
	static std::array<float, SAMPLE_BUFFER_LENGTH> buffer;
	static uint32_t last_output_index; 
	static uint32_t last_input_index;
public:
	//static bool is_
	static bool add_sample(const float sample);
	static Float_optional get_next_sample();
};