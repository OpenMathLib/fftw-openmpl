#include "threads/runtime.h"

void CONSTRUCTOR runtime_init()
{
    openvml_thread_init();
    if (!is_thread_init()){
        printf("Runtime error: thread init failed.\n");
        initialized = -1;
    }
    openvml_memory_init();
    if (!is_memory_init()){
        printf("Runtime error: memory init failed.\n");
        initialized = -2;
    }
    openvml_auto_dispatch_init();
    if (!is_auto_dispatch_init()){
        printf("Runtime error: auto dispatch init failed.\n");
        initialized = -3;
    }
    if(initialized == 0) initialized = 1;
}
int is_runtime_init(void){
    return initialized;
}