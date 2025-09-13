/*
 * fifo.h
 *
 *  Created on: Sep 13, 2025
 *      Author: rfidf
 */

#ifndef INC_FIFO_H_
#define INC_FIFO_H_


#include <stdint.h>

typedef struct queue_item_ {
  uint8_t data_byte;
} queue_item;


#define QUEUE_SIZE 100  // Tamanho máximo da fila, don't change it
typedef struct {
	queue_item buffer[QUEUE_SIZE]; // Array de elementos do tipo string_tcp
    int head;                      // Índice do primeiro elemento da fila
    int tail;                      // Índice do próximo elemento disponível
    int count;                     // Número de elementos na fila
} fifo_queue;


void init_queue(fifo_queue *q);
int enqueue_FIFO(fifo_queue *q, queue_item item);
int enqueue(fifo_queue *q, queue_item item);
int dequeue(fifo_queue *q, queue_item *item);
int is_empty(fifo_queue *q);
int is_full(fifo_queue *q);

#endif /* INC_FIFO_H_ */
