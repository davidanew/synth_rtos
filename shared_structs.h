#pragma once
#include <stm32f4xx_hal.h>


//Holds data for MIDI 'Note On' message
struct Note_on_struct {
	uint8_t note_number {0}	;
	uint8_t velocity {0}	;
	uint32_t get_frequecy()
	{
		//TODO: gte this to work
		return note_number;
	}
}
;

//Holds data for MIDI 'Control Change' message
struct Controller_change_struct {
	uint8_t controller_number {0}	;
	uint8_t controller_data {0}	;
};







