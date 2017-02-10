//
//  struct.h
//  threadpoolsample
//
//  Created by lizi on 17/1/25.
//  Copyright © 2017年 hfut. All rights reserved.
//

#ifndef thread_pool_struct_h
#define thread_pool_struct_h

#include "list.h"
#include "thread_pool_common.h"

#define  THREAD_STATE_INIT      (0)
#define  THREAD_STATE_RUNNING   (1)
#define  THREAD_STATE_STOPPING  (2)
#define  THREAD_STATE_CANCELING (4)
#define  THREAD_STATE_STOPPED   (5)

typedef struct task_list_tag
{
    lcb_list_t list;
    task_main_func func;
    void* tag;
}task_list_t;

#endif /* thread_pool_struct_h */
