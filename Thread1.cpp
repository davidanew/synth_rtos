#include "Thread1.h"

void note_on_update_voice_array(std::array<Voice, NUM_VOICES>& voice_array, const Note_on_struct& note_on_struct, const Global_parameters& global_parameters, const uint64_t sample_number);

//Currently thread 1 takes midi command messages from thread 2 and updates the voice array. The voice array is used to construct the waveform and this is sent to the sample buffer
void thread1(void *argument)
{
	(void) argument;
	uint64_t sample_number {0}	;
	//All voices will be initialised off
	std::array<Voice, NUM_VOICES> voice_array;
	//This holds information which is common to all voices
	Global_parameters global_parameters;
	//Hold sample value and validity if sample
	Sample sample;
	while (1) {
		Midi_command_queue_message midi_command_queue_message;
		//Revieve message from thread 2
		const BaseType_t xQueueReceiveReturn = xQueueReceive(midi_command_queue_handle, &midi_command_queue_message, 0);
		if (xQueueReceiveReturn == pdTRUE) {
			if(midi_command_queue_message.message_type == note_on) {
				//Handle 'note on message'
				note_on_update_voice_array(voice_array, midi_command_queue_message.note_on_struct, global_parameters, sample_number);
			}
		}	
		//Sample may still be valid from last loop, but if not calculate another one
		if(sample.state == invalid) {
			//Running total for the sample value
			float total {0}			;
			//DAC 2 used to timing debug/signalling
			*Dac_2::dac2_fast_ptr = 0xFFF;
			//Loop through voices adding sample value to total
			for(Voice& voice : voice_array) {
				total += voice.get_next_sample(sample_number);
			}
			//We can now move to the next sample
			sample_number++;				
			*Dac_2::dac2_fast_ptr = 0x000;
			sample.value = total;
			sample.state = calculated;
		}
		//This function should always be run as the sample should have been calculated by calculation above or in previous loop
		//This buffer send can't block as a message from the input queue may be waiting
		//If the send is unsuccessful we still have a valid sample for the next loop
		if(sample.state == calculated) {
			//This is a buffer to the ISR that outputs the sample
			const bool buffer_add_success = Sample_buffer::add_sample(sample.value * (float) 0.5 + (float) 0.5); 	
			if (buffer_add_success)
				//Need to calcute a new sample
				sample.state = invalid;
		}
		else 
			//Should never happen
			while(1);
	}
}

void note_on_update_voice_array(std::array<Voice, NUM_VOICES>& voice_array, const Note_on_struct& note_on_struct, const Global_parameters& global_parameters, const uint64_t sample_number) {
	//See if note exists in array
	const auto voice_find_note_it = std::find_if(voice_array.begin(), voice_array.end(), [&](Voice& voice) {return voice.note_number == note_on_struct.note_number;});		
	const bool note_found = !(voice_find_note_it == voice_array.end());	
	//if velocity is greater than zero it is a real note on
	if(note_on_struct.velocity_byte > 0) {	
		if (note_found) //restart voice
			voice_find_note_it->turn_on(global_parameters, note_on_struct.note_number, 1, sample_number);
		else {
			////If note is not in voice array then find oldest note and replace it
			const auto voice_earliest_it = std::min_element(voice_array.begin(), voice_array.end(), [&](Voice& voice1, Voice& voice2) {return voice1.start_sample_number < voice2.start_sample_number;});
			voice_earliest_it->turn_on(global_parameters, note_on_struct.note_number, 1, sample_number);
		}
	}
	//Velocity is 0 so turn this voice off
	else {
		if (note_found) //turn off voice
			voice_find_note_it->turn_off();		
	}
}