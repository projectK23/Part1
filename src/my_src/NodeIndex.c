/**
 * Standard libraries
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Local definitions
 */
#include "NodeIndex.h"
#include "log.h"
#include "result_codes.h"
#include "shared_definitions.h"

/***************************************************
 * Purpose : creates an index list
 * IN      : n/a
 * Returns : pointer to the buffer <-- Success
 *           NULL                  x-- Fail
 */
NodeIndex* createNodeIndex(){
	TRACE_IN
	NodeIndex* nodeIndex;
	if ( ( nodeIndex = malloc(sizeof(NodeIndex)) ) == NULL){
		ERROR("Memory allocation for node index failed")
		TRACE_OUT
		return NULL;
	}
	nodeIndex->size = INIT;
	if ( ( nodeIndex->start = malloc(nodeIndex->size*sizeof(Index_node) ) ) == NULL ){
		ERROR("Memory allocation for nodeIndex failed")
		free(nodeIndex);
		TRACE_OUT
		return NULL;
	}
	memset(nodeIndex->start, 0, nodeIndex->size);
	LOG("Successfully created nodeIndex")
    nodeIndex->end= 0;
    TRACE_OUT
    return nodeIndex;
}

/***************************************************
 * Purpose : insert a node in NodeIndex
 * IN      : nodeId, ptr to buffer
 * Returns : Result code (result_codes.h)
 */
OK_SUCCESS insertNode(NodeIndex* nodeIndex, uint32_t nodeId, ptr pointInBuffer, Boolean force){
	TRACE_IN
	LOG("Check input")
	if ( pointInBuffer == -1 ){
		ERROR("NULL pointer")
		TRACE_OUT
		return Unknown_Failure;
	}
	if ( !force ){
		LOG("Search if node already exists")
			ptr node = 0;
		while (node < nodeIndex->end) {
			if ( (node+nodeIndex->start)->Id == nodeId){
				LOG("Node found")
				TRACE_OUT
				return Request_data_found;
			}
			node += 1;
		}
	}
	LOG("Check index space")
	if ( nodeIndex->end == nodeIndex->size  ){
		LOG("Add space to NodeIndex (realloc)")
		Index_node *new_ptr;
		if ( ( new_ptr = realloc(nodeIndex->start, 2*nodeIndex->size*sizeof(Index_node)) ) == NULL ){
			ERROR("Realloc failed")
			TRACE_OUT
			return Memory_Failure;
		}else{
			LOG("Realloc succeed. Reschedule NodeIndex list")
			nodeIndex->start = new_ptr;
			nodeIndex->size *= 2;
		}
	}
	(*(nodeIndex->start + nodeIndex->end)).Id = nodeId;
	(*(nodeIndex->start + nodeIndex->end)).posAtBuff = pointInBuffer;
	nodeIndex->end += 1;
	TRACE_OUT
	return Success;
}


/***************************************************
 * Purpose : gets the pointer in buffer of a list node (with id)
 * IN      : nodeId
 * Returns : Pointer to buffer
 */
ptr getListHead(NodeIndex* nodeIndex, uint32_t nodeId)
{
	TRACE_IN
	if ( nodeIndex == NULL){
		ERROR("Null nodeIndex provided")
		TRACE_OUT
		return -1;
	}
	if ( nodeIndex->start == NULL){
		ERROR("Uninitialized buffer. Execution should never be here.")
		TRACE_OUT
		return -1;
	}
	ptr node;
	for ( node = 0; node < nodeIndex->end; node++ ){
		if ( (nodeIndex->start + node)->Id == nodeId){
			LOG("Node found")
			TRACE_OUT
			return (node+nodeIndex->start)->posAtBuff;
		}
	}
	LOG("NodeId was not found in index")
	TRACE_OUT
	return -1;
}

/***************************************************
 * Purpose : frees memory index, reset values
 * IN      : nodeIndex
 * Returns : Result code
 */
OK_SUCCESS destroyNodeIndex(NodeIndex* index){
	TRACE_IN
	if ( index == NULL ){
		LOG("Invalid index")
		TRACE_OUT
		return Success;
	}
	if ( index->start == NULL){
		ERROR("List in index is not valid.")
		TRACE_OUT
		return Success;
	}
	LOG("Clear and free memory for index")
	free(index->start);
	free(index);

	TRACE_OUT
	return Success;
}
