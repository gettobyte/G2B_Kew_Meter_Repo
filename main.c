/* MAIN.C file
 * Author : Rohan(Gettobytee)
 * Copyright (c) 2002-2005 STMicroelectronics
 */

#include "stm8s.h"

volatile uint8_t currentDigit = 0;
uint8_t digits[3] = {0, 0, 0};
static uint16_t counter;
uint8_t pattern;
int i;

// Segment Pins
#define PortA GPIOA
#define PortC GPIOC
#define PinA GPIO_PIN_1
#define PinB GPIO_PIN_2
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
#define PORTA_GPIO_PINS	 (PinA | PinB | GPIO_PIN_3)

void Delay_us(uint16_t nCount);
void clearDigits(void);
void displayDigit(uint8_t digit, uint8_t value);
void GPIO_Int(void);
void CLK_Init(void);
void Timer4_Init(void);

main()
{
	CLK_Init();
	GPIO_Int();
	Timer4_Init();  // Start Timer4 for display refresh
	counter = 0;  // Start from 0
	
	while (1)
	{
		// Convert counter to individual digits
		digits[0] = ((counter / 100) % 10);  // Hundreds
		digits[1] = (counter / 10) % 10;   // Tens
		digits[2] = counter % 10;          // Ones
	
		//digits[0] = 1;
		//digits[1] = 2;
		//digits[2] = 3;
		// Increment counter
		Delay_us(50000); // Add delay before increasing counter
		counter++;
		if (counter > 999) counter = 0;
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
	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)PORTD_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)PORTC_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)PORTA_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
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

