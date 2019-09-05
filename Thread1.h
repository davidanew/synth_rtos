#pragma once
#include <stm32f4xx_hal.h>
#include <../CMSIS_RTOS/cmsis_os.h>
#include "Dac.h"
#include "Usart.h"
#include "Sample_buffer.h"
#include "Waves.h"
#include "Voice.h"
#include "Global_parameters.h"
#include "array"
#include "Shared.h"
//#include "Note_lookup.h"
#include "algorithm"


void thread1(void *argument);

#define NUM_VOICES 2

//Thread 1 needs to know the state of the sample calculation to allow operations on 2 independet message queues (see below for explanation). 
enum Sample_value_state { calculated, invalid };


//TODO: Rename this, maybe sample_and_state
//maybe use std::optional
struct Sample {
	float value {0}	;
	Sample_value_state state {invalid}	;	
};

