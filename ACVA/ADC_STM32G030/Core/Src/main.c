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

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

uint16_t digits[9];

#define ADC_HISTORY_LEN 512

#define BUTTON2_Pin GPIO_PIN_6
#define BUTTON2_GPIO_Port GPIOA

#define BUTTON1_Pin GPIO_PIN_7
#define BUTTON1_GPIO_Port GPIOA


#define SAMPLES_PER_CYCLE        512U
#define ADC_NUM_CHANNELS         3U
#define ADC_DMA_COUNT            (SAMPLES_PER_CYCLE * ADC_NUM_CHANNELS)   // 1024 halfwords

uint8_t flag = 0;
static uint16_t adc_buffer[ADC_DMA_COUNT];
//static uint16_t adc_buffer[3];
volatile uint8_t buffer_ready = 0;

uint16_t adc_vrefint = 0;   // latest ADC result for VREFINT
uint16_t vdda_mV = 0;       // calculated VDDA (mV)

//uint16_t AD_RES_BUFFER[ADC_DMA_COUNT];

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

uint32_t buttonPressStartTime = 0;
uint8_t waitingFor2Sec = 0;
uint8_t inPasswordMode = 0;

uint8_t correctPassword[4] = { 1, 0, 0, 0 };
uint8_t passwordEntered[4] = { 0 };
uint8_t settingDigits[4] = { 0 };
uint8_t mode = 0;
uint8_t entryComplete = 0;
uint8_t passwordMatched = 0;
uint32_t lastPress = 0, blinkTimer = 0;
uint8_t blinkState = 0;
uint8_t modeEntryActive = 0;

int programming = 0;

uint8_t readOnlyMode = 0;

uint8_t editMode = 0;                 // Are we editing digits?

uint8_t currentDigitIndex = 0;

uint8_t passwordDigits[4] = { 0, 0, 0, 0 };

// Menu states
uint8_t connState = 0; // 0: show "Conn", 1: show current type, 2: edit type (blink)
uint8_t connType = 0;    // 0: delt, 1: iph, 2: star

// Save state
uint8_t saveEdit = 0;   // 0: just showing SAVE, 1: in Y/N selection
uint8_t saveChoice = 0;   // 0: Y, 1: N

GPIO_PinState b1, b2;

uint8_t dp_index;

//static uint16_t adc_buffer[ADC_DMA_COUNT];


/* Debug (watch in Live Expressions if desired) */
volatile int dbg_nv = -1, dbg_ni = -1;
volatile float dbg_dt = 0.0f, dbg_phi = 0.0f;

float Power_Factor = 0.0;

static float Vrms_filtered = 0;
static float Irms_filtered = 0;

const float alpha_i = 0.1f;  // slower, steadier EMA
float threshold_i = 0.5f;

const float alpha_v = 0.1f;  // slower, steadier EMA
float threshold_v = 0.5f;

float ct_pr_value = 1.0f;
float ct_se_value = 1.0f;

uint8_t ct_pr_digits[4] = { 1, 0, 0, 0 };  // 1.000
uint8_t ct_se_digits[4] = { 1, 0, 0, 0 };  // 1.000
uint8_t ct_pr_decimal_pos = 0;
uint8_t ct_se_decimal_pos = 0;

uint8_t ct_pr_editing_digit = 0;  // Which digit is being edited (0-3)
uint8_t ct_se_editing_digit = 0;  // Which digit is being edited (0-3);

uint8_t ct_pr_state = 0;

uint32_t ct_pr_blinkTimer = 0;
uint8_t  ct_pr_blinkState = 0;

uint8_t  ct_se_state = 0;
uint32_t ct_se_blinkTimer = 0;
uint8_t  ct_se_blinkState = 0;


float PF_Phase_deg = 0.0f;                // optional displacement angle from PF

GPIO_PinState b1, b2;

