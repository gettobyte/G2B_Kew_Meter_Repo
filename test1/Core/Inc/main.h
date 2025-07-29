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
#define C_D3_Pin GPIO_PIN_6
#define C_D3_GPIO_Port GPIOA
#define C_D2_Pin GPIO_PIN_7
#define C_D2_GPIO_Port GPIOA
#define C_D4_Pin GPIO_PIN_0
#define C_D4_GPIO_Port GPIOB
#define B_Pin GPIO_PIN_1
#define B_GPIO_Port GPIOB
#define V_D2_Pin GPIO_PIN_2
#define V_D2_GPIO_Port GPIOB
#define F_Pin GPIO_PIN_8
#define F_GPIO_Port GPIOA
#define A_Pin GPIO_PIN_9
#define A_GPIO_Port GPIOA
#define V_D1_Pin GPIO_PIN_6
#define V_D1_GPIO_Port GPIOC
#define C_D1_Pin GPIO_PIN_10
#define C_D1_GPIO_Port GPIOA
#define Switch2_Pin GPIO_PIN_11
#define Switch2_GPIO_Port GPIOA
#define Switch1_Pin GPIO_PIN_12
#define Switch1_GPIO_Port GPIOA
#define E_Pin GPIO_PIN_15
#define E_GPIO_Port GPIOA
#define D_Pin GPIO_PIN_3
#define D_GPIO_Port GPIOB
#define DP_Pin GPIO_PIN_4
#define DP_GPIO_Port GPIOB
#define C_Pin GPIO_PIN_5
#define C_GPIO_Port GPIOB
#define G_Pin GPIO_PIN_6
#define G_GPIO_Port GPIOB
#define V_D4_Pin GPIO_PIN_7
#define V_D4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
