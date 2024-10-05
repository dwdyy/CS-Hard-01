#include <stdlib.h>
#include <stdio.h>
#include <time.h>
const int N = 100000;

void swap(int *a,int *b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void random_array(int n,int *a){
    for(int i=n;i>=2;i--){
        int x = rand() % (i) + 1;
        swap(&a[i],&a[x]);
    }
} 
int cnt , n ;
int a[1000005];
int main(){
    srand(time(0));

    for(int i=1;i<=1000;i++){
        int size = rand()%  1000000;
        a[++n] = ++cnt;
        printf("1 %d %d\n",cnt,size);
    }

    for(int i=1;i<=N-1000;i++){
        int rd = rand()%3;
        if(rd==0 && n){ // 删除
            if(rand()%100==0)random_array(n,a);
            printf("0 %d\n",a[n--]);
        }else {
            int size =  rand() % 1000000;
            a[++n] = ++ cnt;
            printf("1 %d %d\n",cnt,size);
        }
        
    }

    return 0;
}