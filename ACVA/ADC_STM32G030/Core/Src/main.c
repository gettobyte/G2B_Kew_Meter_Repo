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
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

#define VREFINT_CAL_ADDR   ((uint16_t*)0x1FFF75AA)
#define VREFINT_CAL_VREF   3000UL   // mV

#define ADC_MIDPOINT       2050  // ~2048

uint16_t adc_vrefint = 0;   // latest ADC result for VREFINT
uint32_t vdda_mV = 0;       // calculated VDDA (mV)

float voltage = 0.0f;
float current = 0.0f;

#define ADC_HISTORY_LEN 512

uint16_t v_buf[ADC_HISTORY_LEN];  // stores last 100 samples
uint16_t i_buf[ADC_HISTORY_LEN];
uint16_t buf_index = 0;                 // buffer index

float frequency = 0;
float Vrms_total = 0.0;
float Vrms_AC = 0.0f;
float Irms_total = 0.0;
float Irms_AC = 0.0f;
float Vavg = 0.0;
float Iavg = 0.0;

float Power_Factor = 0.0;

#define SAMPLES_PER_CYCLES 512
#define ADC_NUM_Channel 2
#define ADC_DMA_COUNT (SAMPLES_PER_CYCLES*ADC_NUM_Channel)

static uint16_t adc_buffer[ADC_DMA_COUNT];  // interleaved: V,I,V,I,...
uint32_t AD_RES_BUFFER[2];

uint32_t AC_index = 0;
uint16_t buffer_ready = 0;
uint16_t buffer1_ready = 0;

float value;

static float Vrms_filtered = 0;
static float Vrms_Constant = 0;

static float Irms_filtered = 0;
const float alpha = 0.5;  // 0 < alpha < 1, lower = smoother
float threshold = 0.2f;

volatile uint8_t adc_half_ready = 0;
volatile uint8_t adc_full_ready = 0;

uint32_t last_vref_ms = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
static void process_block(uint16_t *blk, size_t count_halfwords);
static uint32_t ADC_Read_VREFINT(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Called when the first half of the DMA buffer is filled
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {

}

// Called when the entire DMA buffer is filled
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	process_block(&adc_buffer[0], ADC_DMA_COUNT);

	buffer_ready = 1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

}
;

static void process_block(uint16_t *blk, size_t count_halfwords) {
	for (size_t k = 0; k < count_halfwords; k += 2U) {
		v_buf[buf_index] = (blk[k + 0] * vdda_mV) / 4095;
		i_buf[buf_index] = (blk[k + 1] * vdda_mV) / 4095;

		buf_index++;
		if (buf_index >= ADC_HISTORY_LEN) {
			buf_index = 0;

		}
	}
}

void Calculate_Vrms() {

//		    voltage = (AD_RES_BUFFER[0] * vdda_mV) / 65535;
//	        adc_history[adc_index] = voltage;
//			adc_index++;
//			if (adc_index >= ADC_HISTORY_LEN) {
//				adc_index = 0;  // wrap around
//				buffer_ready = 1;
//			}

	if (buffer_ready) {

		float sum = 0.0f;
		float sum_sq = 0.0f;

		// Step 1: Compute mean (DC offset) in volts
		for (int i = 0; i < ADC_HISTORY_LEN; i++) {

			sum += v_buf[i];
		}

		Vavg = sum / ADC_HISTORY_LEN;     // DC offset (avg)

		for (int i = 0; i < ADC_HISTORY_LEN; i++) {
//					 TrueAC_Voltage[i] = adc_history[i] - Vavg;   // store AC sample
			sum_sq += (v_buf[i] - Vavg) * (v_buf[i] - Vavg); // accumulate squared values
		}

		float mean_sq = sum_sq / ADC_HISTORY_LEN; // <v^2>

		// Step 2: Compute RMS values
		Vrms_total = sqrtf(mean_sq);

//				 Vrms_Constant = (Vrms_total) * 0.0013862;

		if (fabsf(Vrms_total - Vrms_AC) >= threshold) {
			Vrms_AC = Vrms_total;
		}

		Vrms_filtered = (alpha * Vrms_AC) + ((1 - alpha) * Vrms_filtered); // total RMS (with DC)
		// float Vrms_ac    = sqrtf(fmaxf(0.0f, mean_sq - Vavg*Vavg)); // AC-only RMS
		buffer_ready = 0;
		buffer1_ready = 1;

	}

}

