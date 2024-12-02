.include "equdefs.inc"
.global P
.global V
.global pv_handler
.global hard_clock
.global init_timer
.extern p_body
.extern v_body
.extern curr_task
.extern ready
.extern addq
.extern sched
.extern swtch

.section .text
.even
********************
**Pシステムコールの入口サブルーチン
**入力：引数（セマフォID） 
**出力：d0=PシステムコールのID(=0)
**	d1=スタックから取り出した引数
*********************
P:
	move.l  %sp,%d1		| %d1 = スタックから取り出した引数
 	addi.l 	#4, %d1
	move.l 	#0,%d0		| %d0 = Pシステムコールの ID(=0)
	trap #1
	rts

********************
**Vシステムコールの入口サブルーチン
**入力：引数（セマフォID） 
**出力：d0=VシステムコールのID(=1)
**	d1=スタックから取り出した引数
*********************
V:
	move.l 	%sp,%d1		| %d1 = スタックから取り出した引数
 	addi.l 	#4, %d1
	move.l 	#1,%d0		| %d0 = Vシステムコールの ID(=1)
	trap #1
	rts
 
****************************************************************************************************
**pv_handler
***************************************************************************************************
** in -> d0 (p => 0 , v => 1) 
** in -> d1 (semaphoreId)
pv_handler:
	movem.l %d0-%d1, -(%sp) /* 実行タスクのレジスタ退避 */
	move.w %sr,-(%sp)	
	move.w #0x2700, %sr	/*割り込み禁止*/	
	move.l %d1,-(%sp) /*引数を追加*/
	cmpi.l #0, %d0	
	beq call_p_body
	bra call_v_body
call_p_body:
	jsr p_body
	bra pv_handler_end
call_v_body:
	jsr v_body
	bra pv_handler_end
pv_handler_end:
	/* TODO:引数除去必要か要検証 */
	add.l #4, %sp
	move.w (%sp)+, %sr	/*走行レベル回復*/
	movem.l (%sp)+, %d0-%d1/*　レジスタ復帰*/
	rte

*****************************
**hard_clock
*****************************
 hard_clock:
	movem.l %d0-%d7/%a0-%a6, -(%sp)
	move.l  curr_task, -(%sp)
	move.l  ready, -(%sp)
	jsr     addq
 	addi.l  #8, %sp
	jsr     sched
	jsr     swtch
	movem.l (%sp)+, %d0-%d7/%a0-%a6
	rts

*******************************
**init_timer
*******************************
init_timer:
	move.l #SYSCALL_NUM_SET_TIMER, %D0
	move.w  #0x64, %d1 /* Linux周期。10ms */
	** move.w  #0x2710, %d1 /* 動作確認用。1sec。 */
	move.l  #hard_clock, %d2
	trap #0
	rts
