/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32g0xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "stm32g0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
const uint8_t Segment_Patterns[38] = { 0b10000001,  // 0   [0]
		0b11001111,  // 1   [1]
		0b10010010,  // 2   [2]
		0b10000110,  // 3   [3]
		0b11001100,  // 4   [4]
		0b10100100,  // 5   [5]
		0b10100000,  // 6   [6]
		0b10001111,  // 7   [7]
		0b10000000,  // 8   [8]
		0b10000100,  // 9   [9]

		0b10001000,  // A   [10]
		0b11100000,  // B   [11] (b in lowercase form)
		0b10110001,  // C   [12]
		0b11000010,  // D   [13] (d in lowercase form)
		0b10110000,  // E   [14]
		0b10111000,  // F   [15]
		0b10100000,  // G   [16] (same as 6)
		0b11101000,  // H   [17]
		0b11110001,  // I   [18] (looks like a vertical bar)
		0b11000111,  // J   [19]
		0b10001001,  // K   [20] (approximated)
		0b11100001,  // L   [21]
		0b10001010,  // M   [22] (approximated)
		0b11101010,  // n   [23] (approximated)
		0b11100010,  // lower O   [24]
		0b10011000,  // P   [25]
		0b10001100,  // Q   [26] (approximated)
		0b11111010,  // R   [27] (approximated)
		0b10100100,  // S   [28]
		0b11110000,  // T   [29] (like E)
		0b11000001,  // U   [30]
		0b11000101,  // V   [31] (approximated)
		0b11010101,  // W   [32] (approximated)
		0b10011010,  // X   [33] (approximated)
		0b11000100,  // Y   [34] (like 4)
		0b10010010,  // Z   [35] (like 2)

		0b11111110,  // -   [36]
		0b11111111,  // space [37]
		0b11100010   // ?   [38]
		};

unsigned char seg = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;

/* USER CODE BEGIN EV */
extern uint16_t digits[];
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while (1) {
	}
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	HAL_IncTick();
  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

//	 __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);


	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET); // Digit 0
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); // Digit 1
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET); // Digit 2
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); // Digit 3
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // Digit 4

	//common anode, SET = off for digits

	// === Get pattern for current digit ===
	uint8_t val = digits[seg];  // Avoid invalid index
	uint8_t pattern = Segment_Patterns[val];

	// === Set segments A-G and DP ===
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,
			(pattern & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET); // A
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1,
			(pattern & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET); // B
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15,
			(pattern & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET); // C
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4,
			(pattern & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET); // D
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5,
			(pattern & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET); //E
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,
			(pattern & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET); // F
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12,
			(pattern & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // G

	if (seg < 4) {
		if (digits[4 + seg]) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
		} else {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
		}
	}

	if (seg == 4) {
	    // 1) Clear all 4 LEDs first (common anode → SET = OFF)
	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); // C
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);  // D
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);  // E
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);  // DP

	    // 2) Turn ON exactly one LED, based on digits[8]
	    switch (digits[8]) {
	    case 10: // first LED -> C
	        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	        break;
	    case 11: // second LED -> D
	        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	        break;
	    case 12: // third LED -> E
	        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	        break;
	    case 13: // fourth LED -> DP
	        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	        break;
	    default:
	        // 0 or anything else -> all OFF (already cleared above)
	        break;
	    }
	}

	switch (seg) {
	//This turns ON digits
	case 0:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		break;

	}
//
//	// === Advance to next digit ===
	seg = (seg + 1) % 5;
//	  HAL_Delay(10);

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles TIM14 global interrupt.
  */
void TIM14_IRQHandler(void)
{
  /* USER CODE BEGIN TIM14_IRQn 0 */

  /* USER CODE END TIM14_IRQn 0 */
  HAL_TIM_IRQHandler(&htim14);
  /* USER CODE BEGIN TIM14_IRQn 1 */

  /* USER CODE END TIM14_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
