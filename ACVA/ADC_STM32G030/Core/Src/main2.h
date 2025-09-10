KUNNU BHAI

/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body (STM32G030 PF with VREFINT scaling)
 ******************************************************************************
 * @attention
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 * This software is licensed under terms in the LICENSE file.
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SAMPLES_PER_CYCLE        512U
#define ADC_NUM_CHANNELS         2U
#define ADC_DMA_COUNT            (SAMPLES_PER_CYCLE * ADC_NUM_CHANNELS)   // 1024 halfwords

#define ADC_MIDPOINT             2050U     // mid-bias code (~2048)
#define HYST_LSB                 8         // zero-cross hysteresis (ADC codes)

#define VREFINT_CAL_ADDR         ((uint16_t*)0x1FFF75AA)  // factory calib at 3.0V
#define VREFINT_CAL_VREF         3000UL                   // mV

/* Set these to your analog front-end gains:
   Vrms[V] = Vrms_codes * (VDDA/4095) * K_V
   Irms[A] = Irms_codes * (VDDA/4095) * K_I                                      */
#define K_V                       1.000f   // TODO: set from divider ratio/op-amp gain
#define K_I                       1.000f   // TODO: set from CT/shunt+amp gain
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
/* Interleaved DMA buffer: V,I,V,I,... */
static uint16_t adc_buffer[ADC_DMA_COUNT];

/* Per-channel one-cycle buffers */
static uint16_t v_buf[SAMPLES_PER_CYCLE];
static uint16_t i_buf[SAMPLES_PER_CYCLE];
static volatile uint16_t buf_index = 0;
static volatile uint8_t  cycle_ready = 0;

/* Results (codes and scaled) */
static volatile float Vrms_codes_filt = 0.0f;
static volatile float Irms_codes_filt = 0.0f;
static volatile float PF = 1.0f;
static volatile float Phase_deg = 0.0f;
static volatile int   LeadLag = 0;        // -1 lag, +1 lead, 0 unknown

/* Scaled physical values (need K_V/K_I + VDDA) */
static volatile uint32_t vdda_mV = 3300;  // updated via VREFINT
static volatile float Vrms_volts = 0.0f;
static volatile float Irms_amps  = 0.0f;
static volatile float P_watts    = 0.0f;

/* Smoothing */
static const float alpha = 0.2f;   // EMA on RMS

/* Debug (watch in Live Expressions if desired) */
volatile int   dbg_nv = -1, dbg_ni = -1;
volatile float dbg_dt = 0.0f, dbg_phi = 0.0f;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
static void process_block(uint16_t *blk, size_t count_halfwords);
static float compute_rms_dc_removed(const uint16_t *x, uint16_t N, float *pMean);
static int   find_rising_zc(const uint16_t *x, uint16_t N, uint16_t offset, int hyst);
static float interp_tzc(uint16_t x0, uint16_t x1, uint16_t offset, float t0, float Ts);
static uint32_t ADC_Read_VREFINT_mV_Blocking(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* DMA half-complete */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    process_block(&adc_buffer[0], ADC_DMA_COUNT / 2U);
}

/* DMA complete */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    process_block(&adc_buffer[ADC_DMA_COUNT / 2U], ADC_DMA_COUNT / 2U);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

/* De-interleave V,I into per-channel ring buffers; flag when a 512-sample cycle is ready */
static void process_block(uint16_t *blk, size_t count_halfwords)
{
    for (size_t k = 0; k < count_halfwords; k += 2U)
    {
        v_buf[buf_index] = blk[k + 0];
        i_buf[buf_index] = blk[k + 1];

        buf_index++;
        if (buf_index >= SAMPLES_PER_CYCLE)
        {
            buf_index = 0;
            cycle_ready = 1;
        }
    }
}

/* RMS with DC removal (input in ADC codes). Writes mean if pMean!=NULL */
static float compute_rms_dc_removed(const uint16_t *x, uint16_t N, float *pMean)
{
    float sum = 0.0f;
    for (uint16_t n = 0; n < N; n++) sum += (float)x[n];
    float mean = sum / (float)N;

    float sumsq = 0.0f;
    for (uint16_t n = 0; n < N; n++)
    {
        float d = (float)x[n] - mean;
        sumsq += d * d;
    }
    if (pMean) *pMean = mean;
    return sqrtf(sumsq / (float)N);
}

