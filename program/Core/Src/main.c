/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "oled.h"
#include "wheel.h"
#include "resistor_calculate.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void show_volt(void);
void show_power_off(void);
void power_handler(void);
void spi_transmit(void);
void resistor_calculate(void);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int volt=0;
uint8_t data=255;
volatile bool oled_switch=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_ADC_Init();
  MX_TIM21_Init();
  MX_TIM2_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Encoder_Start(&htim21,TIM_CHANNEL_ALL);
	OLED_Init();
	OLED_Clear();
	OLED_WriteCmd(0xAE); 
	wheel_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  	power_handler();
	  	wheel_update();
		resistor_calculate();
		spi_transmit();
	  	show_volt();
		HAL_IWDG_Refresh (&hiwdg);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void spi_transmit(){
	static uint8_t last_data=0;
	if(data==last_data) return;
	last_data=data;
	HAL_GPIO_WritePin (GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&last_data,1,50);
	HAL_GPIO_WritePin (GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}
void show_volt(void){
	if(oled_switch==0)return;
	OLED_ShowNum32(0,(volt/1000)%10);
	OLED_ShowNum32(16,(volt/100)%10);
	OLED_ShowNum32(40,(volt/10)%10);
	OLED_ShowNum32(56,volt%10);
}
void show_power_off(){
	OLED_PutChar(80,'O');
	OLED_PutChar(96,'F');
	OLED_PutChar(112,'F');
}
void power_handler(){
	static bool turned_off=1;
	static bool turned_on=0;
	static bool output_enabled=0;
	static bool output_disabled=0;
	if(oled_switch==0&&turned_off==0){
		GPIOA->PUPDR =
			(GPIOA->PUPDR & ~(GPIO_PUPDR_PUPD2_Msk | GPIO_PUPDR_PUPD3_Msk)) |
               (GPIO_PULLDOWN << 4) |  // PA2偏移：2×2=4
               (GPIO_PULLDOWN << 6); // PA3偏移：3×2=6
		HAL_TIM_Base_Stop_IT(&htim2);
		__HAL_TIM_SET_COUNTER(&htim2,0);
		OLED_WriteCmd(0xAE); 
		turned_off=1;
		turned_on=0;
	}
	if(oled_switch==1&&turned_on==0){
		GPIOA->PUPDR =
			(GPIOA->PUPDR & ~(GPIO_PUPDR_PUPD2_Msk | GPIO_PUPDR_PUPD3_Msk)) |
               (GPIO_PULLUP << 4) |  // PA2偏移：2×2=4
               (GPIO_PULLUP << 6); // PA3偏移：3×2=6
		HAL_TIM_Base_Start_IT(&htim2);
		OLED_WriteCmd(0xAF); 
		turned_off=0;
		turned_on=1;
	}
	if(volt<200&&output_disabled==0){
		output_disabled=1;
		output_enabled=0;
		OLED_Clear();
		OLED_ShowDot(3,33);
		show_power_off();
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
	}
	if(volt>=200&&output_enabled==0){
		output_disabled=0;
		output_enabled=1;
		OLED_Clear();
		OLED_PutChar(72,'V');
		OLED_ShowDot(3,33);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint32_t last_interrupt_time = 0;
	uint32_t current_time = HAL_GetTick();
	if(current_time - last_interrupt_time < 50
		&&current_time - last_interrupt_time >10000) return;
    last_interrupt_time = current_time;
	
	if(GPIO_Pin == SWITCH_Pin)
	{
		oled_switch=!oled_switch;
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	oled_switch=0;
	HAL_TIM_Base_Stop(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2,0);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
