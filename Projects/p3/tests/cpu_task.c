// tests/cpu_task.c
#include <stdio.h>
#include <unistd.h>

int main(){
    printf("cpu_task pid=%d\n", getpid());
    while(1){
        // busy loop
        asm volatile("" ::: "memory");
    }
    return 0;
}