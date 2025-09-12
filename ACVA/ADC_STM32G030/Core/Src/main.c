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
#define HYST_LSB           5UL         // zero-cross hysteresis (ADC codes)
#define ADC_HISTORY_LEN 512
#define ADC_MIDPOINT             1645U

uint16_t adc_vrefint = 0;   // latest ADC result for VREFINT
uint32_t vdda_mV = 0;       // calculated VDDA (mV)

float voltage = 0.0f;
float current = 0.0f;


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

//float value;
//static volatile float PF = 1.0f;
//static volatile float Phase_deg = 0.0f;
//static volatile int   LeadLag = 0;        // -1 lag, +1 lead, 0 unknown

static float Vrms_filtered = 0;
static float Irms_filtered = 0;


const float alpha_i     = 0.1f;  // slower, steadier EMA
float       threshold_i = 1.0f;

const float alpha_v     = 0.1f;  // slower, steadier EMA
float       threshold_v = 5.0f;

static volatile uint8_t rms_pair_ready = 0;  // set when a V+I block has been processed

float PF_Phase_deg = 0.0f;                   // optional displacement angle from PF




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
static int find_rising_zc(const uint16_t *x, uint16_t N, uint16_t offset,int hyst);
static float interp_tzc(uint16_t x0, uint16_t x1, uint16_t offset, float t0, float Ts);
static void Compute_PF_FFT(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//
//};

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
	//uint32_t now1 = HAL_GetTick();
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}

// Called when the first half of the DMA buffer is filled
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {

}

// Called when the entire DMA buffer is filled
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	process_block(&adc_buffer[0], ADC_DMA_COUNT);

	buffer_ready = 1;
}




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

		Vrms_total = Vrms_total * 1.03 ;

		if (fabsf(Vrms_total - Vrms_AC) >= threshold_v) {
		    Vrms_AC = Vrms_total;
		}

		Vrms_filtered = (alpha_v * Vrms_AC) + ((1.0f - alpha_v) * Vrms_filtered);

		buffer_ready = 0;
		buffer1_ready = 1;


	}

}

void Calculate_Irms() {


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

		Irms_total = Irms_total * 1.03 ;

		if (fabsf(Irms_total - Irms_AC) >= threshold_i) {
		    Irms_AC = Irms_total;
		}

		Irms_filtered = (alpha_i * Irms_AC) + ((1.0f - alpha_i) * Irms_filtered);

		buffer1_ready = 0;
		 rms_pair_ready = 1;
//			     break;
	}
//	}
}

//static int find_rising_zc(const uint16_t *x, uint16_t N, uint16_t offset,
//		int hyst) {
//	for (uint16_t n = 0; n < (N - 1U); n++) {
//		int16_t d0 = (int16_t) x[n] - (int16_t) offset;
//		int16_t d1 = (int16_t) x[n + 1] - (int16_t) offset;
//		if ((d0 < -hyst) && (d1 >= +hyst))
//			return (int) n;
//	}
//	return -1;
//}
//
///* Linear interpolation of zero-cross between samples n and n+1 */
//static float interp_tzc(uint16_t x0, uint16_t x1, uint16_t offset, float t0,
//		float Ts) {
//	float y0 = (float) ((int) x0 - (int) offset);
//	float y1 = (float) ((int) x1 - (int) offset);
//	if (y1 == y0)
//		return t0 + 0.5f * Ts;
//	float a = (-y0) / (y1 - y0);
//	if (a < 0.0f)
//		a = 0.0f;
//	if (a > 1.0f)
//		a = 1.0f;
//	return t0 + a * Ts;
//}

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

