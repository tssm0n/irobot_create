	.file	"cmcontrol.c"
	.arch atmega168
__SREG__ = 0x3f
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__tmp_reg__ = 0
__zero_reg__ = 1
	.global __do_copy_data
	.global __do_clear_bss
.global	sensors_index
.global	sensors_index
	.section .bss
	.type	sensors_index, @object
	.size	sensors_index, 1
sensors_index:
	.skip 1,0
.global	sensors_flag
.global	sensors_flag
	.type	sensors_flag, @object
	.size	sensors_flag, 1
sensors_flag:
	.skip 1,0
.global	debugCount
.global	debugCount
	.type	debugCount, @object
	.size	debugCount, 1
debugCount:
	.skip 1,0
.global	angle
.global	angle
	.type	angle, @object
	.size	angle, 2
angle:
	.skip 2,0
.global	distance
.global	distance
	.type	distance, @object
	.size	distance, 2
distance:
	.skip 2,0
.global	doRelay
.global	doRelay
	.type	doRelay, @object
	.size	doRelay, 1
doRelay:
	.skip 1,0
.global	localAck
.global	localAck
	.type	localAck, @object
	.size	localAck, 1
localAck:
	.skip 1,0
.global	localAckIndex
.global	localAckIndex
	.type	localAckIndex, @object
	.size	localAckIndex, 1
localAckIndex:
	.skip 1,0
.global	prevVal
	.data
	.type	prevVal, @object
	.size	prevVal, 1
prevVal:
	.byte	-1
.global	myport
.global	myport
	.section .bss
	.type	myport, @object
	.size	myport, 1
myport:
	.skip 1,0
.global	receiveIndex
.global	receiveIndex
	.type	receiveIndex, @object
	.size	receiveIndex, 1
receiveIndex:
	.skip 1,0
.global	timer_on
.global	timer_on
	.type	timer_on, @object
	.size	timer_on, 1
timer_on:
	.skip 1,0
.global	timer_cnt
.global	timer_cnt
	.type	timer_cnt, @object
	.size	timer_cnt, 2
timer_cnt:
	.skip 2,0
	.text
.global	byteTx
	.type	byteTx, @function
byteTx:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	mov r25,r24
.L2:
	lds r24,192
	sbrs r24,5
	rjmp .L2
	sts 198,r25
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function byteTx size 8 (7) */
	.size	byteTx, .-byteTx
.global	checkLocalAck
	.type	checkLocalAck, @function
checkLocalAck:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	mov r25,r24
	lds r24,localAckIndex
	sbrc r24,0
	rjmp .L8
	cpi r25,lo8(-105)
	breq .L7
.L8:
	lds r24,localAckIndex
	sbrs r24,0
	rjmp .L6
	cpi r25,lo8(-119)
	brne .L6
.L7:
	lds r24,localAckIndex
	subi r24,lo8(-(1))
	sts localAckIndex,r24
	rjmp .L9
.L6:
	sts localAckIndex,__zero_reg__
.L9:
	lds r24,localAckIndex
	cpi r24,lo8(6)
	brne .L10
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	ret
.L10:
	ldi r24,lo8(0)
	ldi r25,hi8(0)
	ret
/* epilogue: frame size=0 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function checkLocalAck size 31 (31) */
	.size	checkLocalAck, .-checkLocalAck
.global	__vector_11
	.type	__vector_11, @function
__vector_11:
/* prologue: frame size=0 */
	push __zero_reg__
	push __tmp_reg__
	in __tmp_reg__,__SREG__
	push __tmp_reg__
	clr __zero_reg__
	push r24
	push r25
/* prologue end (size=7) */
	lds r24,timer_cnt
	lds r25,(timer_cnt)+1
	or r24,r25
	breq .L13
	lds r24,timer_cnt
	lds r25,(timer_cnt)+1
	sbiw r24,1
	sts (timer_cnt)+1,r25
	sts timer_cnt,r24
	rjmp .L12
.L13:
	sts timer_on,__zero_reg__
.L12:
/* epilogue: frame size=0 */
	pop r25
	pop r24
	pop __tmp_reg__
	out __SREG__,__tmp_reg__
	pop __tmp_reg__
	pop __zero_reg__
	reti
