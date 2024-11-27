#NO_APP
	.file	"test1.c"
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	link.w %fp,#0
	pea 97.w
	jsr outbyte
	addq.l #4,%sp
	moveq #0,%d0
	unlk %fp
	rts
	.size	main, .-main
	.ident	"GCC: (GNU) 11.4.0"
outbyte:
	rts
