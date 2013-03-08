	.file	"cmcontrol.c"
__SREG__ = 0x3f
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__CCP__  = 0x34
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
.global	__vector_11
	.type	__vector_11, @function
__vector_11:
	push __zero_reg__
	push r0
	in r0,__SREG__
	push r0
	clr __zero_reg__
	push r24
	push r25
/* prologue: Signal */
/* frame size = 0 */
	lds r24,timer_cnt
	lds r25,(timer_cnt)+1
	or r24,r25
	breq .L2
	lds r24,timer_cnt
	lds r25,(timer_cnt)+1
	sbiw r24,1
	sts (timer_cnt)+1,r25
	sts timer_cnt,r24
	rjmp .L4
.L2:
	sts timer_on,__zero_reg__
.L4:
/* epilogue start */
	pop r25
	pop r24
	pop r0
	out __SREG__,r0
	pop r0
	pop __zero_reg__
	reti
	.size	__vector_11, .-__vector_11
.global	__vector_18
	.type	__vector_18, @function
__vector_18:
	push __zero_reg__
	push r0
	in r0,__SREG__
	push r0
	clr __zero_reg__
	push r24
	push r25
	push r30
	push r31
/* prologue: Signal */
/* frame size = 0 */
	lds r25,198
	lds r24,doRelay
	tst r24
	breq .L6
	lds r24,receiveIndex
	cpi r24,lo8(64)
	brsh .L8
	lds r24,receiveIndex
	mov r30,r24
	ldi r31,lo8(0)
	subi r30,lo8(-(receiveBuffer))
	sbci r31,hi8(-(receiveBuffer))
	st Z,r25
	subi r24,lo8(-(1))
	sts receiveIndex,r24
	rjmp .L8
.L6:
	lds r24,sensors_flag
	tst r24
	breq .L8
	lds r24,sensors_index
	mov r30,r24
	ldi r31,lo8(0)
	subi r30,lo8(-(sensors_in))
	sbci r31,hi8(-(sensors_in))
	st Z,r25
	subi r24,lo8(-(1))
	sts sensors_index,r24
	lds r24,sensors_index
	cpi r24,lo8(52)
	brlo .L8
	sts sensors_flag,__zero_reg__
.L8:
/* epilogue start */
	pop r31
	pop r30
	pop r25
	pop r24
	pop r0
	out __SREG__,r0
	pop r0
	pop __zero_reg__
	reti
	.size	__vector_18, .-__vector_18
.global	isBumped
	.type	isBumped, @function
isBumped:
/* prologue: function */
/* frame size = 0 */
	lds r24,sensors+2
	tst r24
	brne .L10
	lds r24,sensors+3
	tst r24
	brne .L10
	lds r24,sensors+4
	tst r24
	brne .L10
	lds r24,sensors+5
	tst r24
	brne .L10
	lds r24,sensors
	ldi r25,lo8(0)
	andi r24,lo8(3)
	andi r25,hi8(3)
	or r24,r25
	brne .L10
	lds r24,customSensors
	lds r25,(customSensors)+1
	subi r24,lo8(301)
	sbci r25,hi8(301)
	brsh .L10
	lds r24,customSensors+2
	lds r25,(customSensors+2)+1
	sbiw r24,13
	brlo .L11
	ldi r24,lo8(0)
	ldi r25,hi8(0)
	ret
.L11:
	lds r24,customSensors+2
	lds r25,(customSensors+2)+1
	ldi r18,lo8(0)
	ldi r19,hi8(0)
	subi r24,lo8(-1)
	sbci r25,hi8(-1)
	breq .L13
	ldi r18,lo8(1)
	ldi r19,hi8(1)
.L13:
	movw r24,r18
	ret
.L10:
	ldi r24,lo8(1)
	ldi r25,hi8(1)
	ret
	.size	isBumped, .-isBumped
.global	drive
	.type	drive, @function
drive:
/* prologue: function */
/* frame size = 0 */
	mov r18,r24
.L16:
	lds r24,192
	sbrs r24,5
	rjmp .L16
	ldi r24,lo8(-119)
	sts 198,r24
.L17:
	lds r24,192
	sbrs r24,5
	rjmp .L17
	sts 198,r25
.L18:
	lds r24,192
	sbrs r24,5
	rjmp .L18
	sts 198,r18
.L19:
	lds r24,192
	sbrs r24,5
	rjmp .L19
	sts 198,r23
.L20:
	lds r24,192
	sbrs r24,5
	rjmp .L20
	sts 198,r22
/* epilogue start */
	ret
	.size	drive, .-drive
.global	driveDirect
	.type	driveDirect, @function
driveDirect:
/* prologue: function */
/* frame size = 0 */
	mov r18,r24
