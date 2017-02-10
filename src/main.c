//
//  main.c
//  threadpoolsample
//
//  Created by lizi on 16/12/9.
//  Copyright © 2016年 hfut. All rights reserved.
//

#include <stdio.h>
#include "thread_pool.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "ordered_thread_pool.h"

void task_main(void* tag)
{
    int sleep_time = rand() % 10000;
    usleep(sleep_time);
    pthread_t thread_id = pthread_self();
    printf("Execute task %d thread id %p\n", (int)tag, thread_id);
}

int main(int argc, const char * argv[]) {
    srand(time(NULL));
#if 0
    thread_pool_handler handler2 = thread_pool_create(4);
    
    for(int i = 0; i < 1000; i++)
    {
        submit_task(handler2, task_main, (void*)i);
    }
#if 0
    thread_pool_terminate(handler2);
    sleep(2);
#else
    thread_pool_stop(handler2);
    thread_pool_destory(handler2);
#endif
    printf("--------finish cancel test----------\n");
    
#else
    int thread_num = 5;
    ordered_thread_pool_handler handler3 = ordered_thread_pool_create(thread_num);
    
    for(int i = 0; i < 1000; i++)
    {
         submit_ordered_task(handler3, task_main, i % thread_num, (void*)i);
    }
    
#if 0
    ordered_thread_pool_terminate(handler3);
    sleep(3);
#else
    ordered_thread_pool_stop(handler3);
#endif
    ordered_thread_pool_destory(handler3);
    printf("--------orderd finish cancel test----------\n");
#endif
    return 0;
}
