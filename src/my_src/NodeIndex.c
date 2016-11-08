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
	nodeIndex->size = INIT*sizeof(Index_node);
	if ( ( nodeIndex->start = malloc(nodeIndex->size ) ) == NULL ){
		ERROR("Memory allocation for nodeIndex failed")
		free(nodeIndex);
		TRACE_OUT
		return NULL;
	}
	memset(nodeIndex->start, 0, nodeIndex->size);
	LOG("Successfully created nodeIndex")
    nodeIndex->end= nodeIndex->start;
    TRACE_OUT
    return nodeIndex;
}

/***************************************************
 * Purpose : insert a node in NodeIndex
 * IN      : nodeId, ptr to buffer
 * Returns : Result code (result_codes.h)
 */
#define START_PTR nodeIndex->start
#define END_PTR   nodeIndex->end
#define ADD_BYTES sizeof(Index_node)

OK_SUCCESS insertNode(NodeIndex* nodeIndex, uint32_t nodeId, ptr pointInBuffer, Boolean force){
	TRACE_IN
	LOG("Check input")
	if ( pointInBuffer == NULL ){
		ERROR("NULL pointer")
		TRACE_OUT
		return Unknown_Failure;
	}
	if ( !force ){
		LOG("Search if node already exists")
			ptr node = nodeIndex->start;
		while (node < nodeIndex->end) {
			if ( ((Index_node*)node)->Id == nodeId){
				LOG("Node found")
					TRACE_OUT
					return Request_data_found;
			}
			node += sizeof(Index_node);
		}
	}
	LOG("Check index space")
	if ( END_PTR + ADD_BYTES - START_PTR > nodeIndex->size  ){
		LOG("Add space to NodeIndex (realloc)")
		ptr new_ptr;
		if ( ( new_ptr = realloc(START_PTR, 2*nodeIndex->size) ) == NULL ){
			ERROR("Realloc failed")
			TRACE_OUT
			return Memory_Failure;
		}else{
			LOG("Realloc succeed. Reschedule NodeIndex list")
			END_PTR += (new_ptr - START_PTR);
			START_PTR = new_ptr;
			nodeIndex->size *= 2;
		}
	}
	((Index_node*)END_PTR)->Id = nodeId;
	((Index_node*)END_PTR)->posAtBuff = pointInBuffer;
	END_PTR += ADD_BYTES;
	TRACE_OUT
	return Success;
}

#undef START_PTR
#undef END_PTR
#undef ADD_BYTES

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
		return NULL;
	}
	if ( nodeIndex->start == NULL){
		ERROR("Uninitialized buffer. Execution should never be i here.")
		TRACE_OUT
		return NULL;
	}
	ptr node = nodeIndex->start;
	while (node < nodeIndex->end) {
		if ( ((Index_node*)node)->Id == nodeId){
			LOG("Node found")
			TRACE_OUT
			return ((Index_node*)node)->posAtBuff;
		}
		node += sizeof(Index_node);
	}
	LOG("NodeId was not found in index")
	TRACE_OUT
	return NULL;
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
	memset(index->start, 0, index->size);
	index->end = NULL;
	free(index->start);
	index->start = NULL;
	memset(index, 0, sizeof(index) );
	free(index);

	TRACE_OUT
	return Success;
}

/***************************************************
 * Purpose : updates index when pointer is moving to the end
 * IN      : NodeIndex *, bias
 * Returns : Result code
 */
OK_SUCCESS updateIndex(NodeIndex *nodeIndex, long bias){
	TRACE_IN
	if ( nodeIndex == NULL){
		ERROR("Null nodeIndex provided")
		TRACE_OUT
		return Success;
	}
	if ( nodeIndex->start == NULL){
		ERROR("Null nodeIndex->start provided")
		TRACE_OUT
		return Success;
	}
	ptr node = nodeIndex->start;
	while (node < nodeIndex->end) {
		if ( ((Index_node*)node)->posAtBuff == NULL){
			FATAL("Node pointing at NULL found")
			continue;
		}
		((Index_node*)node)->posAtBuff += bias;
		node += sizeof(Index_node);
	}

	TRACE_OUT
	return Success;

}