HAL_StatusTypeDef status;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
static void process_block(uint16_t *blk, size_t count_halfwords);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Called when the first half of the DMA buffer is filled
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {

}

// Called when the entire DMA buffer is filled
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	process_block(&adc_buffer, ADC_DMA_COUNT);
	buffer_ready = 1;

}

//static void process_block(uint16_t *blk, size_t count_halfwords) {
//
//	adc_vrefint = AD_RES_BUFFER[2]; //fixed bandgap reference inside the chip
//
//	uint16_t vrefint_cal = *VREFINT_CAL_ADDR;
//
//	vdda_mV = (uint32_t) VREFINT_CAL_VREF * vrefint_cal / adc_vrefint;
//
//	for (size_t k = 0; k < count_halfwords; k += 3U) {
//		v_buf[buf_index] = (blk[k + 0] * vdda_mV) / 4095;
//		//i_buf[buf_index] = (blk[k + 1] * vdda_mV) / 4095;
//
//		buf_index++;
//		if (buf_index >= ADC_HISTORY_LEN) {
//			buf_index = 0;
//
//		}
//	}
//}

static void process_block(uint16_t *blk, size_t count_halfwords) {
	uint32_t vref_acc = 0;
	uint16_t vref_samples = 0;

	// stride of 3: [V, I, VREFINT]
	for (size_t k = 0; k < count_halfwords; k += 3U) {

		uint16_t raw_v = blk[k + 0];
		uint16_t raw_i = blk[k + 1];
		uint16_t raw_vref = blk[k + 2];

		// accumulate VREFINT samples for this block
		vref_acc += raw_vref;
		vref_samples++;

		// use *previous* vdda_mV to scale V and I into mV for RMS math
		v_buf[buf_index] = (raw_v * vdda_mV) / 4095U;
		i_buf[buf_index] = (raw_i * vdda_mV) / 4095U;

		buf_index++;
		if (buf_index >= ADC_HISTORY_LEN) {
			buf_index = 0;
		}
	}

	// update VDDA estimate from average VREFINT of this block
	if (vref_samples > 0) {
		uint16_t vref_raw = (uint16_t) (vref_acc / vref_samples);
		uint16_t vref_cal = *VREFINT_CAL_ADDR;

		// VDDA = VREFINT_CAL_VREF * VREFINT_CAL / vref_raw
		vdda_mV = (uint32_t) VREFINT_CAL_VREF * (uint32_t) vref_cal
				/ (uint32_t) vref_raw;
	}

	// signal to main() that a fresh block is ready for RMS

}

static float CT_Pr_DigitsToValue(void)
{
    uint16_t raw =
        (uint16_t)(ct_pr_digits[0] * 1000U +
                   ct_pr_digits[1] * 100U  +
                   ct_pr_digits[2] * 10U   +
                   ct_pr_digits[3]);

    // decimal after digit index i => divide by 10^(3 - i)
    uint8_t i = ct_pr_decimal_pos;
    if (i > 3) i = 3;

    uint8_t pow10 = 3 - i;
    float scale = 1.0f;
    while (pow10--) {
        scale *= 10.0f;
    }

    return (float)raw / scale;
}

static float CT_Se_DigitsToValue(void)
{
    uint16_t raw =
        (uint16_t)(ct_se_digits[0] * 1000U +
                   ct_se_digits[1] * 100U  +
                   ct_se_digits[2] * 10U   +
                   ct_se_digits[3]);

    // decimal after digit index i => divide by 10^(3 - i)
    uint8_t i = ct_se_decimal_pos;
    if (i > 3) i = 3;

    uint8_t pow10 = 3 - i;
    float scale = 1.0f;
    while (pow10--) {
        scale *= 10.0f;
    }

    return (float)raw / scale;
}




