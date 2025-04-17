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
  77                     	switch	.const
  78  000a               L6:
  79  000a 0000008c      	dc.l	140
  80  000e               L01:
  81  000e 00000064      	dc.l	100
  82  0012               L21:
  83  0012 0000000a      	dc.l	10
  84                     ; 72 main()
  84                     ; 73 {
  85                     	scross	off
  86                     	switch	.text
  87  0000               _main:
  91                     ; 74 	CLK_Init();
  93  0000 cd0142        	call	_CLK_Init
  95                     ; 75 	GPIO_Int();
  97  0003 cd00e7        	call	_GPIO_Int
  99                     ; 76 	Timer4_Init();  // Start Timer4 for display refresh
 101  0006 cd01a4        	call	_Timer4_Init
 103  0009               L32:
 104                     ; 80 		voltage = readAverageADC1(64);
 106  0009 a640          	ld	a,#64
 107  000b cd0277        	call	_readAverageADC1
 109  000e cd0000        	call	c_uitolx
 111  0011 ae000c        	ldw	x,#_voltage
 112  0014 cd0000        	call	c_rtol
 114                     ; 81 		millivolt1 = (voltage * 230L) / 140;
 116  0017 ae000c        	ldw	x,#_voltage
 117  001a cd0000        	call	c_ltor
 119  001d a6e6          	ld	a,#230
 120  001f cd0000        	call	c_smul
 122  0022 ae000a        	ldw	x,#L6
 123  0025 cd0000        	call	c_ludv
 125  0028 ae0016        	ldw	x,#_millivolt1
 126  002b cd0000        	call	c_rtol
 128                     ; 82 		sum = 0;
 130  002e ae0000        	ldw	x,#0
 131  0031 bf09          	ldw	_sum+2,x
 132  0033 ae0000        	ldw	x,#0
 133  0036 bf07          	ldw	_sum,x
 134                     ; 83 		current = readAverageADC2(64);
 136  0038 a640          	ld	a,#64
 137  003a cd02b8        	call	_readAverageADC2
 139  003d cd0000        	call	c_uitolx
 141  0040 ae0008        	ldw	x,#_current
 142  0043 cd0000        	call	c_rtol
 144                     ; 85 		sum = 0;
 146  0046 ae0000        	ldw	x,#0
 147  0049 bf09          	ldw	_sum+2,x
 148  004b ae0000        	ldw	x,#0
 149  004e bf07          	ldw	_sum,x
 150                     ; 87 		digits[0] = ((millivolt1 / 100) % 10);  // Voltage hundreds
 152  0050 ae0016        	ldw	x,#_millivolt1
 153  0053 cd0000        	call	c_ltor
 155  0056 ae000e        	ldw	x,#L01
 156  0059 cd0000        	call	c_ludv
 158  005c ae0012        	ldw	x,#L21
 159  005f cd0000        	call	c_lumd
 161  0062 b603          	ld	a,c_lreg+3
 162  0064 b701          	ld	_digits,a
 163                     ; 88 		digits[1] = ((millivolt1 / 10) % 10);   // Voltage tens
 165  0066 ae0016        	ldw	x,#_millivolt1
 166  0069 cd0000        	call	c_ltor
 168  006c ae0012        	ldw	x,#L21
 169  006f cd0000        	call	c_ludv
 171  0072 ae0012        	ldw	x,#L21
 172  0075 cd0000        	call	c_lumd
 174  0078 b603          	ld	a,c_lreg+3
 175  007a b702          	ld	_digits+1,a
 176                     ; 89 		digits[2] = (millivolt1 % 10);          // Voltage ones
 178  007c ae0016        	ldw	x,#_millivolt1
 179  007f cd0000        	call	c_ltor
 181  0082 ae0012        	ldw	x,#L21
 182  0085 cd0000        	call	c_lumd
 184  0088 b603          	ld	a,c_lreg+3
 185  008a b703          	ld	_digits+2,a
 186                     ; 91 		digits[3] = ((current / 100) % 10);  // Current hundreds
 188  008c ae0008        	ldw	x,#_current
 189  008f cd0000        	call	c_ltor
 191  0092 ae000e        	ldw	x,#L01
 192  0095 cd0000        	call	c_ludv
 194  0098 ae0012        	ldw	x,#L21
 195  009b cd0000        	call	c_lumd
 197  009e b603          	ld	a,c_lreg+3
 198  00a0 b704          	ld	_digits+3,a
 199                     ; 92 		digits[4] = ((current / 10) % 10);   // Current tens
 201  00a2 ae0008        	ldw	x,#_current
 202  00a5 cd0000        	call	c_ltor
 204  00a8 ae0012        	ldw	x,#L21
 205  00ab cd0000        	call	c_ludv
 207  00ae ae0012        	ldw	x,#L21
 208  00b1 cd0000        	call	c_lumd
 210  00b4 b603          	ld	a,c_lreg+3
 211  00b6 b705          	ld	_digits+4,a
 212                     ; 93 		digits[5] = (current % 10);          // Current ones
 214  00b8 ae0008        	ldw	x,#_current
 215  00bb cd0000        	call	c_ltor
 217  00be ae0012        	ldw	x,#L21
 218  00c1 cd0000        	call	c_lumd
 220  00c4 b603          	ld	a,c_lreg+3
 221  00c6 b706          	ld	_digits+5,a
 222                     ; 96 		Delay_us(100000000);
 224  00c8 aee100        	ldw	x,#57600
 225  00cb ad04          	call	_Delay_us
 228  00cd ac090009      	jpf	L32
 262                     ; 101 void Delay_us(uint16_t nCount)
 262                     ; 102 {
 263                     	switch	.text
 264  00d1               _Delay_us:
 266  00d1 89            	pushw	x
 267       00000000      OFST:	set	0
 270  00d2 2002          	jra	L74
 271  00d4               L54:
 272                     ; 104         _asm("nop"); _asm("nop");
 275  00d4 9d            nop
 280  00d5 9d            nop
 282  00d6               L74:
 283                     ; 103     while (nCount--) {
 285  00d6 1e01          	ldw	x,(OFST+1,sp)
 286  00d8 1d0001        	subw	x,#1
 287  00db 1f01          	ldw	(OFST+1,sp),x
 288  00dd 1c0001        	addw	x,#1
 289  00e0 a30000        	cpw	x,#0
 290  00e3 26ef          	jrne	L54
 291                     ; 106 }
 294  00e5 85            	popw	x
 295  00e6 81            	ret
 320                     ; 109 void GPIO_Int(void) {
 321                     	switch	.text
 322  00e7               _GPIO_Int:
 326                     ; 110 	GPIO_DeInit(GPIOD);
 328  00e7 ae500f        	ldw	x,#20495
 329  00ea cd0000        	call	_GPIO_DeInit
 331                     ; 111 	GPIO_DeInit(GPIOC);
 333  00ed ae500a        	ldw	x,#20490
 334  00f0 cd0000        	call	_GPIO_DeInit
 336                     ; 112 	GPIO_DeInit(GPIOA);
 338  00f3 ae5000        	ldw	x,#20480
 339  00f6 cd0000        	call	_GPIO_DeInit
 341                     ; 113 	GPIO_DeInit(GPIOB);
 343  00f9 ae5005        	ldw	x,#20485
 344  00fc cd0000        	call	_GPIO_DeInit
 346                     ; 114 	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)PORTD_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 348  00ff 4bf0          	push	#240
 349  0101 4b70          	push	#112
 350  0103 ae500f        	ldw	x,#20495
 351  0106 cd0000        	call	_GPIO_Init
 353  0109 85            	popw	x
 354                     ; 115 	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);
 356  010a 4b00          	push	#0
 357  010c 4b04          	push	#4
 358  010e ae500f        	ldw	x,#20495
 359  0111 cd0000        	call	_GPIO_Init
 361  0114 85            	popw	x
 362                     ; 116 	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);
 364  0115 4b00          	push	#0
 365  0117 4b08          	push	#8
 366  0119 ae500f        	ldw	x,#20495
 367  011c cd0000        	call	_GPIO_Init
 369  011f 85            	popw	x
 370                     ; 117 	GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)PORTC_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 372  0120 4bf0          	push	#240
 373  0122 4bf8          	push	#248
 374  0124 ae500a        	ldw	x,#20490
 375  0127 cd0000        	call	_GPIO_Init
 377  012a 85            	popw	x
 378                     ; 118 	GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)PORTA_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 380  012b 4bf0          	push	#240
 381  012d 4b0e          	push	#14
 382  012f ae5000        	ldw	x,#20480
 383  0132 cd0000        	call	_GPIO_Init
 385  0135 85            	popw	x
 386                     ; 119 	GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)PORTB_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 388  0136 4bf0          	push	#240
 389  0138 4b30          	push	#48
 390  013a ae5005        	ldw	x,#20485
 391  013d cd0000        	call	_GPIO_Init
 393  0140 85            	popw	x
 394                     ; 120 }
 397  0141 81            	ret
 430                     ; 123 void CLK_Init(void) {
 431                     	switch	.text
 432  0142               _CLK_Init:
 436                     ; 124 	CLK_DeInit();
 438  0142 cd0000        	call	_CLK_DeInit
 440                     ; 126 	CLK_HSECmd(DISABLE);
 442  0145 4f            	clr	a
 443  0146 cd0000        	call	_CLK_HSECmd
 445                     ; 127 	CLK_LSICmd(DISABLE);
 447  0149 4f            	clr	a
 448  014a cd0000        	call	_CLK_LSICmd
 450                     ; 128 	CLK_HSICmd(ENABLE);
 452  014d a601          	ld	a,#1
 453  014f cd0000        	call	_CLK_HSICmd
 456  0152               L57:
 457                     ; 129 	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
 459  0152 ae0102        	ldw	x,#258
 460  0155 cd0000        	call	_CLK_GetFlagStatus
 462  0158 4d            	tnz	a
 463  0159 27f7          	jreq	L57
 464                     ; 131 	CLK_ClockSwitchCmd(ENABLE);
 466  015b a601          	ld	a,#1
 467  015d cd0000        	call	_CLK_ClockSwitchCmd
 469                     ; 132 	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV4);
 471  0160 a610          	ld	a,#16
 472  0162 cd0000        	call	_CLK_HSIPrescalerConfig
 474                     ; 133 	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
 476  0165 a680          	ld	a,#128
 477  0167 cd0000        	call	_CLK_SYSCLKConfig
 479                     ; 135 	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
 479                     ; 136 	DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
 481  016a 4b01          	push	#1
 482  016c 4b00          	push	#0
 483  016e ae01e1        	ldw	x,#481
 484  0171 cd0000        	call	_CLK_ClockSwitchConfig
 486  0174 85            	popw	x
 487                     ; 138 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
 489  0175 ae0100        	ldw	x,#256
 490  0178 cd0000        	call	_CLK_PeripheralClockConfig
 492                     ; 139 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
 494  017b 5f            	clrw	x
 495  017c cd0000        	call	_CLK_PeripheralClockConfig
 497                     ; 140 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
 499  017f ae1301        	ldw	x,#4865
 500  0182 cd0000        	call	_CLK_PeripheralClockConfig
 502                     ; 141 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
 504  0185 ae1200        	ldw	x,#4608
 505  0188 cd0000        	call	_CLK_PeripheralClockConfig
 507                     ; 142 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
 509  018b ae0300        	ldw	x,#768
 510  018e cd0000        	call	_CLK_PeripheralClockConfig
 512                     ; 143 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
 514  0191 ae0700        	ldw	x,#1792
 515  0194 cd0000        	call	_CLK_PeripheralClockConfig
 517                     ; 144 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
 519  0197 ae0500        	ldw	x,#1280
 520  019a cd0000        	call	_CLK_PeripheralClockConfig
 522                     ; 145 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);;
 524  019d ae0401        	ldw	x,#1025
 525  01a0 cd0000        	call	_CLK_PeripheralClockConfig
 527                     ; 146 }
 531  01a3 81            	ret
 559                     ; 149 void Timer4_Init(void) {
 560                     	switch	.text
 561  01a4               _Timer4_Init:
 565                     ; 150 	TIM4_DeInit();
 567  01a4 cd0000        	call	_TIM4_DeInit
 569                     ; 151 	TIM4_TimeBaseInit(TIM4_PRESCALER_64, 156);      
 571  01a7 ae069c        	ldw	x,#1692
 572  01aa cd0000        	call	_TIM4_TimeBaseInit
 574                     ; 152 	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
 576  01ad ae0101        	ldw	x,#257
 577  01b0 cd0000        	call	_TIM4_ITConfig
 579                     ; 153 	TIM4_Cmd(ENABLE);
 581  01b3 a601          	ld	a,#1
 582  01b5 cd0000        	call	_TIM4_Cmd
 584                     ; 155 	enableInterrupts();
 587  01b8 9a            rim
 589                     ; 156 }
 593  01b9 81            	ret
 621                     ; 158 void ADC1_setup(void)
 621                     ; 159 {
 622                     	switch	.text
 623  01ba               _ADC1_setup:
 627                     ; 160   ADC1_DeInit();         
 629  01ba cd0000        	call	_ADC1_DeInit
 631                     ; 162 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
 631                     ; 163 						ADC1_CHANNEL_3,
 631                     ; 164 						ADC1_PRESSEL_FCPU_D18, 
 631                     ; 165 						ADC1_EXTTRIG_TIM, 
 631                     ; 166 						DISABLE, 
 631                     ; 167 						ADC1_ALIGN_RIGHT, 
 631                     ; 168 						ADC1_SCHMITTTRIG_CHANNEL3, 
 631                     ; 169 						DISABLE);
 633  01bd 4b00          	push	#0
 634  01bf 4b03          	push	#3
 635  01c1 4b08          	push	#8
 636  01c3 4b00          	push	#0
 637  01c5 4b00          	push	#0
 638  01c7 4b70          	push	#112
 639  01c9 ae0103        	ldw	x,#259
 640  01cc cd0000        	call	_ADC1_Init
 642  01cf 5b06          	addw	sp,#6
 643                     ; 171 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
 643                     ; 172 						ADC1_CHANNEL_4,
 643                     ; 173 						ADC1_PRESSEL_FCPU_D18, 
 643                     ; 174 						ADC1_EXTTRIG_TIM, 
 643                     ; 175 						DISABLE, 
 643                     ; 176 						ADC1_ALIGN_RIGHT, 
 643                     ; 177 						ADC1_SCHMITTTRIG_CHANNEL4, 
 643                     ; 178 						DISABLE);
 645  01d1 4b00          	push	#0
 646  01d3 4b04          	push	#4
 647  01d5 4b08          	push	#8
 648  01d7 4b00          	push	#0
 649  01d9 4b00          	push	#0
 650  01db 4b70          	push	#112
 651  01dd ae0104        	ldw	x,#260
 652  01e0 cd0000        	call	_ADC1_Init
 654  01e3 5b06          	addw	sp,#6
 655                     ; 180 	ADC1_ConversionConfig(ADC1_CONVERSIONMODE_CONTINUOUS,
 655                     ; 181 												((ADC1_Channel_TypeDef)(ADC1_CHANNEL_3 | ADC1_CHANNEL_4)),
 655                     ; 182 												ADC1_ALIGN_RIGHT);
 657  01e5 4b08          	push	#8
 658  01e7 ae0107        	ldw	x,#263
 659  01ea cd0000        	call	_ADC1_ConversionConfig
 661  01ed 84            	pop	a
 662                     ; 184 	ADC1_DataBufferCmd(ENABLE);
 664  01ee a601          	ld	a,#1
 665  01f0 cd0000        	call	_ADC1_DataBufferCmd
 667                     ; 186 	ADC1_Cmd(ENABLE);
 669  01f3 a601          	ld	a,#1
 670  01f5 cd0000        	call	_ADC1_Cmd
 672                     ; 187 }
 675  01f8 81            	ret
 706                     ; 189 uint32_t ADC1_Read(void)
 706                     ; 190 {
 707                     	switch	.text
 708  01f9               _ADC1_Read:
 712                     ; 191 	ADC1_DeInit();
 714  01f9 cd0000        	call	_ADC1_DeInit
 716                     ; 193 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
 716                     ; 194 						 ADC1_CHANNEL_3,
 716                     ; 195 						 ADC1_PRESSEL_FCPU_D18,
 716                     ; 196 						 ADC1_EXTTRIG_TIM,
 716                     ; 197 						 DISABLE,
 716                     ; 198 						 ADC1_ALIGN_RIGHT,
 716                     ; 199 						 ADC1_SCHMITTTRIG_CHANNEL3,
 716                     ; 200 						 DISABLE);
 718  01fc 4b00          	push	#0
 719  01fe 4b03          	push	#3
 720  0200 4b08          	push	#8
 721  0202 4b00          	push	#0
 722  0204 4b00          	push	#0
 723  0206 4b70          	push	#112
 724  0208 ae0103        	ldw	x,#259
 725  020b cd0000        	call	_ADC1_Init
 727  020e 5b06          	addw	sp,#6
 728                     ; 202 	ADC1_Cmd(ENABLE);
 730  0210 a601          	ld	a,#1
 731  0212 cd0000        	call	_ADC1_Cmd
 733                     ; 204 	ADC1_StartConversion();
 735  0215 cd0000        	call	_ADC1_StartConversion
 738  0218               L331:
 739                     ; 206 	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
 741  0218 a680          	ld	a,#128
 742  021a cd0000        	call	_ADC1_GetFlagStatus
 744  021d 4d            	tnz	a
 745  021e 27f8          	jreq	L331
 746                     ; 208 	adc1_value = ADC1_GetConversionValue();
 748  0220 cd0000        	call	_ADC1_GetConversionValue
 750  0223 cd0000        	call	c_uitolx
 752  0226 ae0004        	ldw	x,#_adc1_value
 753  0229 cd0000        	call	c_rtol
 755                     ; 210 	ADC1_ClearFlag(ADC1_FLAG_EOC);
 757  022c a680          	ld	a,#128
 758  022e cd0000        	call	_ADC1_ClearFlag
 760                     ; 212 	return adc1_value;
 762  0231 ae0004        	ldw	x,#_adc1_value
 763  0234 cd0000        	call	c_ltor
 767  0237 81            	ret
 798                     ; 215 uint32_t ADC2_Read(void)
 798                     ; 216 {
 799                     	switch	.text
 800  0238               _ADC2_Read:
 804                     ; 217 	ADC1_DeInit();
 806  0238 cd0000        	call	_ADC1_DeInit
 808                     ; 219 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
 808                     ; 220 						 ADC1_CHANNEL_4,
 808                     ; 221 						 ADC1_PRESSEL_FCPU_D18,
 808                     ; 222 						 ADC1_EXTTRIG_TIM,
 808                     ; 223 						 DISABLE,
 808                     ; 224 						 ADC1_ALIGN_RIGHT,
 808                     ; 225 						 ADC1_SCHMITTTRIG_CHANNEL4,
 808                     ; 226 						 DISABLE);
 810  023b 4b00          	push	#0
 811  023d 4b04          	push	#4
 812  023f 4b08          	push	#8
 813  0241 4b00          	push	#0
 814  0243 4b00          	push	#0
 815  0245 4b70          	push	#112
 816  0247 ae0104        	ldw	x,#260
 817  024a cd0000        	call	_ADC1_Init
 819  024d 5b06          	addw	sp,#6
 820                     ; 228 	ADC1_Cmd(ENABLE);
 822  024f a601          	ld	a,#1
 823  0251 cd0000        	call	_ADC1_Cmd
 825                     ; 230 	ADC1_StartConversion();
 827  0254 cd0000        	call	_ADC1_StartConversion
 830  0257               L151:
 831                     ; 232 	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
 833  0257 a680          	ld	a,#128
 834  0259 cd0000        	call	_ADC1_GetFlagStatus
 836  025c 4d            	tnz	a
 837  025d 27f8          	jreq	L151
 838                     ; 234 	adc2_value = ADC1_GetConversionValue();
 840  025f cd0000        	call	_ADC1_GetConversionValue
 842  0262 cd0000        	call	c_uitolx
 844  0265 ae0000        	ldw	x,#_adc2_value
 845  0268 cd0000        	call	c_rtol
 847                     ; 236 	ADC1_ClearFlag(ADC1_FLAG_EOC);
 849  026b a680          	ld	a,#128
 850  026d cd0000        	call	_ADC1_ClearFlag
 852                     ; 238 	return adc2_value;
 854  0270 ae0000        	ldw	x,#_adc2_value
 855  0273 cd0000        	call	c_ltor
 859  0276 81            	ret
 897                     ; 241 uint16_t readAverageADC1(uint8_t samples) {
 898                     	switch	.text
 899  0277               _readAverageADC1:
 901  0277 88            	push	a
 902  0278 5204          	subw	sp,#4
 903       00000004      OFST:	set	4
 906                     ; 243 	for (i = 0; i < samples; i++) {
 908  027a 3f11          	clr	_i
 910  027c 2011          	jra	L771
 911  027e               L371:
 912                     ; 244 			sum += ADC1_Read();
 914  027e cd01f9        	call	_ADC1_Read
 916  0281 ae0007        	ldw	x,#_sum
 917  0284 cd0000        	call	c_lgadd
 919                     ; 245 			Delay_us(200);  // small delay between samples
 921  0287 ae00c8        	ldw	x,#200
 922  028a cd00d1        	call	_Delay_us
 924                     ; 243 	for (i = 0; i < samples; i++) {
 926  028d 3c11          	inc	_i
 927  028f               L771:
 930  028f b611          	ld	a,_i
 931  0291 1105          	cp	a,(OFST+1,sp)
 932  0293 25e9          	jrult	L371
 933                     ; 247 	return sum / samples;
 935  0295 7b05          	ld	a,(OFST+1,sp)
 936  0297 b703          	ld	c_lreg+3,a
 937  0299 3f02          	clr	c_lreg+2
 938  029b 3f01          	clr	c_lreg+1
 939  029d 3f00          	clr	c_lreg
 940  029f 96            	ldw	x,sp
 941  02a0 1c0001        	addw	x,#OFST-3
 942  02a3 cd0000        	call	c_rtol
 945  02a6 ae0007        	ldw	x,#_sum
 946  02a9 cd0000        	call	c_ltor
 948  02ac 96            	ldw	x,sp
 949  02ad 1c0001        	addw	x,#OFST-3
 950  02b0 cd0000        	call	c_ludv
 952  02b3 be02          	ldw	x,c_lreg+2
 955  02b5 5b05          	addw	sp,#5
 956  02b7 81            	ret
 994                     ; 250 uint16_t readAverageADC2(uint8_t samples) {
 995                     	switch	.text
 996  02b8               _readAverageADC2:
 998  02b8 88            	push	a
 999  02b9 5204          	subw	sp,#4
1000       00000004      OFST:	set	4
1003                     ; 252 	for (i = 0; i < samples; i++) {
1005  02bb 3f11          	clr	_i
1007  02bd 2011          	jra	L522
1008  02bf               L122:
1009                     ; 253 			sum += ADC2_Read();
1011  02bf cd0238        	call	_ADC2_Read
1013  02c2 ae0007        	ldw	x,#_sum
1014  02c5 cd0000        	call	c_lgadd
1016                     ; 254 			Delay_us(200);  // small delay between samples
1018  02c8 ae00c8        	ldw	x,#200
1019  02cb cd00d1        	call	_Delay_us
1021                     ; 252 	for (i = 0; i < samples; i++) {
1023  02ce 3c11          	inc	_i
1024  02d0               L522:
1027  02d0 b611          	ld	a,_i
1028  02d2 1105          	cp	a,(OFST+1,sp)
1029  02d4 25e9          	jrult	L122
1030                     ; 256 	return sum / samples;
1032  02d6 7b05          	ld	a,(OFST+1,sp)
1033  02d8 b703          	ld	c_lreg+3,a
1034  02da 3f02          	clr	c_lreg+2
1035  02dc 3f01          	clr	c_lreg+1
1036  02de 3f00          	clr	c_lreg
1037  02e0 96            	ldw	x,sp
1038  02e1 1c0001        	addw	x,#OFST-3
1039  02e4 cd0000        	call	c_rtol
1042  02e7 ae0007        	ldw	x,#_sum
1043  02ea cd0000        	call	c_ltor
1045  02ed 96            	ldw	x,sp
1046  02ee 1c0001        	addw	x,#OFST-3
1047  02f1 cd0000        	call	c_ludv
1049  02f4 be02          	ldw	x,c_lreg+2
1052  02f6 5b05          	addw	sp,#5
1053  02f8 81            	ret
1187                     	xdef	_main
1188                     	xdef	_readAverageADC2
1189                     	xdef	_readAverageADC1
1190                     	xdef	_ADC2_Read
1191                     	xdef	_ADC1_Read
1192                     	xdef	_ADC1_setup
1193                     	xdef	_Timer4_Init
1194                     	xdef	_CLK_Init
1195                     	xdef	_GPIO_Int
1196                     	xdef	_Delay_us
1197                     	xdef	_Segment_Patterns
1198                     	switch	.ubsct
1199  0000               _adc2_value:
1200  0000 00000000      	ds.b	4
1201                     	xdef	_adc2_value
1202  0004               _adc1_value:
1203  0004 00000000      	ds.b	4
1204                     	xdef	_adc1_value
1205  0008               _current:
1206  0008 00000000      	ds.b	4
1207                     	xdef	_current
1208  000c               _voltage:
1209  000c 00000000      	ds.b	4
1210                     	xdef	_voltage
1211  0010               _samples:
1212  0010 00            	ds.b	1
1213                     	xdef	_samples
1214  0011               _i:
1215  0011 00            	ds.b	1
1216                     	xdef	_i
1217                     	xdef	_sum
1218  0012               _millivolt2:
1219  0012 00000000      	ds.b	4
1220                     	xdef	_millivolt2
1221  0016               _millivolt1:
1222  0016 00000000      	ds.b	4
1223                     	xdef	_millivolt1
1224  001a               _pattern:
1225  001a 00            	ds.b	1
1226                     	xdef	_pattern
1227                     	xdef	_digits
1228                     	xdef	_currentDigit
1229                     	xref	_TIM4_ITConfig
1230                     	xref	_TIM4_Cmd
1231                     	xref	_TIM4_TimeBaseInit
1232                     	xref	_TIM4_DeInit
1233                     	xref	_GPIO_Init
1234                     	xref	_GPIO_DeInit
1235                     	xref	_CLK_GetFlagStatus
1236                     	xref	_CLK_SYSCLKConfig
1237                     	xref	_CLK_HSIPrescalerConfig
1238                     	xref	_CLK_ClockSwitchConfig
1239                     	xref	_CLK_PeripheralClockConfig
1240                     	xref	_CLK_ClockSwitchCmd
1241                     	xref	_CLK_LSICmd
1242                     	xref	_CLK_HSICmd
1243                     	xref	_CLK_HSECmd
1244                     	xref	_CLK_DeInit
1245                     	xref	_ADC1_ClearFlag
1246                     	xref	_ADC1_GetFlagStatus
1247                     	xref	_ADC1_GetConversionValue
1248                     	xref	_ADC1_StartConversion
1249                     	xref	_ADC1_ConversionConfig
1250                     	xref	_ADC1_DataBufferCmd
1251                     	xref	_ADC1_Cmd
1252                     	xref	_ADC1_Init
1253                     	xref	_ADC1_DeInit
1254                     	xref.b	c_lreg
1255                     	xref.b	c_x
1275                     	xref	c_lgadd
1276                     	xref	c_lumd
1277                     	xref	c_ludv
1278                     	xref	c_smul
1279                     	xref	c_ltor
1280                     	xref	c_rtol
1281                     	xref	c_uitolx
1282                     	end
