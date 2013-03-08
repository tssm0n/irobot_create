	.file	"cm9600.c"
	.arch atmega168
__SREG__ = 0x3f
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__tmp_reg__ = 0
__zero_reg__ = 1
	.global __do_copy_data
	.global __do_clear_bss
	.lcomm gXringNdx,1
	.data
	.type	gBitnum, @object
	.size	gBitnum, 1
gBitnum:
	.byte	1
	.lcomm gXmtState,1
	.lcomm gRhead,1
	.type	gRcvrEnabled, @object
	.size	gRcvrEnabled, 1
gRcvrEnabled:
	.byte	1
	.lcomm gRtail,1
	.text
.global	aux_rcv_enable
	.type	aux_rcv_enable, @function
aux_rcv_enable:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	ldi r24,lo8(1)
	sts gRcvrEnabled,r24
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function aux_rcv_enable size 4 (3) */
	.size	aux_rcv_enable, .-aux_rcv_enable
.global	init_aux_UART
	.type	init_aux_UART, @function
init_aux_UART:
/* prologue: frame size=0 */
	push r17
/* prologue end (size=1) */
	mov r17,r22
/* #APP */
	cli
/* #NOAPP */
	sts gAuxPort,r24
	sts gXringNdx,__zero_reg__
	lds r30,gXring
	lds r31,(gXring)+1
	st Z,__zero_reg__
	sts gXmtState,__zero_reg__
	ldi r24,lo8(1)
	sts gBitnum,r24
	sts gRhead,__zero_reg__
	sts gRtail,__zero_reg__
	call aux_rcv_enable
	lds r24,gAuxPort
	ldi r20,lo8(1)
	ldi r21,hi8(1)
	movw r18,r20
	rjmp 2f
1:	lsl r18
	rol r19
2:	dec r24
	brpl 1b
	in r24,36-0x20
	or r24,r18
	out 36-0x20,r24
	lds r24,gAuxPort
	movw r18,r20
	rjmp 2f
1:	lsl r18
	rol r19
2:	dec r24
	brpl 1b
	in r24,37-0x20
	or r24,r18
	out 37-0x20,r24
	lds r24,gAuxPort
	movw r18,r20
	rjmp 2f
1:	lsl r18
	rol r19
2:	dec r24
	brpl 1b
	ldi r25,lo8(63)
	eor r25,r18
	lds r24,126
	and r24,r25
	sts 126,r24
	lds r24,gAuxPort
	movw r18,r20
	rjmp 2f
1:	lsl r18
	rol r19
2:	dec r24
	brpl 1b
	mov r25,r18
	com r25
	in r24,39-0x20
	and r24,r25
	out 39-0x20,r24
	lds r24,gAuxPort
	rjmp 2f
1:	lsl r20
	rol r21
2:	dec r24
	brpl 1b
	in r24,40-0x20
	or r24,r20
	out 40-0x20,r24
	ldi r25,lo8(2)
	sts 176,r25
	sts 179,r17
	subi r17,lo8(-(-1))
	sts 178,r17
	ldi r24,lo8(3)
	sts 177,r24
	sts 112,r25
/* #APP */
	sei
/* #NOAPP */
/* epilogue: frame size=0 */
	pop r17
	ret
/* epilogue end (size=2) */
/* function init_aux_UART size 108 (105) */
	.size	init_aux_UART, .-init_aux_UART
.global	aux_UART_busy
	.type	aux_UART_busy, @function
aux_UART_busy:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	ldi r18,lo8(0)
	ldi r19,hi8(0)
	lds r24,gXmtState
	tst r24
	breq .L4
	ldi r18,lo8(1)
	ldi r19,hi8(1)
.L4:
	movw r24,r18
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function aux_UART_busy size 10 (9) */
	.size	aux_UART_busy, .-aux_UART_busy
.global	ByteXmtAux
	.type	ByteXmtAux, @function
ByteXmtAux:
/* prologue: frame size=0 */
	push r28
/* prologue end (size=1) */
	mov r28,r24
/* #APP */
	sei
/* #NOAPP */
	rjmp .L6
.L8:
/* #APP */
	nop
/* #NOAPP */
.L6:
	call aux_UART_busy
	tst r24
	brne .L8
/* #APP */
	cli
/* #NOAPP */
	sts gUartXmt,r28
	ldi r24,lo8(1)
	sts gXmtState,r24
/* #APP */
	sei
/* #NOAPP */
/* epilogue: frame size=0 */
	pop r28
	ret
/* epilogue end (size=2) */
/* function ByteXmtAux size 22 (19) */
	.size	ByteXmtAux, .-ByteXmtAux
