#include "my_malloc.h"
/* 堆管理接口 */
static void* heap_listp;  // 堆的开始指针

/**
 * @brief 初始化内存分配器
 * 
 * @return 成功时返回 0，失败时返回 -1
 */
int mm_init(void)
{
    // 初始化堆
    if ((heap_listp = sbrk(4*WSIZE)) == (void*)-1)
        return -1;
    //11
    // 初始化空闲链表
    PUT(heap_listp, 0); 
    PUT(heap_listp + 1*WSIZE, PACK(8, 1));
    PUT(heap_listp + 2*WSIZE, PACK(8, 1));
    PUT(heap_listp, PACK(0, 1));
    heap_listp += (2*WSIZE);

    // 扩展堆
    if (extend_heap(CHUNKSIZE) == NULL)
        return -1;

    return 0;
}

/**
 * @brief 打印当前堆块的状态信息。
 *
 * 根据传入的 verbose 参数决定是否打印堆块信息。此函数用于调试，
 * 以检查堆内存布局、块大小及其分配状态。
 *
 * @param verbose  控制是否打印输出。当 verbose 为 1 时打印信息，为 0 时不打印。
 * @param func     表示调用此函数的上下文或函数名称，用于输出中标识当前的操作阶段。
 */
static void print_heap_blocks(int verbose, const char* func) {
    if (!verbose) return;  // 如果不需要输出调试信息，则直接返回

    char *curbp = (char *)heap_listp;

    fprintf(stderr,"\n=========================== %s ===========================\n", func);

    while (GET_SIZE(HDRP(curbp)) > 0) {
        fprintf(stderr,"Address: %p\n", curbp);
        fprintf(stderr,"Size: %d\n", GET_SIZE(HDRP(curbp)));
        fprintf(stderr,"Allocated: %d\n", GET_ALLOC(HDRP(curbp)));
        fprintf(stderr,"\n");
        curbp = NEXT_BLKP(curbp);
    }

    fprintf(stderr,"Address: %p\n", curbp); // Should point to the end of the heap
    fprintf(stderr,"Size: 0\n");
    fprintf(stderr,"Allocated: 1\n");
    fprintf(stderr,"=========================== %s ===========================\n", func);
}


/**
 * @brief 扩展堆的大小以适应请求的内存。
 * 
 * 这个函数通过调用 sbrk 来扩展堆的大小。扩展的大小会被调整为CHUNKSIZE的整数倍，以便更好地管理堆内存。
 * 
 * @param size 需要扩展的内存大小（以字节为单位）。
 * 
 * @note 初始调用mm_init时扩展一个CHUNKSIZE
 * @return 扩展后的内存块的指针，如果扩展失败则返回 NULL。
 */
static void *extend_heap(size_t size) {
    size = (size-1)/ (CHUNKSIZE) * (CHUNKSIZE)+ (CHUNKSIZE); // 将扩展的大小调整为为CHUNKSIZE的整数倍
    void *curp;
    if((curp = sbrk(size)) == (void*)(-1)){ // 申请堆失败
        fprintf(stderr,"sbrk failed:"); 
        return NULL;
    }
    PUT(HDRP(curp),PACK(size,0));// 把最后的结尾块信息覆盖为为新的空白快头部
    PUT(FTRP(curp),PACK(size,0));// 维护脚部
    PUT(HDRP(NEXT_BLKP(curp)),PACK(0,1)); // 在最后边放置结尾快
    coalesce(curp); // 将新的空白快合并一下
    return curp;
};


/**
 * @brief 在堆中查找合适的空闲块。
 * 
 * 这个函数遍历堆中的所有块，寻找一个大小足够且未分配的空闲块。
 * 
 * @param size 需要分配的内存块的大小（以字节为单位）。
 * 
 * @return 找到的合适的空闲块的指针，如果没有找到则返回 `NULL`。
 */
static void *find_fit(size_t size) {
    // size应已经满足双字对齐
    #ifdef DEBUG 
        if(size != ALIGN(size)) { // 检查size是否对齐到双位了
            fprintf(stderr,"extend_heap : param false");
        }
    #endif
    void *cur = heap_listp; 
    while((GET_SIZE(HDRP(cur))) !=0){ // 从cur开始一块一块扫
        // 去掉分配块
        if(GET_ALLOC(HDRP(cur)) == 0  \ 
           && GET_SIZE(HDRP(cur)) >= size){ //去掉size不够的块
                return cur;
            }
        cur = NEXT_BLKP(cur); // 跳到下一个块
    }
    return NULL;   // 未找到

}


/**
 * @brief 将内存块分配给用户。
 * 
 * 这个函数会根据请求的大小 asize(对齐后的size) 将一个内存块分配给用户。如果分配的块剩余的空间足够大，则将剩余的空间保留为一个新的空闲块。否则，将整个块都分配出去。
 * 
 * @param bp 指向要分配的内存块的指针。
 * @param asize 需要分配的内存块的大小（以字节为单位）。
 */
