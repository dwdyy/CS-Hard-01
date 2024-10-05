#include "my_malloc.h"
static int tot = 0 ;
/**
 * @brief 获得内存分配器所使用的内存
 * 
 * @return 分配器所使用的内存，单位为字节
 * 
 */
unsigned long mm_mallinfo(){
    //print_heap_blocks(1,"0");
    return (unsigned long) mem_brk - (unsigned long)mem_start_brk;
}
//同part1的调试模式
 void print_heap_blocks(int verbose, const char* func) {
    if (verbose) {
        char *curbp = (char *)heap_listp;
        fprintf(stderr,"\n=========================== %s ===========================\n", func);

        while (GET_SIZE(HDRP(curbp)) > 0) {
            fprintf(stderr,"Address: %p\n", curbp);
            fprintf(stderr,"Size: %lu\n", GET_SIZE(HDRP(curbp)));
            fprintf(stderr,"Size: %lu\n", GET_SIZE(FTRP(curbp)));
            fprintf(stderr,"Allocated: %lu %lu\n",GET_PRE_ALLOC(HDRP(curbp)),GET_ALLOC(HDRP(curbp)));
            fprintf(stderr,"\n");
            curbp = NEXT_BLKP(curbp);
        }

        fprintf(stderr,"Address: %p\n", curbp); // Should point to the end of the heap
        fprintf(stderr,"Size: 0\n");
        fprintf(stderr,"Allocated: 1\n");
        
        fprintf(stderr,"空白块\n");
         curbp = free_listp;// pos 用来遍历所有空白块
         while(curbp !=NULL){
             fprintf(stderr,"Address: %p\n", curbp);
            fprintf(stderr,"Size: %lu\n", GET_SIZE(HDRP(curbp)));
            fprintf(stderr,"Allocated: %lu %lu\n",GET_PRE_ALLOC(HDRP(curbp)),GET_ALLOC(HDRP(curbp)));
            fprintf(stderr,"\n");
            curbp = GET_SUCC(curbp);
        }


        fprintf(stderr,"=========================== %s ===========================\n", func);



    }
}

/**
 * @brief 初始化内存分配器。
 * 
 * 实现上文堆的衍生方式中指定的初始化，初始化全局变量，分好堆1堆2
 */
int mm_init(void){
    mem_start_brk = sbrk(4 * WSIZE); // 先申请4个字
    mem_brk = mem_max_addr = mem_start_brk + 4 * WSIZE;//内存块结尾
    free_listp = NULL;//空闲块链头
    /*初始化序言块*/
    heap_listp = mem_start_brk + 2 * WSIZE;
    PUT(HDRP(heap_listp),PACK(DSIZE,1,1)); // 序言快头部
    PUT(FTRP(heap_listp),GET(HDRP(heap_listp)));// 序言快脚部
    PUT(HDRP(NEXT_BLKP(heap_listp)),PACK(0,1,1));// 结尾块维护


    //print_heap_blocks(1,"init_in");

    if(mm_sbrk(CHUNKSIZE) == (void *)-1){ // 分配一个4kb给堆2,堆1增长5MB
        return -1;
    }
    return 0;
}


/**
 * @brief 增加程序的数据空间。
 * 
 * @param incr 要增加的字节数。
 * 
 * @return 扩展后的内存起始地址；如果扩展失败，则返回 (void *)-1。
 * 
 * 实现上文堆的衍生方式中的功能
 */
void *mm_sbrk(int incr){
    //print_heap_blocks(1,"sbrk_in");
    /*堆1不够用*/
    if(mem_brk + incr > mem_max_addr){
        size_t extend_size = (incr-1) / MAX_HEAP * MAX_HEAP + MAX_HEAP; 
      //  fprintf(stderr,"%lu\n",(unsigned long)extend_size);
        // extend_size 设为 5mb 的倍数
        mem_max_addr = sbrk(extend_size) + extend_size;
        if(mem_max_addr == (void*)-1){
            fprintf(stderr,"mm_sbrk : fail\n");
            return (void*) (-1);
        }
    }
    /*扩展堆2*/
    void *cur = mem_brk; 
    // cur为新增块开始,因为把结尾块当作头部，所以不用加减
    mem_brk += incr;

    /*为新分的块维护头部*/
    PUT(HDRP(cur),PACK(incr,GET_PRE_ALLOC(HDRP(cur)),0));//前一个块的状态储存在结尾块中

    /*脚部直接赋值成为头部*/
    PUT(FTRP(cur),GET(HDRP(cur)));

    /*将当前块插入空闲块链表中*/
    add_to_free_list(cur);

    /*维护结尾块*/
    PUT(HDRP(mem_brk),PACK(0,0,1));

    //print_heap_blocks(1,"sbrk_in2");

    /*最后合并空闲块*/
    return coalesce(cur);

}


