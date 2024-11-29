#define NULLTASKID 0 // キューの終端
#define NUMSEMAPHORE 3 // TODO: セマフォの数がわからないから暫定の値
#define NUMTASK 5 // 最大タスク数
#define STKSIZE 8000 // スタックサイズ
#define UNDEFINED 0 // int型フィールドの初期化用

// TCB_TYPE.status
#define TCB_UNDEFINED 0
#define TCB_ACTIVE 1
#define TCB_FINISH 2
