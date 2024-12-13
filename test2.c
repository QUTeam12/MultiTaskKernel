#define TEST2
#include "mtk_c.h"
#include <stdio.h>


void task1() {
  int i = 0;
  while(1){
	  if(i == 10000){
		  printf("task1\n");
		  i = 0;
	  }else{
		  i++;
	  }
  }
}

void task2() {
	int i = 0;
  while(1){
	  if(i == 10000){
		  printf("task2\n");
		  i = 0;
	  }else{
		  i++;
	  }
	}
}

void c_task1(){
	int f = 0;
	while(1){
		printf("task1_a\n");
		P(0);
		printf("task1_b\n");
		if(f == 0){
			skipmt();
			f=1;	
		}
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

volatile int nttask;
void s_task1(){
	while(1){
		/*some tasks here */
		printf("task1.1\n");
		P(0); nttask++; printf("task1.3\n"); V(0);
		printf("task1.4\n");
		P(1);
		printf("task1.5\n");
	}
} 
void s_task2(){
	while(1){
		/*some tasks here */
		printf("task2.1\n");
		P(0); printf("task2.2\n"); nttask++; printf("task2.3\n"); V(0);
		printf("task2.4\n");
		P(1);
		printf("task2.5\n");
	}
} 
void s_task3(){
	while(1){
		/*some tasks here */
		printf("task3.1\n");
		P(0); printf("task3.2\n"); nttask++; printf("task3.3\n"); V(0);
		printf("task3.4\n");
		P(1);
		printf("task3.5\n");
	}
} 

void s_task0(){
	while(1){
	if(nttask == 3){
		printf("task0.1\n");
		nttask = 0;
		printf("task0.3\n");
		for (int k = 0; k < 3; k++){
			V(1);
		}
	}
	}
}

volatile int nttask2;
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
	nttask = 0;
	init_kernel();
	set_task(s_task0);
	set_task(s_task1);
	set_task(s_task2);
	set_task(s_task3);
	begin_sch();
	return 0;
}
