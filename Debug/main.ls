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
  80                     	switch	.const
  81  000a               L6:
  82  000a 000186a0      	dc.l	100000
  83  000e               L01:
  84  000e 000000f1      	dc.l	241
  85  0012               L21:
  86  0012 00000064      	dc.l	100
  87  0016               L41:
  88  0016 0000000a      	dc.l	10
  89                     ; 70 main()
  89                     ; 71 {
  90                     	scross	off
  91                     	switch	.text
  92  0000               _main:
  96                     ; 72 	CLK_Init();
  98  0000 cd0151        	call	_CLK_Init
 100                     ; 73 	GPIO_Int();
 102  0003 cd00f6        	call	_GPIO_Int
 104                     ; 74 	Timer4_Init();  // Start Timer4 for display refresh
 106  0006 cd01b3        	call	_Timer4_Init
 108                     ; 75 	ADC1_setup();
 110  0009 cd01c9        	call	_ADC1_setup
 112                     ; 76 	counter = 0;  // Start from 0
 114  000c 5f            	clrw	x
 115  000d bf17          	ldw	L3_counter,x
 116  000f               L32:
 117                     ; 80 		voltage = readAverageADC1(8);
 119  000f a608          	ld	a,#8
 120  0011 cd025c        	call	_readAverageADC1
 122  0014 bf02          	ldw	_voltage,x
 123                     ; 81 		millivolt1 = (voltage * 1600) / 1000;
 125  0016 be02          	ldw	x,_voltage
 126  0018 90ae0640      	ldw	y,#1600
 127  001c cd0000        	call	c_imul
 129  001f 90ae03e8      	ldw	y,#1000
 130  0023 65            	divw	x,y
 131  0024 cd0000        	call	c_uitolx
 133  0027 ae000e        	ldw	x,#_millivolt1
 134  002a cd0000        	call	c_rtol
 136                     ; 82 		sum = 0;
 138  002d ae0000        	ldw	x,#0
 139  0030 bf09          	ldw	_sum+2,x
 140  0032 ae0000        	ldw	x,#0
 141  0035 bf07          	ldw	_sum,x
 142                     ; 83 		current = readAverageADC2(8);
 144  0037 a608          	ld	a,#8
 145  0039 cd029c        	call	_readAverageADC2
 147  003c bf00          	ldw	_current,x
 148                     ; 84 		millivolt2 = (current * 100000) / 241;
 150  003e be00          	ldw	x,_current
 151  0040 cd0000        	call	c_uitolx
 153  0043 ae000a        	ldw	x,#L6
 154  0046 cd0000        	call	c_lmul
 156  0049 ae000e        	ldw	x,#L01
 157  004c cd0000        	call	c_ldiv
 159  004f ae0006        	ldw	x,#_millivolt2
 160  0052 cd0000        	call	c_rtol
 162                     ; 85 		sum = 0;
 164  0055 ae0000        	ldw	x,#0
 165  0058 bf09          	ldw	_sum+2,x
 166  005a ae0000        	ldw	x,#0
 167  005d bf07          	ldw	_sum,x
 168                     ; 87 		digits[0] = ((millivolt1 / 100) % 10);  // Voltage hundreds
 170  005f ae000e        	ldw	x,#_millivolt1
 171  0062 cd0000        	call	c_ltor
 173  0065 ae0012        	ldw	x,#L21
 174  0068 cd0000        	call	c_ludv
 176  006b ae0016        	ldw	x,#L41
 177  006e cd0000        	call	c_lumd
 179  0071 b603          	ld	a,c_lreg+3
 180  0073 b701          	ld	_digits,a
 181                     ; 88 		digits[1] = ((millivolt1 / 10) % 10);   // Voltage tens
 183  0075 ae000e        	ldw	x,#_millivolt1
 184  0078 cd0000        	call	c_ltor
 186  007b ae0016        	ldw	x,#L41
 187  007e cd0000        	call	c_ludv
 189  0081 ae0016        	ldw	x,#L41
 190  0084 cd0000        	call	c_lumd
 192  0087 b603          	ld	a,c_lreg+3
 193  0089 b702          	ld	_digits+1,a
 194                     ; 89 		digits[2] = (millivolt1 % 10);          // Voltage ones
 196  008b ae000e        	ldw	x,#_millivolt1
 197  008e cd0000        	call	c_ltor
 199  0091 ae0016        	ldw	x,#L41
 200  0094 cd0000        	call	c_lumd
 202  0097 b603          	ld	a,c_lreg+3
 203  0099 b703          	ld	_digits+2,a
 204                     ; 91 		digits[3] = ((millivolt2 / 100) % 10);  // Current hundreds
 206  009b ae0006        	ldw	x,#_millivolt2
 207  009e cd0000        	call	c_ltor
 209  00a1 ae0012        	ldw	x,#L21
 210  00a4 cd0000        	call	c_ludv
 212  00a7 ae0016        	ldw	x,#L41
 213  00aa cd0000        	call	c_lumd
 215  00ad b603          	ld	a,c_lreg+3
 216  00af b704          	ld	_digits+3,a
 217                     ; 92 		digits[4] = ((millivolt2 / 10) % 10);   // Current tens
 219  00b1 ae0006        	ldw	x,#_millivolt2
 220  00b4 cd0000        	call	c_ltor
 222  00b7 ae0016        	ldw	x,#L41
 223  00ba cd0000        	call	c_ludv
 225  00bd ae0016        	ldw	x,#L41
 226  00c0 cd0000        	call	c_lumd
 228  00c3 b603          	ld	a,c_lreg+3
 229  00c5 b705          	ld	_digits+4,a
 230                     ; 93 		digits[5] = (millivolt2 % 10);          // Current ones
 232  00c7 ae0006        	ldw	x,#_millivolt2
 233  00ca cd0000        	call	c_ltor
 235  00cd ae0016        	ldw	x,#L41
 236  00d0 cd0000        	call	c_lumd
 238  00d3 b603          	ld	a,c_lreg+3
 239  00d5 b706          	ld	_digits+5,a
 240                     ; 97 		Delay_us(1000000);
 242  00d7 ae4240        	ldw	x,#16960
 243  00da ad04          	call	_Delay_us
 246  00dc ac0f000f      	jpf	L32
 280                     ; 102 void Delay_us(uint16_t nCount)
 280                     ; 103 {
 281                     	switch	.text
 282  00e0               _Delay_us:
 284  00e0 89            	pushw	x
 285       00000000      OFST:	set	0
 288  00e1 2002          	jra	L74
 289  00e3               L54:
 290                     ; 105         _asm("nop"); _asm("nop");
 293  00e3 9d            nop
 298  00e4 9d            nop
 300  00e5               L74:
 301                     ; 104     while (nCount--) {
 303  00e5 1e01          	ldw	x,(OFST+1,sp)
 304  00e7 1d0001        	subw	x,#1
 305  00ea 1f01          	ldw	(OFST+1,sp),x
 306  00ec 1c0001        	addw	x,#1
 307  00ef a30000        	cpw	x,#0
 308  00f2 26ef          	jrne	L54
 309                     ; 107 }
 312  00f4 85            	popw	x
 313  00f5 81            	ret
 338                     ; 110 void GPIO_Int(void) {
 339                     	switch	.text
 340  00f6               _GPIO_Int:
 344                     ; 111 	GPIO_DeInit(GPIOD);
 346  00f6 ae500f        	ldw	x,#20495
 347  00f9 cd0000        	call	_GPIO_DeInit
 349                     ; 112 	GPIO_DeInit(GPIOC);
 351  00fc ae500a        	ldw	x,#20490
 352  00ff cd0000        	call	_GPIO_DeInit
 354                     ; 113 	GPIO_DeInit(GPIOA);
 356  0102 ae5000        	ldw	x,#20480
 357  0105 cd0000        	call	_GPIO_DeInit
 359                     ; 114 	GPIO_DeInit(GPIOB);
 361  0108 ae5005        	ldw	x,#20485
 362  010b cd0000        	call	_GPIO_DeInit
 364                     ; 115 	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)PORTD_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 366  010e 4bf0          	push	#240
 367  0110 4b70          	push	#112
 368  0112 ae500f        	ldw	x,#20495
 369  0115 cd0000        	call	_GPIO_Init
 371  0118 85            	popw	x
 372                     ; 116 	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);
 374  0119 4b00          	push	#0
 375  011b 4b04          	push	#4
 376  011d ae500f        	ldw	x,#20495
 377  0120 cd0000        	call	_GPIO_Init
 379  0123 85            	popw	x
 380                     ; 117 	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);
 382  0124 4b00          	push	#0
 383  0126 4b08          	push	#8
 384  0128 ae500f        	ldw	x,#20495
 385  012b cd0000        	call	_GPIO_Init
 387  012e 85            	popw	x
 388                     ; 118 	GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)PORTC_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 390  012f 4bf0          	push	#240
 391  0131 4bf8          	push	#248
 392  0133 ae500a        	ldw	x,#20490
 393  0136 cd0000        	call	_GPIO_Init
 395  0139 85            	popw	x
 396                     ; 119 	GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)PORTA_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 398  013a 4bf0          	push	#240
 399  013c 4b0e          	push	#14
 400  013e ae5000        	ldw	x,#20480
 401  0141 cd0000        	call	_GPIO_Init
 403  0144 85            	popw	x
 404                     ; 120 	GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)PORTB_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 406  0145 4bf0          	push	#240
 407  0147 4b30          	push	#48
 408  0149 ae5005        	ldw	x,#20485
 409  014c cd0000        	call	_GPIO_Init
 411  014f 85            	popw	x
 412                     ; 121 }
 415  0150 81            	ret
 448                     ; 124 void CLK_Init(void) {
 449                     	switch	.text
 450  0151               _CLK_Init:
 454                     ; 125 	CLK_DeInit();
 456  0151 cd0000        	call	_CLK_DeInit
 458                     ; 127 	CLK_HSECmd(DISABLE);
 460  0154 4f            	clr	a
 461  0155 cd0000        	call	_CLK_HSECmd
 463                     ; 128 	CLK_LSICmd(DISABLE);
 465  0158 4f            	clr	a
 466  0159 cd0000        	call	_CLK_LSICmd
 468                     ; 129 	CLK_HSICmd(ENABLE);
 470  015c a601          	ld	a,#1
 471  015e cd0000        	call	_CLK_HSICmd
 474  0161               L57:
 475                     ; 130 	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
 477  0161 ae0102        	ldw	x,#258
 478  0164 cd0000        	call	_CLK_GetFlagStatus
 480  0167 4d            	tnz	a
 481  0168 27f7          	jreq	L57
 482                     ; 132 	CLK_ClockSwitchCmd(ENABLE);
 484  016a a601          	ld	a,#1
 485  016c cd0000        	call	_CLK_ClockSwitchCmd
 487                     ; 133 	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV4);
 489  016f a610          	ld	a,#16
 490  0171 cd0000        	call	_CLK_HSIPrescalerConfig
 492                     ; 134 	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
 494  0174 a680          	ld	a,#128
 495  0176 cd0000        	call	_CLK_SYSCLKConfig
 497                     ; 136 	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
 497                     ; 137 	DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
 499  0179 4b01          	push	#1
 500  017b 4b00          	push	#0
 501  017d ae01e1        	ldw	x,#481
 502  0180 cd0000        	call	_CLK_ClockSwitchConfig
 504  0183 85            	popw	x
 505                     ; 139 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
 507  0184 ae0100        	ldw	x,#256
 508  0187 cd0000        	call	_CLK_PeripheralClockConfig
 510                     ; 140 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
 512  018a 5f            	clrw	x
 513  018b cd0000        	call	_CLK_PeripheralClockConfig
 515                     ; 141 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
 517  018e ae1301        	ldw	x,#4865
 518  0191 cd0000        	call	_CLK_PeripheralClockConfig
 520                     ; 142 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
 522  0194 ae1200        	ldw	x,#4608
 523  0197 cd0000        	call	_CLK_PeripheralClockConfig
 525                     ; 143 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
 527  019a ae0300        	ldw	x,#768
 528  019d cd0000        	call	_CLK_PeripheralClockConfig
 530                     ; 144 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
 532  01a0 ae0700        	ldw	x,#1792
 533  01a3 cd0000        	call	_CLK_PeripheralClockConfig
 535                     ; 145 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
 537  01a6 ae0500        	ldw	x,#1280
 538  01a9 cd0000        	call	_CLK_PeripheralClockConfig
 540                     ; 146 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);;
 542  01ac ae0401        	ldw	x,#1025
 543  01af cd0000        	call	_CLK_PeripheralClockConfig
 545                     ; 147 }
 549  01b2 81            	ret
 577                     ; 150 void Timer4_Init(void) {
 578                     	switch	.text
 579  01b3               _Timer4_Init:
 583                     ; 151 	TIM4_DeInit();
 585  01b3 cd0000        	call	_TIM4_DeInit
 587                     ; 152 	TIM4_TimeBaseInit(TIM4_PRESCALER_64, 156);      
 589  01b6 ae069c        	ldw	x,#1692
 590  01b9 cd0000        	call	_TIM4_TimeBaseInit
 592                     ; 153 	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
 594  01bc ae0101        	ldw	x,#257
 595  01bf cd0000        	call	_TIM4_ITConfig
 597                     ; 154 	TIM4_Cmd(ENABLE);
 599  01c2 a601          	ld	a,#1
 600  01c4 cd0000        	call	_TIM4_Cmd
 602                     ; 156 	enableInterrupts();
 605  01c7 9a            rim
 607                     ; 157 }
 611  01c8 81            	ret
 639                     ; 159 void ADC1_setup(void)
 639                     ; 160 {
 640                     	switch	.text
 641  01c9               _ADC1_setup:
 645                     ; 161   ADC1_DeInit();         
 647  01c9 cd0000        	call	_ADC1_DeInit
 649                     ; 163 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
 649                     ; 164 						ADC1_CHANNEL_3,
 649                     ; 165 						ADC1_PRESSEL_FCPU_D18, 
 649                     ; 166 						ADC1_EXTTRIG_TIM, 
 649                     ; 167 						DISABLE, 
 649                     ; 168 						ADC1_ALIGN_RIGHT, 
 649                     ; 169 						ADC1_SCHMITTTRIG_CHANNEL3, 
 649                     ; 170 						DISABLE);
 651  01cc 4b00          	push	#0
 652  01ce 4b03          	push	#3
 653  01d0 4b08          	push	#8
 654  01d2 4b00          	push	#0
 655  01d4 4b00          	push	#0
 656  01d6 4b70          	push	#112
 657  01d8 ae0103        	ldw	x,#259
 658  01db cd0000        	call	_ADC1_Init
 660  01de 5b06          	addw	sp,#6
 661                     ; 172 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, 
 661                     ; 173 						ADC1_CHANNEL_4,
 661                     ; 174 						ADC1_PRESSEL_FCPU_D18, 
 661                     ; 175 						ADC1_EXTTRIG_TIM, 
 661                     ; 176 						DISABLE, 
 661                     ; 177 						ADC1_ALIGN_RIGHT, 
 661                     ; 178 						ADC1_SCHMITTTRIG_CHANNEL4, 
 661                     ; 179 						DISABLE);
 663  01e0 4b00          	push	#0
 664  01e2 4b04          	push	#4
 665  01e4 4b08          	push	#8
 666  01e6 4b00          	push	#0
 667  01e8 4b00          	push	#0
 668  01ea 4b70          	push	#112
 669  01ec ae0104        	ldw	x,#260
 670  01ef cd0000        	call	_ADC1_Init
 672  01f2 5b06          	addw	sp,#6
 673                     ; 181 	ADC1_ConversionConfig(ADC1_CONVERSIONMODE_CONTINUOUS,
 673                     ; 182 												((ADC1_Channel_TypeDef)(ADC1_CHANNEL_3 | ADC1_CHANNEL_4)),
 673                     ; 183 												ADC1_ALIGN_RIGHT);
 675  01f4 4b08          	push	#8
 676  01f6 ae0107        	ldw	x,#263
 677  01f9 cd0000        	call	_ADC1_ConversionConfig
 679  01fc 84            	pop	a
 680                     ; 185 	ADC1_DataBufferCmd(ENABLE);
 682  01fd a601          	ld	a,#1
 683  01ff cd0000        	call	_ADC1_DataBufferCmd
 685                     ; 187 	ADC1_Cmd(ENABLE);
 687  0202 a601          	ld	a,#1
 688  0204 cd0000        	call	_ADC1_Cmd
 690                     ; 188 }
 693  0207 81            	ret
 722                     ; 190 uint32_t ADC1_Read(void)
 722                     ; 191 {
 723                     	switch	.text
 724  0208               _ADC1_Read:
 728                     ; 192 	ADC1_ScanModeCmd(ENABLE);
 730  0208 a601          	ld	a,#1
 731  020a cd0000        	call	_ADC1_ScanModeCmd
 733                     ; 194 	ADC1_StartConversion();
 735  020d cd0000        	call	_ADC1_StartConversion
 738  0210               L331:
 739                     ; 196 	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
 741  0210 a680          	ld	a,#128
 742  0212 cd0000        	call	_ADC1_GetFlagStatus
 744  0215 4d            	tnz	a
 745  0216 27f8          	jreq	L331
 746                     ; 198 	adc1_value = ADC1_GetBufferValue(3);
 748  0218 a603          	ld	a,#3
 749  021a cd0000        	call	_ADC1_GetBufferValue
 751  021d cd0000        	call	c_uitolx
 753  0220 ae0012        	ldw	x,#_adc1_value
 754  0223 cd0000        	call	c_rtol
 756                     ; 200 	ADC1_ClearFlag(ADC1_FLAG_EOC);
 758  0226 a680          	ld	a,#128
 759  0228 cd0000        	call	_ADC1_ClearFlag
 761                     ; 202 	return adc1_value;
 763  022b ae0012        	ldw	x,#_adc1_value
 764  022e cd0000        	call	c_ltor
 768  0231 81            	ret
 797                     ; 205 uint32_t ADC2_Read(void)
 797                     ; 206 {
 798                     	switch	.text
 799  0232               _ADC2_Read:
 803                     ; 207 	ADC1_ScanModeCmd(ENABLE);
 805  0232 a601          	ld	a,#1
 806  0234 cd0000        	call	_ADC1_ScanModeCmd
 808                     ; 209 	ADC1_StartConversion();
 810  0237 cd0000        	call	_ADC1_StartConversion
 813  023a               L151:
 814                     ; 211 	while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
 816  023a a680          	ld	a,#128
 817  023c cd0000        	call	_ADC1_GetFlagStatus
 819  023f 4d            	tnz	a
 820  0240 27f8          	jreq	L151
 821                     ; 213 	adc2_value = ADC1_GetBufferValue(4);
 823  0242 a604          	ld	a,#4
 824  0244 cd0000        	call	_ADC1_GetBufferValue
 826  0247 cd0000        	call	c_uitolx
 828  024a ae000a        	ldw	x,#_adc2_value
 829  024d cd0000        	call	c_rtol
 831                     ; 215 	ADC1_ClearFlag(ADC1_FLAG_EOC);
 833  0250 a680          	ld	a,#128
 834  0252 cd0000        	call	_ADC1_ClearFlag
 836                     ; 217 	return adc2_value;
 838  0255 ae000a        	ldw	x,#_adc2_value
 839  0258 cd0000        	call	c_ltor
 843  025b 81            	ret
 881                     ; 220 uint16_t readAverageADC1(uint8_t samples) {
 882                     	switch	.text
 883  025c               _readAverageADC1:
 885  025c 88            	push	a
 886  025d 5204          	subw	sp,#4
 887       00000004      OFST:	set	4
 890                     ; 222 	for (i = 0; i < samples; i++) {
 892  025f 3f05          	clr	_i
 894  0261 2010          	jra	L771
 895  0263               L371:
 896                     ; 223 			sum += ADC1_Read();
 898  0263 ada3          	call	_ADC1_Read
 900  0265 ae0007        	ldw	x,#_sum
 901  0268 cd0000        	call	c_lgadd
 903                     ; 224 			Delay_us(200);  // small delay between samples
 905  026b ae00c8        	ldw	x,#200
 906  026e cd00e0        	call	_Delay_us
 908                     ; 222 	for (i = 0; i < samples; i++) {
 910  0271 3c05          	inc	_i
 911  0273               L771:
 914  0273 b605          	ld	a,_i
 915  0275 1105          	cp	a,(OFST+1,sp)
 916  0277 25ea          	jrult	L371
 917                     ; 226 	return sum / samples;
 919  0279 7b05          	ld	a,(OFST+1,sp)
 920  027b b703          	ld	c_lreg+3,a
 921  027d 3f02          	clr	c_lreg+2
 922  027f 3f01          	clr	c_lreg+1
 923  0281 3f00          	clr	c_lreg
 924  0283 96            	ldw	x,sp
 925  0284 1c0001        	addw	x,#OFST-3
 926  0287 cd0000        	call	c_rtol
 929  028a ae0007        	ldw	x,#_sum
 930  028d cd0000        	call	c_ltor
 932  0290 96            	ldw	x,sp
 933  0291 1c0001        	addw	x,#OFST-3
 934  0294 cd0000        	call	c_ludv
 936  0297 be02          	ldw	x,c_lreg+2
 939  0299 5b05          	addw	sp,#5
 940  029b 81            	ret
 978                     ; 229 uint16_t readAverageADC2(uint8_t samples) {
 979                     	switch	.text
 980  029c               _readAverageADC2:
 982  029c 88            	push	a
 983  029d 5204          	subw	sp,#4
 984       00000004      OFST:	set	4
 987                     ; 231 	for (i = 0; i < samples; i++) {
 989  029f 3f05          	clr	_i
 991  02a1 2010          	jra	L522
 992  02a3               L122:
 993                     ; 232 			sum += ADC2_Read();
 995  02a3 ad8d          	call	_ADC2_Read
 997  02a5 ae0007        	ldw	x,#_sum
 998  02a8 cd0000        	call	c_lgadd
1000                     ; 233 			Delay_us(200);  // small delay between samples
1002  02ab ae00c8        	ldw	x,#200
1003  02ae cd00e0        	call	_Delay_us
1005                     ; 231 	for (i = 0; i < samples; i++) {
1007  02b1 3c05          	inc	_i
1008  02b3               L522:
1011  02b3 b605          	ld	a,_i
1012  02b5 1105          	cp	a,(OFST+1,sp)
1013  02b7 25ea          	jrult	L122
1014                     ; 235 	return sum / samples;
1016  02b9 7b05          	ld	a,(OFST+1,sp)
1017  02bb b703          	ld	c_lreg+3,a
1018  02bd 3f02          	clr	c_lreg+2
1019  02bf 3f01          	clr	c_lreg+1
1020  02c1 3f00          	clr	c_lreg
1021  02c3 96            	ldw	x,sp
1022  02c4 1c0001        	addw	x,#OFST-3
1023  02c7 cd0000        	call	c_rtol
1026  02ca ae0007        	ldw	x,#_sum
1027  02cd cd0000        	call	c_ltor
1029  02d0 96            	ldw	x,sp
1030  02d1 1c0001        	addw	x,#OFST-3
1031  02d4 cd0000        	call	c_ludv
1033  02d7 be02          	ldw	x,c_lreg+2
1036  02d9 5b05          	addw	sp,#5
1037  02db 81            	ret
1180                     	xdef	_main
1181                     	xdef	_readAverageADC2
1182                     	xdef	_readAverageADC1
1183                     	xdef	_ADC2_Read
1184                     	xdef	_ADC1_Read
1185                     	xdef	_ADC1_setup
1186                     	xdef	_Timer4_Init
1187                     	xdef	_CLK_Init
1188                     	xdef	_GPIO_Int
1189                     	xdef	_Delay_us
1190                     	xdef	_Segment_Patterns
1191                     	switch	.ubsct
1192  0000               _current:
1193  0000 0000          	ds.b	2
1194                     	xdef	_current
1195  0002               _voltage:
1196  0002 0000          	ds.b	2
1197                     	xdef	_voltage
1198  0004               _samples:
1199  0004 00            	ds.b	1
1200                     	xdef	_samples
1201  0005               _i:
1202  0005 00            	ds.b	1
1203                     	xdef	_i
1204                     	xdef	_sum
1205  0006               _millivolt2:
1206  0006 00000000      	ds.b	4
1207                     	xdef	_millivolt2
1208  000a               _adc2_value:
1209  000a 00000000      	ds.b	4
1210                     	xdef	_adc2_value
1211  000e               _millivolt1:
1212  000e 00000000      	ds.b	4
1213                     	xdef	_millivolt1
1214  0012               _adc1_value:
1215  0012 00000000      	ds.b	4
1216                     	xdef	_adc1_value
1217  0016               _pattern:
1218  0016 00            	ds.b	1
1219                     	xdef	_pattern
1220  0017               L3_counter:
1221  0017 0000          	ds.b	2
1222                     	xdef	_digits
1223                     	xdef	_currentDigit
1224                     	xref	_TIM4_ITConfig
1225                     	xref	_TIM4_Cmd
1226                     	xref	_TIM4_TimeBaseInit
1227                     	xref	_TIM4_DeInit
1228                     	xref	_GPIO_Init
1229                     	xref	_GPIO_DeInit
1230                     	xref	_CLK_GetFlagStatus
1231                     	xref	_CLK_SYSCLKConfig
1232                     	xref	_CLK_HSIPrescalerConfig
1233                     	xref	_CLK_ClockSwitchConfig
1234                     	xref	_CLK_PeripheralClockConfig
1235                     	xref	_CLK_ClockSwitchCmd
1236                     	xref	_CLK_LSICmd
1237                     	xref	_CLK_HSICmd
1238                     	xref	_CLK_HSECmd
1239                     	xref	_CLK_DeInit
1240                     	xref	_ADC1_ClearFlag
1241                     	xref	_ADC1_GetFlagStatus
1242                     	xref	_ADC1_GetBufferValue
1243                     	xref	_ADC1_StartConversion
1244                     	xref	_ADC1_ConversionConfig
1245                     	xref	_ADC1_DataBufferCmd
1246                     	xref	_ADC1_ScanModeCmd
1247                     	xref	_ADC1_Cmd
1248                     	xref	_ADC1_Init
1249                     	xref	_ADC1_DeInit
1250                     	xref.b	c_lreg
1251                     	xref.b	c_x
1271                     	xref	c_lgadd
1272                     	xref	c_lumd
1273                     	xref	c_ludv
1274                     	xref	c_ltor
1275                     	xref	c_ldiv
1276                     	xref	c_lmul
1277                     	xref	c_rtol
1278                     	xref	c_uitolx
1279                     	xref	c_imul
1280                     	end
