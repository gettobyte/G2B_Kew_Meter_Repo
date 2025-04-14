/* MAIN.C file
 * Author : Rohan(Gettobytee)
 * Copyright (c) 2002-2005 STMicroelectronics
 */

#include "stm8s.h"

volatile uint8_t currentDigit = 0;
uint8_t digits[6] = {0, 0, 0, 0, 0, 0};
static uint16_t counter;
uint8_t pattern;
uint32_t millivolt1, millivolt2;
uint32_t sum = 0;
uint8_t i, samples;
uint32_t voltage, current;
uint32_t adc1_value;
uint32_t adc2_value;

// Segment Pins
#define PortA GPIOA
#define PinA GPIO_PIN_1
#define PinB GPIO_PIN_2

#define PortC GPIOC
#define PinC GPIO_PIN_4
#define PinD GPIO_PIN_6
#define PinE GPIO_PIN_7
#define PinF GPIO_PIN_5
#define PinG GPIO_PIN_3

// Digit Control Pins
#define PortD GPIOD
#define Digit1 (GPIO_PIN_4)
#define Digit2 (GPIO_PIN_6)
#define Digit3 (GPIO_PIN_5)
#define PortB GPIOB
#define Digit4 (GPIO_PIN_4)
#define Digit5 (GPIO_PIN_5)
#define Digit6 (GPIO_PIN_3)

// Segment Mapping
const uint8_t Segment_Patterns[10] = {
    0b10000001,  // 0
    0b11001111,  // 1
    0b10010010,  // 2
    0b10000110,  // 3
    0b11001100,  // 4
    0b10100100,  // 5
    0b10100000,  // 6
    0b10001111,  // 7
    0b10000000,  // 8
    0b10000100   // 9
};

#define PORTD_GPIO_PINS  (Digit1 | Digit2 | Digit3)
#define PORTC_GPIO_PINS  (PinC | PinD | PinE | PinF |PinG)
#define PORTA_GPIO_PINS	 (PinA | PinB | Digit6)
#define PORTB_GPIO_PINS	 (Digit4 | Digit5)

void Delay_us(uint16_t nCount);
void clearDigits(void);
void displayDigit(uint8_t digit, uint8_t value);
void GPIO_Int(void);
void CLK_Init(void);
void Timer4_Init(void);
void ADC1_setup(void);
uint32_t ADC1_Read(void);
uint32_t ADC2_Read(void);
uint16_t readAverageADC1(uint8_t samples);
uint16_t readAverageADC2(uint8_t samples);

main()
{
	CLK_Init();
	GPIO_Int();
	Timer4_Init();  // Start Timer4 for display refresh
	
	while (1)
	{
		voltage = readAverageADC1(64);
		millivolt1 = (voltage * 230L) / 140;
		sum = 0;
		current = readAverageADC2(32);
		millivolt2 = (current * 1000) / 170;
		sum = 0;
		// Convert counter to individual digits
		digits[0] = ((millivolt1 / 100) % 10);  // Voltage hundreds
		digits[1] = ((millivolt1 / 10) % 10);   // Voltage tens
		digits[2] = (millivolt1 % 10);          // Voltage ones
		
		digits[3] = ((millivolt2 / 100) % 10);  // Current hundreds
		digits[4] = ((millivolt2 / 10) % 10);   // Current tens
		digits[5] = (millivolt2 % 10);          // Current ones
		
		
		Delay_us(100000000);
	}
}

//Custom Delay Function
void Delay_us(uint16_t nCount)
{
    while (nCount--) {
        _asm("nop"); _asm("nop");
    }
}

// GPIO Initialization
void GPIO_Int(void) {
	GPIO_DeInit(GPIOD);
	GPIO_DeInit(GPIOC);
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)PORTD_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);
	GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)PORTC_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)PORTA_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)PORTB_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
}

// Clock Initialization
void CLK_Init(void) {
	CLK_DeInit();
						
	CLK_HSECmd(DISABLE);
	CLK_LSICmd(DISABLE);
	CLK_HSICmd(ENABLE);
	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
						
	CLK_ClockSwitchCmd(ENABLE);
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV4);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
						
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
	DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
						
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);;
}

// Timer4 Initialization for ~5ms Interrupt
void Timer4_Init(void) {
	TIM4_DeInit();
	TIM4_TimeBaseInit(TIM4_PRESCALER_64, 156);      
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	TIM4_Cmd(ENABLE);
	
	enableInterrupts();
}

void ADC1_setup(void)
{
  ADC1_DeInit();         
                
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
						ADC1_CHANNEL_3,
						ADC1_PRESSEL_FCPU_D18, 
						ADC1_EXTTRIG_TIM, 
						DISABLE, 
						ADC1_ALIGN_RIGHT, 
						ADC1_SCHMITTTRIG_CHANNEL3, 
						DISABLE);
						 
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
						ADC1_CHANNEL_4,
						ADC1_PRESSEL_FCPU_D18, 
						ADC1_EXTTRIG_TIM, 
						DISABLE, 
						ADC1_ALIGN_RIGHT, 
						ADC1_SCHMITTTRIG_CHANNEL4, 
						DISABLE);
						
	ADC1_ConversionConfig(ADC1_CONVERSIONMODE_CONTINUOUS,
												((ADC1_Channel_TypeDef)(ADC1_CHANNEL_3 | ADC1_CHANNEL_4)),
												ADC1_ALIGN_RIGHT);
												
	ADC1_DataBufferCmd(ENABLE);
	
	ADC1_Cmd(ENABLE);
}

uint32_t ADC1_Read(void)
{
	ADC1_DeInit();
	
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
						 ADC1_CHANNEL_3,
						 ADC1_PRESSEL_FCPU_D18,
						 ADC1_EXTTRIG_TIM,
						 DISABLE,
						 ADC1_ALIGN_RIGHT,
						 ADC1_SCHMITTTRIG_CHANNEL3,
						 DISABLE);
						 
	ADC1_Cmd(ENABLE);
	
	ADC1_StartConversion();
	
	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);

	adc1_value = ADC1_GetConversionValue();
	
	ADC1_ClearFlag(ADC1_FLAG_EOC);
	
	return adc1_value;
}

uint32_t ADC2_Read(void)
{
	ADC1_DeInit();
	
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
						 ADC1_CHANNEL_4,
						 ADC1_PRESSEL_FCPU_D18,
						 ADC1_EXTTRIG_TIM,
						 DISABLE,
						 ADC1_ALIGN_RIGHT,
						 ADC1_SCHMITTTRIG_CHANNEL4,
						 DISABLE);
						 
	ADC1_Cmd(ENABLE);
	
	ADC1_StartConversion();
	
	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);

	adc2_value = ADC1_GetConversionValue();
	
	ADC1_ClearFlag(ADC1_FLAG_EOC);
	
	return adc2_value;
}

uint16_t readAverageADC1(uint8_t samples) {
    
	for (i = 0; i < samples; i++) {
			sum += ADC1_Read();
			Delay_us(200);  // small delay between samples
	}
	return sum / samples;
}

uint16_t readAverageADC2(uint8_t samples) {
    
	for (i = 0; i < samples; i++) {
			sum += ADC2_Read();
			Delay_us(200);  // small delay between samples
	}
	return sum / samples;
}