void Calculate_Vrms() {

	float sum = 0.0f;
	float sum_sq = 0.0f;

	// Step 1: Compute mean (DC offset) in volts
	for (int i = 0; i < ADC_HISTORY_LEN; i++) {

		sum += v_buf[i];
	}

	Vavg = sum / ADC_HISTORY_LEN;     // DC offset (avg)

	for (int i = 0; i < ADC_HISTORY_LEN; i++) {

		sum_sq += (v_buf[i] - Vavg) * (v_buf[i] - Vavg); // accumulate squared values
	}

	float mean_sq = sum_sq / ADC_HISTORY_LEN; // <v^2>

	// Step 2: Compute RMS values

	Vrms_total = sqrtf(mean_sq);

	const float K_VRMS = 0.2512f;
	Vrms_total *= K_VRMS;

//		Vrms_total = Vrms_total * 1.03;

	if (fabsf(Vrms_total - Vrms_AC) >= threshold_v) {
		Vrms_AC = Vrms_total;
	}

	Vrms_filtered = (alpha_v * Vrms_AC) + ((1.0f - alpha_v) * Vrms_filtered);

}

void Calculate_Irms() {

	float sum = 0.0f;
	float sum_sq = 0.0f;

	// Step 1: Compute mean (DC offset) in volts
	for (int i = 0; i < ADC_HISTORY_LEN; i++) {

		sum += i_buf[i];
	}

	Iavg = sum / ADC_HISTORY_LEN;     // DC offset (avg)

	for (int i = 0; i < ADC_HISTORY_LEN; i++) {

		sum_sq += (i_buf[i] - Iavg) * (i_buf[i] - Iavg); // accumulate squared values
	}

	float mean_sq = sum_sq / ADC_HISTORY_LEN; // <v^2>

	// Step 2: Compute RMS values

	Irms_total = sqrtf(mean_sq);

	const float K_IRMS = 4.56f;   // or 4.6f as a nice round value
	Irms_total *= K_IRMS;

	if (fabsf(Irms_total - Irms_AC) >= threshold_i) {
		Irms_AC = Irms_total;
	}

	Irms_filtered = (alpha_i * Irms_AC) + ((1.0f - alpha_i) * Irms_filtered);

}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
static void Compute_PF_FromBuffers(void) {
	const uint16_t N = ADC_HISTORY_LEN;      // your 512
	if (N == 0)
		return;

	/* --- 1) Guard: ignore if signal too small (prevents jitter/noise PF) --- */
	/* tune these to your scale (mV). Example: require ~10 V and ~0.5 V equiv. */
	const float V_MIN_RMS = 10.0f;
	const float I_MIN_RMS = 0.5f;
	if (Vrms_total < V_MIN_RMS || Irms_total < I_MIN_RMS) {
		return; // keep last Power_Factor
	}

	/* --- 2) Covariance = mean( (v - Vavg)*(i - Iavg) ) --- */
	float acc = 0.0;
	for (uint16_t n = 0; n < N; n++) {
		float dv = (float) v_buf[n] - Vavg;   // your buffers are already in mV
		float di = (float) i_buf[n] - Iavg;
		acc += (float) dv * (float) di;
	}
	const float cov_vi = acc / (float) N;

	/* --- 3) Instantaneous PF (clamped) --- */
	const float denom = Vrms_total * Irms_total;
	if (denom <= 1e-6f)
		return;              // avoid div/0

	float pf_inst = (float) (cov_vi / (float) denom);
	if (!isfinite(pf_inst))
		return;
	if (pf_inst > 1.0f)
		pf_inst = 1.0f;
	if (pf_inst < -1.0f)
		pf_inst = -1.0f;

	/* --- 4) Stability stack: median-of-3 → EMA → deadband --- */
	/* These statics are function-local (not globals) */
	static float q[3] = { 1.0f, 1.0f, 1.0f };
	static uint8_t qi = 0, filled = 0;
	q[qi] = pf_inst;
	qi = (uint8_t) ((qi + 1U) % 3U);
	if (filled < 3U)
		filled++;

	/* median of 3 samples (excellent spike killer, ~0 latency) */
	float pf_med = pf_inst;
	if (filled == 3U) {
		float a = q[0], b = q[1], c = q[2];
		pf_med = ((a <= b && b <= c) || (c <= b && b <= a)) ? b :
					((b <= a && a <= c) || (c <= a && a <= b)) ? a : c;
	}

	/* EMA smoothing (low latency, tunable) */
	static float pf_ema = 1.0f;     // start sane
	const float alpha = 0.15f; // 0.10..0.25 typical; lower = smoother, higher = faster
	pf_ema = pf_ema + alpha * (pf_med - pf_ema);

	/* Deadband: ignore tiny changes (UI stability) */
	static float last_out = 1.0f;
	const float eps = 0.005f;      // 0.5% PF step
	float out = pf_ema;
	if (fabsf(out - last_out) < eps)
		out = last_out;

	/* Final clamp + publish */
	if (out > 1.0f)
		out = 1.0f;
	if (out < -1.0f)
		out = -1.0f;
	last_out = out;
	Power_Factor = out;
}

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
	MX_TIM1_Init();
	/* USER CODE BEGIN 2 */
	if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
		Error_Handler();
	}

	HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);

	HAL_TIM_Base_Start(&htim1);

	status = HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_buffer, ADC_DMA_COUNT);

	digits[0] = 0;
	digits[1] = 0;
	digits[2] = 0;
	digits[3] = 0;
	digits[4] = 1;

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		b1 = HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
		b2 = HAL_GPIO_ReadPin(BUTTON2_GPIO_Port, BUTTON2_Pin);
