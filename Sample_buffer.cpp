#include "Sample_buffer.h"

//This is a buffer between the sample calculation class the the ISR sample output 
//It is needed as sample calculation is not running all the tile as in the 
//previous project (synth_vs)
std::array<float, SAMPLE_BUFFER_LENGTH> Sample_buffer::buffer;

uint32_t Sample_buffer::last_output_index {0};
uint32_t Sample_buffer::last_input_index {0};

//race condition 1:
//input ptr               I->
//buffer ---------oldest--------old-------------
//output ptr             O->
//output is catching up with input
//output must not overtake input
//input should always be ahead of output

//race condition 2:
//input ptr             I->
//buffer ----newest---------new-------------
//output ptr             O->
//input is catching up with output
//input must not overtake output
//output should always be ahead of input

//As this will be difficult to get right it was decided to just diable interruts on the add_sample
//get_sample is safe as it is run from a top priority interrupt

//Return true/false depending if a sample was added
//(buffer may be full)
bool Sample_buffer::add_sample(const float sample) {
	//If this code is interrupted by tim isr they will be a 
	//clash. So turn off interrupts
	__set_PRIMASK(1);
	uint32_t this_input_index = last_input_index + 1;
	if (this_input_index == SAMPLE_BUFFER_LENGTH)
		//Sample buffer is a finite size
		//If we reach the end then go back to the start
		this_input_index = 0;
	if (this_input_index == last_output_index) {
		//We have caught up with the output buffer and can't add any more samples
		__set_PRIMASK(0);
		return false; //buffer full
	}
	//Now add sample to buffer
	buffer[this_input_index] = sample;
	last_input_index = this_input_index;
	__set_PRIMASK(0);
	return true; //(success)
}
Float_optional Sample_buffer::get_next_sample() {
	//This is called by systick which is top priority, so will not be interrupted by
	//task which is adding sample
	if (last_output_index == last_input_index) 
		//There are no new samples
		//The false in the struct return will indicate failure
		//This case means the sample calculation is running behind
		return Float_optional {buffer[last_output_index], false};
	//There is a new sample and we can index it
	uint32_t this_output_index = last_output_index + 1;
	//Loop back round in buffer if necessary
	if (this_output_index == SAMPLE_BUFFER_LENGTH)
		this_output_index = 0;
	//Update class state to indicate what the last sample to be be output was
	last_output_index = this_output_index;
	//Return value and success
	return Float_optional { buffer[this_output_index], true };
}