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
	nodeIndex->size = STEP;
	if ( ( nodeIndex->start = malloc(nodeIndex->size*sizeof(Index_node) ) ) == NULL ){
		ERROR("Memory allocation for nodeIndex failed")
		free(nodeIndex);
		TRACE_OUT
		return NULL;
	}
	memset(nodeIndex->start, 0xFF, nodeIndex->size*sizeof(Index_node));
	LOG("Successfully created nodeIndex")
    TRACE_OUT
    return nodeIndex;
}

/***************************************************
 * Purpose : insert a node in NodeIndex
 * IN      : nodeId, ptr to buffer
 * Returns : Result code (result_codes.h)
 */
OK_SUCCESS insertNode(NodeIndex* nodeIndex, uint32_t nodeId, ptr pointInBuffer){
	TRACE_IN
	LOG("Check input")
	if ( pointInBuffer == -1 ){
		ERROR("NULL pointer")
		TRACE_OUT
		return Unknown_Failure;
	}
	LOG("Check index space")

	if ( nodeIndex->size <= nodeId){
		LOG("Add space to NodeIndex (realloc)")
		Index_node *new_ptr;
		int size = (nodeId / STEP + 1)*STEP;
		if ( ( new_ptr = realloc(nodeIndex->start, size*sizeof(Index_node)) ) == NULL ){
			ERROR("Realloc failed")
			TRACE_OUT
			return Memory_Failure;
		}else{
			LOG("Realloc succeed. Reschedule NodeIndex list")
			nodeIndex->start = new_ptr;
			memset(nodeIndex->start + nodeIndex->size, 0xFF, STEP*sizeof(Index_node));
			nodeIndex->size = size;
		}
	}else{
		LOG("Check if node exists in index")
		if ( (nodeIndex->start + nodeId )->posAtBuff != -1 ){
			LOG("Node exists in index")
			TRACE_OUT
			return Request_data_found;
		}
	}
	(nodeIndex->start + nodeId)->posAtBuff = pointInBuffer;
	(nodeIndex->start + nodeId)->lastBatch = pointInBuffer;
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
	if ( nodeId >= nodeIndex->size){
		LOG("Node not inside")
		TRACE_OUT
		return -1;
	}
	TRACE_OUT
	return (nodeIndex->start + nodeId)->posAtBuff;
}

/***************************************************
 * Purpose : gets the pointer in buffer of the last batch of a list node (with id)
 * IN      : nodeId
 * Returns : Pointer to buffer
 */
ptr getListTail(NodeIndex* nodeIndex, uint32_t nodeId)
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
	if ( nodeId > nodeIndex->size){
		LOG("Node not inside")
		TRACE_OUT
		return -1;
	}
	TRACE_OUT
	return (nodeIndex->start + nodeId)->lastBatch;
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