/* Find first rising zero-cross using hysteresis around offset */
static int find_rising_zc(const uint16_t *x, uint16_t N, uint16_t offset, int hyst)
{
    for (uint16_t n = 0; n < (N - 1U); n++)
    {
        int16_t d0 = (int16_t)x[n]   - (int16_t)offset;
        int16_t d1 = (int16_t)x[n+1] - (int16_t)offset;
        if ((d0 < -hyst) && (d1 >= +hyst))
            return (int)n;
    }
    return -1;
}

/* Linear interpolation of zero-cross between samples n and n+1 */
static float interp_tzc(uint16_t x0, uint16_t x1, uint16_t offset, float t0, float Ts)
{
    float y0 = (float)((int)x0 - (int)offset);
    float y1 = (float)((int)x1 - (int)offset);
    if (y1 == y0) return t0 + 0.5f * Ts;
    float a = (-y0) / (y1 - y0);
    if (a < 0.0f) a = 0.0f;
    if (a > 1.0f) a = 1.0f;
    return t0 + a * Ts;
}

/* Pause fast stream, read VREFINT once (oversampled), resume fast stream. */
static uint32_t ADC_Read_VREFINT_mV_Blocking(void)
{
    ADC_ChannelConfTypeDef s = {0};

    /* Stop fast DMA stream */
    HAL_ADC_Stop_DMA(&hadc1);

    /* Reconfigure ADC for single SW-triggered conversion on VREFINT */
    hadc1.Init.ScanConvMode              = ADC_SCAN_DISABLE;
    hadc1.Init.NbrOfConversion           = 1;
    hadc1.Init.ContinuousConvMode        = DISABLE;
    hadc1.Init.ExternalTrigConv          = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge      = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DMAContinuousRequests     = DISABLE;
    hadc1.Init.OversamplingMode          = ENABLE;
    hadc1.Init.Oversampling.Ratio        = ADC_OVERSAMPLING_RATIO_16;
    hadc1.Init.Oversampling.RightBitShift= ADC_RIGHTBITSHIFT_4;  // back to 12-bit scale
    hadc1.Init.SamplingTimeCommon1       = ADC_SAMPLETIME_160CYCLES_5; // slow & stable
    HAL_ADC_Init(&hadc1);

    /* Select VREFINT */
    s.Channel      = ADC_CHANNEL_VREFINT;
    s.Rank         = ADC_REGULAR_RANK_1;
    s.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
    HAL_ADC_ConfigChannel(&hadc1, &s);

    /* Do the conversion */
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint16_t vref_raw = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    /* Convert to VDDA (mV): VDDA = 3.0V * VREFINT_CAL / vref_raw */
    uint16_t vref_cal = *VREFINT_CAL_ADDR;
    uint32_t vdda = (uint32_t)VREFINT_CAL_VREF * vref_cal / vref_raw;

    /* Restore fast 2-rank stream (same as MX_ADC1_Init) */
    MX_ADC1_Init();
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_DMA_COUNT);

    return vdda;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();

  /* Calibrate ADC before start */
  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
      Error_Handler();
  }

  /* Start timer (TRGO source) and ADC+DMA stream */
  HAL_TIM_Base_Start(&htim3);
  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_DMA_COUNT) != HAL_OK) {
      Error_Handler();
  }

  uint32_t last_vref_ms = 0;

  /* Main loop */
  while (1)
  {
      /* Periodically refresh VDDA via VREFINT (every ~300 ms) */
      uint32_t now = HAL_GetTick();
      if ((now - last_vref_ms) > 300U)
      {
          vdda_mV = ADC_Read_VREFINT_mV_Blocking();
          last_vref_ms = now;
      }

      /* When a full cycle is ready, compute RMS, phase, PF, and scale */
      if (cycle_ready)
      {
          cycle_ready = 0;

          const float fs = 25600.0f;     // 25.6 kHz
          const float Ts = 1.0f / fs;    // 39.0625 us
          const float f  = 50.0f;        // mains
          const float T  = 1.0f / f;

          /* RMS in ADC codes (DC removed) */
          float v_mean_codes = 0.0f, i_mean_codes = 0.0f;
          float Vrms_codes = compute_rms_dc_removed(v_buf, SAMPLES_PER_CYCLE, &v_mean_codes);
          float Irms_codes = compute_rms_dc_removed(i_buf, SAMPLES_PER_CYCLE, &i_mean_codes);

          /* EMA smoothing (optional) */
          Vrms_codes_filt = alpha * Vrms_codes + (1.0f - alpha) * Vrms_codes_filt;
          Irms_codes_filt = alpha * Irms_codes + (1.0f - alpha) * Irms_codes_filt;

          /* Zero-cross detection with interpolation */
          int nv = find_rising_zc(v_buf, SAMPLES_PER_CYCLE, ADC_MIDPOINT, HYST_LSB);
          int ni = find_rising_zc(i_buf, SAMPLES_PER_CYCLE, ADC_MIDPOINT, HYST_LSB);

          if (nv >= 0 && ni >= 0)
          {
              float t_v = interp_tzc(v_buf[nv], v_buf[nv+1], ADC_MIDPOINT, nv*Ts, Ts);
              float t_i = interp_tzc(i_buf[ni], i_buf[ni+1], ADC_MIDPOINT, ni*Ts, Ts);
              float dt  = t_i - t_v;                 // +: I lags; -: I leads

              /* Wrap dt into [-T/2, +T/2] */
              while (dt >  0.5f*T) dt -= T;
              while (dt < -0.5f*T) dt += T;

              float phi = 2.0f * (float)M_PI * f * dt;   // radians

              PF        = cosf(phi);
              if (PF < 0.0f) PF = -PF;                   // displacement PF magnitude
              Phase_deg = phi * (180.0f / (float)M_PI);
              LeadLag   = (dt > 0.0f) ? -1 : +1;

              /* debug */
              dbg_nv = nv; dbg_ni = ni; dbg_dt = dt; dbg_phi = phi;
          }
          else
          {
              LeadLag = 0; // keep previous PF/Phase_deg
          }

          /* Scale RMS codes to physical units using latest VDDA and gains */
          float lsb_V = (float)vdda_mV / 4095.0f;     // Volts at ADC input per code
          Vrms_volts  = Vrms_codes_filt * lsb_V * K_V;
          Irms_amps   = Irms_codes_filt * lsb_V * K_I;

          /* Simple active power estimate from displacement PF (no harmonics) */
          P_watts     = Vrms_volts * Irms_amps * PF;

          /* You can print/log here if needed */
          // printf("V=%.2f V, I=%.3f A, PF=%.3f, Phase=%.1f deg, %s\n",
          //        Vrms_volts, Irms_amps, PF, Phase_deg, (LeadLag<0)?"LAG":"LEAD");
      }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @retval None
  */
static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  /* ADC clock: make sure within spec; switch to DIV2 if needed on your board */
  hadc1.Init.ClockPrescaler         = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution             = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign              = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode           = ADC_SCAN_ENABLE;                  // scan 2 ranks
  hadc1.Init.EOCSelection           = ADC_EOC_SINGLE_CONV;              // 1 DMA req per conversion
  hadc1.Init.LowPowerAutoWait       = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff   = DISABLE;
  hadc1.Init.ContinuousConvMode     = DISABLE;                          // one scan per TRGO
  hadc1.Init.NbrOfConversion        = 2;                                // V & I
  hadc1.Init.DiscontinuousConvMode  = DISABLE;
  hadc1.Init.ExternalTrigConv       = ADC_EXTERNALTRIG_T3_TRGO;
  hadc1.Init.ExternalTrigConvEdge   = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.DMAContinuousRequests  = ENABLE;
  hadc1.Init.Overrun                = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.SamplingTimeCommon1    = ADC_SAMPLETIME_19CYCLES_5;        // fast enough
  hadc1.Init.SamplingTimeCommon2    = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.OversamplingMode       = DISABLE;                          // keep fast
  hadc1.Init.TriggerFrequencyMode   = ADC_TRIGGER_FREQ_HIGH;

  if (HAL_ADC_Init(&hadc1) != HAL_OK) { Error_Handler(); }

  /* Rank 1: Voltage (CH1) */
  sConfig.Channel      = ADC_CHANNEL_1;
  sConfig.Rank         = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) { Error_Handler(); }

  /* Rank 2: Current (CH2) */
  sConfig.Channel      = ADC_CHANNEL_2;
  sConfig.Rank         = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief TIM3 Initialization Function
  * @retval None
  */
static void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance               = TIM3;
  htim3.Init.Prescaler         = 0;                     // 16 MHz / (PSC+1) = 16 MHz
  htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim3.Init.Period            = 624;                  // 16e6/(624+1)=25.6 kHz TRGO
  htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK) { Error_Handler(); }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) { Error_Handler(); }

  /* TRGO on update -> drives ADC external trigger */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) { Error_Handler(); }
}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/**
  * @brief GPIO Initialization Function
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin   = LED_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1) { }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file; (void)line;
}
#endif
