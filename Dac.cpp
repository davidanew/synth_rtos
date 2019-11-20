#include "Dac.h"

//Handles all DAC related operation

//Initialise DAC 
//Based on CubeMX code
void Dac::init(DAC_HandleTypeDef& hdac, uint32_t channel) {
	//channel = channel_arg;
	DAC_ChannelConfTypeDef sConfig {};
	hdac.Instance = DAC;
	hdac.State = HAL_DAC_STATE_RESET;
	//Normally this is a call back, but would rather keep this with the object
	//So manually call member funtion here
	//Standard call back will do nothing
	HAL_DAC_MspInit(channel);				
	if (HAL_DAC_Init(&hdac) != HAL_OK)
	{
		while(1);
	}
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	if (HAL_DAC_ConfigChannel(&hdac, &sConfig, channel) != HAL_OK)
	{
		while(1);
	}
	if (HAL_DAC_Start(&hdac , channel) != HAL_OK)
	{
		while(1);
	}
}	

//Output value to DAC with value range 0->1
void Dac::set_value_rel(DAC_HandleTypeDef& hdac, uint32_t channel, float value_rel) {
	//if (value_rel <(float)-1.0 || value_rel > (float) 1.0) 
	//	while(1);
	if (value_rel > (float) 1.0)
		value_rel = 1.0;
	if (value_rel < (float) -1.0)
		value_rel = -1.0;
	uint32_t dac_value = (uint32_t) ((float) 0xFFF * value_rel );

	set_value(hdac ,channel, dac_value);
}

//Output value to dac with integer value
void Dac::set_value(DAC_HandleTypeDef& hdac, uint32_t channel, uint32_t value) {
//	uint32_t dac_value = value;
	if (value > 0xFFF)
		while(1);
	if (HAL_DAC_SetValue(&hdac, channel, DAC_ALIGN_12B_R, value) != HAL_OK ){
		while(1);
	}
}


//Standard init function as member function, not C call back
void Dac::HAL_DAC_MspInit(uint32_t channel)
{
	GPIO_InitTypeDef GPIO_InitStruct {}; //= {0};
	__HAL_RCC_DAC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();		
	if (channel == DAC_CHANNEL_1)
		GPIO_InitStruct.Pin = GPIO_PIN_4;
	else if (channel == DAC_CHANNEL_2)
		GPIO_InitStruct.Pin = GPIO_PIN_5;
	else //error
		while(1);
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		
}

//Dac_1 and Dac_2 inherited from Dac
//Note that there is some implementation code in the header 
DAC_HandleTypeDef Dac_1::hdac {};

//This funtion bypasses the HAL code
//TODO: maybe put this in the header like the other funtions as it only does one thing
void Dac_1::set_value_fast(uint32_t value) {
	*(uint32_t *)dac1_fast_ptr = value;
}

DAC_HandleTypeDef Dac_2::hdac {};

uint32_t * Dac_2::dac2_fast_ptr = (uint32_t *) (DAC_BASE + 0x14);

//As Dac_1
void Dac_2::set_value_fast(uint32_t value) {
	*dac2_fast_ptr = value;
}
