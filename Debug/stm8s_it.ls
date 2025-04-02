   1                     ; C Compiler for STM8 (COSMIC Software)
   2                     ; Parser V4.13.2 - 04 Jun 2024
   3                     ; Generator (Limited) V4.6.4 - 15 Jan 2025
  14                     	bsct
  15  0000               _seg:
  16  0000 00            	dc.b	0
  53                     ; 39 void TIM4_UPD_IRQHandler(void)
  53                     ; 40 {	
  54                     	switch	.text
  55  0000               f_TIM4_UPD_IRQHandler:
  57  0000 8a            	push	cc
  58  0001 84            	pop	a
  59  0002 a4bf          	and	a,#191
  60  0004 88            	push	a
  61  0005 86            	pop	cc
  62  0006 3b0002        	push	c_x+2
  63  0009 be00          	ldw	x,c_x
  64  000b 89            	pushw	x
  65  000c 3b0002        	push	c_y+2
  66  000f be00          	ldw	x,c_y
  67  0011 89            	pushw	x
  70                     ; 42 	GPIO_WriteHigh(GPIOD, GPIO_PIN_4);
  72  0012 4b10          	push	#16
  73  0014 ae500f        	ldw	x,#20495
  74  0017 cd0000        	call	_GPIO_WriteHigh
  76  001a 84            	pop	a
  77                     ; 43 	GPIO_WriteHigh(GPIOD, GPIO_PIN_6);
  79  001b 4b40          	push	#64
  80  001d ae500f        	ldw	x,#20495
  81  0020 cd0000        	call	_GPIO_WriteHigh
  83  0023 84            	pop	a
  84                     ; 44 	GPIO_WriteHigh(GPIOD, GPIO_PIN_5);
  86  0024 4b20          	push	#32
  87  0026 ae500f        	ldw	x,#20495
  88  0029 cd0000        	call	_GPIO_WriteHigh
  90  002c 84            	pop	a
  91                     ; 47 	pattern = Segment_Patterns[digits[seg]];
  93  002d b600          	ld	a,_seg
  94  002f 5f            	clrw	x
  95  0030 97            	ld	xl,a
  96  0031 e600          	ld	a,(_digits,x)
  97  0033 5f            	clrw	x
  98  0034 97            	ld	xl,a
  99  0035 d60000        	ld	a,(_Segment_Patterns,x)
 100  0038 b700          	ld	_pattern,a
 101                     ; 50 	if (pattern & 0b01000000) GPIO_WriteLow(GPIOA, GPIO_PIN_1); else GPIO_WriteHigh(GPIOA, GPIO_PIN_1); // A
 103  003a b600          	ld	a,_pattern
 104  003c a540          	bcp	a,#64
 105  003e 270b          	jreq	L12
 108  0040 4b02          	push	#2
 109  0042 ae5000        	ldw	x,#20480
 110  0045 cd0000        	call	_GPIO_WriteLow
 112  0048 84            	pop	a
 114  0049 2009          	jra	L32
 115  004b               L12:
 118  004b 4b02          	push	#2
 119  004d ae5000        	ldw	x,#20480
 120  0050 cd0000        	call	_GPIO_WriteHigh
 122  0053 84            	pop	a
 123  0054               L32:
 124                     ; 51 	if (pattern & 0b00100000) GPIO_WriteLow(GPIOA, GPIO_PIN_2); else GPIO_WriteHigh(GPIOA, GPIO_PIN_2); // B
 126  0054 b600          	ld	a,_pattern
 127  0056 a520          	bcp	a,#32
 128  0058 270b          	jreq	L52
 131  005a 4b04          	push	#4
 132  005c ae5000        	ldw	x,#20480
 133  005f cd0000        	call	_GPIO_WriteLow
 135  0062 84            	pop	a
 137  0063 2009          	jra	L72
 138  0065               L52:
 141  0065 4b04          	push	#4
 142  0067 ae5000        	ldw	x,#20480
 143  006a cd0000        	call	_GPIO_WriteHigh
 145  006d 84            	pop	a
 146  006e               L72:
 147                     ; 52 	if (pattern & 0b00010000) GPIO_WriteLow(GPIOC, GPIO_PIN_4); else GPIO_WriteHigh(GPIOC, GPIO_PIN_4); // C
 149  006e b600          	ld	a,_pattern
 150  0070 a510          	bcp	a,#16
 151  0072 270b          	jreq	L13
 154  0074 4b10          	push	#16
 155  0076 ae500a        	ldw	x,#20490
 156  0079 cd0000        	call	_GPIO_WriteLow
 158  007c 84            	pop	a
 160  007d 2009          	jra	L33
 161  007f               L13:
 164  007f 4b10          	push	#16
 165  0081 ae500a        	ldw	x,#20490
 166  0084 cd0000        	call	_GPIO_WriteHigh
 168  0087 84            	pop	a
 169  0088               L33:
 170                     ; 53 	if (pattern & 0b00001000) GPIO_WriteLow(GPIOC, GPIO_PIN_6); else GPIO_WriteHigh(GPIOC, GPIO_PIN_6); // D
 172  0088 b600          	ld	a,_pattern
 173  008a a508          	bcp	a,#8
 174  008c 270b          	jreq	L53
 177  008e 4b40          	push	#64
 178  0090 ae500a        	ldw	x,#20490
 179  0093 cd0000        	call	_GPIO_WriteLow
 181  0096 84            	pop	a
 183  0097 2009          	jra	L73
 184  0099               L53:
 187  0099 4b40          	push	#64
 188  009b ae500a        	ldw	x,#20490
 189  009e cd0000        	call	_GPIO_WriteHigh
 191  00a1 84            	pop	a
 192  00a2               L73:
 193                     ; 54 	if (pattern & 0b00000100) GPIO_WriteLow(GPIOC, GPIO_PIN_7); else GPIO_WriteHigh(GPIOC, GPIO_PIN_7); // E
 195  00a2 b600          	ld	a,_pattern
 196  00a4 a504          	bcp	a,#4
 197  00a6 270b          	jreq	L14
 200  00a8 4b80          	push	#128
 201  00aa ae500a        	ldw	x,#20490
 202  00ad cd0000        	call	_GPIO_WriteLow
 204  00b0 84            	pop	a
 206  00b1 2009          	jra	L34
 207  00b3               L14:
 210  00b3 4b80          	push	#128
 211  00b5 ae500a        	ldw	x,#20490
 212  00b8 cd0000        	call	_GPIO_WriteHigh
 214  00bb 84            	pop	a
 215  00bc               L34:
 216                     ; 55 	if (pattern & 0b00000010) GPIO_WriteLow(GPIOC, GPIO_PIN_5); else GPIO_WriteHigh(GPIOC, GPIO_PIN_5); // F
 218  00bc b600          	ld	a,_pattern
 219  00be a502          	bcp	a,#2
 220  00c0 270b          	jreq	L54
 223  00c2 4b20          	push	#32
 224  00c4 ae500a        	ldw	x,#20490
 225  00c7 cd0000        	call	_GPIO_WriteLow
 227  00ca 84            	pop	a
 229  00cb 2009          	jra	L74
 230  00cd               L54:
 233  00cd 4b20          	push	#32
 234  00cf ae500a        	ldw	x,#20490
 235  00d2 cd0000        	call	_GPIO_WriteHigh
 237  00d5 84            	pop	a
 238  00d6               L74:
 239                     ; 56 	if (pattern & 0b00000001) GPIO_WriteLow(GPIOC, GPIO_PIN_3); else GPIO_WriteHigh(GPIOC, GPIO_PIN_3); // G
 241  00d6 b600          	ld	a,_pattern
 242  00d8 a501          	bcp	a,#1
 243  00da 270b          	jreq	L15
 246  00dc 4b08          	push	#8
 247  00de ae500a        	ldw	x,#20490
 248  00e1 cd0000        	call	_GPIO_WriteLow
 250  00e4 84            	pop	a
 252  00e5 2009          	jra	L35
 253  00e7               L15:
 256  00e7 4b08          	push	#8
 257  00e9 ae500a        	ldw	x,#20490
 258  00ec cd0000        	call	_GPIO_WriteHigh
 260  00ef 84            	pop	a
 261  00f0               L35:
 262                     ; 59 	if (seg == 0) GPIO_WriteLow(GPIOD, GPIO_PIN_4);
 264  00f0 3d00          	tnz	_seg
 265  00f2 260b          	jrne	L55
 268  00f4 4b10          	push	#16
 269  00f6 ae500f        	ldw	x,#20495
 270  00f9 cd0000        	call	_GPIO_WriteLow
 272  00fc 84            	pop	a
 274  00fd 2020          	jra	L75
 275  00ff               L55:
 276                     ; 60 	else if (seg == 1) GPIO_WriteLow(GPIOD, GPIO_PIN_6);
 278  00ff b600          	ld	a,_seg
 279  0101 a101          	cp	a,#1
 280  0103 260b          	jrne	L16
 283  0105 4b40          	push	#64
 284  0107 ae500f        	ldw	x,#20495
 285  010a cd0000        	call	_GPIO_WriteLow
 287  010d 84            	pop	a
 289  010e 200f          	jra	L75
 290  0110               L16:
 291                     ; 61 	else if (seg == 2) GPIO_WriteLow(GPIOD, GPIO_PIN_5);
 293  0110 b600          	ld	a,_seg
 294  0112 a102          	cp	a,#2
 295  0114 2609          	jrne	L75
 298  0116 4b20          	push	#32
 299  0118 ae500f        	ldw	x,#20495
 300  011b cd0000        	call	_GPIO_WriteLow
 302  011e 84            	pop	a
 303  011f               L75:
 304                     ; 64 	seg = (seg + 1) % 3;
 306  011f b600          	ld	a,_seg
 307  0121 5f            	clrw	x
 308  0122 97            	ld	xl,a
 309  0123 5c            	incw	x
 310  0124 a603          	ld	a,#3
 311  0126 cd0000        	call	c_smodx
 313  0129 01            	rrwa	x,a
 314  012a b700          	ld	_seg,a
 315  012c 02            	rlwa	x,a
 316                     ; 66 	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
 318  012d a601          	ld	a,#1
 319  012f cd0000        	call	_TIM4_ClearFlag
 321                     ; 67 }
 324  0132 85            	popw	x
 325  0133 bf00          	ldw	c_y,x
 326  0135 320002        	pop	c_y+2
 327  0138 85            	popw	x
 328  0139 bf00          	ldw	c_x,x
 329  013b 320002        	pop	c_x+2
 330  013e 80            	iret
 353                     ; 88 INTERRUPT_HANDLER(NonHandledInterrupt, 25)
 353                     ; 89 {
 354                     	switch	.text
 355  013f               f_NonHandledInterrupt:
 359                     ; 93 }
 362  013f 80            	iret
 384                     ; 101 INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
 384                     ; 102 {
 385                     	switch	.text
 386  0140               f_TRAP_IRQHandler:
 390                     ; 106 }
 393  0140 80            	iret
 415                     ; 113 INTERRUPT_HANDLER(TLI_IRQHandler, 0)
 415                     ; 114 
 415                     ; 115 {
 416                     	switch	.text
 417  0141               f_TLI_IRQHandler:
 421                     ; 119 }
 424  0141 80            	iret
 446                     ; 126 INTERRUPT_HANDLER(AWU_IRQHandler, 1)
 446                     ; 127 {
 447                     	switch	.text
 448  0142               f_AWU_IRQHandler:
 452                     ; 131 }
 455  0142 80            	iret
 477                     ; 138 INTERRUPT_HANDLER(CLK_IRQHandler, 2)
 477                     ; 139 {
 478                     	switch	.text
 479  0143               f_CLK_IRQHandler:
 483                     ; 143 }
 486  0143 80            	iret
 509                     ; 150 INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
 509                     ; 151 {
 510                     	switch	.text
 511  0144               f_EXTI_PORTA_IRQHandler:
 515                     ; 155 }
 518  0144 80            	iret
 541                     ; 162 INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
 541                     ; 163 {
 542                     	switch	.text
 543  0145               f_EXTI_PORTB_IRQHandler:
 547                     ; 167 }
 550  0145 80            	iret
 573                     ; 174 INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
 573                     ; 175 {
 574                     	switch	.text
 575  0146               f_EXTI_PORTC_IRQHandler:
 579                     ; 179 }
 582  0146 80            	iret
 605                     ; 186 INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
 605                     ; 187 {
 606                     	switch	.text
 607  0147               f_EXTI_PORTD_IRQHandler:
 611                     ; 191 }
 614  0147 80            	iret
 637                     ; 198 INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
 637                     ; 199 {
 638                     	switch	.text
 639  0148               f_EXTI_PORTE_IRQHandler:
 643                     ; 203 }
 646  0148 80            	iret
 668                     ; 250 INTERRUPT_HANDLER(SPI_IRQHandler, 10)
 668                     ; 251 {
 669                     	switch	.text
 670  0149               f_SPI_IRQHandler:
 674                     ; 255 }
 677  0149 80            	iret
 700                     ; 262 INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
 700                     ; 263 {
 701                     	switch	.text
 702  014a               f_TIM1_UPD_OVF_TRG_BRK_IRQHandler:
 706                     ; 267 }
 709  014a 80            	iret
 732                     ; 274 INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12)
 732                     ; 275 {
 733                     	switch	.text
 734  014b               f_TIM1_CAP_COM_IRQHandler:
 738                     ; 279 }
 741  014b 80            	iret
 764                     ; 312  INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
 764                     ; 313  {
 765                     	switch	.text
 766  014c               f_TIM2_UPD_OVF_BRK_IRQHandler:
 770                     ; 317  }
 773  014c 80            	iret
 796                     ; 324  INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
 796                     ; 325  {
 797                     	switch	.text
 798  014d               f_TIM2_CAP_COM_IRQHandler:
 802                     ; 329  }
 805  014d 80            	iret
 828                     ; 366  INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
 828                     ; 367  {
 829                     	switch	.text
 830  014e               f_UART1_TX_IRQHandler:
 834                     ; 371  }
 837  014e 80            	iret
 860                     ; 378  INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
 860                     ; 379  {
 861                     	switch	.text
 862  014f               f_UART1_RX_IRQHandler:
 866                     ; 383  }
 869  014f 80            	iret
 891                     ; 417 INTERRUPT_HANDLER(I2C_IRQHandler, 19)
 891                     ; 418 {
 892                     	switch	.text
 893  0150               f_I2C_IRQHandler:
 897                     ; 422 }
 900  0150 80            	iret
 922                     ; 496  INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
 922                     ; 497  {
 923                     	switch	.text
 924  0151               f_ADC1_IRQHandler:
 928                     ; 501  }
 931  0151 80            	iret
 954                     ; 522  INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
 954                     ; 523  {
 955                     	switch	.text
 956  0152               f_TIM4_UPD_OVF_IRQHandler:
 960                     ; 527  }
 963  0152 80            	iret
 986                     ; 535 INTERRUPT_HANDLER(EEPROM_EEC_IRQHandler, 24)
 986                     ; 536 {
 987                     	switch	.text
 988  0153               f_EEPROM_EEC_IRQHandler:
 992                     ; 540 }
 995  0153 80            	iret