/* epilogue end (size=7) */
/* function __vector_11 size 32 (18) */
	.size	__vector_11, .-__vector_11
.global	__vector_18
	.type	__vector_18, @function
__vector_18:
/* prologue: frame size=0 */
	push __zero_reg__
	push __tmp_reg__
	in __tmp_reg__,__SREG__
	push __tmp_reg__
	clr __zero_reg__
	push r18
	push r24
	push r25
	push r30
	push r31
/* prologue end (size=10) */
	lds r18,198
	lds r25,doRelay
	tst r25
	breq .L16
	lds r24,receiveIndex
	cpi r24,lo8(64)
	brsh .L15
	lds r24,receiveIndex
	mov r30,r24
	clr r31
	subi r30,lo8(-(receiveBuffer))
	sbci r31,hi8(-(receiveBuffer))
	st Z,r18
	subi r24,lo8(-(1))
	sts receiveIndex,r24
	rjmp .L15
.L16:
	lds r24,sensors_flag
	tst r24
	breq .L15
	lds r24,sensors_index
	mov r30,r24
	clr r31
	subi r30,lo8(-(sensors_in))
	sbci r31,hi8(-(sensors_in))
	st Z,r18
	subi r24,lo8(-(1))
	sts sensors_index,r24
	lds r24,sensors_index
	cpi r24,lo8(52)
	brlo .L15
	sts sensors_flag,r25
.L15:
/* epilogue: frame size=0 */
	pop r31
	pop r30
	pop r25
	pop r24
	pop r18
	pop __tmp_reg__
	out __SREG__,__tmp_reg__
	pop __tmp_reg__
	pop __zero_reg__
	reti
/* epilogue end (size=10) */
/* function __vector_18 size 61 (41) */
	.size	__vector_18, .-__vector_18
.global	readIRSensor
	.type	readIRSensor, @function
readIRSensor:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	ldi r24,lo8(69)
	sts 124,r24
	lds r24,122
	ori r24,lo8(64)
	sts 122,r24
.L22:
	lds r24,122
	sbrc r24,6
	rjmp .L22
	lds r24,120
	lds r25,(120)+1
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function readIRSensor size 17 (16) */
	.size	readIRSensor, .-readIRSensor
.global	handleLocalCommand
	.type	handleLocalCommand, @function
handleLocalCommand:
/* prologue: frame size=0 */
	push r16
	push r17
/* prologue end (size=2) */
	clr r25
	cpi r24,2
	cpc r25,__zero_reg__
	breq .L28
	cpi r24,3
	cpc r25,__zero_reg__
	brge .L30
	sbiw r24,1
	breq .L27
	rjmp .L26
.L30:
	sbiw r24,3
	breq .L29
	rjmp .L26
.L27:
	cbi 43-0x20,6
	rjmp .L26
.L28:
	sbi 43-0x20,6
	rjmp .L26
.L29:
	call readIRSensor
	movw r16,r24
	mov r24,r17
	clr r25
	mov r22,r24
	lds r24,myport
	call cm_ByteXmt
	mov r22,r16
	lds r24,myport
	call cm_ByteXmt
.L26:
	sts localAck,__zero_reg__
/* epilogue: frame size=0 */
	pop r17
	pop r16
	ret
/* epilogue end (size=3) */
/* function handleLocalCommand size 41 (36) */
	.size	handleLocalCommand, .-handleLocalCommand
.global	relayCommand
	.type	relayCommand, @function
relayCommand:
/* prologue: frame size=0 */
	push r28
/* prologue end (size=1) */
	rjmp .L51
.L39:
	lds r24,myport
	call cm_read_char
	mov r28,r24
	cpi r24,lo8(-1)
	brne .L35
	lds r24,prevVal
	tst r24
	brge .L34
.L35:
	lds r24,localAck
	tst r24
	brne .L36
	mov r24,r28
	call byteTx
	rjmp .L34
.L36:
	mov r24,r28
	call handleLocalCommand
.L34:
	sts prevVal,r28
	mov r24,r28
	call checkLocalAck
	tst r24
	breq .L51
	ldi r24,lo8(1)
	sts localAck,r24
