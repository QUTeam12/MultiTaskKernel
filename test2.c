#define TEST2
#include "mtk_c.h"
#include <stdio.h>

void b_task1(){
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
void b_task2(){
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
void b_task3(){
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

void c_task1(){
	int f = 0;
	while(1){
		printf("task1_a\n");
		P(0);
		printf("task1_b\n");
		if (f==0) { skipmt(); f =1;}
		printf("task1_c\n");
		V(0);
	}
}

void c_task2(){
	while(1){
		printf("task2_a\n");
		P(0);
		printf("task2_b\n");
		V(0);
	}
}

void c_task3(){
	while(1){
		printf("task3_a\n");
		P(0);
		printf("task3_b\n");
		V(0);
	}
}

int main() {
	init_kernel();
	set_task(c_task1);
	set_task(c_task2);
	set_task(c_task3);
	begin_sch();
	return 0;
}
