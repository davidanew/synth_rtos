/*This sets up and outputs values to the DAC*/

#ifndef DAC_H
#define DAC_H 1
extern "C" {
	#include "stm32f4xx_hal.h"
}

class Dac {

public:	
	//static const uint32_t dac1_fast_ptr = DAC_BASE + 0x8;
	static void init(DAC_HandleTypeDef& hdac, uint32_t channel_arg); 
	static void set_value_rel(DAC_HandleTypeDef& hdac, uint32_t channel, float value_rel);
	static void set_value(DAC_HandleTypeDef& hdac, uint32_t channel, uint32_t value_rel);
	//static void set_value_fast_dac1(uint32_t value_rel);
	static void HAL_DAC_MspInit( uint32_t channel);	
};

class Dac1 : public Dac {
	static DAC_HandleTypeDef hdac ;
public:
	static void init() {Dac::init(hdac , DAC_CHANNEL_1);}
	static void set_value(uint32_t dac_value) { Dac::set_value(hdac, DAC_CHANNEL_1, dac_value);};	
};




#endif

