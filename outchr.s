.include "equdefs.inc"
.global outbyte

.text
.even

outbyte:
	movem.l	%d1-%d3, -(%sp)
	bra	loop

loop:
	move.l	#SYSCALL_NUM_PUTSTRING, %d0
	move.l	#0, %d1
	move.l	%sp, %d2
	add.l	#19, %d2
	move.l	#1, %d3
	trap	#0

	cmpi.l	#1, %d0
	bne	loop

	movem.l	(%sp)+, %d1-%d3
	rts
