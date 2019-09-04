//Currently unused

#pragma once
#include <stm32f4xx_hal.h>
#include <math.h>


//C4	261.63	131.87
// C # 4 / Db4 	277.18	124.47
//D4	293.66	117.48
	
//http://pages.mtu.edu/~suits/notefreqs.html
//https : //en.scratch-wiki.info/wiki/MIDI_Notes

//https : //newt.phys.unsw.edu.au/jw/notes.html
//fm  =  2(m?69)/12(440 Hz).	


//class Note_lookup {
	//float get_freq(uint32_t midi_number) {
		//const float freq = (float ) 440 * powf(  (float ) 2, (float) midi_number - (float ) 69);
		//return freq;
	//}	
//};