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
        init_kernel();
        set_task(task1);
        set_task(task2);
        begin_sch();
}
