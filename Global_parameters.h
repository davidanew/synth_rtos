#pragma once

enum wave_type { sine_wave };
struct Global_parameters {
	//Wave type for each waveform (oscillator in synth terms)
	wave_type wave_1_type {sine_wave};
	wave_type wave_2_type {sine_wave};
	//Amplitute for each waveform
	float ampl_1 {(float )0.5};
	float ampl_2 {(float )0.5};
	//Sample rate currently set to 40K, 25ms
	float sample_tick_freq {(float )40000};
	//TODO: This appears to not be used anymore?
	uint64_t period_in_ticks {1000};
};
