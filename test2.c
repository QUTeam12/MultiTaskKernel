#include <stdio.h>
#include "mtk_c.h"

void task1() {
        while(1){
                printf("task1");
        }
}

void task2() {
        while(1){
                printf("task2");
        }
}

int main() {
	printf("はじめ");
        init_kernel();
	printf("init_kernelおけ");
        set_task(task1);
        set_task(task2);
	printf("set_taskおけ");
        begin_sch(); // 実行すると標準出力が効かなくなる
	printf("begin_schおけ");
	return 0;
}
