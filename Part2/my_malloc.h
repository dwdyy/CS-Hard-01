#ifndef my_malloc
#define my_malloc
// 头文件
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

//宏定义
#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))


#define WSIZE 8 
#define DSIZE 16 
#define CHUNKSIZE (1 << 12) 
#define MAX_HEAP (5 * (1ll << 20))
#define MIN_BLK_SIZE (2 * DSIZE) // 最小块大小
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define PACK(size, prev_alloc, alloc) ((size) & ~(1<<1) | (prev_alloc << 1) & ~(1)| (alloc))
#define PACK_PREV_ALLOC(val, prev_alloc) ((val) & ~(1<<1) | (prev_alloc << 1))
#define PACK_ALLOC(val, alloc) ((val) | (alloc))

#define GET(p) (*(unsigned long *)(p))
#define PUT(p, val) (*(unsigned long *)(p) = (val))

#define GET_PRE_ALLOC(p) ((GET(p) & 0x2)>>1)
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define GET_PRED(bp) (GET(bp))
#define SET_PRED(bp, val) (PUT(bp, val))
#define GET_SUCC(bp) (GET(bp + WSIZE))
#define SET_SUCC(bp, val) (PUT(bp + WSIZE, val))

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

// 全局定义
static void* heap_listp;    // 序言块
static void* free_listp;  // 空闲块
static void* mem_start_brk; //内存块开始
static void* mem_brk; // 内存块结尾(已分配)
static void* mem_max_addr; // 可用最大内存

// 调试定义
#define VERBOSE 0  // 默认不输出调试信息
#ifdef DEBUG
#define VERBOSE 1  // 在调试模式下启用调试信息输出
#endif

// 函数定义
void Print();
int mm_init(void);
void mm_free(void *bp);
void *mm_sbrk(int incr);
void *mm_malloc(size_t size);
static void* coalesce(void* bp);
unsigned long mm_mallinfo();
static void add_to_free_list(void *bp);
static void *extend_heap(size_t words);
void *mm_realloc(void *ptr, size_t size);
static void *find_fit_best(size_t asize);
static void *find_fit_first(size_t asize);
static void place(void* bp, size_t asize);
static void delete_from_free_list(void *bp);
void print_heap_blocks(int verbose, const char* func);

#endif