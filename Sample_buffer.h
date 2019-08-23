#pragma once

extern "C" {
	#include <stm32f4xx_hal.h>
}

struct Float_optional {
	float value;
	bool valid;
};

#define SAMPLE_BUFFER_LENGTH 256
class Sample_buffer {
	//const static  uint32_t buffer_size;
	static float buffer[SAMPLE_BUFFER_LENGTH];
	static uint32_t last_output_index; 
	static uint32_t last_input_index;
public:
	static bool is_
	static bool add_sample(float);
	static Float_optional get_next_sample();
};