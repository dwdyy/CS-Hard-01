#ifndef my_malloc
#define my_malloc


/*引用区*/
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h> 
#include <errno.h>


/*定义函数*/
int mm_init(void);
void mm_free(void *ptr);
void *mm_malloc(size_t size);
static void *coalesce(void *bp);
static void *extend_heap(size_t size);
static void place(char *bp, size_t asize) ;
static void print_heap_blocks(int verbose, const char* func);

/*定义*/
#define ALIGNMENT 8  // 双字对齐
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1)) // 对齐到ALIGNMENT的倍数


#define WSIZE       4  // 头部或尾部的字节数
#define DSIZE       8  // 双字大小
#define CHUNKSIZE  (1<<12)  // 扩展堆时的默认大小
#define MINBLOCK (DSIZE + 2*WSIZE)  // 最小块大小

#define MAX(x, y)  ((x) > (y) ? (x) : (y))  // 返回最大值
#define PACK(val, alloc)  ((val) | (alloc))
#define PACK_ALLOC(val, alloc)  ((val) | (alloc))  // 将 size 和 allocated bit 合并为一个字

#define GET(p)        (*(unsigned int *)(p))  // 读地址p处的一个字
#define PUT(p, val)   (*(unsigned int *)(p) = (val))  // 向地址p处写一个字

#define GET_SIZE(p)   (GET(p) & ~0x07)  // 获取块的大小
#define GET_ALLOC(p)  (GET(p) & 0x1)  // 获取分配标志

#define HDRP(bp)     ((char*)(bp) - WSIZE)  // 获取头部指针(bp是一个块的有效荷载的起始地址)
#define FTRP(bp)     ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)  // 获取尾部指针

#define NEXT_BLKP(bp)    ((char*)(bp) + GET_SIZE((char*)(bp) - WSIZE))  // 获取下一个块的指针
#define PREV_BLKP(bp)    ((char*)(bp) - GET_SIZE((char*)(bp) - DSIZE))  // 获取前一个块的指针


#define VERBOSE 0  // 默认不输出调试信息
#ifdef DEBUG
#define VERBOSE 1  // 在调试模式下启用调试信息输出
#endif

#endif
