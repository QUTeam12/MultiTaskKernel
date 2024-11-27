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

