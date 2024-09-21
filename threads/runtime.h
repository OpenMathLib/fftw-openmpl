#ifndef _OPENVML_RUNTIME_H_
#define _OPENVML_RUNTIME_H_


#define _GNU_SOURCE

#include <dirent.h>   // open dir
#include <time.h>
#include <ctype.h>    // isdigit
#include <stdio.h>    // fopen
#include <string.h>   // strncmp
#include <unistd.h>   // get cpu core
#include <omp.h>      // openmp
#include <sched.h>    // Linux
#include <sys/mman.h> // mmap
#include <numaif.h>   // mbind
#include <numa.h>
#include <errno.h> // bad address
#include <semaphore.h>
#include <pthread.h>

#define CONSTRUCTOR __attribute__((constructor))
#define NUMA_DIR "/sys/devices/system/node/"
#define NUMA_NODE_DIR "/sys/devices/system/node/node%d/"
#define NUMA_NODE_CPULISTS "/sys/devices/system/node/node%d/cpulist"
#define MAX_NUMA_NODES 32
#define MAX_CORE 1024
#define MAX_NODE_CORE 32
#define BUFFER_OFFSET 30
#define BUFFER_SIZE 1 << BUFFER_OFFSET
#define ENTRY_BUFFER 100 
#define MIN_COPY_PAGE_NUM 5
#define MAX_COPY_PAGE_NUM 12
//#define CONCURRENT_CPY_THREAD 128
#define MIN_SIZE_CORE_PROCESS 7000
#define MAX_ASYNC_CPY 4
#define MAX_PIPLINE_CORE 32


typedef struct openvml_arg
{
    int n;
    double *a, *b, *y, *z;
    void *other_params;
} openvml_arg_t;

typedef struct async_cpy_arg{
    void* ori_addr;
    void* dst_addr;
    size_t cpy_size;
}async_cpy_arg_t;

typedef struct openvml_queue
{
    void *routine;
    openvml_arg_t arg;
    unsigned int cpu;
    int range[2]; // [start, end)
    void *sa, *sb, *sy, *sz;
    int mode;
    const char *kernel_name;
    int size; // type size double is 8, float is 4, complex double is 16, complex float is 8
    async_cpy_arg_t cpy_arg[MAX_ASYNC_CPY];
    int pipeline_flag; 
} openvml_queue_t;

typedef struct openvml_numa_info
{
    int num_nodes;                                         // total num of nodes
    int num_core;                                          // total num of cores
    int num_node_core;                                     // the num of cores of a node
    short node_cpu_mapping[MAX_NUMA_NODES][MAX_NODE_CORE]; //
    short cpu_node_mapping[MAX_CORE];                      // give a core id return node id
} openvml_numa_info_t;

typedef struct openvml_buffer
{
    void *addr;
    unsigned int core;
    unsigned int node;
    int is_used;
    unsigned int offset; // unit:byte
} openvml_buffer_t;

/* auto dispatch module */
typedef struct openvml_addr_table
{
    unsigned int table_id;
    unsigned int mem_map_id;
    void *alloc_addr;
    void *ori_addr;
    size_t size;
    struct openvml_addr_table *next_entry;
} openvml_addr_table_t;

typedef struct openvml_memory_block
{
    unsigned int mem_id; // not use
    void *start_addr;
    size_t size;
    char kernel_name[16];
    struct openvml_memory_block *next_block;
} openvml_memory_block_t;

typedef struct openvml_memory_manager
{
    openvml_memory_block_t alloced_mem_list[MAX_CORE];
    openvml_memory_block_t free_mem_list[MAX_CORE];
    openvml_addr_table_t *alloced_addr_table[MAX_CORE];
    size_t buffer_size[MAX_CORE];
} openvml_memory_manager_t;



/* common var */
static long affinity_batch_size = 1;
static openvml_buffer_t buffer[MAX_CORE]; // buffer
static pthread_t async_cpy[MAX_CORE][MAX_ASYNC_CPY];
static int initialized = 0;

void CONSTRUCTOR runtime_init(void);
/* thread server */
int is_thread_init(void);
void openvml_thread_init(void);
int get_cpu_core(void);
int *task_divide_linear(int n, int thread_num);
unsigned int get_node_from_cpu(int cpu);
int get_thread_num(void);
int get_num_core_each_node(void);
void multi_thread_record(int thread_num, struct timeval* start, struct timeval* end, const char* kernel_name, int task_size);

/* memory server */
int is_memory_init(void);
void openvml_memory_init(void);
int get_node_from_addr(void *addr);
void openvml_memory_copy(void *ori_addr, void *buf_free_addr, size_t size);
void *openvml_buffer_alloc(int core, size_t size);
#ifdef BUFFER_NEED_FREE
void OpenVML_FUNCNAME(ad_memory_free(int core, void* ori_addr));
#endif

/* auto dispatch server */
void openvml_auto_dispatch_init(void);
int is_auto_dispatch_init(void);
void *ad_memory_alloc(int core, size_t size, void *ori_addr);
int ad_get_thread_num(int n);
size_t pipline_copy_block(pthread_t *async_t,async_cpy_arg_t *cpy_arg,void *ori_addr, void* dst_addr, size_t block_size, size_t* pos, size_t total_size);
void pipline_wait_copy_block(pthread_t async_cpy);
#endif