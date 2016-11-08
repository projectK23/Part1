/*
 * queue.h
 *
 *  Created on: Nov 8, 2016
 *      Author: root
 */

#ifndef __queue
#define __queue

#include "../libs/include/shared_definitions.h"
#include "../libs/include/result_codes.h"
#include "../libs/include/log.h"

typedef struct _Snode{
	uint32_t nodeId;
	ptr p, n;
}node_t;

typedef struct _Squeue{
	int size;
	ptr first, last;
}queue_t;

typedef queue_t * Q;


/**
 * Definitions:
 */
Q Q_init();
void Q_destroy(Q *q);
Boolean Q_isEmpty(Q q);
int Q_size(Q q);
OK_SUCCESS Q_push(Q q, uint32_t nodeId);
OK_SUCCESS Q_pop(Q q, uint32_t *nodeId);

#endif

