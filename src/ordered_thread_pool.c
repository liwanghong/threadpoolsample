//
//  ordered_thread_pool.c
//  threadpoolsample
//
//  Created by lizi on 17/1/25.
//  Copyright © 2017年 hfut. All rights reserved.
//
#include "ordered_thread_pool.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "thread_pool_struct.h"

typedef struct single_thread_info_tag
{
    int state;
    task_list_t task_list;
    pthread_mutex_t task_list_mutex;
    pthread_t  thread;
}single_thread_info;

typedef struct ordered_thread_pool_tag
{
    int thread_num;
    single_thread_info threads[1];
}ordered_thread_pool_t;

static void* thread_main(void *arg);
static task_list_t* fetch_task_list(single_thread_info* thread_info, int* state);

ordered_thread_pool_handler ordered_thread_pool_create(int thread_num)
{
    size_t size = sizeof(ordered_thread_pool_t) + (thread_num - 1) * sizeof(single_thread_info);
    ordered_thread_pool_t* thread_pool = (ordered_thread_pool_t*)malloc(size);
    memset(thread_pool, 0, size);
    thread_pool->thread_num = thread_num;
    
    for (int i = 0; i < thread_num; i++)
    {
        lcb_list_init(&thread_pool->threads[i].task_list.list);
        pthread_mutex_init(&thread_pool->threads[i].task_list_mutex, NULL);
        thread_pool->threads[i].state = THREAD_STATE_RUNNING;
        pthread_create(&thread_pool->threads[i].thread, NULL, thread_main, &thread_pool->threads[i]);
    }
    
    return thread_pool;
}

void ordered_thread_pool_destory(ordered_thread_pool_handler handler)
{
    ordered_thread_pool_t* thread_pool = (ordered_thread_pool_t*)handler;
    for (int i = 0; i < thread_pool->thread_num; i++)
    {
        pthread_mutex_destroy(&thread_pool->threads[i].task_list_mutex);
    }
    free(thread_pool);
}

int submit_ordered_task(ordered_thread_pool_handler handler, task_main_func func, int job_id, void* tag)
{
    ordered_thread_pool_t* thread_pool = (ordered_thread_pool_t*)handler;
    int thread_index = job_id % thread_pool->thread_num;
    
    single_thread_info* thread_info = &thread_pool->threads[thread_index];
    pthread_mutex_lock(&thread_info->task_list_mutex);
    if (thread_info->state != THREAD_STATE_RUNNING)
    {
        pthread_mutex_unlock(&thread_info->task_list_mutex);
        return -1;
    }
    
    task_list_t *list = (task_list_t*)malloc(sizeof(task_list_t));
    list->func = func;
    list->tag = tag;
    lcb_list_append(&thread_info->task_list.list, list);
    pthread_mutex_unlock(&thread_info->task_list_mutex);
    return 0;
}

void ordered_thread_pool_stop(ordered_thread_pool_handler handler)
{
    ordered_thread_pool_t* thread_pool = (ordered_thread_pool_t*)handler;
    
    for(int i = 0; i < thread_pool->thread_num; i++)
    {
        single_thread_info* thread_info = &thread_pool->threads[i];
        pthread_mutex_lock(&thread_info->task_list_mutex);
        thread_info->state = THREAD_STATE_STOPPING;
        pthread_mutex_unlock(&thread_info->task_list_mutex);
    }
    
    for (int i = 0; i < thread_pool->thread_num; i++)
    {
        pthread_join(thread_pool->threads[i].thread, NULL);
        thread_pool->threads[i].state = THREAD_STATE_STOPPED;
    }
}

void ordered_thread_pool_terminate(ordered_thread_pool_handler handler)
{
    ordered_thread_pool_t* thread_pool = (ordered_thread_pool_t*)handler;
    for(int i = 0; i < thread_pool->thread_num; i++)
    {
        single_thread_info* thread_info = &thread_pool->threads[i];
        thread_info->state = THREAD_STATE_CANCELING;
    }
    
    for (int i = 0; i < thread_pool->thread_num; i++)
    {
        pthread_join(thread_pool->threads[i].thread, NULL);
        thread_pool->threads[i].state = THREAD_STATE_STOPPED;
        
        lcb_list_t* list = thread_pool->threads[i].task_list.list.next;
        while(list != &thread_pool->threads[i].task_list.list)
        {
            task_list_t* task = LCB_LIST_ITEM(list, task_list_t, list);
            list = task->list.next;
            free(task);
        }
    }
}

void* thread_main(void* arg)
{
    single_thread_info* thread_info = (single_thread_info*)arg;
    
    while(1)
    {
        if (thread_info->state == THREAD_STATE_CANCELING)
        {
            break;
        }
        
        int state = 0;
        task_list_t* list = fetch_task_list(thread_info, &state);
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

task_list_t* fetch_task_list(single_thread_info* thread_info, int* state)
{
    //sem_wait(&thread_info->task_list_sem);
    pthread_mutex_lock(&thread_info->task_list_mutex);
    task_list_t* task_list = lcb_list_shift(&thread_info->task_list.list);
    *state = thread_info->state;
    pthread_mutex_unlock(&thread_info->task_list_mutex);
    return task_list;
}