.global	aux_send_line
	.type	aux_send_line, @function
aux_send_line:
/* prologue: frame size=0 */
	push r17
	push r28
	push r29
/* prologue end (size=3) */
	movw r28,r24
	ldi r17,lo8(0)
/* #APP */
	sei
/* #NOAPP */
	rjmp .L10
.L12:
/* #APP */
	nop
/* #NOAPP */
.L10:
	call aux_UART_busy
	tst r24
	brne .L12
	movw r18,r28
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	sts (gXring)+1,r19
	sts gXring,r18
	ldd r24,Y+1
.L18:
	tst r24
	breq .L17
	subi r17,lo8(-(1))
	movw r30,r18
	add r30,r17
	adc r31,__zero_reg__
	ld r24,Z
	rjmp .L18
.L17:
/* #APP */
	cli
/* #NOAPP */
	sts gXringNdx,r17
	ld r24,Y
	sts gUartXmt,r24
	ldi r24,lo8(1)
	sts gXmtState,r24
/* #APP */
	sei
/* #NOAPP */
/* epilogue: frame size=0 */
	pop r29
	pop r28
	pop r17
	ret
/* epilogue end (size=4) */
/* function aux_send_line size 46 (39) */
	.size	aux_send_line, .-aux_send_line
.global	aux_char_rdy
	.type	aux_char_rdy, @function
aux_char_rdy:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	lds r25,gRhead
	lds r24,gRtail
	cp r25,r24
	brlo .L20
	lds r24,gRhead
	lds r25,gRtail
	sub r24,r25
	clr r25
	ret
.L20:
	lds r24,gRtail
	lds r25,gRhead
	sub r24,r25
	ldi r25,lo8(48)
	sub r25,r24
	mov r24,r25
	clr r25
	ret
/* epilogue: frame size=0 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function aux_char_rdy size 23 (23) */
	.size	aux_char_rdy, .-aux_char_rdy
.global	aux_read_char
	.type	aux_read_char, @function
aux_read_char:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	lds r24,gRtail
	mov r30,r24
	clr r31
	subi r30,lo8(-(gRring))
	sbci r31,hi8(-(gRring))
	subi r24,lo8(-(1))
	sts gRtail,r24
	ld r30,Z
	lds r24,gRtail
	cpi r24,lo8(48)
	brlo .L23
	sts gRtail,__zero_reg__
.L23:
	mov r24,r30
	clr r25
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function aux_read_char size 19 (18) */
	.size	aux_read_char, .-aux_read_char
.global	aux_rcv_disable
	.type	aux_rcv_disable, @function
aux_rcv_disable:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	sts gRcvrEnabled,__zero_reg__
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function aux_rcv_disable size 3 (2) */
	.size	aux_rcv_disable, .-aux_rcv_disable
	.lcomm ringNdx.0,1
	.lcomm irqcnt.1,1
	.lcomm rcving.2,1
	.lcomm bittime.3,1
	.lcomm rcvbyte.4,1
.global	__vector_7
	.type	__vector_7, @function
__vector_7:
/* prologue: frame size=0 */
	push __zero_reg__
	push __tmp_reg__
	in __tmp_reg__,__SREG__
	push __tmp_reg__
	clr __zero_reg__
	push r18
	push r19
	push r24
	push r25
	push r30
	push r31
/* prologue end (size=11) */
	lds r24,irqcnt.1
	subi r24,lo8(-(1))
	sts irqcnt.1,r24
	lds r24,irqcnt.1
	cpi r24,lo8(3)
	brlo .L26
	sts irqcnt.1,__zero_reg__
.L26:
	lds r24,gRcvrEnabled
	tst r24
	brne .+2
	rjmp .L27
	lds r19,rcving.2
	tst r19
	breq .L28
	lds r25,irqcnt.1
	lds r24,bittime.3
	cp r25,r24
	breq .+2
	rjmp .L36
	lds r24,rcvbyte.4
	lsr r24
	sts rcvbyte.4,r24
	in r24,38-0x20
	clr r25
	lds r18,gAuxPort
	rjmp 2f
1:	asr r25
	ror r24
2:	dec r18
	brpl 1b
	sbrs r24,0
	rjmp .L30
	lds r24,rcvbyte.4
	ori r24,lo8(-128)
	sts rcvbyte.4,r24
