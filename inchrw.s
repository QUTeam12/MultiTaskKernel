.include	"equdefs.inc"
.global inbyte

.section .bss
.even
result:
	.ds.b	1
	.even

.text
.even

inbyte:
	movem.l	%d1-%d3, -(%sp)
	bra	loop

loop:
	move.l	#SYSCALL_NUM_GETSTRING, %d0	
	move.l	#0, %d1
	move.l	#result, %d2
	move.l	#1, %d3	
	trap	#0

	cmpi.l	#1, %d0
	bne	loop
	move.b	result, %d0

	movem.l	(%sp)+, %d1-%d3
	rts
	
