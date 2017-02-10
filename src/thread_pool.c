//
//  thread_poo.c
//  threadpoolsample
//
//  Created by lizi on 16/12/9.
//  Copyright © 2016年 hfut. All rights reserved.
//

#include "thread_pool.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "list.h"
#include "thread_pool_struct.h"

typedef struct thread_pool_tag
{
    int thread_num;
    int state;
    task_list_t task_list;
    pthread_mutex_t task_list_mutex;
    pthread_t threads[1];
}thread_pool_t;

static void* thread_main(void *arg);
static task_list_t* fetch_task_list(thread_pool_t* thread_pool, int* state);

thread_pool_handler thread_pool_create(int thread_num)
{
    size_t size = sizeof(thread_pool_t) + sizeof(pthread_t) *(thread_num - 1);
    thread_pool_t* thread_pool = (thread_pool_t*)malloc(size);
    memset(thread_pool, 0, size);
    
    thread_pool->thread_num = thread_num;
    lcb_list_init(&thread_pool->task_list.list);
    pthread_mutex_init(&thread_pool->task_list_mutex, NULL);
    thread_pool->state = THREAD_STATE_RUNNING;
    for (int i = 0; i < thread_num; i++)
    {
        pthread_create(&(thread_pool->threads[i]), NULL, thread_main, thread_pool);
    }
    
    return thread_pool;
}

void thread_pool_stop(thread_pool_handler handler)
{
    thread_pool_t* thread_pool = (thread_pool_t*)handler;
    pthread_mutex_lock(&thread_pool->task_list_mutex);
    thread_pool->state = THREAD_STATE_STOPPING;
    pthread_mutex_unlock(&thread_pool->task_list_mutex);
    for (int i = 0; i < thread_pool->thread_num; i++)
    {
        pthread_join(thread_pool->threads[i], NULL);
    }
    thread_pool->state = THREAD_STATE_STOPPED;
}

void thread_pool_terminate(thread_pool_handler handler)
{
    thread_pool_t* thread_pool = (thread_pool_t*)handler;
    thread_pool->state = THREAD_STATE_CANCELING;
    
    for (int i = 0; i < thread_pool->thread_num; i++)
    {
        pthread_join(thread_pool->threads[i], NULL);
    }
    
    //Clear unfinish task
    lcb_list_t* list = thread_pool->task_list.list.next;
    while(list != &thread_pool->task_list.list)
    {
        task_list_t* task = LCB_LIST_ITEM(list, task_list_t, list);
        list = task->list.next;
        free(task);
    }
    
    thread_pool->state = THREAD_STATE_STOPPED;
}

void thread_pool_destory(thread_pool_handler handler)
{
    thread_pool_t* thread_pool = (thread_pool_t*) handler;
    pthread_mutex_destroy(&thread_pool->task_list_mutex);
    free(thread_pool);
}

int submit_task(thread_pool_handler handler, task_main_func func, void* tag)
{
    thread_pool_t* thread_pool = (thread_pool_t*)handler;
    pthread_mutex_lock(&thread_pool->task_list_mutex);
    if (thread_pool->state != THREAD_STATE_RUNNING)
    {
        pthread_mutex_unlock(&thread_pool->task_list_mutex);
        return 1;
    }
    task_list_t *list = (task_list_t*)malloc(sizeof(task_list_t));
    list->func = func;
    list->tag = tag;
    lcb_list_append(&thread_pool->task_list.list, list);
    pthread_mutex_unlock(&thread_pool->task_list_mutex);
    return 0;
}

void* thread_main(void *arg)
{
    thread_pool_t* pool = (thread_pool_t*)arg;
    
    while(1)
    {
        if (pool->state == THREAD_STATE_CANCELING)
        {
            break;
        }
        int state = 0;
        task_list_t* list = fetch_task_list(pool, &state);
        if (list == NULL)
        {
            if (state == THREAD_STATE_STOPPING)
            {
                break;
            }
            else
            {
                usleep(10000);
                continue;
            }
        }
        
        list->func(list->tag);
        free(list);
        sleep(0);
    }
    return NULL;
}

task_list_t* fetch_task_list(thread_pool_t* thread_pool, int* state)
{
    pthread_mutex_lock(&thread_pool->task_list_mutex);
    task_list_t* task_list = lcb_list_shift(&thread_pool->task_list.list);
    *state = thread_pool->state;
    pthread_mutex_unlock(&thread_pool->task_list_mutex);
    return task_list;
}
