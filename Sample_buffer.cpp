#include "Sample_buffer.h"

float Sample_buffer::buffer[SAMPLE_BUFFER_LENGTH] {};
uint32_t Sample_buffer::last_output_index {0};
uint32_t Sample_buffer::last_input_index {0};

bool Sample_buffer::add_sample(float sample) {
	uint32_t this_input_index = last_input_index + 1;
	if (this_input_index == SAMPLE_BUFFER_LENGTH)
		this_input_index = 0;
	if (this_input_index == last_output_index)
		//We have caught up with the output buffer and can't add any more samples
		return false;
	buffer[this_input_index] = sample;
	last_input_index = this_input_index;
	return true; //(success)
}
Float_optional Sample_buffer::get_next_sample() {
	if (last_output_index == last_input_index) 
		//There are no new samples
		//The false in the struct return will indicate failure
		return Float_optional {(float) 0.0, false}
	;
	uint32_t this_output_index = last_output_index + 1;
	if (this_output_index == SAMPLE_BUFFER_LENGTH)
		this_output_index = 0;
	last_output_index = this_output_index;
	return Float_optional { buffer[this_output_index], true };
}