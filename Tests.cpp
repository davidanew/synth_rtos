#include "Tests.h"
#include <stm32f4xx_hal.h>
#include "Waves.h"
#include "Sample_buffer.h"

void Tests::output_sine(void) {
	
	uint32_t test_sample_number {0};
	float test_sample {0};
	float test_sample_rel {0};
	bool buffer_add_success {false}	;
	
	while (1) {
		test_sample = Waves::get_sample_with_sample_number_sine(test_sample_number);
		test_sample_rel = test_sample * (float) 0.5 + (float) 0.5; 	

		buffer_add_success = Sample_buffer::add_sample(test_sample_rel);
		if (buffer_add_success)
			test_sample_number++;
		if (test_sample_number == NUM_SAMPLES_PER_WAVE)
			test_sample_number = 0;
	}
	
}