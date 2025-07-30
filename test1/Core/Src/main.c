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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define SAMPLES 255

#define SMOOTHING_SHIFT 3

#define BUTTON1_Pin GPIO_PIN_12
#define BUTTON1_GPIO_Port GPIOA

#define BUTTON2_Pin GPIO_PIN_11
#define BUTTON2_GPIO_Port GPIOA

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
HAL_StatusTypeDef status;

uint8_t digits[8];

uint32_t buttonPressStartTime = 0;
uint8_t waitingFor2Sec = 0;
uint8_t inPasswordMode = 0;

uint8_t passwordDigits[4] = {0, 0, 0, 0}; // 0000
uint8_t currentDigitIndex = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

// Segment bits: 0b0GFEDCBA (MSB = unused / DP)
uint8_t charTo7Seg(char c)
{
    switch (c)
    {
        case '0': return 0b00111111; // A B C D E F
        case '1': return 0b00000110; // B C
        case '2': return 0b01011011; // A B D E G
        case '3': return 0b01001111; // A B C D G
        case '4': return 0b01100110; // B C F G
        case '5': return 0b01101101; // A C D F G
        case '6': return 0b01111101; // A C D E F G
        case '7': return 0b00000111; // A B C
        case '8': return 0b01111111; // All
        case '9': return 0b01101111; // A B C D F G
        case 'A': return 0b01110111; // A B C E F G
        case 'b': return 0b01111100; // C D E F G
        case 'C': return 0b00111001; // A D E F
        case 'd': return 0b01011110; // B C D E G
        case 'E': return 0b10000110; // A D E F G
        case 'F': return 0b01110001; // A E F G
        case 'H': return 0b01110110; // B C E F G
        case 'L': return 0b00111000; // D E F
        case 'O': return 0b00111111; // A B C D E F
        case 'P': return 0b01110011; // A B E F G
        case 'S': return 0b01101101; // A C D F G
        case 'U': return 0b00111110; // B C D E F
        case 'K': return 0b01110110; // custom for K
        case 'W': return 0b00111110; // like U
        case '-': return 0b01000000; // G
        case ' ': return 0b00000000;
        default:  return 0b00000001; // Just A (shows top segment if unknown)
    }
}



void displayPasswordScreen()
{
    // Show digits at upper 4 (0-3)
    for (int i = 0; i < 4; i++)
        digits[i] = passwordDigits[i];

    // Show "PASS" at lower 4 (4-7)
    digits[4] = charTo7Seg('P');
    digits[5] = charTo7Seg('A');
    digits[6] = charTo7Seg('S');
    digits[7] = charTo7Seg('S');
}




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
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  status = HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	    digits[0] = charTo7Seg('H');
	    digits[1] = charTo7Seg('E');
	    digits[2] = charTo7Seg('L');
	    digits[3] = charTo7Seg('L');

	    digits[4] = charTo7Seg('O');
	    digits[5] = charTo7Seg('F');
	    digits[6] = charTo7Seg('F');
	    //digits[7] = charTo7Seg('K');

//	  GPIO_PinState b1 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12); // Button 1
//	  GPIO_PinState b2 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11); // Button 2
//
//	      if (!inPasswordMode)
//	      {
//	          // Entry Condition: both buttons pressed for 2s
//	          if (b1 == GPIO_PIN_SET && b2 == GPIO_PIN_SET)
//	          {
//	              if (waitingFor2Sec == 0)
//	              {
//	                  buttonPressStartTime = HAL_GetTick();
//	                  waitingFor2Sec = 1;
//	              }
//	              else if (HAL_GetTick() - buttonPressStartTime >= 2000)
//	              {
//	                  // ✅ Enter password mode
//	                  inPasswordMode = 1;
//	                  waitingFor2Sec = 0;
//	                  currentDigitIndex = 0;
//	                  displayPasswordScreen();
//	              }
//	          }
//	          else
//	          {
//	              waitingFor2Sec = 0;
//	          }
//	      }
//	      else
//	      {
//	          // Password Mode Active
//	          displayPasswordScreen();
//
//	          // Example debounce (basic)
//	          static uint32_t lastPress = 0;
//	          if (HAL_GetTick() - lastPress > 300)
//	          {
//	              if (b1 == GPIO_PIN_SET)
//	              {
//	                  // Increment current digit
//	                  passwordDigits[currentDigitIndex]++;
//	                  if (passwordDigits[currentDigitIndex] > 9)
//	                      passwordDigits[currentDigitIndex] = 0;
//	                  lastPress = HAL_GetTick();
//	              }
//	              if (b2 == GPIO_PIN_SET)
//	              {
//	                  // Move to next digit
//	                  currentDigitIndex++;
//	                  if (currentDigitIndex >= 4)
//	                  {
//	                      // ✅ You can now compare entered password here
//	                      // Reset if needed
//	                      currentDigitIndex = 0;
//	                  }
//	                  lastPress = HAL_GetTick();
//	              }
//	          }
//	      }
//	  if (HAL_GetTick() - lastUpdate >= 10)
//	  {
//		  lastUpdate = HAL_GetTick();
//
//		  number++;
//		  if (number > 99999999) number = 0;  // Wrap around after 8 digits
//
//		  updateDigits(number);  // Update display data
//	  }

