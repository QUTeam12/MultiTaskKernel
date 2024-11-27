.global P
.global V
.section .text
.even
********************
**Pシステムコールの入口サブルーチン
**入力：引数（セマフォID） 
**出力：d0=PシステムコールのID(=0)
**	d1=スタックから取り出した引数
*********************
P:
	movem.l (%sp),%d1	| %d1 = スタックから取り出した引数
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
	movem.l (%sp),%d1	| %d1 = スタックから取り出した引数
	move.l 	#1,%d0		| %d0 = Vシステムコールの ID(=1)
	trap #1
	rts
 
****************************************************************************************************
**pv_handler
***************************************************************************************************
** in -> d0 (p => 0 , v => 1) 
** in -> d1 (semaphoreId)
pv_handler:
	movem.l %d0-%d1/%a7, -(%sp) /* 実行タスクのレジスタ退避 */
	move.w %sr,-(%sp)	
	move.w #0x2700, %sr	/*割り込み禁止*/	
	move.l %d1,-(%a7) /*引数を追加*/
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
	move.w (%sp)+, %sr	/*走行レベル回復*/
	movem.l (%sp)+, %d0-%d1/%a7/*　レジスタ復帰*/
	rte