static void Compute_PF_FromBuffers(void)
{
    const uint16_t N = ADC_HISTORY_LEN;      // your 512
    if (N == 0) return;

    /* --- 1) Guard: ignore if signal too small (prevents jitter/noise PF) --- */
    /* tune these to your scale (mV). Example: require ~10 V and ~0.5 V equiv. */
    const float V_MIN_RMS = 10.0f;
    const float I_MIN_RMS = 0.5f;
    if (Vrms_total < V_MIN_RMS || Irms_total < I_MIN_RMS) {
        return; // keep last Power_Factor
    }

    /* --- 2) Covariance = mean( (v - Vavg)*(i - Iavg) ) --- */
    double acc = 0.0;
    for (uint16_t n = 0; n < N; n++) {
        float dv = (float)v_buf[n] - Vavg;   // your buffers are already in mV
        float di = (float)i_buf[n] - Iavg;
        acc += (double)dv * (double)di;
    }
    const double cov_vi = acc / (double)N;

    /* --- 3) Instantaneous PF (clamped) --- */
    const float denom = Vrms_total * Irms_total;
    if (denom <= 1e-6f) return;              // avoid div/0

    float pf_inst = (float)(cov_vi / (double)denom);
    if (!isfinite(pf_inst)) return;
    if (pf_inst >  1.0f) pf_inst =  1.0f;
    if (pf_inst < -1.0f) pf_inst = -1.0f;

    /* --- 4) Stability stack: median-of-3 → EMA → deadband --- */
    /* These statics are function-local (not globals) */
    static float q[3] = {1.0f,1.0f,1.0f};
    static uint8_t qi = 0, filled = 0;
    q[qi] = pf_inst; qi = (uint8_t)((qi + 1U) % 3U); if (filled < 3U) filled++;

    /* median of 3 samples (excellent spike killer, ~0 latency) */
    float pf_med = pf_inst;
    if (filled == 3U) {
        float a=q[0], b=q[1], c=q[2];
        pf_med = ((a<=b && b<=c) || (c<=b && b<=a)) ? b :
                 ((b<=a && a<=c) || (c<=a && a<=b)) ? a : c;
    }

    /* EMA smoothing (low latency, tunable) */
    static float pf_ema = 1.0f;     // start sane
    const  float alpha  = 0.15f;    // 0.10..0.25 typical; lower = smoother, higher = faster
    pf_ema = pf_ema + alpha * (pf_med - pf_ema);

    /* Deadband: ignore tiny changes (UI stability) */
    static float last_out = 1.0f;
    const  float eps = 0.005f;      // 0.5% PF step
    float out = pf_ema;
    if (fabsf(out - last_out) < eps) out = last_out;

    /* Final clamp + publish */
    if (out >  1.0f) out =  1.0f;
    if (out < -1.0f) out = -1.0f;
    last_out = out;
    Power_Factor = out;
}



