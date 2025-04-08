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
  91                     ; 45 	GPIO_WriteHigh(GPIOB, GPIO_PIN_4);
  93  002d 4b10          	push	#16
  94  002f ae5005        	ldw	x,#20485
  95  0032 cd0000        	call	_GPIO_WriteHigh
  97  0035 84            	pop	a
  98                     ; 46 	GPIO_WriteHigh(GPIOB, GPIO_PIN_5);
 100  0036 4b20          	push	#32
 101  0038 ae5005        	ldw	x,#20485
 102  003b cd0000        	call	_GPIO_WriteHigh
 104  003e 84            	pop	a
 105                     ; 47 	GPIO_WriteHigh(GPIOA, GPIO_PIN_3);
 107  003f 4b08          	push	#8
 108  0041 ae5000        	ldw	x,#20480
 109  0044 cd0000        	call	_GPIO_WriteHigh
 111  0047 84            	pop	a
 112                     ; 50 	pattern = Segment_Patterns[digits[seg]];
 114  0048 b600          	ld	a,_seg
 115  004a 5f            	clrw	x
 116  004b 97            	ld	xl,a
 117  004c e600          	ld	a,(_digits,x)
 118  004e 5f            	clrw	x
 119  004f 97            	ld	xl,a
 120  0050 d60000        	ld	a,(_Segment_Patterns,x)
 121  0053 b700          	ld	_pattern,a
 122                     ; 53 	if (pattern & 0b01000000) GPIO_WriteLow(GPIOA, GPIO_PIN_1); else GPIO_WriteHigh(GPIOA, GPIO_PIN_1); // A
 124  0055 b600          	ld	a,_pattern
 125  0057 a540          	bcp	a,#64
 126  0059 270b          	jreq	L53
 129  005b 4b02          	push	#2
 130  005d ae5000        	ldw	x,#20480
 131  0060 cd0000        	call	_GPIO_WriteLow
 133  0063 84            	pop	a
 135  0064 2009          	jra	L73
 136  0066               L53:
 139  0066 4b02          	push	#2
 140  0068 ae5000        	ldw	x,#20480
 141  006b cd0000        	call	_GPIO_WriteHigh
 143  006e 84            	pop	a
 144  006f               L73:
 145                     ; 54 	if (pattern & 0b00100000) GPIO_WriteLow(GPIOA, GPIO_PIN_2); else GPIO_WriteHigh(GPIOA, GPIO_PIN_2); // B
 147  006f b600          	ld	a,_pattern
 148  0071 a520          	bcp	a,#32
 149  0073 270b          	jreq	L14
 152  0075 4b04          	push	#4
 153  0077 ae5000        	ldw	x,#20480
 154  007a cd0000        	call	_GPIO_WriteLow
 156  007d 84            	pop	a
 158  007e 2009          	jra	L34
 159  0080               L14:
 162  0080 4b04          	push	#4
 163  0082 ae5000        	ldw	x,#20480
 164  0085 cd0000        	call	_GPIO_WriteHigh
 166  0088 84            	pop	a
 167  0089               L34:
 168                     ; 55 	if (pattern & 0b00010000) GPIO_WriteLow(GPIOC, GPIO_PIN_4); else GPIO_WriteHigh(GPIOC, GPIO_PIN_4); // C
 170  0089 b600          	ld	a,_pattern
 171  008b a510          	bcp	a,#16
 172  008d 270b          	jreq	L54
 175  008f 4b10          	push	#16
 176  0091 ae500a        	ldw	x,#20490
 177  0094 cd0000        	call	_GPIO_WriteLow
 179  0097 84            	pop	a
 181  0098 2009          	jra	L74
 182  009a               L54:
 185  009a 4b10          	push	#16
 186  009c ae500a        	ldw	x,#20490
 187  009f cd0000        	call	_GPIO_WriteHigh
 189  00a2 84            	pop	a
 190  00a3               L74:
 191                     ; 56 	if (pattern & 0b00001000) GPIO_WriteLow(GPIOC, GPIO_PIN_6); else GPIO_WriteHigh(GPIOC, GPIO_PIN_6); // D
 193  00a3 b600          	ld	a,_pattern
 194  00a5 a508          	bcp	a,#8
 195  00a7 270b          	jreq	L15
 198  00a9 4b40          	push	#64
 199  00ab ae500a        	ldw	x,#20490
 200  00ae cd0000        	call	_GPIO_WriteLow
 202  00b1 84            	pop	a
 204  00b2 2009          	jra	L35
 205  00b4               L15:
 208  00b4 4b40          	push	#64
 209  00b6 ae500a        	ldw	x,#20490
 210  00b9 cd0000        	call	_GPIO_WriteHigh
 212  00bc 84            	pop	a
 213  00bd               L35:
 214                     ; 57 	if (pattern & 0b00000100) GPIO_WriteLow(GPIOC, GPIO_PIN_7); else GPIO_WriteHigh(GPIOC, GPIO_PIN_7); // E
 216  00bd b600          	ld	a,_pattern
 217  00bf a504          	bcp	a,#4
 218  00c1 270b          	jreq	L55
 221  00c3 4b80          	push	#128
 222  00c5 ae500a        	ldw	x,#20490
 223  00c8 cd0000        	call	_GPIO_WriteLow
 225  00cb 84            	pop	a
 227  00cc 2009          	jra	L75
 228  00ce               L55:
 231  00ce 4b80          	push	#128
 232  00d0 ae500a        	ldw	x,#20490
 233  00d3 cd0000        	call	_GPIO_WriteHigh
 235  00d6 84            	pop	a
 236  00d7               L75:
 237                     ; 58 	if (pattern & 0b00000010) GPIO_WriteLow(GPIOC, GPIO_PIN_5); else GPIO_WriteHigh(GPIOC, GPIO_PIN_5); // F
 239  00d7 b600          	ld	a,_pattern
 240  00d9 a502          	bcp	a,#2
 241  00db 270b          	jreq	L16
 244  00dd 4b20          	push	#32
 245  00df ae500a        	ldw	x,#20490
 246  00e2 cd0000        	call	_GPIO_WriteLow
 248  00e5 84            	pop	a
 250  00e6 2009          	jra	L36
 251  00e8               L16:
 254  00e8 4b20          	push	#32
 255  00ea ae500a        	ldw	x,#20490
 256  00ed cd0000        	call	_GPIO_WriteHigh
 258  00f0 84            	pop	a
 259  00f1               L36:
 260                     ; 59 	if (pattern & 0b00000001) GPIO_WriteLow(GPIOC, GPIO_PIN_3); else GPIO_WriteHigh(GPIOC, GPIO_PIN_3); // G
 262  00f1 b600          	ld	a,_pattern
 263  00f3 a501          	bcp	a,#1
 264  00f5 270b          	jreq	L56
 267  00f7 4b08          	push	#8
 268  00f9 ae500a        	ldw	x,#20490
 269  00fc cd0000        	call	_GPIO_WriteLow
 271  00ff 84            	pop	a
 273  0100 2009          	jra	L76
 274  0102               L56:
 277  0102 4b08          	push	#8
 278  0104 ae500a        	ldw	x,#20490
 279  0107 cd0000        	call	_GPIO_WriteHigh
 281  010a 84            	pop	a
 282  010b               L76:
 283                     ; 62 	switch (seg) {
 285  010b b600          	ld	a,_seg
 287                     ; 68 		case 5: GPIO_WriteLow(GPIOA, GPIO_PIN_3); break;
 288  010d 4d            	tnz	a
 289  010e 2711          	jreq	L3
 290  0110 4a            	dec	a
 291  0111 2719          	jreq	L5
 292  0113 4a            	dec	a
 293  0114 2721          	jreq	L7
 294  0116 4a            	dec	a
 295  0117 2729          	jreq	L11
 296  0119 4a            	dec	a
 297  011a 2731          	jreq	L31
 298  011c 4a            	dec	a
 299  011d 2739          	jreq	L51
 300  011f 2040          	jra	L37
 301  0121               L3:
 302                     ; 63 		case 0: GPIO_WriteLow(GPIOD, GPIO_PIN_4); break;  
 304  0121 4b10          	push	#16
 305  0123 ae500f        	ldw	x,#20495
 306  0126 cd0000        	call	_GPIO_WriteLow
 308  0129 84            	pop	a
 311  012a 2035          	jra	L37
 312  012c               L5:
 313                     ; 64 		case 1: GPIO_WriteLow(GPIOD, GPIO_PIN_6); break;
 315  012c 4b40          	push	#64
 316  012e ae500f        	ldw	x,#20495
 317  0131 cd0000        	call	_GPIO_WriteLow
 319  0134 84            	pop	a
 322  0135 202a          	jra	L37
 323  0137               L7:
 324                     ; 65 		case 2: GPIO_WriteLow(GPIOD, GPIO_PIN_5); break;
 326  0137 4b20          	push	#32
 327  0139 ae500f        	ldw	x,#20495
 328  013c cd0000        	call	_GPIO_WriteLow
 330  013f 84            	pop	a
 333  0140 201f          	jra	L37
 334  0142               L11:
 335                     ; 66 		case 3: GPIO_WriteLow(GPIOB, GPIO_PIN_4); break;  
 337  0142 4b10          	push	#16
 338  0144 ae5005        	ldw	x,#20485
 339  0147 cd0000        	call	_GPIO_WriteLow
 341  014a 84            	pop	a
 344  014b 2014          	jra	L37
 345  014d               L31:
 346                     ; 67 		case 4: GPIO_WriteLow(GPIOB, GPIO_PIN_5); break;
 348  014d 4b20          	push	#32
 349  014f ae5005        	ldw	x,#20485
 350  0152 cd0000        	call	_GPIO_WriteLow
 352  0155 84            	pop	a
 355  0156 2009          	jra	L37
 356  0158               L51:
 357                     ; 68 		case 5: GPIO_WriteLow(GPIOA, GPIO_PIN_3); break;
 359  0158 4b08          	push	#8
 360  015a ae5000        	ldw	x,#20480
 361  015d cd0000        	call	_GPIO_WriteLow
 363  0160 84            	pop	a
 366  0161               L37:
 367                     ; 72 	seg = (seg + 1) % 6;
 369  0161 b600          	ld	a,_seg
 370  0163 5f            	clrw	x
 371  0164 97            	ld	xl,a
 372  0165 5c            	incw	x
 373  0166 a606          	ld	a,#6
 374  0168 cd0000        	call	c_smodx
 376  016b 01            	rrwa	x,a
 377  016c b700          	ld	_seg,a
 378  016e 02            	rlwa	x,a
 379                     ; 74 	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
 381  016f a601          	ld	a,#1
 382  0171 cd0000        	call	_TIM4_ClearFlag
 384                     ; 75 }
 387  0174 85            	popw	x
 388  0175 bf00          	ldw	c_y,x
 389  0177 320002        	pop	c_y+2
 390  017a 85            	popw	x
 391  017b bf00          	ldw	c_x,x
 392  017d 320002        	pop	c_x+2
 393  0180 80            	iret
 416                     ; 96 INTERRUPT_HANDLER(NonHandledInterrupt, 25)
 416                     ; 97 {
 417                     	switch	.text
 418  0181               f_NonHandledInterrupt:
 422                     ; 101 }
 425  0181 80            	iret
 447                     ; 109 INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
 447                     ; 110 {
 448                     	switch	.text
 449  0182               f_TRAP_IRQHandler:
 453                     ; 114 }
 456  0182 80            	iret
 478                     ; 121 INTERRUPT_HANDLER(TLI_IRQHandler, 0)
 478                     ; 122 
 478                     ; 123 {
 479                     	switch	.text
 480  0183               f_TLI_IRQHandler:
 484                     ; 127 }
 487  0183 80            	iret
 509                     ; 134 INTERRUPT_HANDLER(AWU_IRQHandler, 1)
 509                     ; 135 {
 510                     	switch	.text
 511  0184               f_AWU_IRQHandler:
 515                     ; 139 }
 518  0184 80            	iret
 540                     ; 146 INTERRUPT_HANDLER(CLK_IRQHandler, 2)
 540                     ; 147 {
 541                     	switch	.text
 542  0185               f_CLK_IRQHandler:
 546                     ; 151 }
 549  0185 80            	iret
 572                     ; 158 INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
 572                     ; 159 {
 573                     	switch	.text
 574  0186               f_EXTI_PORTA_IRQHandler:
 578                     ; 163 }
 581  0186 80            	iret
 604                     ; 170 INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
 604                     ; 171 {
 605                     	switch	.text
 606  0187               f_EXTI_PORTB_IRQHandler:
 610                     ; 175 }
 613  0187 80            	iret
 636                     ; 182 INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
 636                     ; 183 {
 637                     	switch	.text
 638  0188               f_EXTI_PORTC_IRQHandler:
 642                     ; 187 }
 645  0188 80            	iret
 668                     ; 194 INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
 668                     ; 195 {
 669                     	switch	.text
 670  0189               f_EXTI_PORTD_IRQHandler:
 674                     ; 199 }
 677  0189 80            	iret
 700                     ; 206 INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
 700                     ; 207 {
 701                     	switch	.text
 702  018a               f_EXTI_PORTE_IRQHandler:
 706                     ; 211 }
 709  018a 80            	iret
 731                     ; 258 INTERRUPT_HANDLER(SPI_IRQHandler, 10)
 731                     ; 259 {
 732                     	switch	.text
 733  018b               f_SPI_IRQHandler:
 737                     ; 263 }
 740  018b 80            	iret
 763                     ; 270 INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
 763                     ; 271 {
 764                     	switch	.text
 765  018c               f_TIM1_UPD_OVF_TRG_BRK_IRQHandler:
 769                     ; 275 }
 772  018c 80            	iret
 795                     ; 282 INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12)
 795                     ; 283 {
 796                     	switch	.text
 797  018d               f_TIM1_CAP_COM_IRQHandler:
 801                     ; 287 }
 804  018d 80            	iret
 827                     ; 320  INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
 827                     ; 321  {
 828                     	switch	.text
 829  018e               f_TIM2_UPD_OVF_BRK_IRQHandler:
 833                     ; 325  }
 836  018e 80            	iret
 859                     ; 332  INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
 859                     ; 333  {
 860                     	switch	.text
 861  018f               f_TIM2_CAP_COM_IRQHandler:
 865                     ; 337  }
 868  018f 80            	iret
 891                     ; 374  INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
 891                     ; 375  {
 892                     	switch	.text
 893  0190               f_UART1_TX_IRQHandler:
 897                     ; 379  }
 900  0190 80            	iret
 923                     ; 386  INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
 923                     ; 387  {
 924                     	switch	.text
 925  0191               f_UART1_RX_IRQHandler:
 929                     ; 391  }
 932  0191 80            	iret
 954                     ; 425 INTERRUPT_HANDLER(I2C_IRQHandler, 19)
 954                     ; 426 {
 955                     	switch	.text
 956  0192               f_I2C_IRQHandler:
 960                     ; 430 }
 963  0192 80            	iret
 985                     ; 504  INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
 985                     ; 505  {
 986                     	switch	.text
 987  0193               f_ADC1_IRQHandler:
 991                     ; 509  }
 994  0193 80            	iret