.L51:
	lds r24,myport
	call cm_char_rdy
	mov r30,r24
	tst r24
	brne .L39
	lds r24,receiveIndex
	tst r24
	breq .L31
	mov r28,r30
.L50:
	lds r24,receiveIndex
	cp r28,r24
	brsh .L46
	mov r30,r28
	clr r31
	subi r30,lo8(-(receiveBuffer))
	sbci r31,hi8(-(receiveBuffer))
	ld r22,Z
	lds r24,myport
	call cm_ByteXmt
	subi r28,lo8(-(1))
	rjmp .L50
.L46:
	sts receiveIndex,__zero_reg__
.L31:
/* epilogue: frame size=0 */
	pop r28
	ret
/* epilogue end (size=2) */
/* function relayCommand size 66 (63) */
	.size	relayCommand, .-relayCommand
.global	readProximitySensor
	.type	readProximitySensor, @function
readProximitySensor:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	ldi r24,lo8(65)
	sts 124,r24
	lds r24,122
	ori r24,lo8(64)
	sts 122,r24
.L53:
	lds r24,122
	sbrc r24,6
	rjmp .L53
	lds r24,120
	lds r25,(120)+1
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function readProximitySensor size 17 (16) */
	.size	readProximitySensor, .-readProximitySensor
.global	driveDirect
	.type	driveDirect, @function
driveDirect:
/* prologue: frame size=0 */
	push r14
	push r15
	push r16
	push r17
/* prologue end (size=4) */
	movw r16,r24
	movw r14,r22
	ldi r24,lo8(-111)
	call byteTx
	mov r24,r17
	clr r25
	call byteTx
	mov r24,r16
	call byteTx
	mov r24,r15
	clr r25
	call byteTx
	mov r24,r14
	call byteTx
/* epilogue: frame size=0 */
	pop r17
	pop r16
	pop r15
	pop r14
	ret
/* epilogue end (size=5) */
/* function driveDirect size 28 (19) */
	.size	driveDirect, .-driveDirect
.global	drive
	.type	drive, @function
drive:
/* prologue: frame size=0 */
	push r14
	push r15
	push r16
	push r17
/* prologue end (size=4) */
	movw r16,r24
	movw r14,r22
	ldi r24,lo8(-119)
	call byteTx
	mov r24,r17
	clr r25
	call byteTx
	mov r24,r16
	call byteTx
	mov r24,r15
	clr r25
	call byteTx
	mov r24,r14
	call byteTx
/* epilogue: frame size=0 */
	pop r17
	pop r16
	pop r15
	pop r14
	ret
/* epilogue end (size=5) */
/* function drive size 28 (19) */
	.size	drive, .-drive
.global	checkIr
	.type	checkIr, @function
checkIr:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	movw r30,r24
	lds r24,sensors+10
	clr r25
	cpi r24,131
	cpc r25,__zero_reg__
	breq .L63
	cpi r24,132
	cpc r25,__zero_reg__
	brge .L79
	cpi r24,129
	cpc r25,__zero_reg__
	breq .L70
	cpi r24,130
	cpc r25,__zero_reg__
	brlt .+2
	rjmp .L77
	sbiw r24,19
	breq .L60
	ret
.L79:
	cpi r24,136
	cpc r25,__zero_reg__
	breq .L62
	cpi r24,137
	cpc r25,__zero_reg__
	brge .L80
	cpi r24,133
	cpc r25,__zero_reg__
	breq .L61
	ret
.L80:
	cpi r24,137
	cpc r25,__zero_reg__
	brne .+2
	rjmp .L78
	ret
.L60:
	in r24,43-0x20
	andi r24,lo8(-97)
	out 43-0x20,r24
	ret
.L61:
	cbi 43-0x20,6
	sbi 37-0x20,2
	ret
.L62:
	call doDrive
	ret
.L63:
	ld r18,Z
	ldd r19,Z+1
	cp r18,__zero_reg__
	cpc r19,__zero_reg__
	brne .L64
	ldd r24,Z+2
	ldd r25,Z+3
	or r24,r25
	brne .L64
	ldd r24,Z+4
	cpi r24,lo8(2)
	breq .L92
	ldi r24,lo8(25)
	ldi r25,hi8(25)
	std Z+6,r25
	std Z+5,r24
	ldi r24,lo8(2)
	rjmp .L91
