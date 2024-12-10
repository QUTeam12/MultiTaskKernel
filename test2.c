#define TEST2
#include "mtk_c.h"
#include <stdio.h>

void task1() {
        while(1){
		printf("1\n");
        }
}

void task2() {
        while(1){
		printf("2\n");
	}
}

int main() {
	init_kernel();
	set_task(task1);
	set_task(task2);
	begin_sch();
	return 0;
}
