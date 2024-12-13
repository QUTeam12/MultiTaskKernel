#define NULLTASKID 0	// キューの終端
#define NUMSEMAPHORE 3	// セマフォの数。任意。
#define NUMTASK 5	// 最大タスク数。任意。
#define STKSIZE 8000	// スタックサイズ
#define UNDEFINED 0	// int型フィールドの初期化用

// TCB_TYPE.status
#define TCB_UNDEFINED 0
#define TCB_ACTIVE 1
#define TCB_FINISH 2




typedef int TASK_ID_TYPE;

typedef struct {
	int count;		// 現在のリソースアクセス状況(-無限から1)
	int nst; 
	TASK_ID_TYPE task_list;	// TODO: セマフォの一番前のタスクID。実行中なのか待機中なのかは不明
} SEMAPHORE_TYPE;



typedef struct {
	void (*task_addr)();	// タスクの関数ポインタ
	void *stack_ptr;	// タスク毎にユーザスタックとスーパーバイザスタックを分けるスタックのポインタ
	int priority;		// 優先度。今回は使わない。
	int status;		// TCBの使用状態。今回は使わない。
	TASK_ID_TYPE next;	// セマフォかreadyキューの次のタスクID。詳細はp33。
} TCB_TYPE;



typedef struct {
	char ustack[STKSIZE];	// ユーザスタック
	char sstack[STKSIZE];	// スーパーバイザスタック
} STACK_TYPE;
#ifdef MTK
//大域変数
SEMAPHORE_TYPE  semaphore[NUMSEMAPHORE];
TCB_TYPE        task_tab[NUMTASK+1];    // task_tab[1]からID=1のタスクを割り振る
STACK_TYPE      stacks[NUMTASK];        // stacks[0]からID=1のタスクを割り振る

TASK_ID_TYPE curr_task; // 現在実行中のタスクのID
TASK_ID_TYPE new_task;  // 現在登録作業中のタスクのID
TASK_ID_TYPE next_task; // 次に実行するタスクのID
TASK_ID_TYPE ready;     // 実行待ちタスクのキューの先頭タスクのID


//外部関数
extern void first_task();
extern void init_timer();
extern void pv_handler();
extern void swtch();
#else
extern void skipmt();
extern void P(TASK_ID_TYPE taskId);
extern void V(TASK_ID_TYPE taskId);
#endif
// 関数のプロトタイプ宣言
void init_kernel();
void* init_stack(TASK_ID_TYPE id);
void set_task(void (*user_task_func)());
void begin_sch();
void addq(TASK_ID_TYPE *pointer, TASK_ID_TYPE taskId);
TASK_ID_TYPE removeq(TASK_ID_TYPE *pointer);
void sleep(int ch);
void wakeup(int ch);
void p_body(TASK_ID_TYPE semaphoreId);
void v_body(TASK_ID_TYPE semaphoreId);
void sched();