// =============== Step 1: Long-press BOTH BUTTONS to enter password mode =============== //

		if (!inPasswordMode) {
			if (b1 == GPIO_PIN_RESET && b2 == GPIO_PIN_RESET) {

				if (waitingFor2Sec == 0) {
					buttonPressStartTime = HAL_GetTick();
					waitingFor2Sec = 1;
				} else if (HAL_GetTick() - buttonPressStartTime >= 2000) {

					inPasswordMode = 1;
					entryComplete = 0;
					waitingFor2Sec = 0;
					currentDigitIndex = 0;

					for (int i = 0; i < 4; i++) {

						digits[i] = 0;
						passwordEntered[i] = 0;
					}

				}
			} else {
				waitingFor2Sec = 0;
				if (buffer_ready) {
					buffer_ready = 0;
					Calculate_Vrms();
					Calculate_Irms();
					Compute_PF_FromBuffers();
				}
			}
		}

// ============================== Step 2: Enter password mode ============================== //

		else if (!modeEntryActive) {

			// Blinking selected digit
			if (!entryComplete && HAL_GetTick() - blinkTimer >= 300) {
				digits[4] = 0;
				blinkTimer = HAL_GetTick();
				blinkState = !blinkState;
				digits[currentDigitIndex] =
						blinkState ? 37 : passwordEntered[currentDigitIndex]; // 37 = blank
			}

			if (!entryComplete && HAL_GetTick() - lastPress > 300) {

				if (b2 == GPIO_PIN_RESET) {
					passwordEntered[currentDigitIndex]++;
					if (passwordEntered[currentDigitIndex] > 9)
						passwordEntered[currentDigitIndex] = 0;
					digits[currentDigitIndex] =
							passwordEntered[currentDigitIndex];
					lastPress = HAL_GetTick();
				} else if (b1 == GPIO_PIN_RESET) {
					digits[currentDigitIndex] = 36;
					currentDigitIndex++;
					if (currentDigitIndex >= 4) {
						entryComplete = 1;

						// Compare password
						passwordMatched = 1;
						for (int i = 0; i < 4; i++) {
							if (passwordEntered[i] != correctPassword[i]) {
								passwordMatched = 0;
								break;
							}

							if (passwordEntered[i] == correctPassword[i]) {

								modeEntryActive = 1;
							}
						}

						digits[0] = 14; // E
						digits[1] = 23; // n
						digits[2] = 36; // -
						digits[3] = passwordMatched ? 34 : 23; // y : n

						while (1) {
							GPIO_PinState b2 = HAL_GPIO_ReadPin(
							BUTTON2_GPIO_Port, BUTTON2_Pin);
							if (b2 == GPIO_PIN_RESET) {
								programming = 1;
								break;
							}
						}

						if (!passwordMatched) {
							readOnlyMode = 1;
							mode = 0;
							currentDigitIndex = 0;
							blinkState = 0;
							blinkTimer = HAL_GetTick();

						}

						// if matched, go to config mode

						currentDigitIndex = 0;
						mode = 0;
						for (int i = 0; i < 4; i++)
							settingDigits[i] = 0;

					}
					lastPress = HAL_GetTick();

				}
			}
		}
