#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "my_malloc.h"
#define M 100005
#define N 100000
void *a[M]; // 储存指针
unsigned long S[M]; // 储存大小
struct node{
    int opt,size,id; // 类型 大小 id
}t[M];
unsigned long sum; // 内存总和
int main(){
   
    for(int i=1;i<=N;i++){
        scanf("%d",&t[i].opt);
        if(t[i].opt==0) { // 删除操作
            scanf("%d",&t[i].id);
        }
        else {
            scanf("%d%d",&t[i].id,&t[i].size);
        }
    }
    mm_init();
    clock_t begin_time = clock(); // 计算开始时间
 //   printf("%.5lf\n",(double)begin_time);
    double rate =0; //内存分配率  
    for(int i=1;i<=N;i++){
        if(t[i].opt==0) {
            mm_free(a[t[i].id]); // 删除
            sum -= S[t[i].id]; 
        }else {
            a[t[i].id] = mm_malloc(t[i].size); // 申请
            S[t[i].id] = t[i].size; // 储存大小
            sum += S[t[i].id]; 
        }
        if(i == 10000 || i == 20000 || i == 30000 || i == 40000 || i == 50000){ // 计算内存分配率和
            rate += (double)(sum)/(double)(mm_mallinfo());
        }
    }
    clock_t end_time = clock(); 
    double run_time = (double)(end_time - begin_time) / CLOCKS_PER_SEC;// 计算时间
    rate = rate / 5.0;// 计算平均值
    printf("%.5lf %.5lf\n",run_time,rate);

    return 0;
}