//static void Compute_PF_FFT(void)
//{
//    const uint16_t N = ADC_HISTORY_LEN;
//    if (N < 8) return;  // sanity
//
//    /* --- compute fs from timer registers --- */
//    /* TIM3 clock on G0 typically equals APB1 (16 MHz here). If your board differs,
//       adjust tim_clk accordingly. */
//    const float tim_clk = 16000000.0f;
//    const uint32_t psc  = htim3.Instance->PSC;
//    const uint32_t arr  = htim3.Instance->ARR;
//    const float fs = tim_clk / ((float)(psc + 1U) * (float)(arr + 1U));
//    const float f0 = 50.0f;               // fundamental
//    const float omega = 2.0f * (float)M_PI * f0 / fs;
//
//    /* Recurrence for cos(n*omega), sin(n*omega) */
//    float c_step = cosf(omega);
//    float s_step = sinf(omega);
//    float c_n = 1.0f;   // cos(0)
//    float s_n = 0.0f;   // sin(0)
//
//    /* DFT accumulators for fundamental */
//    float ReV = 0.0f, ImV = 0.0f;
//    float ReI = 0.0f, ImI = 0.0f;
//
//    /* Hann window reduces spectral leakage if N != integer cycles */
//    const float invNm1 = 1.0f / (float)(N - 1U);
//    for (uint16_t n = 0; n < N; n++)
//    {
//        /* Hann window w[n] = 0.5 * (1 - cos(2π n/(N-1))) */
//        float w = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * (float)n * invNm1));
//
//        /* DC removal using your Vavg/Iavg (both in mV) */
//        float v = ((float)v_buf[n] - Vavg) * w;
//        float i = ((float)i_buf[n] - Iavg) * w;
//
//        /* DFT at +ω: X(ω) = Σ x[n] * (cos + j sin)  (we'll use atan2(Im, Re)) */
//        ReV += v * c_n;  ImV += v * s_n;
//        ReI += i * c_n;  ImI += i * s_n;
//
//        /* update cos/sin to next n via recurrence:
//           [c_{n+1}] = [ c_step -s_step ] [c_n]
//           [s_{n+1}]   [ s_step  c_step ] [s_n] */
//        float c_next = c_n * c_step - s_n * s_step;
//        float s_next = s_n * c_step + c_n * s_step;
//        c_n = c_next; s_n = s_next;
//    }
//
//    /* Phases of fundamental */
//    float phi_v = atan2f(ImV, ReV);
//    float phi_i = atan2f(ImI, ReI);
//
//    /* Phase difference v - i in [-π, π] */
//    float dphi = phi_v - phi_i;
//    while (dphi >  (float)M_PI)  dphi -= 2.0f * (float)M_PI;
//    while (dphi < -(float)M_PI)  dphi += 2.0f * (float)M_PI;
//
//    /* Displacement PF magnitude */
//    float pf = cosf(dphi);
//    if (pf < 0.0f) pf = -pf;
//
//    PF        = pf;
//    Phase_deg = dphi * (180.0f / (float)M_PI);
//
//   LeadLag   = (dphi > 0.0f) ? -1 : +1;  // +dphi: I lags (inductive) → -1
//}
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
		 if (rms_pair_ready) {

		     Compute_PF_FromBuffers();
			 rms_pair_ready = 0;


		    }
//		Compute_PF_FFT();

//		const float fs = 10000.0f;     // 10 kHz
//		const float Ts = 1.0f / fs;    // 100 us
//		const float f = 50.0f;        // mains
//		const float T = 1.0f / f;
//
//        int nv = find_rising_zc(v_buf, SAMPLES_PER_CYCLES, Vavg, HYST_LSB);
//        int ni = find_rising_zc(i_buf, SAMPLES_PER_CYCLES, Iavg, HYST_LSB);
//
//        if (nv >= 0 && ni >= 0)
//        {
//            float t_v = interp_tzc(v_buf[nv], v_buf[nv+1], Vavg, nv*Ts, Ts);
//            float t_i = interp_tzc(i_buf[ni], i_buf[ni+1], Iavg, ni*Ts, Ts);
//            float dt  = t_i - t_v;                 // +: I lags; -: I leads
//
//            /* Wrap dt into [-T/2, +T/2] */
//            while (dt >  0.5f*T) dt -= T;
//            while (dt < -0.5f*T) dt += T;
//
//            float phi = 2.0f * (float)M_PI * f * dt;   // radians
//
//            PF        = cosf(phi);
//            if (PF < 0.0f) PF = -PF;                   // displacement PF magnitude
//            Phase_deg = phi * (180.0f / (float)M_PI);
//            LeadLag   = (dt > 0.0f) ? -1 : +1;
//
//            /* debug */
//            //dbg_nv = nv; dbg_ni = ni; dbg_dt = dt; dbg_phi = phi;
//        }
//        else
//        {
//            LeadLag = 0; // keep previous PF/Phase_deg
//        }

		//	Power_Factor = Calculate_Phase_Difference();

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
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV8;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T3_TRGO;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
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
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1600;
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
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1600;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
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
static void MX_DMA_Init(void)
{

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
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
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
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
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