/**
 * @brief 扩展堆空间。
 * 
 * @param words 要扩展的字数。
 * 
 * @return 返回新扩展区域的起始地址；如果扩展失败，则返回 NULL。
 * 
 * extend_heap函数是对mem_sbrk的一层封装，接收的参数是要分配的字数
 * 在堆初始化以及malloc找不到合适内存块时使用
 * 它首先对请求大小进行地址对齐，然后调用mem_sbrk获取空间
 */
static void *extend_heap(size_t words){
    // words = words + WSIZE 在malloc中已加上这里不在需要加上块头
    words = ALIGN(words); // 对齐
    void *cur = mm_sbrk(words);
    if(cur == (void*) (-1)) return NULL; // 扩展堆区失败
    return cur;
}

/**
 * @brief 查找适合分配的空闲块（首次适应）。
 * 
 * @param asize 请求的块大小（以字节为单位）。
 * 
 * @return 返回找到的适合的空闲块的指针；如果没有找到合适的空闲块，则返回 NULL。
 * 
 * 该函数在空闲链表中查找第一个合适的块，以满足请求的大小。
 */
static void *find_fit_first(size_t asize){
    void *pos = free_listp;// pos 用来遍历所有空白块
    while(pos !=NULL){
        // 找到第一个
        if(GET_SIZE(HDRP(pos)) >= asize) return pos;
        pos = GET_SUCC(pos);
    }
    return NULL; // 未找到
}

//类似find_fit_first
static void *find_fit_best(size_t asize){
    //print_heap_blocks(1,"find_fit_best_IN");
    // pos 用来遍历所有空白块
    // cur 记录 最佳位置
    void *cur = NULL,*pos= free_listp;
    while(pos !=NULL){
        // 找到第一个
        if(GET_SIZE(HDRP(pos)) >= asize){
            if(cur == NULL) cur = pos;
            else {
                // 找个大小最小的
                if(GET_SIZE(HDRP(cur)) > GET_SIZE(HDRP(pos))) cur = pos;
            }
        }
        pos = GET_SUCC(pos);
    }
    // 如果未找到cur=NULL 相当于返回NULL
    return cur;
}


    
/**
 * @brief 将指定的块放置到分配状态。
 * 
 * @param bp 要分配的块的起始地址。
 * @param asize 请求的块大小（以字节为单位）。
 * 
 * 该函数将指定的块标记为已分配，并更新块的大小信息。
 */
static void place(void* bp, size_t asize){
   
    // 从空闲块中删除
    //print_heap_blocks(1,"place");
   // printf("%p\n",bp);
    delete_from_free_list(bp);

    // 首先切割块,这里空白快大小需要大于 2 * DSIZE 也就是最小快大小
        if(GET_SIZE(HDRP(bp)) - asize < MIN_BLK_SIZE){// 多的块不够最小块大小，我们直接分配到填充块，不再切割
            //这里在后面的块标记前一个块已经被已使用
            asize = GET_SIZE(HDRP(bp));
            PUT(HDRP(NEXT_BLKP(bp)),PACK(GET_SIZE(HDRP(NEXT_BLKP(bp))),1,GET_ALLOC(HDRP(NEXT_BLKP(bp)))));
            if(GET_ALLOC(HDRP(NEXT_BLKP(bp))) == 0 )
            PUT(FTRP(NEXT_BLKP(bp)),PACK(GET_SIZE(HDRP(NEXT_BLKP(bp))),1,GET_ALLOC(HDRP(NEXT_BLKP(bp)))));
        }
        else { // 切割
            // 计算空白快大小
            size_t bsize = GET_SIZE(HDRP(bp)) - asize;
             
            // 切割出分配块
            PUT(HDRP(bp),PACK(asize,GET_PRE_ALLOC(HDRP(bp)),1));
            PUT(FTRP(bp),PACK(asize,GET_PRE_ALLOC(HDRP(bp)),1));
            // 切割出空白快 + 插入链表 + 尝试合并 
            PUT(HDRP(NEXT_BLKP(bp)),PACK(bsize,1,0));
            PUT(FTRP(NEXT_BLKP(bp)),PACK(bsize,1,0));
            add_to_free_list(NEXT_BLKP(bp));
            coalesce(NEXT_BLKP(bp));
        }
    // 切割完成,标记使用
    PUT(HDRP(bp),PACK(GET_SIZE(HDRP(bp)),GET_PRE_ALLOC(HDRP(bp)),1));
    PUT(FTRP(bp),PACK(GET_SIZE(HDRP(bp)),GET_PRE_ALLOC(HDRP(bp)),1));
    return ;
}
    