.L64:
	ldi r20,hi8(500)
	cpi r18,lo8(500)
	cpc r19,r20
	brne .L68
	ldd r24,Z+2
	ldd r25,Z+3
	sbiw r24,25
	rjmp .L90
.L68:
	subi r18,lo8(-(25))
	sbci r19,hi8(-(25))
	rjmp .L93
.L70:
	ld r18,Z
	ldd r19,Z+1
	cp r18,__zero_reg__
	cpc r19,__zero_reg__
	brne .L71
	ldd r24,Z+2
	ldd r25,Z+3
	or r24,r25
	brne .L71
	ldd r24,Z+4
	cpi r24,lo8(1)
	brne .L72
.L92:
	ldd r24,Z+5
	ldd r25,Z+6
	adiw r24,25
	std Z+6,r25
	std Z+5,r24
	rjmp .L89
.L72:
	ldi r24,lo8(25)
	ldi r25,hi8(25)
	std Z+6,r25
	std Z+5,r24
	ldi r24,lo8(1)
.L91:
	std Z+4,r24
	rjmp .L89
.L71:
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r20,hi8(500)
	cpi r24,lo8(500)
	cpc r25,r20
	brne .L75
	subi r18,lo8(-(-25))
	sbci r19,hi8(-(-25))
.L93:
	std Z+1,r19
	st Z,r18
	rjmp .L89
.L75:
	adiw r24,25
.L90:
	std Z+3,r25
	std Z+2,r24
	rjmp .L89
.L77:
	std Z+4,__zero_reg__
	std Z+6,__zero_reg__
	std Z+5,__zero_reg__
	ld r24,Z
	ldd r25,Z+1
	adiw r24,25
	std Z+1,r25
	st Z,r24
	ldd r24,Z+2
	ldd r25,Z+3
	rjmp .L75
.L78:
	std Z+1,__zero_reg__
	st Z,__zero_reg__
	std Z+3,__zero_reg__
	std Z+2,__zero_reg__
.L89:
	ld r24,Z
	ldd r25,Z+1
	subi r24,lo8(501)
	sbci r25,hi8(501)
	brlo .L82
	ldi r24,lo8(500)
	ldi r25,hi8(500)
	std Z+1,r25
	st Z,r24
.L82:
	ldd r24,Z+2
	ldd r25,Z+3
	subi r24,lo8(501)
	sbci r25,hi8(501)
	brlo .L83
	ldi r24,lo8(500)
	ldi r25,hi8(500)
	std Z+3,r25
	std Z+2,r24
.L83:
	ldd r24,Z+4
	mov r22,r24
	clr r23
	cpi r22,1
	cpc r23,__zero_reg__
	breq .L94
	cpi r22,2
	cpc r23,__zero_reg__
	brge .L88
	or r22,r23
	breq .L85
	ret
.L88:
	cpi r22,2
	cpc r23,__zero_reg__
	breq .L87
	ret
.L85:
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	call driveDirect
	ret
.L87:
	ldi r22,lo8(-1)
	ldi r23,hi8(-1)
.L94:
	ldd r24,Z+5
	ldd r25,Z+6
	call drive
	ret
/* epilogue: frame size=0 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function checkIr size 180 (180) */
	.size	checkIr, .-checkIr
.global	delayMs
	.type	delayMs, @function
delayMs:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	movw r18,r24
	ldi r24,lo8(1)
	sts timer_on,r24
	sts (timer_cnt)+1,r19
	sts timer_cnt,r18
.L96:
	lds r24,timer_on
	tst r24
	brne .L96
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function delayMs size 13 (12) */
	.size	delayMs, .-delayMs
.global	powerOnRobot
	.type	powerOnRobot, @function
powerOnRobot:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	sbic 35-0x20,5
	rjmp .L99
	sbic 35-0x20,5
	rjmp .L105
