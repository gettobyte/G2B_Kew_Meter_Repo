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

#define SAMPLES 1024.0f


#define BUTTON1_Pin GPIO_PIN_7
#define BUTTON1_GPIO_Port GPIOA

#define BUTTON2_Pin GPIO_PIN_0
#define BUTTON2_GPIO_Port GPIOB


#define OFFSET_ADC_Volt       5.0f

#define VREFINT_CAL_ADDR   ((uint16_t*)0x1FFF75AA)
#define VREFINT_CAL_VREF   3000UL   // mV


#define FLASH_USER_PAGE_ADDR   0x0800F800  // Last page
#define FLASH_USER_PAGE_SIZE   2048
#define ENTRY_SIZE             8           // 64-bit writes
#define MAX_ENTRIES            (FLASH_USER_PAGE_SIZE / ENTRY_SIZE)

//extern uint8_t modeSettings[5][4];  // Your array

GPIO_PinState b2;
GPIO_PinState b1;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
HAL_StatusTypeDef status;

//uint16_t flag1 = 0;
uint16_t digits[16];

uint32_t buttonPressStartTime = 0;
uint8_t waitingFor2Sec = 0;
uint8_t inPasswordMode = 0;

uint8_t currentDigitIndex = 0;

uint8_t passwordDigits[4] = { 0, 0, 0, 0 };
const uint8_t defaultPassword[4] = { 1, 2, 3, 4 };

uint8_t correctPassword[4] = { 1, 0, 0, 0 };
uint8_t passwordEntered[4] = { 0 };
uint8_t settingDigits[4] = { 0 };
uint8_t mode = 0;
uint8_t entryComplete = 0;
uint8_t passwordMatched = 0;
uint32_t lastPress = 0, blinkTimer = 0;
uint8_t blinkState = 0;
uint8_t modeEntryActive = 0;

uint8_t editMode = 0;                 // Are we editing digits?

uint8_t readOnlyMode = 0;

uint32_t sum_V = 0;

uint32_t sum_C = 0;

float cv_ref = 0;

uint32_t ref_thresh = 0;

float average_V = 0;

float average_C = 0;

float voltage = 0;
float current = 0;

float display_voltage = 0;

float corrected_V = 0;

float display_current =0;

static float ema_current = 0.0f;
const float EMA_ALPHA = 0.05f; // smoothing factor (0.01 = very stable, 0.2 = faster response)

/* --- Stable display logic with hysteresis --- */
static float last_display_current = 0.0f;
const float DISPLAY_THRESHOLD = 0.5f;   // 100 mV stability band

int16_t corrected_A = 0;

int val_int;


float ema_voltage = 0;

float offset_correction;
float gain_correction;

uint16_t adc_vrefint = 0;   // latest ADC result for VREFINT
uint32_t vdda_mV = 0;       // calculated VDDA (mV)

uint16_t AD_RES_BUFFER[4];

const uint8_t modeLabels[5][4] = { { 28, 17, 23, 29 }, // SHnt
		{ 13, 30, 1, 13 },  // du1d
		{ 11, 30, 10, 13 }, // bUAd
		{ 25, 10, 27, 29 }, // PArt
		{ 28, 10, 30, 14 }  // SAVE
};

uint8_t modeSettings[5][4] = { { 2, 0, 0, 0 }, // Default SHnt Value
		{ 0, 1, 0, 37 }, // Default du1d Value
		{ 9, 6, 0, 0 }, // Default bUAd
		{ 14, 30, 14, 23 }, // Default: EVEn (E, V, E, n)
		{ 34, 14, 5, 37 }  // Default SAVE: y
};

uint8_t partModes[3][4] = { { 14, 30, 14, 23 }, // EVEn
		{ 0, 13, 13, 37 }, // odd
		{ 23, 24, 23, 37 }  // non
};

uint8_t saveModes[2][4] = { { 23, 24, 37, 37 }, // YES
		{ 34, 14, 5, 37 }, // NO
		};

const uint8_t baudModes[4][4] = { { 9, 6, 0, 0 },   // 9600
		{ 4, 8, 0, 0 },   // 4800
		{ 2, 4, 0, 0 },   // 2400
		{ 1, 9, 2, 0 }    // 1920
};
uint8_t baudModeIndex = 0;

uint8_t modeEditBlink = 0; // Used in Mode 3 (PArt) and Mode 4 (SAVE)

