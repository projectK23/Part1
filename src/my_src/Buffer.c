/**
 * Standard libraries
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Local definitions
 */
#include "Buffer.h" 
#include "log.h"
#include "result_codes.h"
#include "shared_definitions.h"


/***************************************************
 * Purpose : creates a buffer
 * IN      : n/a
 * Returns : initialize NULL
 * Comment : n/a
 */
Buffer* createBuffer() 
{
	TRACE_IN
	Buffer *bf;
	if ( (bf = malloc(sizeof(Buffer) ) ) == NULL){
		FATAL("Memory failed")
		TRACE_OUT
		return NULL;
	}
	bf->max_size = INIT;
	if ( ( bf->list_nodes = malloc( bf->max_size *sizeof(list_node)) ) == NULL ){
		FATAL("Malloc at list  failed. Free buffer structure")
		free(bf);
		TRACE_OUT
		return NULL;
	}
	LOG("Initialize structure")
	memset(bf->list_nodes, 0xFF, bf->max_size*sizeof(list_node) );
	bf->end = 0;
	TRACE_OUT
    return bf;
}

/*******************************************************************************
 * Purpose : Allocates a new node to the Buffer
 * IN      : Buffer
 * Returns : pointer to the node allocated <-- SUCCESS
 *           NULL                          X-- FAIL
 * Comment : If buffer is doubled then a pointer to buffer may change
 *           This should be distinguished by application code, so as to update list
 */

ptr allocNewNode(Buffer* buffer)
{
	TRACE_IN
	ptr ret_val;
	if ( buffer->end == buffer->max_size ){
		LOG("Double size buffer")
		list_node *new_ptr;
		if ( ( new_ptr = realloc(buffer->list_nodes, (2*buffer->max_size)*sizeof(list_node) ) ) == NULL ){
			ERROR("Realloc failed")
			TRACE_OUT
			return -1;
		}else{
			LOG("Buffer was successfully doubled")
			memset(new_ptr + buffer->max_size, 0xFF, buffer->max_size*sizeof(list_node));
			buffer->max_size *= 2;
			buffer->list_nodes = new_ptr;
			ret_val = buffer->end;
			buffer->end += 1;
			uint32_t pos;
		}
	}else{
		ret_val = buffer->end;
		buffer->end += 1;
	}
	LOG("Initialize last pointer")
	TRACE_OUT
	return ret_val;
}

/***************************************************
 * Purpose : Reverts last allocation
 * IN      : Buffer
 * Returns : n/a
 * Comment : Dangerous !!! :@
 */
void freeLastNode(Buffer *buf){
	buf->end -= 1;
	memset(buf->end + buf->list_nodes, 0xff, sizeof(list_node));
}

/*******************************************************************************
 * Purpose : Returns a struct list_node
 * IN      : void * ptr
 * Returns : struct list_node *
 * Comment : Should be used with construct node
 */

list_node* getListNode(Buffer *buf, ptr node){
	TRACE_IN
	TRACE_OUT
	return buf->list_nodes + node;
}


/*******************************************************************************
 * Purpose : Frees all allocated memory, initializes memory
 * IN      : Buffer *
 * Returns : Return cause*
 * Comment : n/a
 */
OK_SUCCESS destroyBuffer(Buffer* buffer)
{
	TRACE_IN
	if ( buffer == NULL){
		LOG("Uninitialized buffer struct.")
		TRACE_OUT
		return Success;
	}
	if ( buffer->list_nodes == NULL ){
		ERROR("Uninitialized buffer.. Should never be here. Something bad happens")
		memset(buffer, 0, sizeof(*buffer) );
		free(buffer);
		TRACE_OUT
		return Unknown_Failure;
	}
	memset(buffer->list_nodes, 0, buffer->max_size);
	free(buffer->list_nodes);
	buffer->list_nodes = NULL;
	free(buffer);
	buffer = NULL;
	TRACE_OUT
	return Success;
}