// =============== Step 3: Simple configuration MENU after correct password ================== //
		        else if (programming == 1) {
		            // Common debounce
		            uint32_t now = HAL_GetTick();

		            // ------------------- 1) Display logic (what to show on digits) ------------------- //
		            switch (mode) {

		            // ---------- MODE 0: Conn menu ----------
		            case 0:
		                if (connState == 0) {
		                    // Show "Conn"
		                    digits[0] = 12;   // C
		                    digits[1] = 24;   // o
		                    digits[2] = 23;   // n
		                    digits[3] = 23;   // n

		                    // No decimals on label
		                    digits[4] = 0;
		                    digits[5] = 0;
		                    digits[6] = 0;
		                    digits[7] = 0;
		                } else {
		                    if (connState == 2) {
		                        // Blink current type
		                        if (now - blinkTimer >= 300) {
		                            blinkTimer = now;
		                            blinkState = !blinkState;
		                        }

		                        if (blinkState) {
		                            digits[0] = 37;
		                            digits[1] = 37;
		                            digits[2] = 37;
		                            digits[3] = 37;
		                            digits[4] = 0;
		                            digits[5] = 0;
		                            digits[6] = 0;
		                            digits[7] = 0;
		                        } else {
		                            goto SHOW_CONN_TYPE;
		                        }
		                    } else {
		SHOW_CONN_TYPE:
		                        if (connType == 0) {
		                            // "delt"
		                            digits[0] = 13;  // d
		                            digits[1] = 14;  // e
		                            digits[2] = 21;  // L
		                            digits[3] = 29;  // t
		                        } else if (connType == 1) {
		                            // "iph"
		                            digits[0] = 18;  // I
		                            digits[1] = 25;  // P
		                            digits[2] = 17;  // H
		                            digits[3] = 37;  // blank
		                        } else {
		                            // "star"
		                            digits[0] = 28;  // S
		                            digits[1] = 29;  // T
		                            digits[2] = 10;  // A
		                            digits[3] = 27;  // R
		                        }
		                        // No decimals for text
		                        digits[4] = 0;
		                        digits[5] = 0;
		                        digits[6] = 0;
		                        digits[7] = 0;
		                    }
		                }
		                break;

		            // ---------- MODE 1: ct.pr ----------
		            case 1:
		            {
		                if (ct_pr_state == 0)
		                {
		                    // Label: "ct.pr"
		                    digits[0] = 12;  // C
		                    digits[1] = 29;  // T
		                    digits[2] = 25;  // P
		                    digits[3] = 27;  // R

		                    // No decimal dots on label
		                    digits[4] = 0;
		                    digits[5] = 1;
		                    digits[6] = 0;
		                    digits[7] = 0;
		                }
		                else
		                {
		                    // Numeric display using ct_pr_digits + decimal position
		                    digits[0] = ct_pr_digits[0];
		                    digits[1] = ct_pr_digits[1];
		                    digits[2] = ct_pr_digits[2];
		                    digits[3] = ct_pr_digits[3];

		                    // Clear all decimal flags first
//		                        digits[4] = 0;
//		                    	digits[5] = 0;
//		                    	digits[6] = 0;
//		                    	digits[7] = 0;


		                    dp_index = ct_pr_decimal_pos;
		                    if (dp_index > 3) dp_index = 3;

		                    if (ct_pr_state == 1)
		                    {

		                        // View only, steady decimal
		                        digits[4 + dp_index] = 1;
		                    }
		                    else if (ct_pr_state == 2)
		                    {
		                        // Edit digits with blink on selected digit
		                        if (now - ct_pr_blinkTimer >= 300) {
		                            ct_pr_blinkTimer = now;
		                            ct_pr_blinkState = !ct_pr_blinkState;
		                        }

		                        // Decimal ON steadily while editing digits
		                        digits[4 + dp_index] = 1;

		                        if (ct_pr_blinkState) {
		                            // Make current digit blank while blinkState=1
		                            digits[ct_pr_editing_digit] = 37;
		                            HAL_Delay(100);
		                        }
		                    }
		                    else if (ct_pr_state == 3)


		                    {
		                        // Editing decimal position, blink the DP
		                        if (now - ct_pr_blinkTimer >= 400) {
		                            ct_pr_blinkTimer = now;
		                            ct_pr_blinkState = !ct_pr_blinkState;
		                        }

		                        if (ct_pr_blinkState) {
		                            digits[4 + dp_index] = 0;  // decimal OFF

		                        } else {
		                            digits[4 + dp_index] = 1;  // decimal ON
		                            HAL_Delay(100);
		                        }
		                    }
		                }
		            }
		                break;

		            // ---------- MODE 2: ct.se ----------
		            case 2:
		            {
		                if (ct_se_state == 0)
		                {
		                    // Label: "ct.se"
		                    digits[0] = 12;  // C
		                    digits[1] = 29;  // T
		                    digits[2] = 28;  // S
		                    digits[3] = 14;  // E

		                    // No decimal dots on label
		                    digits[4] = 0;
		                    digits[5] = 1;
		                    digits[6] = 0;
		                    digits[7] = 0;
		                }
		                else
		                {
		                    // Numeric display using ct_se_digits + decimal position
		                    digits[0] = ct_se_digits[0];
		                    digits[1] = ct_se_digits[1];
		                    digits[2] = ct_se_digits[2];
		                    digits[3] = ct_se_digits[3];

		                    // Clear decimals
//		                    digits[4] = 0;
//		                    digits[5] = 0;
//		                    digits[6] = 0;
//		                    digits[7] = 0;

		                    uint8_t dp_index = ct_se_decimal_pos;
		                    if (dp_index > 3) dp_index = 3;

		                    if (ct_se_state == 1)
		                    {
		                        // View only
		                        digits[4 + dp_index] = 1;
		                    }
		                    else if (ct_se_state == 2)
		                    {
		                        // Edit digits with blink
		                        if (now - ct_se_blinkTimer >= 400) {
		                            ct_se_blinkTimer = now;
		                            ct_se_blinkState = !ct_se_blinkState;
		                        }

		                        digits[4 + dp_index] = 1;  // decimal steady

		                        if (ct_se_blinkState) {
		                            digits[ct_se_editing_digit] = 37;
		                            HAL_Delay(100);
		                        }
		                    }
		                    else if (ct_se_state == 3)
		                    {
		                        // Edit decimal position, blink DP
		                        if (now - ct_se_blinkTimer >= 400) {
		                            ct_se_blinkTimer = now;
		                            ct_se_blinkState = !ct_se_blinkState;
		                        }

		                        if (ct_se_blinkState) {
		                            digits[4 + dp_index] = 0;
		                        } else {
		                            digits[4 + dp_index] = 1;
		                        }
		                    }
		                }
		            }
		                break;

		            // ---------- MODE 3: SAVE y/n ----------
		            case 3:
		                // "SAV"
		                digits[0] = 28;  // S
		                digits[1] = 10;  // A
		                digits[2] = 30;  // V

		                // No decimals
		                digits[4] = 0;
		                digits[5] = 0;
		                digits[6] = 1;
		                digits[7] = 0;

		                if (now - blinkTimer >= 300) {
		                    blinkTimer = now;
		                    blinkState = !blinkState;
		                }

		                if (blinkState) {
		                    digits[3] = 37;  // blank
		                } else {
		                    digits[3] = (saveChoice == 0) ? 34 : 23;   // Y / n

		                }
		                break;
		            }

		            // ------------------- 2) Button handling (B1 / B2) ------------------- //
		            if (now - lastPress > 200)   // simple debounce
		            {
		                // BUTTON 1: "enter / edit / confirm"
		                if (b1 == GPIO_PIN_RESET) {
		                    lastPress = now;

		                    // ----- MODE 0: Conn -----
		                    if (mode == 0) {
		                        if (connState == 0) {
		                            connState = 1;      // show current type
		                        } else if (connState == 1) {
		                            connState = 2;      // start editing (blink)
		                            blinkState = 0;
		                            blinkTimer = now;
		                        } else { // connState == 2
		                            connState = 0;      // finish editing
		                        }
		                    }

		                    // ----- MODE 1: ct.pr -----
		                    else if (mode == 1)
		                    {
		                        if (ct_pr_state == 0) {
		                            // Label -> numeric view
		                            ct_pr_state = 1;
		                            digits[5] = 0;
		                        }
		                        else if (ct_pr_state == 1) {
		                            // View -> edit digit 0
		                            ct_pr_state         = 2;
		                            ct_pr_editing_digit = 0;
		                            ct_pr_blinkState    = 0;
		                            ct_pr_blinkTimer    = now;
		                        }
		                        else if (ct_pr_state == 2) {
		                            // Next digit or decimal edit
		                            if (ct_pr_editing_digit < 3) {
		                                ct_pr_editing_digit++;
		                                ct_pr_blinkState = 0;
		                                ct_pr_blinkTimer = now;
		                            } else {
		                                ct_pr_state      = 3;  // go to decimal edit
		                                ct_pr_blinkState = 0;

		                                ct_pr_blinkTimer = now;
		                            }
		                        }
		                        else if (ct_pr_state == 3) {
		                            // Finish edit, compute value, go back to label
		                            ct_pr_value = CT_Pr_DigitsToValue();
		                            ct_pr_state = 0;

		                        }
		                    }

		                    // ----- MODE 2: ct.se -----
		                    else if (mode == 2)
		                    {
		                        if (ct_se_state == 0) {
		                            // Label -> numeric view
		                        	 digits[5] = 0;
		                            ct_se_state = 1;
		                        }
		                        else if (ct_se_state == 1) {
		                            // View -> edit digit 0
		                            ct_se_state         = 2;
		                            ct_se_editing_digit = 0;
		                            ct_se_blinkState    = 0;
		                            ct_se_blinkTimer    = now;
		                        }
		                        else if (ct_se_state == 2) {
		                            if (ct_se_editing_digit < 3) {
		                                ct_se_editing_digit++;
		                                ct_se_blinkState = 0;
		                                ct_se_blinkTimer = now;
		                            } else {
		                                ct_se_state      = 3;  // decimal edit
		                                ct_se_blinkState = 0;
		                                ct_se_blinkTimer = now;
		                            }
		                        }
		                        else if (ct_se_state == 3) {
		                            // Finish edit
		                            ct_se_value = CT_Se_DigitsToValue();
		                            ct_se_state = 0;
		                        }
		                    }

		                    // ----- MODE 3: SAVE -----
		                    else if (mode == 3) {
		                        if (saveChoice == 0) {
		                            // YES → exit programming
		                            programming     = 0;
		                            inPasswordMode  = 0;
		                            modeEntryActive = 0;
		                            connState       = 0;
		                            mode            = 0;
		                            // Clear main digits
		                            digits[0] = digits[1] = digits[2] = digits[3] = 0;
		                            digits[5] = digits[6] = digits[7] = 0;
		                            digits[4] = 1;
		                        } else {
		                            // NO → back to Conn
		                            mode      = 0;
		                            connState = 0;
		                            digits[6] = 0;
		                        }
		                    }
		                }

		                // BUTTON 2: "next / increment / toggle"
		                else if (b2 == GPIO_PIN_RESET) {
		                    lastPress = now;

		                    // ----- MODE 0: Conn -----
		                    if (mode == 0) {
		                        if (connState == 2) {
		                            connType = (connType + 1) % 3;
		                        } else if (connState == 0) {
		                            mode        = 1;   // go to ct.pr
		                            ct_pr_state = 0;
		                        }
		                    }

		                    // ----- MODE 1: ct.pr -----
		                    else if (mode == 1)
		                    {
		                        if (ct_pr_state == 0) {
		                            // Label: B2 -> next menu (ct.se)
		                            mode        = 2;
		                            ct_se_state = 0;
		                        }
		                        else if (ct_pr_state == 1) {
		                            // View only: B2 no-op (optional)
		                        }
		                        else if (ct_pr_state == 2) {
		                            // Increment current digit
		                            uint8_t *pd = &ct_pr_digits[ct_pr_editing_digit];
		                            (*pd)++;
		                            if (*pd > 9) *pd = 0;
		                        }
		                        else if (ct_pr_state == 3) {
		                            // Cycle decimal position 0..3
		                            digits[4] = 0;
		                            digits[5] = 0;
		                            digits[6] = 0;
		                            digits[7] = 0;
		                            ct_pr_decimal_pos++;
		                            if (ct_pr_decimal_pos > 3)
		                                ct_pr_decimal_pos = 0;
		                        }
		                    }

		                    // ----- MODE 2: ct.se -----
		                    else if (mode == 2)
		                    {
		                        if (ct_se_state == 0) {
		                            // Label: B2 -> SAVE menu
		                            mode       = 3;
		                            saveChoice = 0;
		                            blinkState = 0;
		                            blinkTimer = now;
		                        }
		                        else if (ct_se_state == 1) {
		                            // View only: no-op
		                        }
		                        else if (ct_se_state == 2) {
		                            // Increment digit
		                            uint8_t *pd = &ct_se_digits[ct_se_editing_digit];
		                            (*pd)++;
		                            if (*pd > 9) *pd = 0;
		                        }
		                        else if (ct_se_state == 3) {
		                            // Cycle decimal position

		                            digits[4] = 0;
		                     		digits[5] = 0;
		                     		digits[6] = 0;
		                     		digits[7] = 0;
		                            ct_se_decimal_pos++;
		                            if (ct_se_decimal_pos > 3)
		                                ct_se_decimal_pos = 0;
		                        }
		                    }

		                    // ----- MODE 3: SAVE -----
		                    else if (mode == 3) {
		                        // Toggle Y/N
		                        saveChoice ^= 1;
		                    }
		                }
		            }
		        }



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
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
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
	hadc1.Init.NbrOfConversion = 3;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO2;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;
	hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_160CYCLES_5;
	hadc1.Init.OversamplingMode = DISABLE;
	hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_11;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_VREFINT;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 1600;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_Handler();
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

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
	htim3.Init.Prescaler = 1;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 32768;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_OC_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_TIMING;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 2, 0);
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
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | DP_Pin | D_Pin | E_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA,
	GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | C_D4_Pin | G_Pin | C_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

	/*Configure GPIO pins : PA6 PA7 */
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB1 PB2 DP_Pin
	 D_Pin E_Pin */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | DP_Pin | D_Pin
			| E_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PA8 PA9 PA10 C_D4_Pin
	 G_Pin C_Pin */
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | C_D4_Pin
			| G_Pin | C_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PC6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM14) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

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
