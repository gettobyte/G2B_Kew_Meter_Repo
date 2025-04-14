   1                     ; C Compiler for STM8 (COSMIC Software)
   2                     ; Parser V4.13.2 - 04 Jun 2024
   3                     ; Generator (Limited) V4.6.4 - 15 Jan 2025
  14                     	bsct
  15  0000               _currentDigit:
  16  0000 00            	dc.b	0
  17  0001               _digits:
  18  0001 00            	dc.b	0
  19  0002 00            	dc.b	0
  20  0003 00            	dc.b	0
  21  0004 00            	dc.b	0
  22  0005 00            	dc.b	0
  23  0006 00            	dc.b	0
  24  0007               _sum:
  25  0007 00000000      	dc.l	0
  26                     .const:	section	.text
  27  0000               _Segment_Patterns:
  28  0000 81            	dc.b	129
  29  0001 cf            	dc.b	207
  30  0002 92            	dc.b	146
  31  0003 86            	dc.b	134
  32  0004 cc            	dc.b	204
  33  0005 a4            	dc.b	164
  34  0006 a0            	dc.b	160
  35  0007 8f            	dc.b	143
  36  0008 80            	dc.b	128
  37  0009 84            	dc.b	132
  78                     	switch	.const
  79  000a               L6:
  80  000a 0000008c      	dc.l	140
  81  000e               L01:
  82  000e 000003e8      	dc.l	1000
  83  0012               L21:
  84  0012 000000aa      	dc.l	170
  85  0016               L41:
  86  0016 00000064      	dc.l	100
  87  001a               L61:
  88  001a 0000000a      	dc.l	10
  89                     ; 72 main()
  89                     ; 73 {
  90                     	scross	off
  91                     	switch	.text
  92  0000               _main:
  96                     ; 74 	CLK_Init();
  98  0000 cd015a        	call	_CLK_Init
 100                     ; 75 	GPIO_Int();
 102  0003 cd00ff        	call	_GPIO_Int
 104                     ; 76 	Timer4_Init();  // Start Timer4 for display refresh
 106  0006 cd01bc        	call	_Timer4_Init
 108  0009               L32:
 109                     ; 80 		voltage = readAverageADC1(64);
 111  0009 a640          	ld	a,#64
 112  000b cd028f        	call	_readAverageADC1
 114  000e cd0000        	call	c_uitolx
 116  0011 ae000c        	ldw	x,#_voltage
 117  0014 cd0000        	call	c_rtol
 119                     ; 81 		millivolt1 = (voltage * 230L) / 140;
 121  0017 ae000c        	ldw	x,#_voltage
 122  001a cd0000        	call	c_ltor
 124  001d a6e6          	ld	a,#230
 125  001f cd0000        	call	c_smul
 127  0022 ae000a        	ldw	x,#L6
 128  0025 cd0000        	call	c_ludv
 130  0028 ae0016        	ldw	x,#_millivolt1
 131  002b cd0000        	call	c_rtol
 133                     ; 82 		sum = 0;
 135  002e ae0000        	ldw	x,#0
 136  0031 bf09          	ldw	_sum+2,x
 137  0033 ae0000        	ldw	x,#0
 138  0036 bf07          	ldw	_sum,x
 139                     ; 83 		current = readAverageADC2(32);
 141  0038 a620          	ld	a,#32
 142  003a cd02d0        	call	_readAverageADC2
 144  003d cd0000        	call	c_uitolx
 146  0040 ae0008        	ldw	x,#_current
 147  0043 cd0000        	call	c_rtol
 149                     ; 84 		millivolt2 = (current * 1000) / 170;
 151  0046 ae0008        	ldw	x,#_current
 152  0049 cd0000        	call	c_ltor
 154  004c ae000e        	ldw	x,#L01
 155  004f cd0000        	call	c_lmul
 157  0052 ae0012        	ldw	x,#L21
 158  0055 cd0000        	call	c_ludv
 160  0058 ae0012        	ldw	x,#_millivolt2
 161  005b cd0000        	call	c_rtol
 163                     ; 85 		sum = 0;
 165  005e ae0000        	ldw	x,#0
 166  0061 bf09          	ldw	_sum+2,x
 167  0063 ae0000        	ldw	x,#0
 168  0066 bf07          	ldw	_sum,x
 169                     ; 87 		digits[0] = ((millivolt1 / 100) % 10);  // Voltage hundreds
 171  0068 ae0016        	ldw	x,#_millivolt1
 172  006b cd0000        	call	c_ltor
 174  006e ae0016        	ldw	x,#L41
 175  0071 cd0000        	call	c_ludv
 177  0074 ae001a        	ldw	x,#L61
 178  0077 cd0000        	call	c_lumd
 180  007a b603          	ld	a,c_lreg+3
 181  007c b701          	ld	_digits,a
 182                     ; 88 		digits[1] = ((millivolt1 / 10) % 10);   // Voltage tens
 184  007e ae0016        	ldw	x,#_millivolt1
 185  0081 cd0000        	call	c_ltor
 187  0084 ae001a        	ldw	x,#L61
 188  0087 cd0000        	call	c_ludv
 190  008a ae001a        	ldw	x,#L61
 191  008d cd0000        	call	c_lumd
 193  0090 b603          	ld	a,c_lreg+3
 194  0092 b702          	ld	_digits+1,a
 195                     ; 89 		digits[2] = (millivolt1 % 10);          // Voltage ones
 197  0094 ae0016        	ldw	x,#_millivolt1
 198  0097 cd0000        	call	c_ltor
 200  009a ae001a        	ldw	x,#L61
 201  009d cd0000        	call	c_lumd
 203  00a0 b603          	ld	a,c_lreg+3
 204  00a2 b703          	ld	_digits+2,a
 205                     ; 91 		digits[3] = ((millivolt2 / 100) % 10);  // Current hundreds
 207  00a4 ae0012        	ldw	x,#_millivolt2
 208  00a7 cd0000        	call	c_ltor
 210  00aa ae0016        	ldw	x,#L41
 211  00ad cd0000        	call	c_ludv
 213  00b0 ae001a        	ldw	x,#L61
 214  00b3 cd0000        	call	c_lumd
 216  00b6 b603          	ld	a,c_lreg+3
 217  00b8 b704          	ld	_digits+3,a
 218                     ; 92 		digits[4] = ((millivolt2 / 10) % 10);   // Current tens
 220  00ba ae0012        	ldw	x,#_millivolt2
 221  00bd cd0000        	call	c_ltor
 223  00c0 ae001a        	ldw	x,#L61
 224  00c3 cd0000        	call	c_ludv
 226  00c6 ae001a        	ldw	x,#L61
 227  00c9 cd0000        	call	c_lumd
 229  00cc b603          	ld	a,c_lreg+3
 230  00ce b705          	ld	_digits+4,a
 231                     ; 93 		digits[5] = (millivolt2 % 10);          // Current ones
 233  00d0 ae0012        	ldw	x,#_millivolt2
 234  00d3 cd0000        	call	c_ltor
 236  00d6 ae001a        	ldw	x,#L61
 237  00d9 cd0000        	call	c_lumd
 239  00dc b603          	ld	a,c_lreg+3
 240  00de b706          	ld	_digits+5,a
 241                     ; 96 		Delay_us(100000000);
 243  00e0 aee100        	ldw	x,#57600
 244  00e3 ad04          	call	_Delay_us
 247  00e5 ac090009      	jpf	L32
 281                     ; 101 void Delay_us(uint16_t nCount)
 281                     ; 102 {
 282                     	switch	.text
 283  00e9               _Delay_us:
 285  00e9 89            	pushw	x
 286       00000000      OFST:	set	0
 289  00ea 2002          	jra	L74
 290  00ec               L54:
 291                     ; 104         _asm("nop"); _asm("nop");
 294  00ec 9d            nop
 299  00ed 9d            nop
 301  00ee               L74:
 302                     ; 103     while (nCount--) {
 304  00ee 1e01          	ldw	x,(OFST+1,sp)
 305  00f0 1d0001        	subw	x,#1
 306  00f3 1f01          	ldw	(OFST+1,sp),x
 307  00f5 1c0001        	addw	x,#1
 308  00f8 a30000        	cpw	x,#0
 309  00fb 26ef          	jrne	L54
 310                     ; 106 }
 313  00fd 85            	popw	x
 314  00fe 81            	ret
 339                     ; 109 void GPIO_Int(void) {
 340                     	switch	.text
 341  00ff               _GPIO_Int:
 345                     ; 110 	GPIO_DeInit(GPIOD);
 347  00ff ae500f        	ldw	x,#20495
 348  0102 cd0000        	call	_GPIO_DeInit
 350                     ; 111 	GPIO_DeInit(GPIOC);
 352  0105 ae500a        	ldw	x,#20490
 353  0108 cd0000        	call	_GPIO_DeInit
 355                     ; 112 	GPIO_DeInit(GPIOA);
 357  010b ae5000        	ldw	x,#20480
 358  010e cd0000        	call	_GPIO_DeInit
 360                     ; 113 	GPIO_DeInit(GPIOB);
 362  0111 ae5005        	ldw	x,#20485
 363  0114 cd0000        	call	_GPIO_DeInit
 365                     ; 114 	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)PORTD_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 367  0117 4bf0          	push	#240
 368  0119 4b70          	push	#112
 369  011b ae500f        	ldw	x,#20495
 370  011e cd0000        	call	_GPIO_Init
 372  0121 85            	popw	x
 373                     ; 115 	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);
 375  0122 4b00          	push	#0
 376  0124 4b04          	push	#4
 377  0126 ae500f        	ldw	x,#20495
 378  0129 cd0000        	call	_GPIO_Init
 380  012c 85            	popw	x
 381                     ; 116 	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);
 383  012d 4b00          	push	#0
 384  012f 4b08          	push	#8
 385  0131 ae500f        	ldw	x,#20495
 386  0134 cd0000        	call	_GPIO_Init
 388  0137 85            	popw	x
 389                     ; 117 	GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)PORTC_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 391  0138 4bf0          	push	#240
 392  013a 4bf8          	push	#248
 393  013c ae500a        	ldw	x,#20490
 394  013f cd0000        	call	_GPIO_Init
 396  0142 85            	popw	x
 397                     ; 118 	GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)PORTA_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 399  0143 4bf0          	push	#240
 400  0145 4b0e          	push	#14
 401  0147 ae5000        	ldw	x,#20480
 402  014a cd0000        	call	_GPIO_Init
 404  014d 85            	popw	x
 405                     ; 119 	GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)PORTB_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 407  014e 4bf0          	push	#240
 408  0150 4b30          	push	#48
 409  0152 ae5005        	ldw	x,#20485
 410  0155 cd0000        	call	_GPIO_Init
 412  0158 85            	popw	x
 413                     ; 120 }
 416  0159 81            	ret
 449                     ; 123 void CLK_Init(void) {
 450                     	switch	.text
 451  015a               _CLK_Init:
 455                     ; 124 	CLK_DeInit();
 457  015a cd0000        	call	_CLK_DeInit
 459                     ; 126 	CLK_HSECmd(DISABLE);
 461  015d 4f            	clr	a
 462  015e cd0000        	call	_CLK_HSECmd
 464                     ; 127 	CLK_LSICmd(DISABLE);
 466  0161 4f            	clr	a
 467  0162 cd0000        	call	_CLK_LSICmd
 469                     ; 128 	CLK_HSICmd(ENABLE);
 471  0165 a601          	ld	a,#1
 472  0167 cd0000        	call	_CLK_HSICmd
 475  016a               L57:
 476                     ; 129 	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
 478  016a ae0102        	ldw	x,#258
 479  016d cd0000        	call	_CLK_GetFlagStatus
 481  0170 4d            	tnz	a
 482  0171 27f7          	jreq	L57
 483                     ; 131 	CLK_ClockSwitchCmd(ENABLE);
 485  0173 a601          	ld	a,#1
 486  0175 cd0000        	call	_CLK_ClockSwitchCmd
 488                     ; 132 	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV4);
 490  0178 a610          	ld	a,#16
 491  017a cd0000        	call	_CLK_HSIPrescalerConfig
 493                     ; 133 	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
 495  017d a680          	ld	a,#128
 496  017f cd0000        	call	_CLK_SYSCLKConfig
 498                     ; 135 	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
 498                     ; 136 	DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
 500  0182 4b01          	push	#1
 501  0184 4b00          	push	#0
 502  0186 ae01e1        	ldw	x,#481
 503  0189 cd0000        	call	_CLK_ClockSwitchConfig
 505  018c 85            	popw	x
 506                     ; 138 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
 508  018d ae0100        	ldw	x,#256
 509  0190 cd0000        	call	_CLK_PeripheralClockConfig
 511                     ; 139 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
 513  0193 5f            	clrw	x
 514  0194 cd0000        	call	_CLK_PeripheralClockConfig
 516                     ; 140 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
 518  0197 ae1301        	ldw	x,#4865
 519  019a cd0000        	call	_CLK_PeripheralClockConfig
 521                     ; 141 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
 523  019d ae1200        	ldw	x,#4608
 524  01a0 cd0000        	call	_CLK_PeripheralClockConfig
 526                     ; 142 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
 528  01a3 ae0300        	ldw	x,#768
 529  01a6 cd0000        	call	_CLK_PeripheralClockConfig
 531                     ; 143 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
 533  01a9 ae0700        	ldw	x,#1792
 534  01ac cd0000        	call	_CLK_PeripheralClockConfig
 536                     ; 144 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
 538  01af ae0500        	ldw	x,#1280
 539  01b2 cd0000        	call	_CLK_PeripheralClockConfig
 541                     ; 145 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);;
 543  01b5 ae0401        	ldw	x,#1025
 544  01b8 cd0000        	call	_CLK_PeripheralClockConfig
 546                     ; 146 }
 550  01bb 81            	ret
 578                     ; 149 void Timer4_Init(void) {
 579                     	switch	.text
 580  01bc               _Timer4_Init:
 584                     ; 150 	TIM4_DeInit();
 586  01bc cd0000        	call	_TIM4_DeInit
 588                     ; 151 	TIM4_TimeBaseInit(TIM4_PRESCALER_64, 156);      
 590  01bf ae069c        	ldw	x,#1692
 591  01c2 cd0000        	call	_TIM4_TimeBaseInit
 593                     ; 152 	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
 595  01c5 ae0101        	ldw	x,#257
 596  01c8 cd0000        	call	_TIM4_ITConfig
 598                     ; 153 	TIM4_Cmd(ENABLE);
 600  01cb a601          	ld	a,#1
 601  01cd cd0000        	call	_TIM4_Cmd
 603                     ; 155 	enableInterrupts();
 606  01d0 9a            rim
 608                     ; 156 }
 612  01d1 81            	ret
 640                     ; 158 void ADC1_setup(void)
 640                     ; 159 {
 641                     	switch	.text
 642  01d2               _ADC1_setup:
 646                     ; 160   ADC1_DeInit();         
 648  01d2 cd0000        	call	_ADC1_DeInit
 650                     ; 162 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
 650                     ; 163 						ADC1_CHANNEL_3,
 650                     ; 164 						ADC1_PRESSEL_FCPU_D18, 
 650                     ; 165 						ADC1_EXTTRIG_TIM, 
 650                     ; 166 						DISABLE, 
 650                     ; 167 						ADC1_ALIGN_RIGHT, 
 650                     ; 168 						ADC1_SCHMITTTRIG_CHANNEL3, 
 650                     ; 169 						DISABLE);
 652  01d5 4b00          	push	#0
 653  01d7 4b03          	push	#3
 654  01d9 4b08          	push	#8
 655  01db 4b00          	push	#0
 656  01dd 4b00          	push	#0
 657  01df 4b70          	push	#112
 658  01e1 ae0103        	ldw	x,#259
 659  01e4 cd0000        	call	_ADC1_Init
 661  01e7 5b06          	addw	sp,#6
 662                     ; 171 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
 662                     ; 172 						ADC1_CHANNEL_4,
 662                     ; 173 						ADC1_PRESSEL_FCPU_D18, 
 662                     ; 174 						ADC1_EXTTRIG_TIM, 
 662                     ; 175 						DISABLE, 
 662                     ; 176 						ADC1_ALIGN_RIGHT, 
 662                     ; 177 						ADC1_SCHMITTTRIG_CHANNEL4, 
 662                     ; 178 						DISABLE);
 664  01e9 4b00          	push	#0
 665  01eb 4b04          	push	#4
 666  01ed 4b08          	push	#8
 667  01ef 4b00          	push	#0
 668  01f1 4b00          	push	#0
 669  01f3 4b70          	push	#112
 670  01f5 ae0104        	ldw	x,#260
 671  01f8 cd0000        	call	_ADC1_Init
 673  01fb 5b06          	addw	sp,#6
 674                     ; 180 	ADC1_ConversionConfig(ADC1_CONVERSIONMODE_CONTINUOUS,
 674                     ; 181 												((ADC1_Channel_TypeDef)(ADC1_CHANNEL_3 | ADC1_CHANNEL_4)),
 674                     ; 182 												ADC1_ALIGN_RIGHT);
 676  01fd 4b08          	push	#8
 677  01ff ae0107        	ldw	x,#263
 678  0202 cd0000        	call	_ADC1_ConversionConfig
 680  0205 84            	pop	a
 681                     ; 184 	ADC1_DataBufferCmd(ENABLE);
 683  0206 a601          	ld	a,#1
 684  0208 cd0000        	call	_ADC1_DataBufferCmd
 686                     ; 186 	ADC1_Cmd(ENABLE);
 688  020b a601          	ld	a,#1
 689  020d cd0000        	call	_ADC1_Cmd
 691                     ; 187 }
 694  0210 81            	ret
 725                     ; 189 uint32_t ADC1_Read(void)
 725                     ; 190 {
 726                     	switch	.text
 727  0211               _ADC1_Read:
 731                     ; 191 	ADC1_DeInit();
 733  0211 cd0000        	call	_ADC1_DeInit
 735                     ; 193 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
 735                     ; 194 						 ADC1_CHANNEL_3,
 735                     ; 195 						 ADC1_PRESSEL_FCPU_D18,
 735                     ; 196 						 ADC1_EXTTRIG_TIM,
 735                     ; 197 						 DISABLE,
 735                     ; 198 						 ADC1_ALIGN_RIGHT,
 735                     ; 199 						 ADC1_SCHMITTTRIG_CHANNEL3,
 735                     ; 200 						 DISABLE);
 737  0214 4b00          	push	#0
 738  0216 4b03          	push	#3
 739  0218 4b08          	push	#8
 740  021a 4b00          	push	#0
 741  021c 4b00          	push	#0
 742  021e 4b70          	push	#112
 743  0220 ae0103        	ldw	x,#259
 744  0223 cd0000        	call	_ADC1_Init
 746  0226 5b06          	addw	sp,#6
 747                     ; 202 	ADC1_Cmd(ENABLE);
 749  0228 a601          	ld	a,#1
 750  022a cd0000        	call	_ADC1_Cmd
 752                     ; 204 	ADC1_StartConversion();
 754  022d cd0000        	call	_ADC1_StartConversion
 757  0230               L331:
 758                     ; 206 	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
 760  0230 a680          	ld	a,#128
 761  0232 cd0000        	call	_ADC1_GetFlagStatus
 763  0235 4d            	tnz	a
 764  0236 27f8          	jreq	L331
 765                     ; 208 	adc1_value = ADC1_GetConversionValue();
 767  0238 cd0000        	call	_ADC1_GetConversionValue
 769  023b cd0000        	call	c_uitolx
 771  023e ae0004        	ldw	x,#_adc1_value
 772  0241 cd0000        	call	c_rtol
 774                     ; 210 	ADC1_ClearFlag(ADC1_FLAG_EOC);
 776  0244 a680          	ld	a,#128
 777  0246 cd0000        	call	_ADC1_ClearFlag
 779                     ; 212 	return adc1_value;
 781  0249 ae0004        	ldw	x,#_adc1_value
 782  024c cd0000        	call	c_ltor
 786  024f 81            	ret
 817                     ; 215 uint32_t ADC2_Read(void)
 817                     ; 216 {
 818                     	switch	.text
 819  0250               _ADC2_Read:
 823                     ; 217 	ADC1_DeInit();
 825  0250 cd0000        	call	_ADC1_DeInit
 827                     ; 219 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
 827                     ; 220 						 ADC1_CHANNEL_4,
 827                     ; 221 						 ADC1_PRESSEL_FCPU_D18,
 827                     ; 222 						 ADC1_EXTTRIG_TIM,
 827                     ; 223 						 DISABLE,
 827                     ; 224 						 ADC1_ALIGN_RIGHT,
 827                     ; 225 						 ADC1_SCHMITTTRIG_CHANNEL4,
 827                     ; 226 						 DISABLE);
 829  0253 4b00          	push	#0
 830  0255 4b04          	push	#4
 831  0257 4b08          	push	#8
 832  0259 4b00          	push	#0
 833  025b 4b00          	push	#0
 834  025d 4b70          	push	#112
 835  025f ae0104        	ldw	x,#260
 836  0262 cd0000        	call	_ADC1_Init
 838  0265 5b06          	addw	sp,#6
 839                     ; 228 	ADC1_Cmd(ENABLE);
 841  0267 a601          	ld	a,#1
 842  0269 cd0000        	call	_ADC1_Cmd
 844                     ; 230 	ADC1_StartConversion();
 846  026c cd0000        	call	_ADC1_StartConversion
 849  026f               L151:
 850                     ; 232 	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
 852  026f a680          	ld	a,#128
 853  0271 cd0000        	call	_ADC1_GetFlagStatus
 855  0274 4d            	tnz	a
 856  0275 27f8          	jreq	L151
 857                     ; 234 	adc2_value = ADC1_GetConversionValue();
 859  0277 cd0000        	call	_ADC1_GetConversionValue
 861  027a cd0000        	call	c_uitolx
 863  027d ae0000        	ldw	x,#_adc2_value
 864  0280 cd0000        	call	c_rtol
 866                     ; 236 	ADC1_ClearFlag(ADC1_FLAG_EOC);
 868  0283 a680          	ld	a,#128
 869  0285 cd0000        	call	_ADC1_ClearFlag
 871                     ; 238 	return adc2_value;
 873  0288 ae0000        	ldw	x,#_adc2_value
 874  028b cd0000        	call	c_ltor
 878  028e 81            	ret
 916                     ; 241 uint16_t readAverageADC1(uint8_t samples) {
 917                     	switch	.text
 918  028f               _readAverageADC1:
 920  028f 88            	push	a
 921  0290 5204          	subw	sp,#4
 922       00000004      OFST:	set	4
 925                     ; 243 	for (i = 0; i < samples; i++) {
 927  0292 3f11          	clr	_i
 929  0294 2011          	jra	L771
 930  0296               L371:
 931                     ; 244 			sum += ADC1_Read();
 933  0296 cd0211        	call	_ADC1_Read
 935  0299 ae0007        	ldw	x,#_sum
 936  029c cd0000        	call	c_lgadd
 938                     ; 245 			Delay_us(200);  // small delay between samples
 940  029f ae00c8        	ldw	x,#200
 941  02a2 cd00e9        	call	_Delay_us
 943                     ; 243 	for (i = 0; i < samples; i++) {
 945  02a5 3c11          	inc	_i
 946  02a7               L771:
 949  02a7 b611          	ld	a,_i
 950  02a9 1105          	cp	a,(OFST+1,sp)
 951  02ab 25e9          	jrult	L371
 952                     ; 247 	return sum / samples;
 954  02ad 7b05          	ld	a,(OFST+1,sp)
 955  02af b703          	ld	c_lreg+3,a
 956  02b1 3f02          	clr	c_lreg+2
 957  02b3 3f01          	clr	c_lreg+1
 958  02b5 3f00          	clr	c_lreg
 959  02b7 96            	ldw	x,sp
 960  02b8 1c0001        	addw	x,#OFST-3
 961  02bb cd0000        	call	c_rtol
 964  02be ae0007        	ldw	x,#_sum
 965  02c1 cd0000        	call	c_ltor
 967  02c4 96            	ldw	x,sp
 968  02c5 1c0001        	addw	x,#OFST-3
 969  02c8 cd0000        	call	c_ludv
 971  02cb be02          	ldw	x,c_lreg+2
 974  02cd 5b05          	addw	sp,#5
 975  02cf 81            	ret
1013                     ; 250 uint16_t readAverageADC2(uint8_t samples) {
1014                     	switch	.text
1015  02d0               _readAverageADC2:
1017  02d0 88            	push	a
1018  02d1 5204          	subw	sp,#4
1019       00000004      OFST:	set	4
1022                     ; 252 	for (i = 0; i < samples; i++) {
1024  02d3 3f11          	clr	_i
1026  02d5 2011          	jra	L522
1027  02d7               L122:
1028                     ; 253 			sum += ADC2_Read();
1030  02d7 cd0250        	call	_ADC2_Read
1032  02da ae0007        	ldw	x,#_sum
1033  02dd cd0000        	call	c_lgadd
1035                     ; 254 			Delay_us(200);  // small delay between samples
1037  02e0 ae00c8        	ldw	x,#200
1038  02e3 cd00e9        	call	_Delay_us
1040                     ; 252 	for (i = 0; i < samples; i++) {
1042  02e6 3c11          	inc	_i
1043  02e8               L522:
1046  02e8 b611          	ld	a,_i
1047  02ea 1105          	cp	a,(OFST+1,sp)
1048  02ec 25e9          	jrult	L122
1049                     ; 256 	return sum / samples;
1051  02ee 7b05          	ld	a,(OFST+1,sp)
1052  02f0 b703          	ld	c_lreg+3,a
1053  02f2 3f02          	clr	c_lreg+2
1054  02f4 3f01          	clr	c_lreg+1
1055  02f6 3f00          	clr	c_lreg
1056  02f8 96            	ldw	x,sp
1057  02f9 1c0001        	addw	x,#OFST-3
1058  02fc cd0000        	call	c_rtol
1061  02ff ae0007        	ldw	x,#_sum
1062  0302 cd0000        	call	c_ltor
1064  0305 96            	ldw	x,sp
1065  0306 1c0001        	addw	x,#OFST-3
1066  0309 cd0000        	call	c_ludv
1068  030c be02          	ldw	x,c_lreg+2
1071  030e 5b05          	addw	sp,#5
1072  0310 81            	ret
1206                     	xdef	_main
1207                     	xdef	_readAverageADC2
1208                     	xdef	_readAverageADC1
1209                     	xdef	_ADC2_Read
1210                     	xdef	_ADC1_Read
1211                     	xdef	_ADC1_setup
1212                     	xdef	_Timer4_Init
1213                     	xdef	_CLK_Init
1214                     	xdef	_GPIO_Int
1215                     	xdef	_Delay_us
1216                     	xdef	_Segment_Patterns
1217                     	switch	.ubsct
1218  0000               _adc2_value:
1219  0000 00000000      	ds.b	4
1220                     	xdef	_adc2_value
1221  0004               _adc1_value:
1222  0004 00000000      	ds.b	4
1223                     	xdef	_adc1_value
1224  0008               _current:
1225  0008 00000000      	ds.b	4
1226                     	xdef	_current
1227  000c               _voltage:
1228  000c 00000000      	ds.b	4
1229                     	xdef	_voltage
1230  0010               _samples:
1231  0010 00            	ds.b	1
1232                     	xdef	_samples
1233  0011               _i:
1234  0011 00            	ds.b	1
1235                     	xdef	_i
1236                     	xdef	_sum
1237  0012               _millivolt2:
1238  0012 00000000      	ds.b	4
1239                     	xdef	_millivolt2
1240  0016               _millivolt1:
1241  0016 00000000      	ds.b	4
1242                     	xdef	_millivolt1
1243  001a               _pattern:
1244  001a 00            	ds.b	1
1245                     	xdef	_pattern
1246                     	xdef	_digits
1247                     	xdef	_currentDigit
1248                     	xref	_TIM4_ITConfig
1249                     	xref	_TIM4_Cmd
1250                     	xref	_TIM4_TimeBaseInit
1251                     	xref	_TIM4_DeInit
1252                     	xref	_GPIO_Init
1253                     	xref	_GPIO_DeInit
1254                     	xref	_CLK_GetFlagStatus
1255                     	xref	_CLK_SYSCLKConfig
1256                     	xref	_CLK_HSIPrescalerConfig
1257                     	xref	_CLK_ClockSwitchConfig
1258                     	xref	_CLK_PeripheralClockConfig
1259                     	xref	_CLK_ClockSwitchCmd
1260                     	xref	_CLK_LSICmd
1261                     	xref	_CLK_HSICmd
1262                     	xref	_CLK_HSECmd
1263                     	xref	_CLK_DeInit
1264                     	xref	_ADC1_ClearFlag
1265                     	xref	_ADC1_GetFlagStatus
1266                     	xref	_ADC1_GetConversionValue
1267                     	xref	_ADC1_StartConversion
1268                     	xref	_ADC1_ConversionConfig
1269                     	xref	_ADC1_DataBufferCmd
1270                     	xref	_ADC1_Cmd
1271                     	xref	_ADC1_Init
1272                     	xref	_ADC1_DeInit
1273                     	xref.b	c_lreg
1274                     	xref.b	c_x
1294                     	xref	c_lgadd
1295                     	xref	c_lumd
1296                     	xref	c_lmul
1297                     	xref	c_ludv
1298                     	xref	c_smul
1299                     	xref	c_ltor
1300                     	xref	c_rtol
1301                     	xref	c_uitolx
1302                     	end
