#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "my_malloc.h"
#define N 100005
bool vis[N]; // 是否已经释放
void *p[N]; // 储存申请堆区指针
int main(){
    
    // 题目测试内容
    // if (mm_init() == -1) {
    //     printf("Memory manager initialization failed.\n");
    //     return -1;
    // }
  
    // void *p1 = mm_malloc(8000);
    // void *p2 = mm_malloc(200);
    // void *p3 = mm_malloc(50);

    // mm_free(p2);

    // void *p4 = mm_malloc(150);

    // mm_free(p1);
    // mm_free(p3);    
    // mm_free(p4);
    


   // 自行测试
   clock_t st_time = clock();
    mm_init();
    int cnt = 0; // 申请的块个数
    for(int i=1;i<=80000;i++){
        int opt = rand() % 3;
        if(opt==0){
            if(cnt){
                mm_free(p[cnt--]);
            }
        }else {
            int siz = rand()%100000+ 1;
            p[++cnt] = mm_malloc(siz) ;
        }
    }

    while(cnt){mm_free(p[cnt--]);}
    clock_t ed_time = clock();
    double elapsed_time = (double)(ed_time - st_time) / CLOCKS_PER_SEC;
    printf("程序运行时间: %.6f 秒\n", elapsed_time);
    
    return 0;
}