.L28:
	lds r24,192
	sbrs r24,5
	rjmp .L28
	ldi r24,lo8(-111)
	sts 198,r24
.L29:
	lds r24,192
	sbrs r24,5
	rjmp .L29
	sts 198,r25
.L30:
	lds r24,192
	sbrs r24,5
	rjmp .L30
	sts 198,r18
.L31:
	lds r24,192
	sbrs r24,5
	rjmp .L31
	sts 198,r23
.L32:
	lds r24,192
	sbrs r24,5
	rjmp .L32
	sts 198,r22
/* epilogue start */
	ret
	.size	driveDirect, .-driveDirect
.global	byteTx
	.type	byteTx, @function
byteTx:
/* prologue: function */
/* frame size = 0 */
	mov r25,r24
.L40:
	lds r24,192
	sbrs r24,5
	rjmp .L40
	sts 198,r25
/* epilogue start */
	ret
	.size	byteTx, .-byteTx
.global	checkLocalAck
	.type	checkLocalAck, @function
checkLocalAck:
/* prologue: function */
/* frame size = 0 */
	mov r25,r24
	lds r24,localAckIndex
	sbrc r24,0
	rjmp .L44
	cpi r25,lo8(-105)
	breq .L45
.L44:
	lds r24,localAckIndex
	sbrs r24,0
	rjmp .L46
	cpi r25,lo8(-119)
	brne .L46
.L45:
	lds r24,localAckIndex
	subi r24,lo8(-(1))
	sts localAckIndex,r24
	rjmp .L47
.L46:
	sts localAckIndex,__zero_reg__
.L47:
	lds r24,localAckIndex
	ldi r25,lo8(0)
	cpi r24,lo8(6)
	brne .L48
	ldi r25,lo8(1)
.L48:
	mov r24,r25
/* epilogue start */
	ret
	.size	checkLocalAck, .-checkLocalAck
.global	handleLocalCommand
	.type	handleLocalCommand, @function
handleLocalCommand:
/* prologue: function */
/* frame size = 0 */
	cpi r24,lo8(1)
	breq .L52
	cpi r24,lo8(2)
	brne .L51
	rjmp .L55
.L52:
	cbi 43-32,6
	rjmp .L51
.L55:
	sbi 43-32,6
.L51:
	sts localAck,__zero_reg__
/* epilogue start */
	ret
	.size	handleLocalCommand, .-handleLocalCommand
.global	i2cStop
	.type	i2cStop, @function
i2cStop:
/* prologue: function */
/* frame size = 0 */
	ldi r24,lo8(-108)
	sts 188,r24
/* epilogue start */
	ret
	.size	i2cStop, .-i2cStop
.global	readIRSensor
	.type	readIRSensor, @function
readIRSensor:
/* prologue: function */
/* frame size = 0 */
	ldi r24,lo8(66)
	sts 124,r24
	lds r24,122
	ori r24,lo8(64)
	sts 122,r24
.L59:
	lds r24,122
	sbrc r24,6
	rjmp .L59
	lds r18,120
	lds r19,(120)+1
	movw r24,r18
/* epilogue start */
	ret
	.size	readIRSensor, .-readIRSensor
.global	delayMs
	.type	delayMs, @function
delayMs:
/* prologue: function */
/* frame size = 0 */
	movw r20,r24
	ldi r18,lo8(0)
	ldi r19,hi8(0)
	ldi r25,lo8(1)
	ldi r22,lo8(10)
	ldi r23,hi8(10)
	rjmp .L63
.L65:
	sts timer_on,r25
	sts (timer_cnt)+1,r23
	sts timer_cnt,r22
.L64:
	lds r24,timer_on
	tst r24
	brne .L64
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
.L63:
	cp r18,r20
	cpc r19,r21
	brlo .L65
/* epilogue start */
	ret
	.size	delayMs, .-delayMs
.global	delay100us
	.type	delay100us, @function
delay100us:
/* prologue: function */
/* frame size = 0 */
	ldi r18,lo8(1)
	sts timer_on,r18
	sts (timer_cnt)+1,r25
	sts timer_cnt,r24
.L69:
	lds r24,timer_on
	tst r24
	brne .L69
/* epilogue start */
	ret
	.size	delay100us, .-delay100us
.global	powerOnRobot
	.type	powerOnRobot, @function
powerOnRobot:
/* prologue: function */
/* frame size = 0 */
	sbis 35-32,5
	rjmp .L73
	ret
.L79:
	cbi 43-32,7
	ldi r18,lo8(0)
	ldi r19,hi8(0)
.L76:
	sts timer_on,r25
	sts (timer_cnt)+1,r21
	sts timer_cnt,r20
.L75:
	lds r24,timer_on
	tst r24
	brne .L75
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	ldi r24,hi8(500)
	cpi r18,lo8(500)
	cpc r19,r24
	brne .L76
	sbi 43-32,7
	ldi r18,lo8(0)
	ldi r19,hi8(0)
