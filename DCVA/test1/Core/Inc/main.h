/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define C_D4_Pin GPIO_PIN_9
#define C_D4_GPIO_Port GPIOB
#define Voltage_Pin GPIO_PIN_1
#define Voltage_GPIO_Port GPIOA
#define Current_Pin GPIO_PIN_2
#define Current_GPIO_Port GPIOA
#define C_D3_Pin GPIO_PIN_5
#define C_D3_GPIO_Port GPIOA
#define CV_ref_Pin GPIO_PIN_6
#define CV_ref_GPIO_Port GPIOA
#define SW_1_Pin GPIO_PIN_7
#define SW_1_GPIO_Port GPIOA
#define SW_2_Pin GPIO_PIN_0
#define SW_2_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_1
#define LED_GPIO_Port GPIOB
#define B_Pin GPIO_PIN_2
#define B_GPIO_Port GPIOB
#define V_D3_Pin GPIO_PIN_8
#define V_D3_GPIO_Port GPIOA
#define V_D2_Pin GPIO_PIN_9
#define V_D2_GPIO_Port GPIOA
#define F_Pin GPIO_PIN_6
#define F_GPIO_Port GPIOC
#define A_Pin GPIO_PIN_10
#define A_GPIO_Port GPIOA
#define V_D1_Pin GPIO_PIN_11
#define V_D1_GPIO_Port GPIOA
#define V_D4_Pin GPIO_PIN_12
#define V_D4_GPIO_Port GPIOA
#define C_D2_Pin GPIO_PIN_15
#define C_D2_GPIO_Port GPIOA
#define C_D1_Pin GPIO_PIN_3
#define C_D1_GPIO_Port GPIOB
#define E_Pin GPIO_PIN_4
#define E_GPIO_Port GPIOB
#define D_Pin GPIO_PIN_5
#define D_GPIO_Port GPIOB
#define DP_Pin GPIO_PIN_6
#define DP_GPIO_Port GPIOB
#define C_Pin GPIO_PIN_7
#define C_GPIO_Port GPIOB
#define G_Pin GPIO_PIN_8
#define G_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