.L103:
	cbi 43-0x20,7
	ldi r24,lo8(500)
	ldi r25,hi8(500)
	call delayMs
	sbi 43-0x20,7
	ldi r24,lo8(100)
	ldi r25,hi8(100)
	call delayMs
	cbi 43-0x20,7
	sbis 35-0x20,5
	rjmp .L103
.L105:
	ldi r24,lo8(3500)
	ldi r25,hi8(3500)
	call delayMs
.L99:
	ret
/* epilogue: frame size=0 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function powerOnRobot size 22 (22) */
	.size	powerOnRobot, .-powerOnRobot
.global	checkBump
	.type	checkBump, @function
checkBump:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	movw r30,r24
	ldd r24,Z+2
	ldd r25,Z+3
	or r24,r25
	brne .L107
	ld r24,Z
	ldd r25,Z+1
	or r24,r25
	breq .L106
.L107:
	lds r24,sensors+2
	tst r24
	brne .L109
	lds r24,sensors+3
	tst r24
	brne .L109
	lds r24,sensors+4
	tst r24
	brne .L109
	lds r24,sensors+5
	tst r24
	brne .L109
	lds r24,sensors
	tst r24
	brne .L109
	lds r24,customSensors
	lds r25,(customSensors)+1
	cpi r24,221
	cpc r25,__zero_reg__
	brlo .L106
.L109:
	std Z+3,__zero_reg__
	std Z+2,__zero_reg__
	std Z+1,__zero_reg__
	st Z,__zero_reg__
	std Z+4,__zero_reg__
	std Z+6,__zero_reg__
	std Z+5,__zero_reg__
	ldi r22,lo8(-100)
	ldi r23,hi8(-100)
	movw r24,r22
	call driveDirect
	ldi r24,lo8(1000)
	ldi r25,hi8(1000)
	call delayMs
	ldi r22,lo8(0)
	ldi r23,hi8(0)
	movw r24,r22
	call driveDirect
.L106:
	ret
/* epilogue: frame size=0 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function checkBump size 58 (58) */
	.size	checkBump, .-checkBump
.global	baud
	.type	baud, @function
baud:
/* prologue: frame size=0 */
	push r28
/* prologue end (size=1) */
	mov r28,r24
	cpi r24,lo8(12)
	brlo .+2
	rjmp .L110
	ldi r24,lo8(-127)
	call byteTx
	lds r24,192
	ori r24,lo8(64)
	sts 192,r24
	mov r24,r28
	call byteTx
.L112:
	lds r24,192
	sbrs r24,6
	rjmp .L112
/* #APP */
	cli
/* #NOAPP */
	cpi r28,lo8(11)
	brne .L115
	ldi r24,lo8(9)
	ldi r25,hi8(9)
	rjmp .L138
.L115:
	cpi r28,lo8(10)
	brne .L117
	ldi r24,lo8(19)
	ldi r25,hi8(19)
	rjmp .L138
.L117:
	cpi r28,lo8(9)
	brne .L119
	ldi r24,lo8(29)
	ldi r25,hi8(29)
	rjmp .L138
.L119:
	cpi r28,lo8(8)
	brne .L121
	ldi r24,lo8(39)
	ldi r25,hi8(39)
	rjmp .L138
.L121:
	cpi r28,lo8(7)
	brne .L123
	ldi r24,lo8(59)
	ldi r25,hi8(59)
	rjmp .L138
.L123:
	cpi r28,lo8(6)
	brne .L125
	ldi r24,lo8(79)
	ldi r25,hi8(79)
	rjmp .L138
.L125:
	cpi r28,lo8(5)
	brne .L127
	ldi r24,lo8(119)
	ldi r25,hi8(119)
	rjmp .L138
.L127:
	cpi r28,lo8(4)
	brne .L129
	ldi r24,lo8(239)
	ldi r25,hi8(239)
	rjmp .L138
.L129:
	cpi r28,lo8(3)
	brne .L131
	ldi r24,lo8(479)
	ldi r25,hi8(479)
	rjmp .L138
.L131:
	cpi r28,lo8(2)
	brne .L133
	ldi r24,lo8(959)
	ldi r25,hi8(959)
	rjmp .L138
.L133:
	cpi r28,lo8(1)
	brne .L135
	ldi r24,lo8(1919)
	ldi r25,hi8(1919)
	rjmp .L138