.L78:
	sts timer_on,r25
	sts (timer_cnt)+1,r21
	sts timer_cnt,r20
.L77:
	lds r24,timer_on
	tst r24
	brne .L77
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	cpi r18,100
	cpc r19,__zero_reg__
	brne .L78
	cbi 43-32,7
	rjmp .L89
.L73:
	ldi r25,lo8(1)
	ldi r20,lo8(10)
	ldi r21,hi8(10)
.L89:
	sbis 35-32,5
	rjmp .L79
	ldi r18,lo8(0)
	ldi r19,hi8(0)
	ldi r25,lo8(1)
	ldi r20,lo8(10)
	ldi r21,hi8(10)
.L81:
	sts timer_on,r25
	sts (timer_cnt)+1,r21
	sts timer_cnt,r20
.L80:
	lds r24,timer_on
	tst r24
	brne .L80
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	ldi r24,hi8(3500)
	cpi r18,lo8(3500)
	cpc r19,r24
	brne .L81
	ret
	.size	powerOnRobot, .-powerOnRobot
.global	baud
	.type	baud, @function
baud:
/* prologue: function */
/* frame size = 0 */
	mov r25,r24
	cpi r24,lo8(12)
	brlo .+2
	rjmp .L109
.L110:
	lds r24,192
	sbrs r24,5
	rjmp .L110
	ldi r24,lo8(-127)
	sts 198,r24
	lds r24,192
	ori r24,lo8(64)
	sts 192,r24
.L93:
	lds r24,192
	sbrs r24,5
	rjmp .L93
	sts 198,r25
.L94:
	lds r24,192
	sbrs r24,6
	rjmp .L94
/* #APP */
 ;  691 "cmcontrol.c" 1
	cli
 ;  0 "" 2
/* #NOAPP */
	cpi r25,lo8(11)
	brne .L95
	ldi r24,lo8(9)
	ldi r25,hi8(9)
	rjmp .L116
.L95:
	cpi r25,lo8(10)
	brne .L97
	ldi r24,lo8(19)
	ldi r25,hi8(19)
	rjmp .L116
.L97:
	cpi r25,lo8(9)
	brne .L98
	ldi r24,lo8(29)
	ldi r25,hi8(29)
	rjmp .L116
.L98:
	cpi r25,lo8(8)
	brne .L99
	ldi r24,lo8(39)
	ldi r25,hi8(39)
	rjmp .L116
.L99:
	cpi r25,lo8(7)
	brne .L100
	ldi r24,lo8(59)
	ldi r25,hi8(59)
	rjmp .L116
.L100:
	cpi r25,lo8(6)
	brne .L101
	ldi r24,lo8(79)
	ldi r25,hi8(79)
	rjmp .L116
.L101:
	cpi r25,lo8(5)
	brne .L102
	ldi r24,lo8(119)
	ldi r25,hi8(119)
	rjmp .L116
.L102:
	cpi r25,lo8(4)
	brne .L103
	ldi r24,lo8(239)
	ldi r25,hi8(239)
	rjmp .L116
.L103:
	cpi r25,lo8(3)
	brne .L104
	ldi r24,lo8(479)
	ldi r25,hi8(479)
	rjmp .L116
.L104:
	cpi r25,lo8(2)
	brne .L105
	ldi r24,lo8(959)
	ldi r25,hi8(959)
	rjmp .L116
.L105:
	cpi r25,lo8(1)
	brne .L106
	ldi r24,lo8(1919)
	ldi r25,hi8(1919)
	rjmp .L116
.L106:
	ldi r24,lo8(3839)
	ldi r25,hi8(3839)
.L116:
	sts (196)+1,r25
	sts 196,r24
/* #APP */
 ;  719 "cmcontrol.c" 1
	sei
 ;  0 "" 2
/* #NOAPP */
	ldi r18,lo8(0)
	ldi r19,hi8(0)
	ldi r25,lo8(1)
	ldi r20,lo8(10)
	ldi r21,hi8(10)
.L108:
	sts timer_on,r25
	sts (timer_cnt)+1,r21
	sts timer_cnt,r20
.L107:
	lds r24,timer_on
	tst r24
	brne .L107
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	cpi r18,100
	cpc r19,__zero_reg__
	brne .L108
.L109:
	ret
	.size	baud, .-baud
	.data
.LC0:
	.string	" "
	.text
.global	debugInt
	.type	debugInt, @function
debugInt:
	push r16
	push r17
	push r29
	push r28
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,16
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue: function */
/* frame size = 16 */
	movw r16,r28
	subi r16,lo8(-(1))
	sbci r17,hi8(-(1))
	movw r22,r16
	ldi r20,lo8(10)
	ldi r21,hi8(10)
	call itoa
	lds r24,myport
	movw r22,r16
	call cm_send_line
	lds r24,myport
	ldi r22,lo8(.LC0)
	ldi r23,hi8(.LC0)
	call cm_send_line