1018                     	xdef	_seg
1019                     	xref.b	_pattern
1020                     	xref	_Segment_Patterns
1021                     	xref.b	_digits
1022                     	xdef	f_EEPROM_EEC_IRQHandler
1023                     	xdef	f_TIM4_UPD_OVF_IRQHandler
1024                     	xdef	f_ADC1_IRQHandler
1025                     	xdef	f_I2C_IRQHandler
1026                     	xdef	f_UART1_RX_IRQHandler
1027                     	xdef	f_UART1_TX_IRQHandler
1028                     	xdef	f_TIM2_CAP_COM_IRQHandler
1029                     	xdef	f_TIM2_UPD_OVF_BRK_IRQHandler
1030                     	xdef	f_TIM1_UPD_OVF_TRG_BRK_IRQHandler
1031                     	xdef	f_TIM1_CAP_COM_IRQHandler
1032                     	xdef	f_SPI_IRQHandler
1033                     	xdef	f_EXTI_PORTE_IRQHandler
1034                     	xdef	f_EXTI_PORTD_IRQHandler
1035                     	xdef	f_EXTI_PORTC_IRQHandler
1036                     	xdef	f_EXTI_PORTB_IRQHandler
1037                     	xdef	f_EXTI_PORTA_IRQHandler
1038                     	xdef	f_CLK_IRQHandler
1039                     	xdef	f_AWU_IRQHandler
1040                     	xdef	f_TLI_IRQHandler
1041                     	xdef	f_TRAP_IRQHandler
1042                     	xdef	f_NonHandledInterrupt
1043                     	xref	_TIM4_ClearFlag
1044                     	xdef	f_TIM4_UPD_IRQHandler
1045                     	xref	_GPIO_WriteLow
1046                     	xref	_GPIO_WriteHigh
1047                     	xref.b	c_x
1048                     	xref.b	c_y
1067                     	xref	c_smodx
1068                     	end
