/*
 * fifo.c
 *
 *  Created on: Sep 13, 2025
 *      Author: rfidf
 */

#include "fifo.h"

//QUEUE FUNCTIONS
void init_queue(fifo_queue *q)
{
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

//FIFO
int enqueue_FIFO(fifo_queue *q, queue_item item)
{
    if (q->count == QUEUE_SIZE)
    {
        // Fila cheia
        return -1;
    }

    q->buffer[q->tail] = item;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;

    return 0; // Sucesso
}


//fila circular com sobrescrição
int enqueue(fifo_queue *q, queue_item item)
{
    if (q->count == QUEUE_SIZE)
    {
        // Fila cheia: descarta o mais antigo (head)
        q->head = (q->head + 1) % QUEUE_SIZE;
        q->count--; // já que vamos inserir um novo, decrementamos antes
    }

    q->buffer[q->tail] = item;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;

    return 0; // Sucesso
}



int dequeue(fifo_queue *q, queue_item *item)
{
    if (q->count == 0)
    {
        // Fila vazia
        return -1;
    }

    *item = q->buffer[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;

    return 0; // Sucesso
}
int is_empty(fifo_queue *q)
{
    return (q->count == 0);
}
int is_full(fifo_queue *q)
{
    return (q->count == QUEUE_SIZE);
}




