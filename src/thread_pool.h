//
//  thread_pool.h
//  threadpoolsample
//
//  Created by lizi on 16/12/9.
//  Copyright © 2016年 hfut. All rights reserved.
//

#ifndef thread_pool_h
#define thread_pool_h

#include "thread_pool_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* thread_pool_handler;

thread_pool_handler thread_pool_create(int thread_num);
void thread_pool_destory(thread_pool_handler handler);
int submit_task(thread_pool_handler handler, task_main_func func, void* tag);
/**
 **  Stop thread pool until task finished
 **/
void thread_pool_stop(thread_pool_handler handler);
  
/**
 **  Terminate thread pool and clear unfinished task
 **/
void thread_pool_terminate(thread_pool_handler handler);
    
#ifdef __cplusplus
}
#endif
    
#endif /* thread_pool_h */
