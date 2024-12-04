.global first_task
.global P
.global V
.global pv_handler

.extern curr_task
.extern	task_tab
.extern p_body
.extern v_body

.section .text
.even

********************
** first_task: カーネル使用スタックをcurr_taskのタスクに切り替えマルチタスク処理を開始するサブルーチン
** 起動時点でスーパーバイザモードである必要がある
*********************
first_task: 
	move.l	task_tab, %d0	| TCB配列の先頭アドレス
	move.l	curr_task, %d1	| 現在のタスクID
	mulu.w	#10, %d1
	add.l	#2, %d1		| TCBの先頭から4バイト目にSSPが格納されているため4を加算
	add.l	%d1, %d0	| curr_taskが指すTCBのアドレス計算
	move.l	(%d0), %sp	| TCBに記録されるSSPの回復	
	move.l	(%sp)+, %a7	| スタックからUSPを取り出し
	movem.l	(%sp)+, %d0-%d7/%a0-%a6	| SSPに積まれる残り15本のレジスタの回復
	rte			| SR, PCを回復してユーザタスク開始

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