/* epilogue start */
	adiw r28,16
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
	pop r28
	pop r29
	pop r17
	pop r16
	ret
	.size	debugInt, .-debugInt
.global	debug
	.type	debug, @function
debug:
/* prologue: function */
/* frame size = 0 */
	movw r22,r24
	lds r24,myport
	call cm_send_line
/* epilogue start */
	ret
	.size	debug, .-debug
	.data
.LC1:
	.string	"i2c read error"
	.text
.global	i2cReadByte
	.type	i2cReadByte, @function
i2cReadByte:
/* prologue: function */
/* frame size = 0 */
	ldi r24,lo8(-92)
	sts 188,r24
.L122:
	lds r24,188
	sbrs r24,7
	rjmp .L122
	ldi r24,lo8(5)
	sts 187,r24
	ldi r24,lo8(-124)
	sts 188,r24
.L123:
	lds r24,188
	sbrs r24,7
	rjmp .L123
	lds r24,185
	sbrs r24,6
	rjmp .L124
	ldi r24,lo8(-60)
	sts 188,r24
.L125:
	lds r24,188
	sbrs r24,7
	rjmp .L125
	lds r25,187
	ldi r24,lo8(-108)
	sts 188,r24
	rjmp .L126
.L124:
	lds r24,myport
	ldi r22,lo8(.LC1)
	ldi r23,hi8(.LC1)
	call cm_send_line
	ldi r25,lo8(0)
.L126:
	mov r24,r25
/* epilogue start */
	ret
	.size	i2cReadByte, .-i2cReadByte
	.data
.LC2:
	.string	"i2c send error"
	.text
.global	i2cWriteByte
	.type	i2cWriteByte, @function
i2cWriteByte:
/* prologue: function */
/* frame size = 0 */
	mov r25,r24
	ldi r24,lo8(-92)
	sts 188,r24
.L132:
	lds r24,188
	sbrs r24,7
	rjmp .L132
	sts 187,r25
	ldi r24,lo8(-124)
	sts 188,r24
.L133:
	lds r24,188
	sbrs r24,7
	rjmp .L133
	lds r24,185
	andi r24,lo8(24)
	breq .L134
	sts 187,r22
	ldi r24,lo8(-124)
	sts 188,r24
.L135:
	lds r24,188
	sbrs r24,7
	rjmp .L135
	lds r24,185
	andi r24,lo8(40)
	breq .L134
	ldi r18,lo8(1)
	ldi r19,hi8(1)
	rjmp .L136
.L134:
	lds r24,myport
	ldi r22,lo8(.LC2)
	ldi r23,hi8(.LC2)
	call cm_send_line
	ldi r18,lo8(0)
	ldi r19,hi8(0)
.L136:
	movw r24,r18
/* epilogue start */
	ret
	.size	i2cWriteByte, .-i2cWriteByte
.global	turnServo
	.type	turnServo, @function
turnServo:
/* prologue: function */
/* frame size = 0 */
	mov r22,r24
	sts servoTurned,r24
	cpi r24,lo8(112)
	brne .L142
	sts servoTurned,__zero_reg__
.L142:
	ldi r24,lo8(4)
	ldi r25,hi8(4)
	call i2cWriteByte
/* epilogue start */
	ret
	.size	turnServo, .-turnServo
	.data
.LC3:
	.string	"---"
	.text
.global	readSensorInt
	.type	readSensorInt, @function
readSensorInt:
	push r15
	push r16
	push r17
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
	mov r22,r24
	ldi r24,lo8(4)
	ldi r25,hi8(4)
	call i2cWriteByte
	ldi r24,lo8(4)
	ldi r25,hi8(4)
	call i2cReadByte
	mov r15,r24
	ldi r24,lo8(4)
	ldi r25,hi8(4)
	call i2cReadByte
	mov r16,r24
	ldi r17,lo8(0)
	mov r25,r15
	ldi r24,lo8(0)
	movw r28,r24
	or r28,r16
	or r29,r17
	lds r24,myport
	ldi r22,lo8(.LC3)
	ldi r23,hi8(.LC3)
	call cm_send_line
	mov r24,r15
	ldi r25,lo8(0)
	call debugInt
	movw r24,r16
	call debugInt
	movw r24,r28
	call debugInt
	cpi r28,250
	cpc r29,__zero_reg__
	brsh .L145
	sbi 43-32,6
	rjmp .L146
.L145:
	cbi 43-32,6
.L146:
	movw r24,r28
/* epilogue start */
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	ret
	.size	readSensorInt, .-readSensorInt
