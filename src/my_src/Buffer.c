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
	bf->max_size = INIT*sizeof(list_node);
	if ( ( bf->list_nodes = malloc( bf->max_size ) ) == NULL ){
		FATAL("Malloc at list  failed. Free buffer structure")
		free(bf);
		TRACE_OUT
		return NULL;
	}
	LOG("Initialize structure")
	memset(bf->list_nodes, 0xFF, bf->max_size );
	int i = 0;
	for ( i = 0; i < INIT; i++){
		((list_node*)(bf->list_nodes) + i )->nextListNode = NULL;
	}
	bf->end = bf->list_nodes;
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

#define LAST_PTR       (buffer->end)
#define START_PTR      (buffer->list_nodes)

ptr allocNewNode(Buffer* buffer)
{
	TRACE_IN
	ptr ret_val;
	if ( ( LAST_PTR + sizeof(list_node) - START_PTR )  > buffer->max_size ){
		LOG("Double size buffer")
		ptr new_ptr;
		if ( ( new_ptr = realloc(START_PTR, 2*buffer->max_size) ) == NULL ){
			ERROR("Realloc failed")
			TRACE_OUT
			return NULL;
		}else{
			LOG("Buffer was successfully doubled")
			memset(new_ptr + buffer->max_size, 0xFF, buffer->max_size);
			buffer->max_size *= 2;
			LAST_PTR += (new_ptr - START_PTR);
			START_PTR = new_ptr;
			ret_val = LAST_PTR;
			LAST_PTR += sizeof(list_node);
			new_ptr += buffer->max_size/2;
			for ( ;new_ptr < buffer->list_nodes + buffer->max_size; new_ptr += sizeof(list_node)){
				((list_node*)new_ptr)->nextListNode = NULL;
			}
		}
	}else{
		ret_val = LAST_PTR;
		LAST_PTR += sizeof(list_node);
	}
	LOG("Initialize last pointer")
	TRACE_OUT
	return ret_val;
}

#undef LAST_PTR
#undef START_PTR

/***************************************************
 * Purpose : Updates buffer indexes
 * IN      : Buffer, bias
 * Returns : n/a
 * Comment : Dangerous !!! :@
 */
void updateBufferIndex(Buffer *buf, long bias){
	TRACE_IN
	ptr node;
	for( node = buf->list_nodes; node < buf->list_nodes+buf->max_size/2; node+=sizeof(list_node)){
		if ( ((list_node*)node)->nextListNode != NULL ){
			((list_node*)node)->nextListNode += bias;
		}
	}
	TRACE_OUT
}

/***************************************************
 * Purpose : Reverts last allocation
 * IN      : Buffer
 * Returns : n/a
 * Comment : Dangerous !!! :@
 */
void freeLastNode(Buffer *buf){
	buf->end -= sizeof(list_node);
}

/*******************************************************************************
 * Purpose : Returns a struct list_node
 * IN      : void * ptr
 * Returns : struct list_node *
 * Comment : Should be used with construct node
 */

list_node* getListNode(ptr node){
	TRACE_IN
	TRACE_OUT
	return (list_node*)node;
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
	buffer->end = NULL;
	free(buffer->list_nodes);
	buffer->list_nodes = NULL;
	free(buffer);
	buffer = NULL;
	TRACE_OUT
	return Success;
}