1017                     ; 530  INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
1017                     ; 531  {
1018                     	switch	.text
1019  0194               f_TIM4_UPD_OVF_IRQHandler:
1023                     ; 535  }
1026  0194 80            	iret
1049                     ; 543 INTERRUPT_HANDLER(EEPROM_EEC_IRQHandler, 24)
1049                     ; 544 {
1050                     	switch	.text
1051  0195               f_EEPROM_EEC_IRQHandler:
1055                     ; 548 }
1058  0195 80            	iret
1081                     	xdef	_seg
1082                     	xref.b	_pattern
1083                     	xref	_Segment_Patterns
1084                     	xref.b	_digits
1085                     	xdef	f_EEPROM_EEC_IRQHandler
1086                     	xdef	f_TIM4_UPD_OVF_IRQHandler
1087                     	xdef	f_ADC1_IRQHandler
1088                     	xdef	f_I2C_IRQHandler
1089                     	xdef	f_UART1_RX_IRQHandler
1090                     	xdef	f_UART1_TX_IRQHandler
1091                     	xdef	f_TIM2_CAP_COM_IRQHandler
1092                     	xdef	f_TIM2_UPD_OVF_BRK_IRQHandler
1093                     	xdef	f_TIM1_UPD_OVF_TRG_BRK_IRQHandler
1094                     	xdef	f_TIM1_CAP_COM_IRQHandler
1095                     	xdef	f_SPI_IRQHandler
1096                     	xdef	f_EXTI_PORTE_IRQHandler
1097                     	xdef	f_EXTI_PORTD_IRQHandler
1098                     	xdef	f_EXTI_PORTC_IRQHandler
1099                     	xdef	f_EXTI_PORTB_IRQHandler
1100                     	xdef	f_EXTI_PORTA_IRQHandler
1101                     	xdef	f_CLK_IRQHandler
1102                     	xdef	f_AWU_IRQHandler
1103                     	xdef	f_TLI_IRQHandler
1104                     	xdef	f_TRAP_IRQHandler
1105                     	xdef	f_NonHandledInterrupt
1106                     	xref	_TIM4_ClearFlag
1107                     	xdef	f_TIM4_UPD_IRQHandler
1108                     	xref	_GPIO_WriteLow
1109                     	xref	_GPIO_WriteHigh
1110                     	xref.b	c_x
1111                     	xref.b	c_y
1130                     	xref	c_smodx
1131                     	end
