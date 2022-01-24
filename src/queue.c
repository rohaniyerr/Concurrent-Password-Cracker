#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct node {
    void *value;
    struct node *next;
} node_t;

/* The fields of the queue */ 
typedef struct queue {
    node_t *head;
    node_t *tail;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} queue_t;


/* Initialize the queue as empty with no header */
queue_t *queue_init(void){
    queue_t *queue = (queue_t *) malloc(sizeof(queue_t));
    queue->head = NULL;
    queue->tail = NULL;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->cond, NULL);
    return queue;
}

void queue_enqueue(queue_t *queue, void *value){
    pthread_mutex_lock(&queue->lock);
    node_t *node = (node_t *) malloc(sizeof(node_t));
    node->value = value;
    node->next = NULL;
    if (queue->head == NULL && queue->tail == NULL){ //there is nothing in the queue
        queue->head = node;
        queue->tail = node;
    }
    else{
        queue->tail->next = node;
        queue->tail = node;
    }
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
}

void *queue_dequeue(queue_t *queue){
    if(queue == NULL){return NULL;}
    pthread_mutex_lock(&queue->lock);
    while(queue->head == NULL && queue->tail == NULL){ // wait till something is enqueued
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    void *pop_val = queue->head->value;    
    if(queue->head == queue->tail){//only one item left in the queue
        free(queue->head);
        queue->head = NULL;
        queue->tail = NULL;
    }
    else{
        node_t *old_head = queue->head;
        queue->head = queue->head->next;
        free(old_head);
    }
    pthread_mutex_unlock(&queue->lock);
    return pop_val;
}

void queue_free(queue_t *queue){
    free(queue);
}