//	  sum = 0;
//
//	  for (uint8_t i = 0; i < SAMPLES; i++)
//	  {
//		  adc_Value = ADC_Convert_Rank2();
//		  sum += adc_Value;
//	  }
//
//	  average = sum / SAMPLES;
//
//	  voltage = (average * 100) / 4095;
//
//	  HAL_Delay(10);

	  // === Case 1: Show raw 12-bit ADC value (0–4095) ===
	  // Comment this block when testing voltage

//	  sum_1 = 0;
//	  sum_2 = 0;
//
//	  for (uint8_t i = 0; i < SAMPLES; i++)
//	  {
//		  a = ADC_Convert_Rank1();
//		  b = ADC_Convert_Rank2();
//
//		  sum_1 += a;
//		  sum_2 += b;
//	  }
//
//	  average_1 = sum_1 / SAMPLES;
//
//	  average_2 = sum_2 / SAMPLES;
//
//	  deviation = ((average_1 - average_2));
//
//	  corrected_1 = deviation + 4;
//
////	  Apply EMA filtering
//	  filtered_adc_1 = ((filtered_adc_1 * ((1 << SMOOTHING_SHIFT) - 1)) + corrected_1) >> SMOOTHING_SHIFT;
//
//	  current_A = ((filtered_adc_1) * 1075) / (4095);  // Scale to mV
//
//	  // Show raw value on first 4 digits (pad with zeros)
//	  digits[2] = (current_A / 1000) % 10;
//	  digits[3] = (current_A / 100) % 10;
//	  digits[4] = (current_A / 10) % 10;
//	  digits[5] = current_A % 10;
//
//	  HAL_Delay(200);
//
//	  // Show raw value on first 4 digits (pad with zeros)
//	  digits[2] = (average_2 / 1000) % 10;
//	  digits[3] = (average_2 / 100) % 10;
//	  digits[4] = (average_2 / 10) % 10;
//	  digits[5] = average_2 % 10;
//
	  // Optional: blank last 2 digits
//	  digits[0] = digits[1] = 10;  // Assuming 10 means blank pattern
//
//	  HAL_Delay(10);


	  // === Case 2: Show voltage (e.g., 3.245 V = 3245 mV) ===
	  // Comment this block when testing raw ADC value

//		sum = 0;
//
//		for (uint8_t i = 0; i < SAMPLES; i++)
//		{
//			adc_Value = ADC_Convert_Rank3();
//			sum += adc_Value;
//		}
//
//		average = sum / SAMPLES;
//
//		corrected_2 = (average > 82) ? (average - 82) : 0;

	    // Apply EMA filtering
//	    filtered_adc_2 = ((filtered_adc_2 * ((1 << SMOOTHING_SHIFT) - 1)) + corrected_2) >> SMOOTHING_SHIFT;

//		voltage_V = ((corrected_2) * 1000) / (4095);  // Scale to mV

//
//		// Show millivolts as 3.245V → digits: [3][2][4][5]
//		digits[2] = (voltage_mV / 1000) % 10;  // 3
//		digits[3] = (voltage_mV / 100) % 10;   // 2
//		digits[4] = (voltage_mV / 10) % 10;    // 4
//		digits[5] = voltage_mV % 10;           // 5
//
////		 Optional: enable DP on digit 0 or 1
////		 In TIM callback, check seg==0 or seg==1 and enable DP accordingly
//
//		digits[0] = digits[1] = 10;  // Blank
//		HAL_Delay(10);

//		power = voltage_V * current_A;
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DiscontinuousConvMode = ENABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_39CYCLES_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_39CYCLES_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 32768;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, C_D3_Pin|C_D2_Pin|F_Pin|A_Pin
                          |C_D1_Pin|E_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, C_D4_Pin|B_Pin|V_D2_Pin|D_Pin
                          |DP_Pin|C_Pin|G_Pin|V_D4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(V_D1_GPIO_Port, V_D1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : C_D3_Pin C_D2_Pin F_Pin A_Pin
                           C_D1_Pin E_Pin */
  GPIO_InitStruct.Pin = C_D3_Pin|C_D2_Pin|F_Pin|A_Pin
                          |C_D1_Pin|E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : C_D4_Pin B_Pin V_D2_Pin D_Pin
                           DP_Pin C_Pin G_Pin V_D4_Pin */
  GPIO_InitStruct.Pin = C_D4_Pin|B_Pin|V_D2_Pin|D_Pin
                          |DP_Pin|C_Pin|G_Pin|V_D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : V_D1_Pin */
  GPIO_InitStruct.Pin = V_D1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(V_D1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Switch2_Pin Switch1_Pin */
  GPIO_InitStruct.Pin = Switch2_Pin|Switch1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
