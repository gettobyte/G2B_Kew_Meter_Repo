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
  21                     .const:	section	.text
  22  0000               _Segment_Patterns:
  23  0000 81            	dc.b	129
  24  0001 cf            	dc.b	207
  25  0002 92            	dc.b	146
  26  0003 86            	dc.b	134
  27  0004 cc            	dc.b	204
  28  0005 a4            	dc.b	164
  29  0006 a0            	dc.b	160
  30  0007 8f            	dc.b	143
  31  0008 80            	dc.b	128
  32  0009 84            	dc.b	132
  67                     ; 56 main()
  67                     ; 57 {
  69                     	switch	.text
  70  0000               _main:
  74                     ; 58 	CLK_Init();
  76  0000 cd0094        	call	_CLK_Init
  78                     ; 59 	GPIO_Int();
  80  0003 ad5b          	call	_GPIO_Int
  82                     ; 60 	Timer4_Init();  // Start Timer4 for display refresh
  84  0005 cd00f6        	call	_Timer4_Init
  86                     ; 61 	counter = 0;  // Start from 0
  88  0008 5f            	clrw	x
  89  0009 bf03          	ldw	L3_counter,x
  90  000b               L32:
  91                     ; 66 		digits[0] = ((counter / 100) % 10);  // Hundreds
  93  000b be03          	ldw	x,L3_counter
  94  000d a664          	ld	a,#100
  95  000f 62            	div	x,a
  96  0010 a60a          	ld	a,#10
  97  0012 62            	div	x,a
  98  0013 5f            	clrw	x
  99  0014 97            	ld	xl,a
 100  0015 01            	rrwa	x,a
 101  0016 b701          	ld	_digits,a
 102  0018 02            	rlwa	x,a
 103                     ; 67 		digits[1] = (counter / 10) % 10;   // Tens
 105  0019 be03          	ldw	x,L3_counter
 106  001b a60a          	ld	a,#10
 107  001d 62            	div	x,a
 108  001e a60a          	ld	a,#10
 109  0020 62            	div	x,a
 110  0021 5f            	clrw	x
 111  0022 97            	ld	xl,a
 112  0023 01            	rrwa	x,a
 113  0024 b702          	ld	_digits+1,a
 114  0026 02            	rlwa	x,a
 115                     ; 68 		digits[2] = counter % 10;          // Ones
 117  0027 be03          	ldw	x,L3_counter
 118  0029 a60a          	ld	a,#10
 119  002b 62            	div	x,a
 120  002c 5f            	clrw	x
 121  002d 97            	ld	xl,a
 122  002e 01            	rrwa	x,a
 123  002f b703          	ld	_digits+2,a
 124  0031 02            	rlwa	x,a
 125                     ; 74 		Delay_us(50000); // Add delay before increasing counter
 127  0032 aec350        	ldw	x,#50000
 128  0035 ad13          	call	_Delay_us
 130                     ; 75 		counter++;
 132  0037 be03          	ldw	x,L3_counter
 133  0039 1c0001        	addw	x,#1
 134  003c bf03          	ldw	L3_counter,x
 135                     ; 76 		if (counter > 999) counter = 0;
 137  003e be03          	ldw	x,L3_counter
 138  0040 a303e8        	cpw	x,#1000
 139  0043 25c6          	jrult	L32
 142  0045 5f            	clrw	x
 143  0046 bf03          	ldw	L3_counter,x
 144  0048 20c1          	jra	L32
 178                     ; 81 void Delay_us(uint16_t nCount)
 178                     ; 82 {
 179                     	switch	.text
 180  004a               _Delay_us:
 182  004a 89            	pushw	x
 183       00000000      OFST:	set	0
 186  004b 2002          	jra	L15
 187  004d               L74:
 188                     ; 84         _asm("nop"); _asm("nop");
 191  004d 9d            nop
 196  004e 9d            nop
 198  004f               L15:
 199                     ; 83     while (nCount--) {
 201  004f 1e01          	ldw	x,(OFST+1,sp)
 202  0051 1d0001        	subw	x,#1
 203  0054 1f01          	ldw	(OFST+1,sp),x
 204  0056 1c0001        	addw	x,#1
 205  0059 a30000        	cpw	x,#0
 206  005c 26ef          	jrne	L74
 207                     ; 86 }
 210  005e 85            	popw	x
 211  005f 81            	ret
 236                     ; 89 void GPIO_Int(void) {
 237                     	switch	.text
 238  0060               _GPIO_Int:
 242                     ; 90 	GPIO_DeInit(GPIOD);
 244  0060 ae500f        	ldw	x,#20495
 245  0063 cd0000        	call	_GPIO_DeInit
 247                     ; 91 	GPIO_DeInit(GPIOC);
 249  0066 ae500a        	ldw	x,#20490
 250  0069 cd0000        	call	_GPIO_DeInit
 252                     ; 92 	GPIO_DeInit(GPIOA);
 254  006c ae5000        	ldw	x,#20480
 255  006f cd0000        	call	_GPIO_DeInit
 257                     ; 93 	GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)PORTD_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 259  0072 4bf0          	push	#240
 260  0074 4b70          	push	#112
 261  0076 ae500f        	ldw	x,#20495
 262  0079 cd0000        	call	_GPIO_Init
 264  007c 85            	popw	x
 265                     ; 94 	GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)PORTC_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 267  007d 4bf0          	push	#240
 268  007f 4bf8          	push	#248
 269  0081 ae500a        	ldw	x,#20490
 270  0084 cd0000        	call	_GPIO_Init
 272  0087 85            	popw	x
 273                     ; 95 	GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)PORTA_GPIO_PINS, GPIO_MODE_OUT_PP_HIGH_FAST);
 275  0088 4bf0          	push	#240
 276  008a 4b0e          	push	#14
 277  008c ae5000        	ldw	x,#20480
 278  008f cd0000        	call	_GPIO_Init
 280  0092 85            	popw	x
 281                     ; 96 }
 284  0093 81            	ret
 317                     ; 99 void CLK_Init(void) {
 318                     	switch	.text
 319  0094               _CLK_Init:
 323                     ; 100  CLK_DeInit();
 325  0094 cd0000        	call	_CLK_DeInit
 327                     ; 102  CLK_HSECmd(DISABLE);
 329  0097 4f            	clr	a
 330  0098 cd0000        	call	_CLK_HSECmd
 332                     ; 103  CLK_LSICmd(DISABLE);
 334  009b 4f            	clr	a
 335  009c cd0000        	call	_CLK_LSICmd
 337                     ; 104  CLK_HSICmd(ENABLE);
 339  009f a601          	ld	a,#1
 340  00a1 cd0000        	call	_CLK_HSICmd
 343  00a4               L77:
 344                     ; 105  while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
 346  00a4 ae0102        	ldw	x,#258
 347  00a7 cd0000        	call	_CLK_GetFlagStatus
 349  00aa 4d            	tnz	a
 350  00ab 27f7          	jreq	L77
 351                     ; 107  CLK_ClockSwitchCmd(ENABLE);
 353  00ad a601          	ld	a,#1
 354  00af cd0000        	call	_CLK_ClockSwitchCmd
 356                     ; 108  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV4);
 358  00b2 a610          	ld	a,#16
 359  00b4 cd0000        	call	_CLK_HSIPrescalerConfig
 361                     ; 109  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
 363  00b7 a680          	ld	a,#128
 364  00b9 cd0000        	call	_CLK_SYSCLKConfig
 366                     ; 111  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, 
 366                     ; 112  DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
 368  00bc 4b01          	push	#1
 369  00be 4b00          	push	#0
 370  00c0 ae01e1        	ldw	x,#481
 371  00c3 cd0000        	call	_CLK_ClockSwitchConfig
 373  00c6 85            	popw	x
 374                     ; 114  CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
 376  00c7 ae0100        	ldw	x,#256
 377  00ca cd0000        	call	_CLK_PeripheralClockConfig
 379                     ; 115  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
 381  00cd 5f            	clrw	x
 382  00ce cd0000        	call	_CLK_PeripheralClockConfig
 384                     ; 116  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
 386  00d1 ae1301        	ldw	x,#4865
 387  00d4 cd0000        	call	_CLK_PeripheralClockConfig
 389                     ; 117  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
 391  00d7 ae1200        	ldw	x,#4608
 392  00da cd0000        	call	_CLK_PeripheralClockConfig
 394                     ; 118  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
 396  00dd ae0300        	ldw	x,#768
 397  00e0 cd0000        	call	_CLK_PeripheralClockConfig
 399                     ; 119  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
 401  00e3 ae0700        	ldw	x,#1792
 402  00e6 cd0000        	call	_CLK_PeripheralClockConfig
 404                     ; 120  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
 406  00e9 ae0500        	ldw	x,#1280
 407  00ec cd0000        	call	_CLK_PeripheralClockConfig
 409                     ; 121  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);;
 411  00ef ae0401        	ldw	x,#1025
 412  00f2 cd0000        	call	_CLK_PeripheralClockConfig
 414                     ; 122 }
 418  00f5 81            	ret
 446                     ; 125 void Timer4_Init(void) {
 447                     	switch	.text
 448  00f6               _Timer4_Init:
 452                     ; 126 	 TIM4_DeInit();
 454  00f6 cd0000        	call	_TIM4_DeInit
 456                     ; 127 	 TIM4_TimeBaseInit(TIM4_PRESCALER_64, 156);      
 458  00f9 ae069c        	ldw	x,#1692
 459  00fc cd0000        	call	_TIM4_TimeBaseInit
 461                     ; 128 	 TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
 463  00ff ae0101        	ldw	x,#257
 464  0102 cd0000        	call	_TIM4_ITConfig
 466                     ; 129 	 TIM4_Cmd(ENABLE);
 468  0105 a601          	ld	a,#1
 469  0107 cd0000        	call	_TIM4_Cmd
 471                     ; 131 	 enableInterrupts();
 474  010a 9a            rim
 476                     ; 132 }
 480  010b 81            	ret
 551                     	xdef	_main
 552                     	xdef	_Timer4_Init
 553                     	xdef	_CLK_Init
 554                     	xdef	_GPIO_Int
 555                     	xdef	_Delay_us
 556                     	xdef	_Segment_Patterns
 557                     	switch	.ubsct
 558  0000               _i:
 559  0000 0000          	ds.b	2
 560                     	xdef	_i
 561  0002               _pattern:
 562  0002 00            	ds.b	1
 563                     	xdef	_pattern
 564  0003               L3_counter:
 565  0003 0000          	ds.b	2
 566                     	xdef	_digits
 567                     	xdef	_currentDigit
 568                     	xref	_TIM4_ITConfig
 569                     	xref	_TIM4_Cmd
 570                     	xref	_TIM4_TimeBaseInit
 571                     	xref	_TIM4_DeInit
 572                     	xref	_GPIO_Init
 573                     	xref	_GPIO_DeInit
 574                     	xref	_CLK_GetFlagStatus
 575                     	xref	_CLK_SYSCLKConfig
 576                     	xref	_CLK_HSIPrescalerConfig
 577                     	xref	_CLK_ClockSwitchConfig
 578                     	xref	_CLK_PeripheralClockConfig
 579                     	xref	_CLK_ClockSwitchCmd
 580                     	xref	_CLK_LSICmd
 581                     	xref	_CLK_HSICmd
 582                     	xref	_CLK_HSECmd
 583                     	xref	_CLK_DeInit
 603                     	end