int partModeIndex = 0;
int saveToggle = 0; // 0 = EVEn, 1 = odd
int programming = 0;

typedef struct {
	uint32_t shuntValue;
	uint8_t modeSettings[5][4];
	uint32_t crc;   // optional: for data integrity
} MeterSettings;

MeterSettings settings;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

void SaveShuntValueToFlash(MeterSettings *settings) {
	HAL_FLASH_Unlock();

	// Erase last page
	FLASH_EraseInitTypeDef eraseInitStruct;
	uint32_t PageError = 0;
	eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInitStruct.Page = 31;
	eraseInitStruct.NbPages = 1;
	HAL_FLASHEx_Erase(&eraseInitStruct, &PageError);

	// Write struct to flash
	uint64_t *src = (uint64_t*) settings;
	uint32_t flashAddr = FLASH_USER_PAGE_ADDR;
	for (uint32_t i = 0; i < sizeof(MeterSettings); i += 8) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flashAddr, *src++);
		flashAddr += 8;
	}

	HAL_FLASH_Lock();
}

//--------------------------------------------------
// Load modeSettings[5][4] from Flash
//--------------------------------------------------
uint32_t LoadShuntValueFromFlash(MeterSettings *settings) {
	uint8_t *src = (uint8_t*) FLASH_USER_PAGE_ADDR;
	memcpy(settings, src, sizeof(MeterSettings));

	// If flash is empty, apply defaults
	if (*(uint64_t*) FLASH_USER_PAGE_ADDR == 0xFFFFFFFFFFFFFFFFULL) {
		settings->shuntValue = 2000;  // default
		uint8_t modeSettings[5][4] = { { 2, 0, 0, 0 },    // Default SHnt
				{ 0, 1, 0, 37 },   // Default du1d
				{ 9, 6, 0, 0 },    // Default bUAd
				{ 14, 30, 14, 23 },    // Default EVEn
				{ 34, 14, 5, 37 }  // Default SAVE
		};
		memcpy(settings->modeSettings, modeSettings, sizeof(modeSettings));
	}
}

