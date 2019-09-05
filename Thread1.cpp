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
	//Hold sample value and valididty if sample
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
		if(note_on_struct.velocity_byte > 1) {
		//bool note_found {false}		;
			//See if note exists in array
			
		//for loop version	
		//for(Voice& voice : voice_array) {
			//if (voice.note_number == note_on_struct.note_number) {
				//note_found = true;
				////restart voice
				//voice.turn_on(global_parameters, note_on_struct.note_number, 1, sample_number);					
			//}				
		//}
			//
		//STL version (under test)
		//Note it still needs to turn voice on
		//may be able to do this in some other execution functiond	
			
		//See if note exists in array
		const auto voice_it_find_note = std::find_if(voice_array.begin(), voice_array.end(), [&](Voice& voice) {return voice.note_number == note_on_struct.note_number;});		
		const bool note_found = !(voice_it_find_note == voice_array.end());	
		if (note_found) //restart voice
			voice_it_find_note->turn_on(global_parameters, note_on_struct.note_number, 1, sample_number);
		
			
			//if (note_found_stl != note_found)
				//while (1) ; // stl test error
			//const bool note_found {note_found_stl};
		

		if (!note_found) {
			////If note is not in voice array then find oldest note and replace it
			//uint32_t oldest_voice_index = 0;
			////TODO: use std::array better
			//for(uint32_t i = 0 ; i < NUM_VOICES ; i++) {
				//if(voice_array[i].start_sample_number < voice_array[oldest_voice_index].start_sample_number)
					//oldest_voice_index = i;
			//}
			//Stl version
			const auto voice_earliest_it = std::min_element(voice_array.begin(), voice_array.end(), [&](Voice& voice1, Voice& voice2) {return voice1.start_sample_number < voice2.start_sample_number;});
			voice_earliest_it->turn_on(global_parameters, note_on_struct.note_number, 1, sample_number);
			
			
			//now test and turn on voice
			//if(voice_it2->start_sample_number != voice_array[oldest_voice_index].start_sample_number)
				//while(1); //STL error
			
			//this is part of the for loop version
			//voice_array[oldest_voice_index].turn_on(global_parameters, note_on_struct.note_number, 1, sample_number);		


		}
	}
	else {
		//Velocity is 0 so turn this voice off
		bool note_found {false}		;
		//See if note exists in array
		//If so then turn it off
		for(Voice& voice : voice_array) {
			if (voice.note_number == note_on_struct.note_number) 
				voice.turn_off();
		}
	}
}