void Calculate_Irms() {
//	while(1){
//		current = (AD_RES_BUFFER[1] * vdda_mV) / 65535;
//	        adc_history1[adc_index1] = current;
//			adc_index1++;
//
//			if (adc_index1 >= ADC_HISTORY_LEN1) {
//				adc_index1 = 0;  // wrap around
//				buffer1_ready = 1;
//			}

	if (buffer1_ready) {

		float sum = 0.0f;
		float sum_sq = 0.0f;

		// Step 1: Compute mean (DC offset) in volts
		for (int i = 0; i < ADC_HISTORY_LEN; i++) {

			sum += i_buf[i];
		}

		Iavg = sum / ADC_HISTORY_LEN;     // DC offset (avg)

		for (int i = 0; i < ADC_HISTORY_LEN; i++) {
//					 TrueAC_Current[i] = adc_history1[i] - Iavg;
			sum_sq += (i_buf[i] - Iavg) * (i_buf[i] - Iavg); // accumulate squared values
		}

		float mean_sq = sum_sq / ADC_HISTORY_LEN; // <v^2>

		// Step 2: Compute RMS values
		Irms_total = sqrtf(mean_sq);

//				  float Irms_Constant = (Irms_filtered) * 0.291;

		if (fabsf(Irms_total - Irms_AC) >= threshold) {
			Irms_AC = Irms_total;
		}

		Irms_filtered = (alpha * Irms_AC) + ((1 - alpha) * Irms_filtered); // total RMS (with DC)
		// float Vrms_ac    = sqrtf(fmaxf(0.0f, mean_sq - Vavg*Vavg)); // AC-only RMS
		buffer1_ready = 0;
//			     break;
	}
//	}
}

float Calculate_Phase_Difference(void) {
	float sumV = 0, sumI = 0;
	float sumV2 = 0, sumI2 = 0, sumVI = 0;

	for (int n = 0; n < ADC_HISTORY_LEN; n++) {
		sumV += v_buf[n];
		sumI += i_buf[n];
	}
	float offsetV = sumV / ADC_HISTORY_LEN;
	float offsetI = sumI / ADC_HISTORY_LEN;
//
//	    for (int n = 0; n < ADC_HISTORY_LEN; n++) {
//	        float v = adc_history[n] - offsetV;
//	        float i = adc_history1[n] - offsetI;
//
//	        sumV2 += v * v;
//	        sumI2 += i * i;
//	        sumVI += v * i;
//	    }

//	    float Vrms = sqrtf(sumV2 / ADC_HISTORY_LEN);
//	    float Irms = sqrtf(sumI2 / ADC_HISTORY_LEN);

	float P = Irms_filtered * Vrms_filtered;

//	    float PF = P / (Vrms * Irms);

	return P;
}

static uint32_t ADC_Read_VREFINT(void) {
	ADC_ChannelConfTypeDef s = { 0 };

	/* Stop fast DMA stream */
	HAL_ADC_Stop_DMA(&hadc1);

	/* Reconfigure ADC for single SW-triggered conversion on VREFINT */
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.OversamplingMode = ENABLE;
	hadc1.Init.Oversampling.Ratio = ADC_OVERSAMPLING_RATIO_16;
	hadc1.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_4; // back to 12-bit scale
	hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5; // slow & stable
	HAL_ADC_Init(&hadc1);

	/* Select VREFINT */
	s.Channel = ADC_CHANNEL_VREFINT;
	s.Rank = ADC_REGULAR_RANK_1;
	s.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	HAL_ADC_ConfigChannel(&hadc1, &s);

	/* Do the conversion */
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	uint16_t vref_raw = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);

	/* Convert to VDDA (mV): VDDA = 3.0V * VREFINT_CAL / vref_raw */
	uint16_t vref_cal = *VREFINT_CAL_ADDR;
	uint32_t vdda = (uint32_t) VREFINT_CAL_VREF * vref_cal / vref_raw;

	/* Restore fast 2-rank stream (same as MX_ADC1_Init) */
	MX_ADC1_Init();
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_buffer, ADC_DMA_COUNT);

	return vdda;
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */
	if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
		Error_Handler();
	}

	HAL_TIM_Base_Start(&htim3);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_buffer, ADC_DMA_COUNT);
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		uint32_t now = HAL_GetTick();
		if ((now - last_vref_ms) > 300U) {
			vdda_mV = ADC_Read_VREFINT();
			last_vref_ms = now;
		}

//	    adc_vrefint = AD_RES_BUFFER[2];   // internal bandgap reference
//		adc_vrefint = (adc_vrefint * 4095UL) / 65535UL; // for 16 bit resolution
//		uint16_t vrefint_cal = *VREFINT_CAL_ADDR;
//
//		vdda_mV = (uint32_t) VREFINT_CAL_VREF * vrefint_cal / adc_vrefint;

		Calculate_Vrms();

		Calculate_Irms();

		//	Power_Factor = Calculate_Phase_Difference();

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

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
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV8;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

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
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T3_TRGO;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_19CYCLES_5;
	hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
	hadc1.Init.OversamplingMode = DISABLE;
	hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
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
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 1600;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1600;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_Handler();
	}
	__HAL_TIM_DISABLE_OCxPRELOAD(&htim3, TIM_CHANNEL_1);
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */
	HAL_TIM_MspPostInit(&htim3);

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin : LED_Pin */
	GPIO_InitStruct.Pin = LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