/**
 * @brief 合并相邻的空闲块。
 * 
 * @param bp 要合并的块的起始地址。
 * 
 * @return 合并后的块的起始地址。
 * 
 * 该函数合并当前块与其相邻的空闲块，以减少内存碎片并增加大块的可用内存。
 */
static void* coalesce(void* bp){ 
    /* 这里具体思路同part1 
    分为 4种情况 0/1为块未分配/分配
    分为四步 链表删除 + 重算size  + 维护头部脚部 + 链表增添
    */   
   //print_heap_blocks(1,"coalesce");
   //printf("%p\n",bp);
   //print_heap_blocks(1,"coalesce");
    bool tag1 = GET_PRE_ALLOC(HDRP((bp)));// 前一个块

   // fprintf(stderr,"%p\n",NEXT_BLKP(bp));

    bool tag2 = GET_ALLOC(HDRP(NEXT_BLKP(bp)));// 后一个块
    //  0 bp 0 合并三个快
    if(!tag1 && !tag2){
        // 链表删除
//            print_heap_blocks(1,"coalesce");
//    printf("%p\n",bp);
        delete_from_free_list(PREV_BLKP(bp));
        delete_from_free_list(bp);
        delete_from_free_list(NEXT_BLKP(bp));
        // 重算size
        size_t  asize = GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(NEXT_BLKP(bp))) ;
        // 维护头部脚部
        PUT(HDRP(PREV_BLKP(bp)),PACK(asize,GET_PRE_ALLOC(HDRP(PREV_BLKP(bp))),0));
        PUT(FTRP(PREV_BLKP(bp)),PACK(asize,GET_PRE_ALLOC(HDRP(PREV_BLKP(bp))),0));
        // 链表增添
        add_to_free_list(PREV_BLKP(bp));
    }else {
        if(!tag1 && tag2){// 0 bp 1 合并前两个
        //print_heap_blocks(1,"coalesce");
            // 链表删除
            delete_from_free_list(PREV_BLKP(bp));
            delete_from_free_list(bp);
            //print_heap_blocks(1,"coalesce");
            // 重算size
            size_t  asize = GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(bp))  ;
            //print_heap_blocks(1,"coalesce");
            // 维护头部脚部((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
            //fprintf(stderr,"%p",PREV_BLKP(bp));
            PUT(HDRP(PREV_BLKP(bp)),PACK(asize,GET_PRE_ALLOC(HDRP(PREV_BLKP(bp))),0));
            //print_heap_blocks(1,"coalesce");
            PUT(FTRP(PREV_BLKP(bp)),PACK(asize,GET_PRE_ALLOC(HDRP(PREV_BLKP(bp))),0));
            //print_heap_blocks(1,"coalesce");
            // 链表增添
            add_to_free_list(PREV_BLKP(bp));
            //print_heap_blocks(1,"coalesce");
        }
        else {
            if(tag1 && !tag2) {// 1 bp 0 合并后两个
                // 链表删除
                delete_from_free_list(bp);
                delete_from_free_list(NEXT_BLKP(bp));
                // 重算size
                size_t  asize =  GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(NEXT_BLKP(bp))) ;
                // 维护头部脚部
                PUT(HDRP(bp),PACK(asize,GET_PRE_ALLOC(HDRP((bp))),0));
                PUT(FTRP(bp),PACK(asize,GET_PRE_ALLOC(HDRP((bp))),0));
                // 链表增添
                add_to_free_list(bp);
            }
            else {// 1 bp 1无需操作
                
            }
        }
    }
    return bp;

}
    
/**
 * @brief 从空闲链表中移除指定的空闲块。
 * 
 * @param bp 要移除的空闲块的起始地址。
 * 
 * 该函数将指定的空闲块从空闲链表中移除，以便将其标记为已分配。
 */
static void delete_from_free_list(void *bp){
    // 分四种情况 0/1表示前一个空闲内存块不存在/存在
    void *tag1 = GET_PRED(bp); // 前一个内存块
    void *tag2 = GET_SUCC(bp); // 后一个内存块

    // 1 bp 1
    if(tag1!=NULL && tag2!=NULL){
        //print_heap_blocks(1,"qwq");

        SET_PRED(tag2,tag1);
        SET_SUCC(tag1,tag2);
        SET_PRED(bp,0);
        SET_SUCC(bp,0);
    }

    // 0 bp 0
    else if(tag1==NULL && tag2 ==NULL){
        SET_PRED(bp,0);
        SET_SUCC(bp,0);
        free_listp = NULL ;
    }
    // 1 bp 0 
    else if(tag1!=NULL && tag2==NULL){
        SET_SUCC(tag1,0);
        SET_PRED(bp,0);
        SET_SUCC(bp,0);
    }
    // 0 bp 1
    else {
        SET_PRED(tag2,0);
        free_listp = tag2;
        SET_PRED(bp,0);
        SET_SUCC(bp,0);
    }
    //print_heap_blocks(1,"delete_from_free_list_OUT");
    return ;
}