.L135:
	tst r28
	brne .L116
	ldi r24,lo8(3839)
	ldi r25,hi8(3839)
.L138:
	sts (196)+1,r25
	sts 196,r24
.L116:
/* #APP */
	sei
/* #NOAPP */
	ldi r24,lo8(100)
	ldi r25,hi8(100)
	call delayMs
.L110:
/* epilogue: frame size=0 */
	pop r28
	ret
/* epilogue end (size=2) */
/* function baud size 93 (90) */
	.size	baud, .-baud
.global	debug
	.type	debug, @function
debug:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	movw r22,r24
	lds r24,myport
	call cm_send_line
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function debug size 6 (5) */
	.size	debug, .-debug
	.data
.LC0:
	.string	"CMControl Initialized\r\n"
	.text
.global	initialize
	.type	initialize, @function
initialize:
/* prologue: frame size=0 */
/* prologue end (size=0) */
/* #APP */
	cli
/* #NOAPP */
	ldi r24,lo8(16)
	out 36-0x20,r24
	ldi r24,lo8(-49)
	out 37-0x20,r24
	out 39-0x20,__zero_reg__
	in r24,40-0x20
	andi r24,lo8(-35)
	out 40-0x20,r24
	ldi r24,lo8(-26)
	out 42-0x20,r24
	ldi r24,lo8(125)
	out 43-0x20,r24
	sbi 36-0x20,2
	cbi 37-0x20,2
	sts 128,__zero_reg__
	ldi r24,lo8(12)
	sts 129,r24
	ldi r24,lo8(71)
	ldi r25,hi8(71)
	sts (136)+1,r25
	sts 136,r24
	ldi r24,lo8(2)
	sts 111,r24
	ldi r24,lo8(19)
	ldi r25,hi8(19)
	sts (196)+1,r25
	sts 196,r24
	ldi r24,lo8(-104)
	sts 193,r24
	ldi r24,lo8(6)
	sts 194,r24
/* #APP */
	sei
/* #NOAPP */
	ldi r22,lo8(3)
	ldi r24,lo8(0)
	call cm_init_UART
	sts myport,r24
	lds r24,126
	ori r24,lo8(32)
	sts 126,r24
	lds r24,100
	andi r24,lo8(-2)
	sts 100,r24
	ldi r24,lo8(-121)
	sts 122,r24
	ldi r24,lo8(69)
	sts 124,r24
	lds r24,126
	ori r24,lo8(2)
	sts 126,r24
	ldi r24,lo8(250)
	ldi r25,hi8(250)
	call delayMs
	ldi r24,lo8(.LC0)
	ldi r25,hi8(.LC0)
	call debug
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function initialize size 80 (79) */
	.size	initialize, .-initialize
	.data
.LC1:
	.string	" "
	.text
.global	debugInt
	.type	debugInt, @function
debugInt:
/* prologue: frame size=16 */
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,16
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue end (size=10) */
	ldi r20,lo8(10)
	ldi r21,hi8(10)
	movw r22,r28
	subi r22,lo8(-(1))
	sbci r23,hi8(-(1))
	call itoa
	movw r22,r28
	subi r22,lo8(-(1))
	sbci r23,hi8(-(1))
	lds r24,myport
	call cm_send_line
	ldi r22,lo8(.LC1)
	ldi r23,hi8(.LC1)
	lds r24,myport
	call cm_send_line
/* epilogue: frame size=16 */
	adiw r28,16
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
	pop r29
	pop r28
	ret
/* epilogue end (size=9) */
/* function debugInt size 39 (20) */
	.size	debugInt, .-debugInt
	.data
.LC2:
	.string	"IR Sensor: "
.LC3:
	.string	"\r\n"
.LC4:
	.string	"Proximity Sensor: "
	.text
.global	delayAndUpdateSensors
	.type	delayAndUpdateSensors, @function
delayAndUpdateSensors:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	ldi r18,lo8(1)
	sts timer_on,r18
	sts (timer_cnt)+1,r25
	sts timer_cnt,r24