.global	delayAndUpdateSensors
	.type	delayAndUpdateSensors, @function
delayAndUpdateSensors:
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
	ldi r18,lo8(1)
	sts timer_on,r18
	movw r18,r24
	ldi r21,3
1:	lsl r24
	rol r25
	dec r21
	brne 1b
	lsl r18
	rol r19
	add r24,r18
	adc r25,r19
	sts (timer_cnt)+1,r25
	sts timer_cnt,r24
	ldi r25,lo8(-114)
	mov r11,r25
	ldi r24,lo8(6)
	mov r12,r24
	clr r13
	inc r13
	clr r14
	dec r14
	mov r15,r14
	rjmp .L160
.L153:
	lds r24,sensors_flag
	tst r24
	breq .+2
	rjmp .L160
	ldi r18,lo8(0)
	ldi r19,hi8(0)
.L150:
	movw r30,r18
	subi r30,lo8(-(sensors_in))
	sbci r31,hi8(-(sensors_in))
	ld r24,Z
	movw r30,r18
	subi r30,lo8(-(sensors))
	sbci r31,hi8(-(sensors))
	st Z,r24
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	cpi r18,52
	cpc r19,__zero_reg__
	brne .L150
	lds r20,distance
	lds r21,(distance)+1
	lds r18,sensors+12
	lds r24,sensors+13
	mov r17,r18
	ldi r16,lo8(0)
	ldi r25,lo8(0)
	or r24,r16
	or r25,r17
	add r24,r20
	adc r25,r21
	sts (distance)+1,r25
	sts distance,r24
	lds r20,angle
	lds r21,(angle)+1
	lds r18,sensors+14
	lds r24,sensors+15
	mov r29,r18
	ldi r28,lo8(0)
	ldi r25,lo8(0)
	or r24,r28
	or r25,r29
	add r24,r20
	adc r25,r21
	sts (angle)+1,r25
	sts angle,r24
.L151:
	lds r24,192
	sbrs r24,5
	rjmp .L151
	sts 198,r11
.L152:
	lds r24,192
	sbrs r24,5
	rjmp .L152
	sts 198,r12
	sts sensors_index,__zero_reg__
	sts sensors_flag,r13
	ldi r24,lo8(116)
	call readSensorInt
	sts (customSensors)+1,r25
	sts customSensors,r24
	sts (customSensors+2)+1,r15
	sts customSensors+2,r14
.L160:
	lds r24,timer_on
	tst r24
	breq .+2
	rjmp .L153
/* epilogue start */
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	ret
	.size	delayAndUpdateSensors, .-delayAndUpdateSensors
	.data
.LC4:
	.string	"I: "
	.text
.global	initSensorMod
	.type	initSensorMod, @function
initSensorMod:
	push r17
/* prologue: function */
/* frame size = 0 */
.L162:
	ldi r24,lo8(4)
	ldi r25,hi8(4)
	ldi r22,lo8(105)
	call i2cWriteByte
	ldi r24,lo8(4)
	ldi r25,hi8(4)
	call i2cReadByte
	mov r17,r24
	lds r24,myport
	ldi r22,lo8(.LC4)
	ldi r23,hi8(.LC4)
	call cm_send_line
	mov r24,r17
	ldi r25,lo8(0)
	call debugInt
	cpi r17,lo8(104)
	brne .L162
/* epilogue start */
	pop r17
	ret
	.size	initSensorMod, .-initSensorMod
	.data
.LC5:
	.string	"Bump found: Cliff, SenBumpDrop, IR, Proximity: \r\n"
	.text
.global	checkBump
	.type	checkBump, @function
checkBump:
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
	movw r28,r24
	ldd r24,Y+2
	ldd r25,Y+3
	or r24,r25
	brne .L166
	ld r24,Y
	ldd r25,Y+1
	or r24,r25
	breq .L170
.L166:
	call isBumped
	tst r24
	breq .L170
	std Y+3,__zero_reg__
	std Y+2,__zero_reg__
	std Y+1,__zero_reg__
	st Y,__zero_reg__
	std Y+4,__zero_reg__
	std Y+6,__zero_reg__
	std Y+5,__zero_reg__
	ldi r24,lo8(-100)
	ldi r25,hi8(-100)
	ldi r22,lo8(-100)
	ldi r23,hi8(-100)
	call driveDirect
	ldi r18,lo8(0)
	ldi r19,hi8(0)
	ldi r25,lo8(1)
	ldi r20,lo8(10)
	ldi r21,hi8(10)
.L169:
	sts timer_on,r25
	sts (timer_cnt)+1,r21
	sts timer_cnt,r20
