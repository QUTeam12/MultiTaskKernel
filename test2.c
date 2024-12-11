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

int main() {
	init_kernel();
	set_task(task1);
	set_task(task2);
	begin_sch();
	return 0;
}
