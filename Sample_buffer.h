#pragma once

//This is a buffer between the sample calculation class and the ISR sample output 
//It is needed as sample calculation is not running all the time 

//TODO: This doesn't need to to be extern'd
extern "C" {
	#include <stm32f4xx_hal.h>
}
#include <array>

//When the sample is recieved it may not be valid if the buffer is empty
//So a validity is needed
//TODO: Look at std::optional for this
struct Float_optional {
	float value;
	bool valid;
};

//40 samples per tick
//At worst the sample calculator should be running 50% of the time
//This should ne enough
#define SAMPLE_BUFFER_LENGTH 256
class Sample_buffer {
	static std::array<float, SAMPLE_BUFFER_LENGTH> buffer;
	static uint32_t last_output_index; 
	static uint32_t last_input_index;
public:
	static bool add_sample(const float sample);
	static Float_optional get_next_sample();
};