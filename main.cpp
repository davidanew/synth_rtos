/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_ThreadCreation/Src/main.c
  * @author  MCD Application Team
  * @version V1.2.2
  * @date    25-May-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>
#include <../CMSIS_RTOS/cmsis_os.h>
#include "cpp_test.h"
#include "Dac.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
osThreadId LEDThread1Handle, LEDThread2Handle;

/* Private function prototypes -----------------------------------------------*/
static void LED_Thread1(void const *argument);
static void LED_Thread2(void const *argument);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

void SystemClock_Config(void);


int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
	
	//cpp_test();
	
	
	HAL_Init(); 
	
	SystemClock_Config();

	
	
	Dac1::init();
	Dac1::set_value(0xFFF);
	//while (1) ;

	
	
	//__GPIOA_CLK_ENABLE();
	//GPIO_InitTypeDef GPIO_InitStructure;
//
	//GPIO_InitStructure.Pin = GPIO_PIN_5 | GPIO_PIN_4;
//
	//GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	//GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	//GPIO_InitStructure.Pull = GPIO_NOPULL;
	//HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	char led1[] = "LED1"; 
	char led2[] = "LED2"; 

	/* Thread 1 definition */
	const osThreadDef_t os_thread_def_LED1 = \
     { led1, LED_Thread1, osPriorityNormal, 0, configMINIMAL_STACK_SIZE };
  
	/*  Thread 2 definition */
	const osThreadDef_t os_thread_def_LED2 = \
	{ led2, LED_Thread2, osPriorityNormal, 0, configMINIMAL_STACK_SIZE };
  
	/* Start thread 1 */	
	LEDThread1Handle = osThreadCreate(&os_thread_def_LED1, NULL);

	/* Start thread 2 */
	LEDThread2Handle = osThreadCreate(&os_thread_def_LED2, NULL);

	/* Start scheduler */
	osKernelStart();

	 /* We should never get here as control is now taken by the scheduler */
	for (;;);
}


extern "C" {
	void SysTick_Handler(void)
	{
		Dac1::set_value_fast(0xFFF);
		HAL_IncTick();
		osSystickHandler();
		Dac1::set_value_fast(0x000);
	}
}

/**
  * @brief  Toggle LED1
  * @param  thread not used
  * @retval None
  */
static void LED_Thread1(void const *argument)
{
	(void) argument;
	
	while (1)
		Dac1::set_value_fast(0x7FF);
  
	//for (;;)
	//{
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		//osDelay(2000);
		//
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		//osThreadSuspend(LEDThread2Handle);
		//osDelay(2000);r
		//
		//osThreadResume(LEDThread2Handle);
	//}
}

/**
  * @brief  Toggle LED2 thread
  * @param  argument not used
  * @retval None
  */
static void LED_Thread2(void const *argument)
{
	//uint32_t count;
	(void) argument;
	
	
	
	
	
  
	//for (;;)
	//{
		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
		//osDelay(200);
	//}
	while(1)
		Dac1::set_value_fast(0xFFF);

	
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
	while (1)
	{
	}
}
#endif


//Mainly autogenerated code
//180MHz clock
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct {}
	;
	RCC_ClkInitTypeDef RCC_ClkInitStruct {}
	;

	/** Configure the main internal regulator output voltage 
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	//Tuned for my microcontroller
  RCC_OscInitStruct.PLL.PLLN = 374;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		while (1) ;
	}
	/** Activate the Over-Drive mode 
	*/
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	{
		while (1) ;
	}
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		while (1) ;
	}
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
