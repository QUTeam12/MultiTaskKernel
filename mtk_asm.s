.include "equdefs.inc"

.global P
.global V
.global first_task
.global pv_handler
.global hard_clock
.global init_timer
.global swtch

.extern curr_task
.extern next_task
.extern ready
.extern	task_tab

.extern addq
.extern p_body
.extern v_body
.extern sched
.extern stacks

.section .text
.even

********************
** first_task: カーネル使用スタックをcurr_taskのタスクに切り替えマルチタスク処理を開始するサブルーチン
** 起動時点でスーパーバイザモードである必要がある
** 製作者: 執行
*********************
first_task:
	move.w #0x2000, %SR	| スーパーバイザモード(割り込み許可)
	move.l	task_tab, %d0	| TCB配列の先頭アドレス
	move.l	curr_task, %d1	| 現在のタスクID
	mulu.w	#10, %d1
	add.l	#2, %d1		| TCBの先頭から4バイト目にSSPが格納されているため4を加算
	add.l	%d1, %d0	| curr_taskが指すTCBのアドレス計算
	move.l	%d0, %sp	| TCBに記録されるSSPの回復	
	move.l	(%sp)+, %a0	| %uspの制約によりアドレスレジスタからmoveする必要がある
	move.l	%a0, %usp	| スタックからUSPを取り出し
	movem.l	(%sp)+, %d0-%d7/%a0-%a6	| SSPに積まれる残り15本のレジスタの回復
	move.b	#'e', LED4 || TODO: debug
	rte			| SR, PCを回復してユーザタスク開始

********************
** Pシステムコールの入口サブルーチン
** 入力：引数（セマフォID） 
** 出力：d0=PシステムコールのID(=0)
**	 d1=スタックから取り出した引数
** 製作者: 宗藤 
*********************
P:
	move.l  %sp,%d1		| %d1 = スタックから取り出した引数
 	addi.l 	#4, %d1
	move.l 	#0,%d0		| %d0 = Pシステムコールの ID(=0)
	trap #1
	rts

********************
** Vシステムコールの入口サブルーチン
** 入力：引数（セマフォID） 
** 出力：d0=VシステムコールのID(=1)
**	 d1=スタックから取り出した引数
** 製作者: 宗藤 
*********************
V:
	move.l 	%sp,%d1		| %d1 = スタックから取り出した引数
 	addi.l 	#4, %d1
	move.l 	#1,%d0		| %d0 = Vシステムコールの ID(=1)
	trap #1
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
	move.b	#'h', LED3 | TODO: debug
	movem.l %d0-%d7/%a0-%a6, -(%sp)
	move.l  curr_task, -(%sp)
	move.l  ready, -(%sp)
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
	move.l #SYSCALL_NUM_SET_TIMER, %D0
	|| move.w  #0x64, %d1 /* Linux周期。10ms */
	move.w  #0x2710, %d1 /* 動作確認用。1sec。 */
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
	move.l	%a7,-(%sp)	/*USPの値をスタックに積む*/
	movem.l %a0, -(%sp)     | 使用レジスタの退避[TODO]レジスタ追加
	/*ここからしばらくSSPの保存*/
	move.l	task_tab, %d0	| TCB配列の先頭アドレス
	move.l	curr_task, %d1	| 現在のタスクID
	mulu.w	#10, %d1
	add.l	#2, %d1		| TCBの先頭から4バイト目にSSPが格納されているため2を加算
	add.l	%d1, %d0	| curr_taskが指すTCBのアドレス計算
	move.l	%d0, %a0	|上までで計算したTCBのSSPの格納先アドレス
	
        move.l  stacks, %d0   	| stacks配列の先頭アドレス
        move.l  curr_task, %d1  | 現在のタスクID
	sub.l	#1,%d1		| stacks配列では0スタートだが、taskIDは1スタートなので、変換

	/*ここからしばらくワード型で計算taskIDは多くても10とかだし、STKSIZEは8000なので、二倍して16000これを10倍してもワード型をあふれることはないだろう*/
	mulu.w	#STKSIZE, %d1	|ここまでの計算で(taskID -1) * STKSIZE(char(1Byte)*STKSIZE) / 2(2Byteで1アドレス) * 2(構造体内の配列の個数)をした
	
        add.l   #half_STKSIZE, %d1         | stacksの先頭からSTKSIZE Byte目にSSPが格納されているためSTKSIZE/2を加算
        add.l   %d1, %d0        | curr_taskのstacksのSSPのアドレスをd0に保存
	move.l	%d0,(%a0)		|TCBのSSPアドレスにSSPのアドレスを保存	
	/*SSPの保存終わり*/
	
	move.l next_task, curr_task	|curr_taskにnext_taskをいれた
	move.l	%a0, %sp	| TCBに記録されるSSPの回復	
	
	move.l	(%sp)+, %a7	|USPの値を回復
	movem.l (%sp)+, %d0-%d7/%a0-%a6
	rte
