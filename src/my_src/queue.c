/*
 * queue.c
 *
 *  Created on: Nov 8, 2016
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>

#include "../libs/include/shared_definitions.h"
#include "../libs/include/result_codes.h"
#include "../libs/include/log.h"
#include "../../include/queue.h"


Q Q_init(){
	TRACE_IN
	Q q = malloc(sizeof(queue_t) );
	if ( q == NULL ){
		ERROR("Malloc failed")
		TRACE_OUT
		return NULL;
	}
	q->first = NULL;
	q->last = NULL;
	q->size = 0;
	TRACE_OUT
	return q;
}

void Q_destroy(Q *q){
	TRACE_IN
	if ( q == NULL ){
		ERROR("NULL pointer")
		TRACE_OUT
		return;
	}
	if ( *q == NULL ){
		ERROR("Uninitialized queue")
		TRACE_OUT
		return;

	}
	data_t dmy;
	OK_SUCCESS ret;
	while( (ret = Q_pop(*q, &dmy) ) != Empty_list ){
		if ( ret != Success){
			ERROR("Error in q_destroy. Possible memory leak")
			free( *q);
			*q = NULL;
			TRACE_OUT
			return;
		}
	}
	if ( (*q)->first == NULL && (*q)->first == NULL ){
		free( *q);
		*q = NULL;
		TRACE_OUT
		return;
	}
	ERROR("Unknown failure")
	TRACE_OUT
}

Boolean Q_isEmpty(Q q){
	TRACE_IN
	TRACE_OUT
	return q->size == 0;
}

int Q_size(Q q){
	TRACE_IN
	TRACE_OUT
	return q->size;
}



OK_SUCCESS Q_push(Q q, data_t d){
	TRACE_IN
	if ( q == NULL ){
		ERROR("Uninitialized queue")
		TRACE_OUT
		return Unknown_Failure;
	}
	ptr node = malloc(sizeof(node_t) );
	if (node == NULL){
		ERROR("Uninitialized queue")
		TRACE_OUT
		return Memory_Failure;
	}
	((node_t*)node)->n = q->first;
	((node_t*)node)->p = NULL;
	((node_t*)node)->data.nodeId = d.nodeId;
	((node_t*)node)->data.step = d.step;
	q->first = node;
	if ( q->last == NULL)
		q->last = node;
	else
		((node_t*)(((node_t*)(q->first))->n))->p = q->first;
	q->size += 1;
	TRACE_OUT
	return Success;
}


OK_SUCCESS Q_pop(Q q, data_t *d){
	TRACE_IN
	if ( q == NULL ){
		ERROR("Uninitialized queue")
		TRACE_OUT
		return Unknown_Failure;
	}
	if ( q->last == NULL ){
		LOG("Empty queue")
		TRACE_OUT
		return Empty_list;
	}
	node_t *node;
	node = q->last;
	d->nodeId= node->data.nodeId;
	d->step= node->data.step;

	if ( q->first == q->last){
		free(node);
		q->first = NULL;
		q->last = NULL;
		q->size -= 1;
		TRACE_OUT
		return Success;
	}else{
		q->last = node->p;
		free(node);
		q->size -= 1;
		TRACE_OUT
		return Success;
	}
}