.L30:
	lds r24,rcving.2
	subi r24,lo8(-(1))
	sts rcving.2,r24
	lds r24,rcving.2
	cpi r24,lo8(10)
	brlt .L36
	sts rcving.2,__zero_reg__
	lds r25,gRhead
	mov r30,r25
	clr r31
	subi r30,lo8(-(gRring))
	sbci r31,hi8(-(gRring))
	lds r24,rcvbyte.4
	st Z,r24
	subi r25,lo8(-(1))
	sts gRhead,r25
	lds r24,gRhead
	cpi r24,lo8(48)
	brlo .L36
	sts gRhead,__zero_reg__
	rjmp .L36
.L28:
	in r24,38-0x20
	clr r25
	lds r18,gAuxPort
	rjmp 2f
1:	asr r25
	ror r24
2:	dec r18
	brpl 1b
	sbrc r24,0
	rjmp .L36
	lds r24,irqcnt.1
	subi r24,lo8(-(1))
	sts bittime.3,r24
	lds r24,bittime.3
	subi r24,lo8(-(1))
	sts bittime.3,r24
	lds r24,bittime.3
	cpi r24,lo8(3)
	brlo .L35
	sts bittime.3,r19
.L35:
	ldi r24,lo8(1)
	sts rcving.2,r24
	sts rcvbyte.4,r19
	rjmp .L36
.L27:
	sts rcving.2,r24
.L36:
	lds r24,irqcnt.1
	tst r24
	breq .+2
	rjmp .L25
	lds r24,gXmtState
	mov r18,r24
	clr r19
	cpi r18,2
	cpc r19,__zero_reg__
	breq .L40
	cpi r18,3
	cpc r19,__zero_reg__
	brge .L49
	cpi r18,1
	cpc r19,__zero_reg__
	breq .L39
	rjmp .L25
.L49:
	cpi r18,3
	cpc r19,__zero_reg__
	brne .+2
	rjmp .L45
	rjmp .L25
.L39:
	lds r24,gAuxPort
	rjmp 2f
1:	lsl r18
	rol r19
2:	dec r24
	brpl 1b
	mov r25,r18
	com r25
	in r24,37-0x20
	and r24,r25
	out 37-0x20,r24
	ldi r24,lo8(2)
	sts gXmtState,r24
	ldi r24,lo8(1)
	rjmp .L52
.L40:
	lds r24,gUartXmt
	lds r25,gBitnum
	and r24,r25
	breq .L41
	lds r24,gAuxPort
	ldi r18,lo8(1)
	ldi r19,hi8(1)
	rjmp 2f
1:	lsl r18
	rol r19
2:	dec r24
	brpl 1b
	in r24,37-0x20
	or r24,r18
	rjmp .L50
.L41:
	lds r18,gAuxPort
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	rjmp 2f
1:	lsl r24
	rol r25
2:	dec r18
	brpl 1b
	mov r25,r24
	com r25
	in r24,37-0x20
	and r24,r25
.L50:
	out 37-0x20,r24
	lds r24,gBitnum
	sbrs r24,7
	rjmp .L43
	ldi r24,lo8(3)
	rjmp .L51
.L43:
	lds r24,gBitnum
	lsl r24
.L52:
	sts gBitnum,r24
	rjmp .L25
.L45:
	lds r24,gAuxPort
	ldi r18,lo8(1)
	ldi r19,hi8(1)
	rjmp 2f
1:	lsl r18
	rol r19
2:	dec r24
	brpl 1b
	in r24,37-0x20
	or r24,r18
	out 37-0x20,r24
	ldi r25,lo8(1)
	sts gBitnum,r25
	lds r24,gXringNdx
	tst r24
	breq .L46
	lds r24,ringNdx.0
	lds r30,gXring
	lds r31,(gXring)+1
	add r30,r24
	adc r31,__zero_reg__
	add r24,r25
	sts ringNdx.0,r24
	ld r24,Z
	sts gUartXmt,r24
	sts gXmtState,r25
	lds r24,gXringNdx
	subi r24,lo8(-(-1))
	sts gXringNdx,r24
	rjmp .L25
.L46:
	sts ringNdx.0,r24
.L51:
	sts gXmtState,r24
.L25:
/* epilogue: frame size=0 */
	pop r31
	pop r30
	pop r25
	pop r24
	pop r19
	pop r18
	pop __tmp_reg__
	out __SREG__,__tmp_reg__
	pop __tmp_reg__
	pop __zero_reg__
	reti
/* epilogue end (size=11) */
/* function __vector_7 size 272 (250) */
	.size	__vector_7, .-__vector_7
	.lcomm gAuxPort,1
	.lcomm gUartXmt,1
	.lcomm gXring,2
	.lcomm gRring,48
/* File "cm9600.c": code  507 = 0x01fb ( 468), prologues  16, epilogues  23 */
