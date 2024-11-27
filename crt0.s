        .data
        .align 2
environ:
        .long 0
 	.align	2

.extern main
.extern exit
.extern monitor_begin
.extern hardware_init_hook
.extern software_init_hook
.extern atexit
.extern __do_global_dtors
.extern __bss_start
.extern _end

.global start

.text
.even
	/* See if user supplied their own stack (__stack != 0).  If not, then
	 * default to using the value of %sp as set by the ROM monitor.
	 */
	movel	#__stack, %a0
	cmpl	#0, %a0
	jbeq    1f
	movel	%a0, %sp
1:
	/* set up initial stack frame */
	link	%a6, #-8

	/* zero out the bss section */
	movel	#__bss_start, %d1
	movel	#_end, %d0
	cmpl	%d0, %d1
	jbeq	3f
	movl	%d1, %a0
	subl	%d1, %d0
	subql	#1, %d0
2:
	clrb	(%a0)+
	dbra	%d0, 2b
	clrw	%d0
	subql	#1, %d0
	jbcc	2b
3:
	/* jmp monitor initialize after making up  bss */
	jmp	monitor_begin
start:
	/* monitor returns here.
	 * re-setup stack for C programs.
         * See if user supplied their own stack (__stack != 0).  If not, then
	 * default to using the value of %sp as set by the ROM monitor.
	 */
	movel	#__stack, %a0
	cmpl	#0, %a0
	jbeq    skip
	movel	%a0, %sp
skip:
	/* set up initial stack frame */
	link	%a6, #-8

	/*
	 * initialize target specific stuff. Only execute these
	 * functions it they exist.
	 */
	lea	hardware_init_hook, %a0
	cmpl	#0, %a0
	jbeq	4f
	jsr     (%a0)
4:
	lea	software_init_hook, %a0
	cmpl	#0, %a0
	jbeq	5f
	jsr     (%a0)
5:

	/*
	 * call the main routine from the application to get it going.
	 * main (argc, argv, environ)
	 * we pass argv as a pointer to NULL.
	 */

	/* put __do_global_dtors in the atexit list so 
	 * the destructors get run */
/* 
	movel	#__do_global_dtors,(%sp)
	jsr	atexit
	movel	#__FINI_SECTION__,(%sp)
	jsr	atexit
	jsr	__INIT_SECTION__
*/
	
	** setup argv, argc and jump into main
        pea     0
        pea     environ
        pea     %sp@(4)
        pea     0
	jsr	main
	movel	%d0, %sp@-

        jsr     exit
.even