.L168:
	lds r24,timer_on
	tst r24
	brne .L168
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	ldi r24,hi8(1000)
	cpi r18,lo8(1000)
	cpc r19,r24
	brne .L169
	ldi r24,lo8(0)
	ldi r25,hi8(0)
	ldi r22,lo8(0)
	ldi r23,hi8(0)
	call driveDirect
	lds r24,myport
	ldi r22,lo8(.LC5)
	ldi r23,hi8(.LC5)
	call cm_send_line
.L170:
/* epilogue start */
	pop r29
	pop r28
	ret
	.size	checkBump, .-checkBump
	.data
.LC6:
	.string	"CMControl Initialized\r\n"
	.text
.global	initialize
	.type	initialize, @function
initialize:
/* prologue: function */
/* frame size = 0 */
/* #APP */
 ;  589 "cmcontrol.c" 1
	cli
 ;  0 "" 2
/* #NOAPP */
	ldi r24,lo8(16)
	out 36-32,r24
	ldi r24,lo8(-49)
	out 37-32,r24
	ldi r18,lo8(2)
	out 39-32,r18
	ldi r24,lo8(-1)
	out 40-32,r24
	in r24,40-32
	andi r24,lo8(-7)
	out 40-32,r24
	ldi r24,lo8(-26)
	out 42-32,r24
	ldi r24,lo8(125)
	out 43-32,r24
	in r24,36-32
	ori r24,lo8(12)
	out 36-32,r24
	in r24,37-32
	andi r24,lo8(-13)
	out 37-32,r24
	sts 128,__zero_reg__
	ldi r24,lo8(10)
	sts 129,r24
	ldi r24,lo8(229)
	ldi r25,hi8(229)
	sts (136)+1,r25
	sts 136,r24
	sts 111,r18
	ldi r24,lo8(19)
	ldi r25,hi8(19)
	sts (196)+1,r25
	sts 196,r24
	ldi r24,lo8(-104)
	sts 193,r24
	ldi r24,lo8(6)
	sts 194,r24
	ldi r24,lo8(0)
	ldi r22,lo8(3)
	call cm_init_UART
	sts myport,r24
	sts 185,__zero_reg__
	ldi r24,lo8(-88)
	sts 184,r24
	cbi 36-32,1
	sbi 37-32,1
/* #APP */
 ;  642 "cmcontrol.c" 1
	sei
 ;  0 "" 2
/* #NOAPP */
	ldi r18,lo8(0)
	ldi r19,hi8(0)
	ldi r25,lo8(1)
	ldi r20,lo8(10)
	ldi r21,hi8(10)
.L175:
	sts timer_on,r25
	sts (timer_cnt)+1,r21
	sts timer_cnt,r20
.L174:
	lds r24,timer_on
	tst r24
	brne .L174
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	cpi r18,250
	cpc r19,__zero_reg__
	brne .L175
	lds r24,myport
	ldi r22,lo8(.LC6)
	ldi r23,hi8(.LC6)
	call cm_send_line
/* epilogue start */
	ret
	.size	initialize, .-initialize
.global	main
	.type	main, @function
main:
	push r15
	push r16
	push r17
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
	call initialize
	call powerOnRobot
.L180:
	lds r24,192
	sbrs r24,5
	rjmp .L180
	ldi r24,lo8(-128)
	sts 198,r24
.L181:
	lds r24,192
	sbrs r24,5
	rjmp .L181
	ldi r24,lo8(-124)
	sts 198,r24
	in r24,43-32
	ori r24,lo8(96)
	out 43-32,r24
	ldi r25,lo8(1)
	ldi r20,lo8(10)
	ldi r21,hi8(10)
	rjmp .L182
.L185:
	ldi r18,lo8(0)
	ldi r19,hi8(0)
.L184:
	sts timer_on,r25
	sts (timer_cnt)+1,r21
	sts timer_cnt,r20
.L183:
	lds r24,timer_on
	tst r24
	brne .L183
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	ldi r24,hi8(500)
	cpi r18,lo8(500)
	cpc r19,r24
	brne .L184
.L182:
	sbis 35-32,1
	rjmp .L185
	cbi 43-32,5
	call initSensorMod
	cbi 43-32,6
	clr r15
	inc r15
	ldi r16,lo8(10)
	ldi r17,hi8(10)
.L191:
	ldi r28,lo8(0)
	ldi r29,hi8(0)
.L186:
	adiw r28,1
	ldi r24,lo8(5000)
	ldi r25,hi8(5000)
	call delayAndUpdateSensors
	cpi r28,90
	cpc r29,__zero_reg__
	brne .L186
	ldi r24,lo8(112)
	call turnServo
	ldi r18,lo8(0)
	ldi r19,hi8(0)
.L188:
	sts timer_on,r15
	sts (timer_cnt)+1,r17
	sts timer_cnt,r16
