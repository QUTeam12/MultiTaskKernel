#include "mtk_c.h"


typedef int TASK_ID_TYPE;

typedef struct {
	int count;
	int nst; /+ reserved +/
	TASK_ID_TYPE task_list;
} SEMAPHORE_TYPE;

SEMAPHORE_TYPE	semaphore[NUMSEMAPHORE];

typedef struct {
	void (*task_addr)();
	void *stack_ptr;
	int priority;
	int status;
	TASK_ID_TYPE next;
} TCB_TYPE;

TCB_TYPE	task_tab[NUMTASK+1];

typedef struct {
	char ustack[STKSIZE];
	char sstack[STKSIZE];
} STACK_TYPE;

STACK_TYPE	stacks[NUMTASK];

void init_kernel(){

	//TCV配列の初期化
	for(i=0; i< NUMTASK;i++){
		task_tab[i].task_addr	=NULL
		task_tab[i].stack_ptr	=NULL
		task_tab[i].priority 	=NULL
		task_tab[i].status	=NULL
	}
	//readyキューの初期化
	TASK_ID_TYPE=NULLTASKID
	//[TODO]PVシステムコールの割り込み処理ルーチン(pv_handler)をTRAP1の割り込みベクタに登録する
	//セマフォの値を初期化
	for(i=0; i< NUMSEMAPHORE;i++){
		semaphore[i].count	=NULL
		semaphore[i].nst	=NULL
		semaphore[i].task_list 	=NULL
	}


}
