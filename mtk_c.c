#include "mtk_c.h"

extern void first_task();
extern void init_timer();
extern void pv_handler();
SEMAPHORE_TYPE	semaphore[NUMSEMAPHORE];
TCB_TYPE	task_tab[NUMTASK+1];	// task_tab[1]からID=1のタスクを割り振る
STACK_TYPE	stacks[NUMTASK];	// stacks[0]からID=1のタスクを割り振る

TASK_ID_TYPE curr_task;	// 現在実行中のタスクのID
TASK_ID_TYPE new_task;	// 現在登録作業中のタスクのID
TASK_ID_TYPE next_task;	// 次に実行するタスクのID
TASK_ID_TYPE ready;	// 実行待ちタスクのキューの先頭タスクのID

/***********************************
 * @brief カーネルの初期化
 * @author 小紫
 **********************************/
void init_kernel() {
	// TCB配列の初期化
	for(int i=1; i <= NUMTASK; i++){
		task_tab[i].task_addr = NULL;
		task_tab[i].stack_ptr = NULL;
		task_tab[i].priority = UNDEFINED;
		task_tab[i].status = TCB_UNDEFINED;
		task_tab[i].next = NULLTASKID;
	}
	// readyキューの初期化
	ready = NULLTASKID;
	// PVシステムコールの割り込み処理ルーチン(pv_handler)をTRAP1の割り込みベクタに登録
	*(void(**) ())0x084 = pv_handler; // TODO: trap1のアドレスがあってるかわからない
	// セマフォのフィールド群の初期化
	for(int i=0; i< NUMSEMAPHORE;i++){
		semaphore[i].count = 1; // TODO: 初期のリソースアクセス状況は1だが正直わからん
		semaphore[i].nst = UNDEFINED;
		semaphore[i].task_list 	= NULLTASKID;
	}
}

/***********************************
 * @brief ユーザタスクの用のスタック初期化
 * @param id: タスクID
 * @author 小紫
 **********************************/
void* init_stack(TASK_ID_TYPE id) {
	int *ssp;
	// タスクのエントリポイントをtask_addrに設定
	task_tab[id].task_addr = (void (*)(void))0x12345678; // TODO: 0x12345678は例なので後で消す
  	// スタックポインタsspをスタックの末尾に設定
  	ssp = (int *)(&stacks[id-1].sstack[STKSIZE]);
 	// スタックにタスクのアドレスをプッシュ
  	*(--ssp) = (int)task_tab[id].task_addr;
	//initial SRを0x0000に設定
	// TODO: アドレス計算再チェック
	ssp = (int *)(ssp - 1); // ssp のアドレスを 2 バイト減らす
	*(ssp) = (unsigned short int)0;
	//sspを15x4byte for register分減らす
	ssp -= 30;	
	//ユーザースタックへのポインタを追加
	*(--ssp) = (int)(&stacks[id -1].ustack[STKSIZE]);
	return ssp;
}

/***********************************
 * @brief ユーザタスクの初期化と登録
 * @param user_task_func: ユーザタスク関数へのポインタ(タスク関数の先頭番地)
 * @author 執行
 **********************************/
void set_task(void (*user_task_func)()) {
	for(int i=1; i <= NUMTASK; i++){
		if (task_tab[i].task_addr == NULL) { 
			// TCB配列の空きスロットにユーザタスク関数を定義
			new_task = i;
			task_tab[i].task_addr = user_task_func;
			task_tab[i].status = TCB_ACTIVE;
			task_tab[i].stack_ptr = init_stack(new_task);
			ready = new_task;
			return;
		}
	}
}

/***********************************
 * @brief 最初のタスクとタイマを設定してタスク起動
 * @author 執行
 **********************************/
void begin_sch() {
	curr_task = removeq(&ready);
	init_timer();
	first_task();
}

/***********************************
 * @brief タスクのキューの最後尾へのTCBの追加
 * @param pointer: キューへのポインタ
 * @param taskId: 登録したいタスクのID
 * @author 首藤・宗藤
 **********************************/
void addq(TASK_ID_TYPE pointer, TASK_ID_TYPE taskId){
	TASK_ID_TYPE next_task = task_tab[pointer].next; // キューの先頭から次のタスクを取得
	while(1){
		if(next_task == NULLTASKID){
			task_tab[pointer].next = taskId; // キューの最後尾にタスクを追加	
			break;
		}else{
			next_task = task_tab[next_task].next;
		}
	}
}

/***********************************
 * @brief タスクのキューの先頭からのTCBの除去
 * @param pointer: キューへのポインタ
 * @return キューから取り除いたタスクのID
 * @author 首藤・宗藤
 **********************************/
TASK_ID_TYPE removeq(TASK_ID_TYPE *pointer){
	TASK_ID_TYPE retval = *pointer;
	*pointer = task_tab[*pointer].next;
	return retval;	
}

/***********************************
 * @brief タスクを休眠状態にする
 * @param ch: チャンネルだがセマフォIDとしてよい
 * @author 宗藤
 **********************************/
void sleep(int ch){
	addq(semaphore[ch].task_list, curr_task);  // セマフォにcurr_taskを追加
	sched();
	swtch();
}

/***********************************
 * @brief タスクを実行可能状態にする
 * @param ch: チャンネルだがセマフォIDとしてよい
 * @author 宗藤
 **********************************/
void wakeup(int ch){
	int task = removeq(&semaphore[ch].task_list); // task = セマフォから取り出したタスク
	if(task != NULLTASKID){
		addq(ready, task);  // readyにtaskを追加
	}
}

/***********************************
 * @brief P命令のうちセマフォ値とタスクリストの操作を行う
 * @param semaphoreId: セマフォID
 * @author 首藤
 **********************************/
void p_body(TASK_ID_TYPE semaphoreId){
	semaphore[semaphoreId].count -= 1; // セマフォの値を減らす
	if(semaphore[semaphoreId].count < 0){
		// タスクを休眠状態に
		sleep(semaphoreId);
	}
}

/***********************************
 * @brief V命令のうちセマフォ値とタスクリストの操作を行う
 * @param semaphoreId: セマフォID
 * @author 首藤
 **********************************/
void v_body(TASK_ID_TYPE semaphoreId){
	semaphore[semaphoreId].count += 1;
	if(semaphore[semaphoreId].count <= 0){
		wakeup(semaphoreId);
	}
}

/************************
** @brief ready キューの先頭のタスク ID を取り出し，next task にセットする
** @author 宮坂
*************************/
void sched(){
	TASK_ID_TYPE a = removeq(&ready);
	next_task = a;
	
	if(next_task == NULLTASKID){
		sched();
	}
}
