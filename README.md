# ThreadPool sample in C language

本示例实现了两种简单的线程池模型。

## 1. thread_pool

任务提交到公共的列表，空闲工作线程从任务队列获取并执行任务

## 2. ordered_thread_pool
当任务需要按次序执行时，例如服务器中希望同一个用户（连接）的请求在同一个线程中按次序执行，可以使用这种模式。在该模式中，提交用户时需要制定jobId，同一个jobId的任务会在同一个线程内按提交次序执行。

## 3. 停止任务

有两种停止任务的接口.
### 3.1 *_stop
 * thread_pool_stop
 * ordered_thread_pool_stop
 
 该接口会先让提交的任务执行结束，再退出线程
 
### 3.2 *_terminate
 * thread_pool_terminate
 * ordered_thread_pool_terminate

该接口会在工作线程执行完当前的任务后立即停止，未完成的任务会被清除。

## 4. 使用实例

main.c 有各种模式使用的例子。

 
 