.L157:
	lds r24,timer_on
	tst r24
	brne .+2
	rjmp .L153
	lds r24,sensors_flag
	tst r24
	brne .L157
	ldi r26,lo8(sensors_in)
	ldi r27,hi8(sensors_in)
	ldi r30,lo8(sensors)
	ldi r31,hi8(sensors)
	ldi r25,lo8(51)
.L149:
	ld r24,X+
	st Z+,r24
	subi r25,lo8(-(-1))
	sbrs r25,7
	rjmp .L149
	lds r24,sensors+12
	clr r25
	mov r19,r24
	clr r18
	lds r24,sensors+13
	clr r25
	or r18,r24
	or r19,r25
	lds r24,distance
	lds r25,(distance)+1
	add r24,r18
	adc r25,r19
	sts (distance)+1,r25
	sts distance,r24
	lds r24,sensors+14
	clr r25
	mov r19,r24
	clr r18
	lds r24,sensors+15
	clr r25
	or r18,r24
	or r19,r25
	lds r24,angle
	lds r25,(angle)+1
	add r24,r18
	adc r25,r19
	sts (angle)+1,r25
	sts angle,r24
	ldi r24,lo8(-114)
	call byteTx
	ldi r24,lo8(6)
	call byteTx
	sts sensors_index,__zero_reg__
	ldi r24,lo8(1)
	sts sensors_flag,r24
	call readIRSensor
	sts (customSensors)+1,r25
	sts customSensors,r24
	call readProximitySensor
	sts (customSensors+2)+1,r25
	sts customSensors+2,r24
	lds r24,debugCount
	cpi r24,lo8(120)
	brlt .L150
	ldi r24,lo8(.LC2)
	ldi r25,hi8(.LC2)
	call debug
	lds r24,customSensors
	lds r25,(customSensors)+1
	call debugInt
	ldi r24,lo8(.LC3)
	ldi r25,hi8(.LC3)
	call debug
	ldi r24,lo8(.LC4)
	ldi r25,hi8(.LC4)
	call debug
	lds r24,customSensors+2
	lds r25,(customSensors+2)+1
	call debugInt
	ldi r24,lo8(.LC3)
	ldi r25,hi8(.LC3)
	call debug
	sts debugCount,__zero_reg__
.L150:
	lds r24,debugCount
	subi r24,lo8(-(1))
	sts debugCount,r24
	rjmp .L157
.L153:
	ret
/* epilogue: frame size=0 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function delayAndUpdateSensors size 130 (130) */
	.size	delayAndUpdateSensors, .-delayAndUpdateSensors
.global	main
	.type	main, @function
main:
/* prologue: frame size=7 */
	ldi r28,lo8(__stack - 7)
	ldi r29,hi8(__stack - 7)
	out __SP_H__,r29
	out __SP_L__,r28
/* prologue end (size=4) */
	call initialize
	call powerOnRobot
	ldi r24,lo8(-128)
	call byteTx
	ldi r24,lo8(-124)
	call byteTx
	in r24,43-0x20
	ori r24,lo8(96)
	out 43-0x20,r24
	cbi 43-0x20,5
.L167:
	lds r24,doRelay
	tst r24
	breq .L161
	call relayCommand
	rjmp .L162
.L161:
	ldi r24,lo8(15)
	ldi r25,hi8(15)
	call delayAndUpdateSensors
	movw r24,r28
	adiw r24,1
	call checkIr
	movw r24,r28
	adiw r24,1
	call checkBump
.L162:
	sbic 41-0x20,4
	rjmp .L167
	ldi r24,lo8(-111)
	call byteTx
	ldi r24,lo8(0)
	call byteTx
	ldi r24,lo8(0)
	call byteTx
	ldi r24,lo8(0)
	call byteTx
	ldi r24,lo8(0)
	call byteTx
	ldi r24,lo8(-128)
	call byteTx
	rjmp .L167
/* epilogue: frame size=7 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function main size 58 (54) */
	.size	main, .-main
	.comm receiveBuffer,64,1
	.comm sensors_in,52,1
	.comm sensors,52,1
	.comm customSensors,4,1
/* File "cmcontrol.c": code 1008 = 0x03f0 ( 916), prologues  43, epilogues  49 */
