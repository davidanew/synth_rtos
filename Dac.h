/*This sets up and outputs values to the DAC*/

#pragma once

#include "stm32f4xx_hal.h"

class Dac {

protected:	
	static void init(DAC_HandleTypeDef& hdac, uint32_t channel_arg); 
	static void set_value_rel(DAC_HandleTypeDef& hdac, uint32_t channel, float value_rel);
	static void set_value(DAC_HandleTypeDef& hdac, uint32_t channel, uint32_t value_rel);
	static void HAL_DAC_MspInit( uint32_t channel);	
};

class Dac_1 : public Dac {
	static DAC_HandleTypeDef hdac ;
public:
	static const uint32_t dac1_fast_ptr = DAC_BASE + 0x8;

	static void init() {Dac::init(hdac , DAC_CHANNEL_1);}
	static void set_value(uint32_t dac_value) { Dac::set_value(hdac, DAC_CHANNEL_1, dac_value);};
	static void set_value_rel(float dac_value)  {Dac::set_value_rel(hdac, DAC_CHANNEL_1, dac_value);};
	static void set_value_fast(uint32_t value);

};

class Dac_2 : public Dac {
	static DAC_HandleTypeDef hdac;
public:
	//TODO: Do the same with dac1 ptr
	static uint32_t * dac2_fast_ptr; // = (uint32_t *) DAC_BASE + 0x14;
	static void init() {Dac::init(hdac, DAC_CHANNEL_2); }
	static void set_value(uint32_t dac_value) { Dac::set_value(hdac, DAC_CHANNEL_2, dac_value); }	;
	static void set_value_rel(float dac_value)  {Dac::set_value_rel(hdac, DAC_CHANNEL_2, dac_value); }	;
	static void set_value_fast(uint32_t value);

};