void CurrentValue() {

	float shunt_A = ((float) modeSettings[0][0] * 100.0f)
			+ ((float) modeSettings[0][1] * 10.0f)
			+ ((float) modeSettings[0][2] * 1.0f)
			+ ((float) modeSettings[0][3] * 0.1f);
	float shunt_mV = 75.0f;    // default 75 mV
	float INA_gain = 20.0f;    // INA181 gain (fixed)

	// Derived shunt resistance
	float R_shunt = (shunt_mV) / shunt_A;  // mOhms
	sum_C = 0;
	ref_thresh = 0;

	  //cv_ref = AD_RES_BUFFER[3];

	 // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1,GPIO_PIN_SET);

	for (uint16_t i = 0; i < SAMPLES; i++) {
		HAL_ADC_Start_DMA(&hadc1, AD_RES_BUFFER, 4);
		//adcBuffer_C[i] = AD_RES_BUFFER[1];   // save each sample in buffer
		sum_C += AD_RES_BUFFER[1];         // also sum if you still need average
		ref_thresh += AD_RES_BUFFER[3];
	}

	average_C = sum_C / SAMPLES;

	cv_ref = ref_thresh / SAMPLES;

	//corrected_A = (average_C > OFFSET_ADC_Curr) ? (average_C - OFFSET_ADC_Curr) : 0;

	adc_vrefint = AD_RES_BUFFER[2] ; //fixed bandgap reference inside the chip

	uint16_t vrefint_cal = *VREFINT_CAL_ADDR;

	vdda_mV = (uint32_t) VREFINT_CAL_VREF * vrefint_cal /adc_vrefint;

	/////////////////////////////////////////////////////////////////////////////////////////////////

//	float v_adc = (corrected_A * vdda_mV) / 4095;
//
//	current = ((v_adc) / (INA_gain * R_shunt));

	// --- Delta between INA output and REF (use averaged codes) ---
	int32_t delta_code = (int32_t)average_C  -  (int32_t)cv_ref ;

	// ADC codes -> millivolts
	float v_diff_mV = (float)delta_code * ((float)vdda_mV / 4095.0f);


	current = (v_diff_mV / (INA_gain * R_shunt));




	ema_current = ema_current + EMA_ALPHA * (current - ema_current);

	if (ema_current < 0.0f) {
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);     // ON for negative
	} else {
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);   // OFF for positive/zero
	}

	if (fabsf(ema_current - last_display_current) >= DISPLAY_THRESHOLD) {
		last_display_current = ema_current;
	}

	 display_current = last_display_current;


	float abs_curr = fabsf(display_current);

	if (abs_curr < 2.0)
	{
		//abs_curr = 0.0f;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	}

	if (abs_curr < 0.8)
	{
		abs_curr = 0.0f;
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	}

	// === Clear digits & DP flags ===
	for (int i = 4; i < 8; i++) {
	    digits[i] = 0;
	    digits[8 + i] = 0;
	}
	digits[12] = 0;  // DP after first digit
	digits[13] = 0;  // DP after second digit
	digits[14] = 0;  // DP after third digit

	// === Format magnitude (identical layout for pos/neg) ===
	int val_int;
	if (abs_curr < 10.0f) {
	    // X.XX
	    val_int = (int)(abs_curr * 100.0f + 0.5f);   // e.g. 2.34 -> 234
	    digits[4] = (val_int / 100) % 10;            // integer part
	    digits[5] = (val_int / 10)  % 10;            // first decimal
	    digits[6] =  val_int        % 10;            // second decimal
	    digits[12] = 1;                               // DP after first digit (X.XX)

	} else if (abs_curr < 100.0f) {
	    // XX.XX
	    val_int = (int)(abs_curr * 100.0f + 0.5f);   // e.g. 23.45 -> 2345
	    digits[4] = (val_int / 1000) % 10;           // tens
	    digits[5] = (val_int / 100)  % 10;           // ones
	    digits[6] = (val_int / 10)   % 10;           // first decimal
	    digits[7] =  val_int         % 10;           // second decimal
	    digits[13] = 1;                               // DP after second digit (XX.XX)

	} else {
	    // XXX.X
	    val_int = (int)(abs_curr * 10.0f + 0.5f);    // e.g. 123.4 -> 1234
	    digits[4] = (val_int / 1000) % 10;           // hundreds
	    digits[5] = (val_int / 100)  % 10;           // tens
	    digits[6] = (val_int / 10)   % 10;           // ones
	    digits[7] =  val_int         % 10;           // first decimal
	    digits[14] = 1;                               // DP after third digit (XXX.X)
	}

}

