#define TEST2
#include "mtk_c.h"
#include <stdio.h>

void task1(){
	printf("task1enter\n");
	if(1 == NUMSEMAPHORE) V(1);
	P(1);
	while(1){
		/*some tasks */
		printf("task1\n");
		V((1 % NUMSEMAPHORE) +1);
		P(1);
	}
}
void task2(){
	printf("task2enter\n");
	if(2 == NUMSEMAPHORE) V(1);
	P(2);
	while(1){
		/*some tasks */
		printf("task2\n");
		V((2 % NUMSEMAPHORE) +1);
		P(2);
	}
}
void task3(){
	printf("task3enter\n");
	if(3 == NUMSEMAPHORE) V(1);
	P(3);
	while(1){
		/*some tasks */
		printf("task3\n");
		V((3 % NUMSEMAPHORE) +1);
		P(3);
	}
}


int main() {
	init_kernel();
	set_task(task1);
	set_task(task2);
	set_task(task3);
	begin_sch();
	return 0;
}
