#include "mtk_c.h"
#include <stdio.h>
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
	TASK_ID_TYPE next; // セマフォかreadyキューの次のタスクID
} TCB_TYPE;

TCB_TYPE	task_tab[NUMTASK+1]; // task_tab[1]からID=1のタスクを割り振る

typedef struct {
	char ustack[STKSIZE]; // ユーザスタック
	char sstack[STKSIZE]; // スーパーバイザスタック
} STACK_TYPE;

STACK_TYPE	stacks[NUMTASK]; // stacks[0]からID=1のタスクを割り振る

void init_kernel() {
	// TCB配列の初期化
	for(int i=1; i <= NUMTASK; i++){
		task_tab[i].task_addr = NULL;
		task_tab[i].stack_ptr = NULL;
		task_tab[i].priority = UNDEFINED;
		task_tab[i].status = UNDEFINED;
		task_tab[i].next = NULLTASKID;
	}
	// readyキューの初期化
	TASK_ID_TYPE ready = NULLTASKID;
	// TODO: PVシステムコールの割り込み処理ルーチン(pv_handler)をTRAP1の割り込みベクタに登録する
	*(void(**) ())0x084 = pv_handler; // TODO: trap1のアドレスがあってるかわからない
	// セマフォの値の初期化
	for(int i=0; i< NUMSEMAPHORE;i++){
		semaphore[i].count = 1; // TODO: 初期のリソースアクセス状況は1だが正直わからん
		semaphore[i].nst = UNDEFINED;// TODO: nstの意味がそもそもわからない
		semaphore[i].task_list 	= NULLTASKID;
	}
}


//semaphore
// タスクを休眠状態にする関数
void sleep(int ch){
	task_tab[curr_task].next = semaphore[ch].task_list;  // セマフォの先頭に実行中タスクを追加
	semaphore[ch].task_list = curr_task;  		     // セマフォの先頭を更新
	sched();
	swtch();
}


// タスクを実行可能状態にする関数
void wakeup(int ch){
	int task = semaphore[ch].task_list;  			// task = 実行可能にしたいタスク
	if(task != NULLTASKID){
		semaphore[ch].task_list = task_tab[task].next;  // セマフォの先頭を更新
		task_tab[task].next = ready;  			// readyの先頭にtaskを連結
		ready = task;  					// readyの更新
	}
}

void p_body(int semaphoreId){
	semaphore[semaphoreId].count -= 1; /* セマフォの値を減らす */
	if(semaphore[semaphoreId].count < 0){
		// タスクを休眠状態に
		sleep(semaphoreId);
	}
}

void v_body(int semaphoreId){
	semaphore[semaphoreId].count += 1;
	if(semaphore[semaphoreId].count <= 0){
		wakeup(semaphoreId);
	}
}

