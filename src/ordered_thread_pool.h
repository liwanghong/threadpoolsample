//
//  ordered_thread_pool.h
//  threadpoolsample
//
//  Created by lizi on 17/1/25.
//  Copyright © 2017年 hfut. All rights reserved.
//

#ifndef ordered_thread_pool_h
#define ordered_thread_pool_h

#include "thread_pool_common.h"

#ifdef __cpluscplus
extern "C" {
#endif
    
typedef void* ordered_thread_pool_handler;

ordered_thread_pool_handler ordered_thread_pool_create(int thread_num);
void ordered_thread_pool_destory(ordered_thread_pool_handler handler);
int submit_ordered_task(ordered_thread_pool_handler handler, task_main_func func, int job_id, void* tag);
/**
 **  Stop thread pool until task finished
 **/
void ordered_thread_pool_stop(ordered_thread_pool_handler handler);

/**
 **  Terminate thread pool and clear unfinished task
 **/
void ordered_thread_pool_terminate(ordered_thread_pool_handler handler);
    

#ifdef __cplusplus
}
#endif

#endif /* ordered_thread_pool_h */
