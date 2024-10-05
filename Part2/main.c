#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "my_malloc.h"
#define N 100005
bool vis[N]; // 是否已经释放
void *p[N]; // 储存申请堆区指针
int main() {
    if (mm_init() == -1) {
        printf("Memory manager initialization failed.\n");
        return -1;
    }

    void *p1 = mm_malloc(8000);
    void *p2 = mm_malloc(200);
    void *p3 = mm_malloc(50);

    mm_free(p2);

    void *p4 = mm_malloc(150);

    mm_free(p1);
    mm_free(p3);    
    mm_free(p4);
   
    return 0;
}