.L187:
	lds r24,timer_on
	tst r24
	brne .L187
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	ldi r24,hi8(1000)
	cpi r18,lo8(1000)
	cpc r19,r24
	brne .L188
	ldi r24,lo8(111)
	call turnServo
	ldi r18,lo8(0)
	ldi r19,hi8(0)
.L190:
	sts timer_on,r15
	sts (timer_cnt)+1,r17
	sts timer_cnt,r16
.L189:
	lds r24,timer_on
	tst r24
	brne .L189
	subi r18,lo8(-(1))
	sbci r19,hi8(-(1))
	ldi r24,hi8(1000)
	cpi r18,lo8(1000)
	cpc r19,r24
	brne .L190
	ldi r24,lo8(113)
	call turnServo
	rjmp .L191
	.size	main, .-main
.global	checkIr
	.type	checkIr, @function
checkIr:
/* prologue: function */
/* frame size = 0 */
	movw r30,r24
	lds r24,sensors+10
	cpi r24,lo8(-124)
	breq .L207
	cpi r24,lo8(-123)
	brsh .L212
	cpi r24,lo8(-126)
	brne .+2
	rjmp .L205
	cpi r24,lo8(-125)
	brsh .L206
	cpi r24,lo8(-127)
	breq .+2
	rjmp .L227
	rjmp .L235
.L212:
	cpi r24,lo8(-120)
	breq .L209
	cpi r24,lo8(-119)
	brsh .L213
	cpi r24,lo8(-123)
	breq .+2
	rjmp .L227
	rjmp .L236
.L213:
	cpi r24,lo8(-119)
	brne .+2
	rjmp .L210
	cpi r24,lo8(-112)
	breq .+2
	rjmp .L227
	in r24,43-32
	andi r24,lo8(-97)
	rjmp .L233
.L236:
	in r24,43-32
	ori r24,lo8(96)
.L233:
	out 43-32,r24
	ret
.L209:
	call doDrive
	ret
.L207:
	call doFollow
	ret
.L206:
	ld r18,Z
	ldd r19,Z+1
	cp r18,__zero_reg__
	cpc r19,__zero_reg__
	brne .L214
	ldd r24,Z+2
	ldd r25,Z+3
	or r24,r25
	brne .L214
	ldd r24,Z+4
	cpi r24,lo8(2)
	breq .L231
.L215:
	ldi r24,lo8(25)
	ldi r25,hi8(25)
	std Z+6,r25
	std Z+5,r24
	ldi r24,lo8(2)
	rjmp .L230
.L214:
	ldi r20,hi8(500)
	cpi r18,lo8(500)
	cpc r19,r20
	brne .L217
	ldd r24,Z+2
	ldd r25,Z+3
	sbiw r24,25
	rjmp .L229
.L217:
	subi r18,lo8(-(25))
	sbci r19,hi8(-(25))
	rjmp .L228
.L235:
	ld r18,Z
	ldd r19,Z+1
	cp r18,__zero_reg__
	cpc r19,__zero_reg__
	brne .L218
	ldd r24,Z+2
	ldd r25,Z+3
	or r24,r25
	brne .L218
	ldd r24,Z+4
	cpi r24,lo8(1)
	brne .L219
.L231:
	ldd r24,Z+5
	ldd r25,Z+6
	adiw r24,25
	std Z+6,r25
	std Z+5,r24
	rjmp .L216
.L219:
	ldi r24,lo8(25)
	ldi r25,hi8(25)
	std Z+6,r25
	std Z+5,r24
	ldi r24,lo8(1)
.L230:
	std Z+4,r24
	rjmp .L216
.L218:
	ldd r24,Z+2
	ldd r25,Z+3
	ldi r20,hi8(500)
	cpi r24,lo8(500)
	cpc r25,r20
	brne .L234
	subi r18,lo8(-(-25))
	sbci r19,hi8(-(-25))
.L228:
	std Z+1,r19
	st Z,r18
	rjmp .L216
.L205:
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
.L234:
	adiw r24,25
.L229:
	std Z+3,r25
	std Z+2,r24
	rjmp .L216
.L210:
	std Z+1,__zero_reg__
	st Z,__zero_reg__
	std Z+3,__zero_reg__
	std Z+2,__zero_reg__
	rjmp .L216
.L225:
	ldi r24,lo8(500)
	ldi r25,hi8(500)
	std Z+1,r25
	st Z,r24
.L226:
	ldd r24,Z+2
	ldd r25,Z+3
	subi r24,lo8(501)
	sbci r25,hi8(501)
	brlo .L221
	ldi r24,lo8(500)
	ldi r25,hi8(500)
	std Z+3,r25
	std Z+2,r24
.L221:
	ldd r24,Z+4
	cpi r24,lo8(1)
	breq .L223
	cpi r24,lo8(1)
	brlo .L222
	cpi r24,lo8(2)
	brne .L227
	rjmp .L237