static void place(char *bp, size_t asize) {
    if(GET_SIZE(HDRP(bp)) == asize) {
        /*把头部和脚部标记上已经使用*/
        PUT(HDRP(bp),PACK(asize,1));
        PUT(FTRP(bp),PACK(asize,1)); 
        return ;
    } 
    size_t bsize = GET_SIZE(HDRP(bp)) - asize ; // 记录空白块size
    /*分配块更新头部和脚部*/
    PUT(HDRP(bp),PACK(asize,1));
    PUT(FTRP(bp),PACK(asize,1));
    /*空白快更新头部和脚部*/
    PUT(HDRP(NEXT_BLKP(bp)),PACK(bsize,0));
    PUT(FTRP(NEXT_BLKP(bp)),PACK(bsize,0));
    /*
        coalesce(NEXT_BLKP(bp)); 
        这里不需要合并空白快，因为开始的空白快的左右两侧一定是分配块，
        分配后不可能合并
    */
    return ;
}


/**
 * @brief 分配一个指定大小的内存块。
 * 
 * 这个函数会根据请求的内存大小size来分配内存。如果请求的大小为零，则返回 NULL。首先，函数尝试从空闲块列表中找到一个合适的块，如果找到，则进行分配。如果没有找到合适的块，则会扩展堆的大小，并再次尝试分配内存。
 * 
 * @param size 需要分配的内存大小（以字节为单位）。
 * 
 * @return 指向分配的内存块的指针，如果分配失败则返回 `NULL`。
 */
void *mm_malloc(size_t size) {
    //print_heap_blocks(1,"mm_malloc : in");
    size = ALIGN(size + DSIZE); // 将size加上头部和尾部以后对齐
    void *cur = find_fit(size); // 寻找合适块
    /*扩展堆区，重新寻找*/
    if(cur == NULL) {
        if(extend_heap(size)==NULL){
            fprintf(stderr,"malloc error");
            return NULL;
        }
        cur = find_fit(size);
    }
    /*扩展堆区后仍未找到*/
    if(cur == NULL){
        fprintf(stderr,"malloc error");
        return NULL;
    }
    place((char * )cur,size);
    //print_heap_blocks(1,"mm_malloc : out");
    return cur;
    
}


/**
 * @brief 合并相邻的空闲块。
 * 
 * 这个函数检查指定块 bp 的前后相邻块的分配状态。如果相邻的块为空闲块，则将这些块合并成一个更大的空闲块，并更新其头部和尾部。函数会返回合并后的空闲块的指针。
 * 
 * @param bp 指向要进行合并操作的内存块的指针。
 * 
 * @return 合并后的空闲块的指针。
 */
static void *coalesce(void *bp) {

    //print_heap_blocks(1,"coalesce in");
    if(GET_ALLOC(bp) == 1){
        fprintf(stderr,"coalesce error");
        return NULL;
    }

    /*这里用0表示未分配1表示已分配，分四种情况讨论*/
    bool tag1 = GET_ALLOC(HDRP(PREV_BLKP(bp))); // 前一个块的分配情况
    bool tag2 = GET_ALLOC(HDRP(NEXT_BLKP(bp))); // 后一个块的分配情况
    size_t size;
    // 0 bp 0 
    if(!tag1 && !tag2){
        // size 为三者size 之和
        size = GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(NEXT_BLKP(bp))); 
        /*切换状态为 bp 0 0 */
        bp = PREV_BLKP(bp); 
        PUT(HDRP(bp),size);
        PUT(FTRP(bp),size);
    }
    // 0 bp 1

    if(!tag1 && tag2){
        // size 为前两者size 之和
        size = GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(bp)); 
        /*切换状态为 bp 0 1 */
        bp = PREV_BLKP(bp); 
        PUT(HDRP(bp),size);
        PUT(FTRP(bp),size);
    }

    // 1 bp 0

    if(tag1 && !tag2){
        // size 为后两者size 之和
        size = GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(NEXT_BLKP(bp))); 
        PUT(HDRP(bp),size);
        PUT(FTRP(bp),size);
    }
    // 1 bp 1
    // 无需合并
    //print_heap_blocks(1,"coalesce out");
    return bp;
}


/**
 * @brief 释放一个已分配的内存块。
 * 
 * 这个函数将指定的内存块 ptr 标记为未分配，并尝试将其与相邻的空闲块合并。
 * 
 * @param ptr 指向要释放的内存块的指针。
 */
void mm_free(void *ptr) {
    //print_heap_blocks(1,"mm_free : in");
    /*更改状态 1 -> 0 */
    PUT(HDRP(ptr),PACK(GET_SIZE(HDRP(ptr)),0));
    PUT(FTRP(ptr),PACK(GET_SIZE(HDRP(ptr)),0));
    /*合并*/
    coalesce(ptr);
    //print_heap_blocks(1,"mm_free : out");
}