#include "thread_pool.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct thread_pool {
    size_t length;
    queue_t *queue;
    pthread_t *work_list;
} thread_pool_t;

typedef struct work {
    work_function_t work_function;
    void *aux;
} work_t;


void *thread_func(void *queue){
    while(1){
        queue = (queue_t *) queue;
        void *task = queue_dequeue(queue);
        if (task == NULL){
            return NULL;
        }
        work_t *work = (work_t *) task;
        (work->work_function)(work->aux);
        free(task);
    }
    return NULL;
}


thread_pool_t *thread_pool_init(size_t num_worker_threads){
    thread_pool_t *thread_pool = (thread_pool_t *) malloc(sizeof(thread_pool_t)); //make the thread pool
     pthread_t *workers = (pthread_t *) malloc(num_worker_threads * sizeof(pthread_t)); // make num_worker_thread amount of pthreads
    thread_pool->queue = queue_init();
    thread_pool->work_list = workers;
    thread_pool->length = num_worker_threads;
    for(size_t i = 0; i < num_worker_threads; i++){
        pthread_create(&thread_pool->work_list[i], NULL, thread_func, thread_pool->queue);
    }
    return thread_pool;
}


void thread_pool_add_work(thread_pool_t *pool, work_function_t function, void *aux){
    work_t *work = (work_t *) malloc(sizeof(work_t));
    if (work == NULL){return;}
    work->work_function = function;
    work->aux = aux;
    queue_enqueue(pool->queue, (void *) work);
}

void thread_pool_finish(thread_pool_t *pool){
    for(size_t i = 0; i < pool->length; i++){
        queue_enqueue(pool->queue, NULL);
    }
    for(size_t i = 0; i < pool->length; i++){
        pthread_join(pool->work_list[i], NULL);
    }
    queue_free(pool->queue);
    free(pool->work_list);
    free(pool);
}