.L222:
	ld r22,Z
	ldd r23,Z+1
	ldd r24,Z+2
	ldd r25,Z+3
	call driveDirect
	ret
.L223:
	ldd r24,Z+5
	ldd r25,Z+6
	ldi r22,lo8(1)
	ldi r23,hi8(1)
	rjmp .L232
.L237:
	ldd r24,Z+5
	ldd r25,Z+6
	ldi r22,lo8(-1)
	ldi r23,hi8(-1)
.L232:
	call drive
	ret
.L216:
	ld r24,Z
	ldd r25,Z+1
	subi r24,lo8(501)
	sbci r25,hi8(501)
	brsh .L225
	rjmp .L226
.L227:
	ret
	.size	checkIr, .-checkIr
.global	relayCommand
	.type	relayCommand, @function
relayCommand:
	push r17
/* prologue: function */
/* frame size = 0 */
	ldi r17,lo8(1)
	rjmp .L256
.L247:
	lds r24,myport
	call cm_read_char
	mov r25,r24
	cpi r24,lo8(-1)
	brne .L240
	lds r24,prevVal
	sbrs r24,7
	rjmp .L241
.L240:
	lds r24,localAck
	tst r24
	brne .L242
.L253:
	lds r24,192
	sbrs r24,5
	rjmp .L253
	sts 198,r25
	rjmp .L241
.L242:
	cpi r25,lo8(1)
	breq .L245
	cpi r25,lo8(2)
	brne .L244
	rjmp .L257
.L245:
	cbi 43-32,6
	rjmp .L244
.L257:
	sbi 43-32,6
.L244:
	sts localAck,__zero_reg__
.L241:
	sts prevVal,r25
	mov r24,r25
	call checkLocalAck
	tst r24
	breq .L256
	sts localAck,r17
.L256:
	lds r24,myport
	call cm_char_rdy
	tst r24
	brne .L247
	lds r24,receiveIndex
	tst r24
	breq .L251
	ldi r17,lo8(0)
	rjmp .L249
.L250:
	lds r24,myport
	mov r30,r17
	ldi r31,lo8(0)
	subi r30,lo8(-(receiveBuffer))
	sbci r31,hi8(-(receiveBuffer))
	ld r22,Z
	call cm_ByteXmt
	subi r17,lo8(-(1))
.L249:
	lds r24,receiveIndex
	cp r17,r24
	brlo .L250
	sts receiveIndex,__zero_reg__
.L251:
/* epilogue start */
	pop r17
	ret
	.size	relayCommand, .-relayCommand
.global	timer_cnt
.global	timer_cnt
	.section .bss
	.type	timer_cnt, @object
	.size	timer_cnt, 2
timer_cnt:
	.skip 2,0
.global	timer_on
.global	timer_on
	.type	timer_on, @object
	.size	timer_on, 1
timer_on:
	.skip 1,0
.global	receiveIndex
.global	receiveIndex
	.type	receiveIndex, @object
	.size	receiveIndex, 1
receiveIndex:
	.skip 1,0
.global	myport
.global	myport
	.type	myport, @object
	.size	myport, 1
myport:
	.skip 1,0
.global	prevVal
	.data
	.type	prevVal, @object
	.size	prevVal, 1
prevVal:
	.byte	-1
.global	localAckIndex
.global	localAckIndex
	.section .bss
	.type	localAckIndex, @object
	.size	localAckIndex, 1
localAckIndex:
	.skip 1,0
.global	localAck
.global	localAck
	.type	localAck, @object
	.size	localAck, 1
localAck:
	.skip 1,0
.global	doRelay
.global	doRelay
	.type	doRelay, @object
	.size	doRelay, 1
doRelay:
	.skip 1,0
.global	distance
.global	distance
	.type	distance, @object
	.size	distance, 2
distance:
	.skip 2,0
.global	angle
.global	angle
	.type	angle, @object
	.size	angle, 2
angle:
	.skip 2,0
.global	debugCount
.global	debugCount
	.type	debugCount, @object
	.size	debugCount, 1
debugCount:
	.skip 1,0
.global	servoTurned
.global	servoTurned
	.type	servoTurned, @object
	.size	servoTurned, 1
servoTurned:
	.skip 1,0
.global	sensors_flag
.global	sensors_flag
	.type	sensors_flag, @object
	.size	sensors_flag, 1
sensors_flag:
	.skip 1,0
.global	sensors_index
.global	sensors_index
	.type	sensors_index, @object
	.size	sensors_index, 1
sensors_index:
	.skip 1,0
	.comm receiveBuffer,64,1
	.comm sensors_in,52,1
	.comm sensors,52,1
	.comm customSensors,4,1
.global __do_copy_data
.global __do_clear_bss
