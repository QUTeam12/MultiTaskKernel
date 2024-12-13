#define TEST2
#include "mtk_c.h"
#include <stdio.h>
int nttask2;
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

void d_task0(){
	while(1){
		printf("task0-1st\n");
		if(nttask2 ==3){
			printf("task0\n");
			nttask2=0;
			for (int k=0;k<3;k++){
				V(1);
			}
		
		}
		printf("skipmt\n");
		skipmt();
	}
}


void d_task1(){
        while(1){
		/*some tasks here*/
                printf("task1_a\n");
                P(0);
               	printf("task1_b\n");
		nttask2++;
                V(0);
               	printf("task1_c\n");
		P(1);
		printf("task1_d\n");
        }
}

void d_task2(){
        while(1){
		/*some tasks here*/
                printf("task2_a\n");
                P(0);
               	printf("task2_b\n");
		nttask2++;
                V(0);
               	printf("task2_c\n");
		P(1);
		printf("task2_d\n");
        }
}

void d_task3(){
        while(1){
		/*some tasks here*/
                printf("task3_a\n");
                P(0);
               	printf("task3_b\n");
		printf("ntask2:%d\n",nttask2);
		nttask2++;
                V(0);
               	printf("task3_c\n");
		printf("ntask2:%d\n",nttask2);
		P(1);
		printf("task3_d\n");
        }
}
int main() {
	nttask2=0;
	init_kernel();
	set_task(d_task0);
	set_task(d_task1);
	set_task(d_task2);
	set_task(d_task3);
	begin_sch();
	return 0;
}
