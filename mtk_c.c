#include "mtk_c.h"
#include <stdio.h>

extern void first_task();
extern void init_timer();
extern void pv_handler();

typedef int TASK_ID_TYPE;

typedef struct {
	int count; // 現在のリソースアクセス状況(-無限から1)
	int nst; 
	TASK_ID_TYPE task_list; // TODO: セマフォの一番前のタスクID。実行中なのか待機中なのかは不明
} SEMAPHORE_TYPE;

SEMAPHORE_TYPE	semaphore[NUMSEMAPHORE];

typedef struct {
	void (*task_addr)(); // タスクの関数ポインタ
	void *stack_ptr; // タスク毎にユーザスタックとスーパーバイザスタックを分けるスタックのポインタ
	int priority; // 優先度。今回は使わない。
	int status; // TCBの使用状態。今回は使わない。
	TASK_ID_TYPE next; // セマフォかreadyキューの次のタスクID。詳細はp33。
} TCB_TYPE;

TCB_TYPE	task_tab[NUMTASK+1]; // task_tab[1]からID=1のタスクを割り振る

typedef struct {
	char ustack[STKSIZE]; // ユーザスタック
	char sstack[STKSIZE]; // スーパーバイザスタック
} STACK_TYPE;

STACK_TYPE	stacks[NUMTASK]; // stacks[0]からID=1のタスクを割り振る

TASK_ID_TYPE curr_task;
TASK_ID_TYPE new_task;
TASK_ID_TYPE next_task;
TASK_ID_TYPE ready;

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
		semaphore[i].nst = UNDEFINED;// TODO: nstの意味がそもそもわからない
		semaphore[i].task_list 	= NULLTASKID;
	}
}

void* init_stack(TASK_ID_TYPE id) {
	int *ssp;
	// タスクのエントリポイントをtask_addrに設定
	task_tab[id].task_addr = (void (*)(void))0x12345678; // TODO: 0x12345678は例なので後で消す
  	// スタックポインタsspをスタックの末尾に設定
  	ssp = (int *)(&stacks[id-1].sstack); 
	ssp += NUMTASK; // TODO: NUMTASKの加算あってる？
 	// スタックにタスクのアドレスをプッシュ
  	*(--ssp) = (int)task_tab[id].task_addr;
	//initial SRを0x0000に設定
	ssp = (int *)((char *)ssp - 2); // ssp のアドレスを 2 バイト減らす
	*(ssp) = (unsigned short int)0;
	//sspを15x4byte for register分減らす
	ssp -= 15;	
	//ユーザースタックへのポインタを追加
	*(--ssp) = (int)(&stacks[id -1].ustack);
	return ssp;
}

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

void begin_sch() {
	// 最初のタスクとタイマを設定してタスク起動
	curr_task = removeq(); // TODO: 引数わかんない
	init_timer();
	first_task();
}

//semaphore
// タスクを休眠状態にする関数
void sleep(TASK_ID_TYPE ch){
	task_tab[curr_task].next = semaphore[ch].task_list;  // セマフォの先頭に実行中タスクを追加
	semaphore[ch].task_list = curr_task;  		     // セマフォの先頭を更新
	sched();
	swtch();
}


// タスクを実行可能状態にする関数
void wakeup(TASK_ID_TYPE ch){
	int task = semaphore[ch].task_list;  			// task = 実行可能にしたいタスク
	if(task != NULLTASKID){
		semaphore[ch].task_list = task_tab[task].next;  // セマフォの先頭を更新
		task_tab[task].next = ready;  			// readyの先頭にtaskを連結
		ready = task;  					// readyの更新
	}
}

void p_body(TASK_ID_TYPE semaphoreId){
	semaphore[semaphoreId].count -= 1; /* セマフォの値を減らす */
	if(semaphore[semaphoreId].count < 0){
		// タスクを休眠状態に
		sleep(semaphoreId);
	}
}

void v_body(TASK_ID_TYPE semaphoreId){
	semaphore[semaphoreId].count += 1;
	if(semaphore[semaphoreId].count <= 0){
		wakeup(semaphoreId);
	}
}