void VoltageValue() {
	sum_V = 0;

	for (uint16_t i = 0; i < SAMPLES; i++) {
		HAL_ADC_Start_DMA(&hadc1, AD_RES_BUFFER, 4);
		//adcBuffer_V[i] = AD_RES_BUFFER[0];   // save each sample in buffer
		sum_V += AD_RES_BUFFER[0];               // also sum if you still need

	}

	average_V = sum_V / SAMPLES;

	corrected_V = (average_V > 0) ? (average_V - 0) : 0;

	adc_vrefint = AD_RES_BUFFER[2]; //fixed bandgap reference inside the chip

	uint16_t vrefint_cal = *VREFINT_CAL_ADDR;

	vdda_mV = (uint32_t) VREFINT_CAL_VREF * vrefint_cal / adc_vrefint;

	voltage = (corrected_V * vdda_mV) / 4095;



	float conversion_rate = 0.05367f;
//	float conversion_rate = 0.0864378565f;   // was 9.76 → slightly smaller to keep high-end aligned
//	float gain_correction = 1.0107f;  // near-unity; keep slope stable
//	float offset_correction = -0.046f; // was 2.40 → pull offset down by ~1.0 V


	voltage = ((voltage * conversion_rate) * 2 );

	/* --- Exponential Moving Average Filter --- */
	static float ema_voltage = 0.0f;
	const float EMA_ALPHA = 0.05f;      // (0.01 = very stable, 0.2 = faster response) faster settling without looking jumpy

	ema_voltage = ema_voltage + EMA_ALPHA * (voltage - ema_voltage);

	/* --- Stable display logic with hysteresis --- */
	static float last_display_voltage = 0.0f;
	const float DISPLAY_THRESHOLD = 0.1f;  // tighter update band (50 mV)

	if (fabsf(ema_voltage - last_display_voltage) >= DISPLAY_THRESHOLD) {
	    last_display_voltage = ema_voltage;
	}


	 display_voltage = last_display_voltage;

	// After computing display_voltage
	if (fabsf(display_voltage) <= 0.3f)   // anything below 50 mV = 0.00
		display_voltage = 0.0f;

	/* ---------- clamp display_voltage (unchanged behavior) ---------- */
	if (display_voltage > 200.0f)
		display_voltage = 200.0f;
	if (display_voltage < 0.0f)
		display_voltage = 0.0f;

	/* ---------- Clear digits & DPs ---------- */
	for (int i = 0; i < 4; i++) {
		digits[i] = 0;
		digits[8 + i] = 0;
	}

	/* ---------- Extract digits + Set DP ---------- */
	/* NOTE: Using display_voltage (ring-averaged) for all formatting */
	if (display_voltage < 10.0f) {
		// Format: X.XX (1 integer + 2 decimals). Keep last digit as the 3rd decimal if you want.
		val_int = (int) (display_voltage * 100 + 0.5f); // e.g. 2.34 -> 234 (hundreds=tens etc.)

		digits[0] = (val_int / 100) % 10;  // integer part
		digits[1] = (val_int / 10) % 10;   // first decimal
		digits[2] = val_int % 10;          // second decimal
		digits[3] = 0;                     // optional: fill with fake digit if desired

		digits[8] = 1;  // DP after first digit (X.XX)
	} else if (display_voltage < 100.0f) {
		// Format: XX.XX (tens, ones, two decimals)
		val_int = (int) (display_voltage * 100 + 0.5f);  // e.g. 23.45 -> 2345

		digits[0] = (val_int / 1000) % 10; // tens
		digits[1] = (val_int / 100) % 10;  // ones
		digits[2] = (val_int / 10) % 10;   // first decimal
		digits[3] = val_int % 10;         // second decimal

		digits[9] = 1;  // DP after second digit (XX.XX)
	} else {
		// Format: XXX.X  (since you only have 4 digits, reduce decimals for 3-digit numbers)
		val_int = (int) (display_voltage * 10 + 0.5f);  // e.g. 123.4 -> 1234

		digits[0] = (val_int / 1000) % 10; // hundreds
		digits[1] = (val_int / 100) % 10;  // tens
		digits[2] = (val_int / 10) % 10;   // ones
		digits[3] = val_int % 10;         // first decimal

		digits[10] = 1;  // DP after third digit (XXX.X)
	}

//	 HAL_Delay(200);

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

	if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
		Error_Handler();
	}

	status = HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1);

	digits[8] = 1;
	digits[12] = 1;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		GPIO_PinState b1 = HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
		GPIO_PinState b2 = HAL_GPIO_ReadPin(BUTTON2_GPIO_Port, BUTTON2_Pin);
// =============== Step 1: Long-press BOTH BUTTONS to enter password mode =============== //

		if (!inPasswordMode) {
			if (b1 == GPIO_PIN_RESET && b2 == GPIO_PIN_RESET) {

				if (waitingFor2Sec == 0) {
					buttonPressStartTime = HAL_GetTick();
					waitingFor2Sec = 1;
				} else if (HAL_GetTick() - buttonPressStartTime >= 2000) {

					digits[8] = 0;
					digits[12] = 0;
					digits[9] = 0;
					digits[10] = 0;

					inPasswordMode = 1;
					entryComplete = 0;
					waitingFor2Sec = 0;
					currentDigitIndex = 0;

					// Show "PASS"
					digits[4] = 25; // P
					digits[5] = 10; // A
					digits[6] = 28; // S
					digits[7] = 28; // S

					for (int i = 0; i < 4; i++) {

						digits[i] = 0;
						passwordEntered[i] = 0;
					}

				}
			} else {
				waitingFor2Sec = 0;
				HAL_ADC_Start_DMA(&hadc1, AD_RES_BUFFER, 4);
				CurrentValue();
				VoltageValue();
			}
		}

