#include <stdio.h>
#include "mtk_c.h"

void task1() {
        while(1){
                printf("task1\r\n");
        }
}

void task2() {
        while(1){
                printf("task2\r\n");
        }
}

int main() {
	printf("はじめ\r\n");
        init_kernel();
	printf("init_kernelおけ\r\n");
        set_task(task1);
        // set_task(task2);
	printf("set_taskおけ\r\n");
        begin_sch();
	printf("begin_schおけ\r\n");
	return 0;
}
