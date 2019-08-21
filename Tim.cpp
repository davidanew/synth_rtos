#include "Tim.h"

//Currently uses TIM2
//Hard coded values for 40kHz
void Tim::init(void)
{
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 90;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 25;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	//Call callback as member funtion for encapsulation purposes
	//Standard callback will do nothing
	HAL_TIM_Base_MspInit(&htim2);
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		while (1) ;
	}
	if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
	{
		while (1) ;
	}
}
void Tim::HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	if (htim_base->Instance == TIM2)
	{
		__HAL_RCC_TIM2_CLK_ENABLE();		
		HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	}
}

//Return sample tick time in us
uint64_t Tim::sample_tick_us() {
	return 25;
}

TIM_HandleTypeDef Tim::htim2 {};