// =============== Step 2: Enter password mode =============== //

		else if (!modeEntryActive) {

			// Blinking selected digit
			if (!entryComplete && HAL_GetTick() - blinkTimer >= 300) {
				blinkTimer = HAL_GetTick();
				blinkState = !blinkState;
				digits[currentDigitIndex] =
						blinkState ? 37 : passwordEntered[currentDigitIndex]; // 37 = blank
			}

			if (!entryComplete && HAL_GetTick() - lastPress > 300) {

				if (b1 == GPIO_PIN_RESET) {
					passwordEntered[currentDigitIndex]++;
					if (passwordEntered[currentDigitIndex] > 9)
						passwordEntered[currentDigitIndex] = 0;
					digits[currentDigitIndex] =
							passwordEntered[currentDigitIndex];
					lastPress = HAL_GetTick();
				} else if (b2 == GPIO_PIN_RESET) {
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
								programming = 1;
								modeEntryActive = 1;
							}
						}

						digits[0] = 14; // E
						digits[1] = 23; // n
						digits[2] = 36; // -
						digits[3] = passwordMatched ? 34 : 23; // y : n
						while (1) {
							GPIO_PinState b1 = HAL_GPIO_ReadPin(
									BUTTON1_GPIO_Port, BUTTON1_Pin);
							if (b1 == GPIO_PIN_RESET) {
								break;
							}
						}
//                           HAL_Delay(1000);

						if (!passwordMatched) {
							readOnlyMode = 1;
							mode = 0;
							currentDigitIndex = 0;
							blinkState = 0;
							blinkTimer = HAL_GetTick();

							const uint8_t defaultShnt[4] = { modeSettings[0][0],
									modeSettings[0][1], modeSettings[0][2],
									modeSettings[0][3] };
							const uint8_t defaultDuid[4] = { modeSettings[1][0],
									modeSettings[1][1], modeSettings[1][2], 37 };

							int localMode = 0; // Tracks what to show: 0 → SHnt, 1 → du1d, 2 → reset

							while (1) {
								// Show SHnt

								if (localMode == 0) {
									digits[10] = 1;
									for (int j = 0; j < 4; j++) {
										digits[j] = defaultShnt[j];
										digits[4 + j] = modeLabels[0][j];
									}
								}
								// Show du1d
								while (1) {

									GPIO_PinState b1 = HAL_GPIO_ReadPin(
											BUTTON1_GPIO_Port, BUTTON1_Pin);
									if (b1 == GPIO_PIN_RESET) {

										break;
									}
								}
								if (localMode == 1) {
									digits[10] = 0;
									for (int j = 0; j < 4; j++) {
										digits[j] = defaultDuid[j];
										digits[4 + j] = modeLabels[1][j];
									}

								}

								// Reset system after du1d
								else if (localMode == 2) {
									// === Reset system ===
									memcpy(settingDigits, defaultShnt, 4); // SHnt
									memcpy(settingDigits,
											baudModes[baudModeIndex], 4); // bUAd

									// Fully reset all state flags
									inPasswordMode = 0;
									modeEntryActive = 0;
									entryComplete = 0;
									waitingFor2Sec = 0;
									currentDigitIndex = 0;
									readOnlyMode = 0;
									programming = 0;
									editMode = 0;
									blinkState = 0;
									digits[8] = 1;
									digits[12] = 1;

									// Clear screen
									for (int i = 0; i < 8; i++)
										digits[i] = 0;

									HAL_Delay(300);

									// === Add this: exit wrong password loop ===
									break;
								}

								// Handle button 1 to cycle modes
								if (HAL_GPIO_ReadPin(BUTTON1_GPIO_Port,
										BUTTON1_Pin) == GPIO_PIN_RESET) {
									HAL_Delay(300); // debounce
									localMode++;
									if (localMode > 2)
										localMode = 2;
								}
							}
							inPasswordMode = 0;
							programming = 0;
							modeEntryActive = 0;

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

// ===============Step 3: Configuration mode (if password matched)========= //
		else if (programming == 1) {
			// === INIT DEFAULT VALUES ON FIRST ENTRY ONLY ===
			static uint8_t initialized = 0;
			if (!initialized) {
				const uint8_t defaultShnt[4] = { modeSettings[0][0],
						modeSettings[0][1], modeSettings[0][2],
						modeSettings[0][3] };
				const uint8_t defaultDuid[3] = { modeSettings[1][0],
						modeSettings[1][1], modeSettings[1][2] };

				memcpy(modeSettings[0], defaultShnt, 4);           // SHnt
				memcpy(modeSettings[1], defaultDuid, 3);            // du1d
				memcpy(modeSettings[2], baudModes[baudModeIndex], 4); // bUAd
				memcpy(modeSettings[3], partModes[partModeIndex], 4); // PArt
				memcpy(modeSettings[4], saveModes[saveToggle], 4);    // SAVE

				memcpy(settingDigits, modeSettings[0], 4); // Load SHnt initially
				initialized = 1;

				digits[10] = 1;
			}

			// === MODE LABEL DISPLAY (digits 4–7) ===
			for (int i = 0; i < 4; i++)
				digits[4 + i] = modeLabels[mode][i];

			// === UPPER DIGITS DISPLAY (0–3) ===
			if (mode == 4 && editMode)  // SAVE Mode with blinking
					{
				if (HAL_GetTick() - blinkTimer >= 300) {
					blinkTimer = HAL_GetTick();
					blinkState = !blinkState;
				}

				for (int i = 0; i < 4; i++)
					digits[i] = blinkState ? 37 : settingDigits[i];
			} else if (editMode) {
				if (HAL_GetTick() - blinkTimer >= 300) {
					blinkTimer = HAL_GetTick();
					blinkState = !blinkState;
				}

				for (int i = 0; i < 4; i++)
					digits[i] =
							(blinkState && (mode >= 2))
									|| (i == currentDigitIndex && blinkState) ?
									37 : settingDigits[i];
			} else {
				for (int i = 0; i < 4; i++)
					digits[i] = settingDigits[i];
			}

			// === BUTTON HANDLING ===
			if (HAL_GetTick() - lastPress > 300) {
				// ===== BUTTON 1 =====
				if (b1 == GPIO_PIN_RESET) {
					while (HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin)
							== GPIO_PIN_RESET)
						;

					if (editMode)

					{
						if (mode == 0 || mode == 1)
						{

							settingDigits[currentDigitIndex]++;
							if (settingDigits[currentDigitIndex] > 9)
								settingDigits[currentDigitIndex] = 0;
						}

						else if (mode == 1)  // duid mode
								{
							// Only allow editing first 3 digits (0,1,2)
							if (currentDigitIndex < 3) {
								settingDigits[currentDigitIndex]++;
								if (settingDigits[currentDigitIndex] > 9)
									settingDigits[currentDigitIndex] = 0;
							}
						}

						else if (mode == 2)  // bUAd
								{
							baudModeIndex = (baudModeIndex + 1) % 4;
							memcpy(settingDigits, baudModes[baudModeIndex], 4);
						} else if (mode == 3)  // PArt
								{
							partModeIndex = (partModeIndex + 1) % 3;
							memcpy(settingDigits, partModes[partModeIndex], 4);
						} else if (mode == 4)  // SAVE → Toggle YES/NO
								{
							saveToggle = !saveToggle;
							memcpy(settingDigits, saveModes[saveToggle], 4);
						}
					} else {
						// Save settings before switching mode
						for (int i = 0; i < 4; i++)
							modeSettings[mode][i] = settingDigits[i];

						mode = (mode + 1) % 5;
						for (int i = 0; i < 4; i++)
							settingDigits[i] = modeSettings[mode][i];

						if (mode == 0) {

							digits[10] = 1;

						} else {

							digits[10] = 0;

						}
						// Automatically enter editMode in SAVE mode
						if (mode == 4) {
							editMode = 1;
							blinkTimer = HAL_GetTick();
							saveToggle = 1;  // Start at YES
							memcpy(settingDigits, saveModes[saveToggle], 4);
						} else {
							editMode = 0;
							currentDigitIndex = 0;
						}
					}

					lastPress = HAL_GetTick();
				}

				// ===== BUTTON 2 =====
				else if (b2 == GPIO_PIN_RESET) {
					while (HAL_GPIO_ReadPin(BUTTON2_GPIO_Port, BUTTON2_Pin)
							== GPIO_PIN_RESET)
						;

					if (mode == 0 || mode == 1) {
						if (!editMode) {
							digits[10] = 1;
							editMode = 1;
							currentDigitIndex = 0;
							blinkTimer = HAL_GetTick();
						} else {
							currentDigitIndex++;
							if (mode == 1 && currentDigitIndex >= 3) // stop at digit 2
									{
								currentDigitIndex = 0;
								editMode = 0;
								for (int i = 0; i < 3; i++) // only save first 3 digits
									modeSettings[mode][i] = settingDigits[i];
							} else if (mode != 1 && currentDigitIndex >= 4) {
								currentDigitIndex = 0;
								editMode = 0;
								for (int i = 0; i < 4; i++)
									modeSettings[mode][i] = settingDigits[i];
							}

						}
					} else if (mode == 2 || mode == 3) {
						editMode = !editMode;
						blinkTimer = HAL_GetTick();

						if (!editMode) {
							for (int i = 0; i < 4; i++)
								modeSettings[mode][i] = settingDigits[i];
						}
					} else if (mode == 4)  // SAVE mode confirmation
							{
						editMode = 0;  // Stop blinking

						for (int i = 0; i < 4; i++)
							digits[i] = settingDigits[i];

						HAL_Delay(200);

						// SAVE = YES
						if (settingDigits[0] == saveModes[1][0]
								&& settingDigits[1] == saveModes[1][1]
								&& settingDigits[2] == saveModes[1][2]
								&& settingDigits[3] == saveModes[1][3]) {
							while (1) {
								GPIO_PinState b1_state = HAL_GPIO_ReadPin(
										BUTTON1_GPIO_Port, BUTTON1_Pin);
								GPIO_PinState b2_state = HAL_GPIO_ReadPin(
										BUTTON2_GPIO_Port, BUTTON2_Pin);

								if (b1_state == GPIO_PIN_RESET) {
									while (HAL_GPIO_ReadPin(BUTTON1_GPIO_Port,
											BUTTON1_Pin) == GPIO_PIN_RESET)
										;

									programming = 0;
									inPasswordMode = 0;
									modeEntryActive = 0;
									editMode = 0;
									currentDigitIndex = 0;
									initialized = 0;
									digits[8] = 1;
									digits[12] = 1;
									for (int i = 0; i < 8; i++)
										digits[i] = 0;
									HAL_Delay(300);

									//SaveShuntValueToFlash(&settings);  //SAVE FLASH VALUES

									break;
								}

								else if (b2_state == GPIO_PIN_RESET) {
									while (HAL_GPIO_ReadPin(BUTTON2_GPIO_Port,
											BUTTON2_Pin) == GPIO_PIN_RESET)
										;

									editMode = 1;
									blinkTimer = HAL_GetTick();
									currentDigitIndex = 0;
									break;
								}

								if (editMode) {
									if (HAL_GetTick() - blinkTimer >= 300) {
										blinkTimer = HAL_GetTick();
										blinkState = !blinkState;

										for (int i = 0; i < 4; i++)
											digits[i] =
													blinkState ?
															37 :
															settingDigits[i];
									}
								}
							}
						} else {

							// SAVE = NO → loop to next mode
							for (int i = 0; i < 4; i++)
								modeSettings[mode][i] = settingDigits[i];

							mode = 0;
							digits[10] = 1;
							for (int i = 0; i < 4; i++)
								settingDigits[i] = modeSettings[mode][i];
						}
					}

					lastPress = HAL_GetTick();
				}
			}
		}

////////////////////////////////////////////////////////////////////////////

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
  hadc1.Init.NbrOfConversion = 4;
  hadc1.Init.DiscontinuousConvMode = ENABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_160CYCLES_5;
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
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_4;
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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, C_D4_Pin|B_Pin|C_D1_Pin|E_Pin
                          |D_Pin|DP_Pin|C_Pin|G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, C_D3_Pin|V_D3_Pin|V_D2_Pin|A_Pin
                          |V_D1_Pin|V_D4_Pin|C_D2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : C_D4_Pin G_Pin */
  GPIO_InitStruct.Pin = C_D4_Pin|G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : C_D3_Pin V_D2_Pin A_Pin V_D1_Pin
                           V_D4_Pin */
  GPIO_InitStruct.Pin = C_D3_Pin|V_D2_Pin|A_Pin|V_D1_Pin
                          |V_D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SW_1_Pin */
  GPIO_InitStruct.Pin = SW_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SW_2_Pin */
  GPIO_InitStruct.Pin = SW_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW_2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Pin B_Pin C_D1_Pin E_Pin
                           D_Pin DP_Pin C_Pin */
  GPIO_InitStruct.Pin = LED_Pin|B_Pin|C_D1_Pin|E_Pin
                          |D_Pin|DP_Pin|C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : V_D3_Pin C_D2_Pin */
  GPIO_InitStruct.Pin = V_D3_Pin|C_D2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : F_Pin */
  GPIO_InitStruct.Pin = F_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(F_GPIO_Port, &GPIO_InitStruct);

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
