.include "equdefs.inc"
.global skipmt
.global first_task
.global P
.global V
.global first_task
.global pv_handler
.global hard_clock
.global init_timer
.global swtch

.extern curr_task
.extern next_task
.extern	task_tab
.extern ready

.extern p_body
.extern v_body
.extern addq
.extern sched
.extern stacks
.extern printdebug

.section .text
.even

***********************
**skipmt
*********************
skipmt:	
	movem.l %d0, -(%sp)
	move.l #SYSCALL_NUM_SKIPMT, %d0
	trap #0
	movem.l (%sp)+, %d0
	rts

*********************
** debug
******************
debug:
	movem.l %d0, -(%sp)
	jsr printdebug
	movem.l (%sp)+, %d0
	rts
********************
** first_task: カーネル使用スタックをcurr_taskのタスクに切り替えマルチタスク処理を開始するサブルーチン
** 起動時点でスーパーバイザモードである必要がある
** 製作者: 執行
*********************
first_task: 
	move.w #0x2000, %sr
	lea.l	task_tab, %a0	| TCB配列の先頭アドレス
	move.l	curr_task, %d1	| 現在のタスクID
	mulu.w	#20, %d1
	add.l	#4, %d1		| TCBの先頭から4バイト目にSSPが格納されているため4を加算
	add.l	%d1, %a0	| curr_taskが指すTCBのアドレス計算
	move.l	(%a0), %sp	| TCBに記録されるSSPの回復	
	move.l (%sp)+,%a0
	move.l %a0,%usp
	movem.l	(%sp)+, %d0-%d7/%a0-%a6	| SSPに積まれる残り15本のレジスタの回復
	rte			| SR, PCを回復してユーザタスク開始

********************
** Pシステムコールの入口サブルーチン
** 入力：引数（セマフォID） 
** 出力：d0=PシステムコールのID(=0)
**	 d1=スタックから取り出した引数
** 製作者: 宗藤 
*********************
P:
 	add.l 	#4, %sp
	move.l  (%sp),%d1		| %d1 = スタックから取り出した引数
	move.l 	#0,%d0		| %d0 = Pシステムコールの ID(=0)
	trap #1
	sub.l #4, %sp
	rts

********************
** Vシステムコールの入口サブルーチン
** 入力：引数（セマフォID） 
** 出力：d0=VシステムコールのID(=1)
**	 d1=スタックから取り出した引数
** 製作者: 宗藤 
*********************
V:
 	add.l 	#4, %sp
	move.l 	(%sp),%d1		| %d1 = スタックから取り出した引数
	move.l 	#1,%d0		| %d0 = Vシステムコールの ID(=1)
	trap #1
	sub.l #4, %sp
	rts
 
********************
** TRAP #1割り込み処理ルーチン
** in -> d0 (p => 0 , v => 1) 
** in -> d1 (semaphoreId)
** 製作者: 首藤
*********************
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
** タイマ割り込みルーチン
** 製作者: 宮坂
*****************************
hard_clock:
	movem.l %d0-%d7/%a0-%a6, -(%sp)
	move.l  curr_task, -(%sp)
	lea.l ready, %a0
	move.l  %a0, -(%sp)
	jsr     addq
 	add.l  #8, %sp
	jsr     sched
	jsr     swtch
	movem.l (%sp)+, %d0-%d7/%a0-%a6
	rts

*****************************
** クロック割り込みルーチン
** 製作者: 宮坂
*****************************
init_timer:
	move.l #SYSCALL_NUM_RESET_TIMER,%D0
	trap #0
	move.l #SYSCALL_NUM_SET_TIMER, %D0
	move.w  #100, %d1 /* Linux周期。10ms */
	move.l  #hard_clock, %d2
	trap #0
	rts

******************************
**swtch
**製作者:小紫
**[TODO]４日目以降の目標に書かれている注意事項に未対応
******************************
swtch:
	move.w 	%sr,-(%sp)	/*SRの値をスタックに積んでRTEで復帰できるようにする*/
	movem.l %d0-%d7/%a0-%a6, -(%sp)
	move.l %usp, %a0
	move.l	%a0,-(%sp)	/*USPの値をスタックに積む*/
	/*ここからしばらくSSPの保存*/
	lea.l	task_tab, %a0	| TCB配列の先頭アドレス
	move.l	curr_task, %d1	| 現在のタスクID
	mulu.w	#20, %d1
	add.l	#4, %d1		| TCBの先頭から4バイト目にSSPが格納されているため2を加算
	add.l	%d1, %a0	| curr_taskが指すTCBのアドレス計算
	move.l %sp, (%a0)      |SSPを正しい位置に記録
	
	move.l next_task, curr_task	|curr_taskにnext_taskをいれた
	lea.l	task_tab, %a0	| TCB配列の先頭アドレス
	move.l	curr_task, %d1	| 現在のタスクID
	mulu.w	#20, %d1
	add.l	#4, %d1		| TCBの先頭から4バイト目にSSPが格納されているため4を加算
	add.l	%d1, %a0	| curr_taskが指すTCBのアドレス計算
	move.l	(%a0), %sp	| TCBに記録されるSSPの回復	
	
	move.l (%sp)+,%a0
	move.l %a0,%usp
	movem.l	(%sp)+, %d0-%d7/%a0-%a6	| SSPに積まれる残り15本のレジスタの回復
	rte			| SR, PCを回復してユーザタスク開始