/**
 * @brief 将指定的空闲块插入到空闲链表中。
 * 
 * @param bp 要插入的空闲块的起始地址。
 * 
 * 该函数用头插法将指定的空闲块插入到空闲链表中，以便将其标记为可用的空闲块。
 */
static void add_to_free_list(void *bp){
    // 没有空闲块
    if(free_listp == NULL) {
        free_listp = bp;
        /*前驱后继都设置为0*/
        SET_PRED(bp,0);
        SET_SUCC(bp,0);
        return ;
    }
    /*将原来内存块前驱改为 bp*/
    SET_PRED(free_listp,bp);
    /*将 bp 后驱去 改为原链表头部*/
    SET_SUCC(bp,free_listp);
    /*将bp前驱设置为0*/
    SET_PRED(bp,0);
    free_listp = bp;
    return ;
}

/**
 * @brief 分配内存块。
 * 
 * @param size 请求的内存块大小（以字节为单位）。该大小包括块头和块尾所需的空间。
 * 
 * @return 返回分配的内存块的起始地址；如果分配失败，则返回 NULL。
 * 
 * 该函数根据请求的大小在堆中查找适合的空闲块，如果找到合适的块则分配该块，并返回其起始地址。如果没有足够的空间，则可能会扩展堆或返回 NULL。
 */
void *mm_malloc(size_t size){
    // 分配快中没有脚部,只用加上WSIZE
    // 因为空白块minsize > 分配快minsize 为了能够正常释放，需要取和MIN_BLK_SIZE 取max
    size = size + WSIZE;
    size = ALIGN(size);
    size = MAX(size,MIN_BLK_SIZE);

    // 寻找空白快
    void * cur = find_fit_best(size);

    

    if(cur == NULL) { // 开始分配重新寻找
        if(extend_heap(size) == NULL) return NULL;

        //print_heap_blocks(1,"mm_malloc_in");

        cur = find_fit_best(size); 
    }
    // 放置块
    place(cur,size);
    return cur;
}
    
/**
 * @brief 释放内存块。
 * 
 * @param bp 要释放的内存块的起始地址。
 * 
 * 该函数将指定的内存块标记为已释放，并尝试将其合并到相邻的空闲块中，以减少内存碎片。
 */
void mm_free(void *bp){
    /*
        分为3步
        加入空白块链表中 + 修改脚部头部 + 尝试合并
    */
    add_to_free_list(bp);// 加入空白块链表中
    /*修改这个块的头部脚部*/
    PUT(HDRP(bp),PACK(GET_SIZE(HDRP(bp)),GET_PRE_ALLOC(HDRP(bp)),0));// 修改脚部头部
    //print_heap_blocks(1,"0");
    PUT(FTRP(bp),PACK(GET_SIZE(HDRP(bp)),GET_PRE_ALLOC(HDRP(bp)),0));//修改脚部头部
    /*修改下一个块的头部脚部(只有空闲块才能修改脚部)*/
    PUT(HDRP(NEXT_BLKP(bp)),PACK(GET_SIZE(HDRP(NEXT_BLKP(bp))),0,GET_ALLOC(HDRP(NEXT_BLKP(bp)))));
    if(GET_ALLOC(HDRP(NEXT_BLKP(bp))) == 0 )
    PUT(FTRP(NEXT_BLKP(bp)),PACK(GET_SIZE(HDRP(NEXT_BLKP(bp))),0,GET_ALLOC(HDRP(NEXT_BLKP(bp)))));
    coalesce(bp);//尝试合并
}

/**
 * @brief 重新分配内存块。
 * 
 * @param ptr 指向要重新分配的内存块的起始地址。如果 ptr 为 NULL，则相当于调用 mm_malloc。
 * @param size 请求的新内存块大小（以字节为单位）。该大小包括块头和块尾所需的空间。
 * 
 * @return 返回重新分配的内存块的起始地址；如果重新分配失败，则返回 NULL。
 * 
 * 该函数调整指定内存块的大小。如果新的大小大于原始大小，会在堆中分配新的内存块，并将原始数据复制到新内存块中。如果新的大小小于原始大小，则会截断原始数据。原始内存块的内容在重新分配之前被保留。
 */
void *mm_realloc(void *ptr, size_t size){

    /*
        先用malloc分配出新块
        在copy数据
    */

    if(ptr == NULL) return mm_malloc(size);
    void *cur = mm_malloc(size);
    // 复制的大小 1.原来的size,新增的size 取最小值
    size_t bsize = MIN(size,GET_SIZE(HDRP(ptr)) - WSIZE);
    memcpy(cur,ptr,bsize);
    return cur;
}
void Print(){
    fprintf(stderr,"%d\n",tot);
}