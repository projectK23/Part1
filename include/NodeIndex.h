#ifndef NODEINDEX_H__
#define NODEINDEX_H__

/**
 * Includes
 */
#include "../libs/include/shared_definitions.h"
#include "../libs/include/result_codes.h"
#include "../libs/include/shared_definitions.h"

typedef struct _SIndex_node{
	uint32_t Id;              //node Id
	ptr posAtBuff;            //pointer to listhead
} Index_node;

typedef struct _SNodeIndex{
	uint32_t size;       //number of Index nodes
	Index_node* start;   //pointer to first Index node
	ptr end;             //pointer to last Index node
} NodeIndex;


/***************************************************
 * Purpose : creates an index list
 * IN      : n/a
 * Returns : pointer to the buffer <-- Success
 *           NULL                  x-- Fail
 */
NodeIndex* createNodeIndex();

/***************************************************
 * Purpose : insert a node in NodeIndex
 * IN      : Index,nodeId, ptr to buffer
 * Returns : Result code (result_codes.h)
 */
OK_SUCCESS insertNode(NodeIndex*, uint32_t nodeId, ptr point_in_buffer, Boolean force);

/***************************************************
 * Purpose : gets the pointer in buffer of a list node (with id)
 * IN      : Index, nodeId
 * Returns : Pointer to buffer
 */
ptr getListHead(NodeIndex*, uint32_t nodeId);

/***************************************************
 * Purpose : gets the pointer in buffer of a list node (with id)
 * IN      : nodeIndex
 * Returns : Result code
 */
OK_SUCCESS destroyNodeIndex(NodeIndex*);

/***************************************************
 * Purpose : updates index when pointer is moving to the end
 * IN      : NodeIndex *, bias
 * Returns : Result code
 */
OK_SUCCESS updateIndex(NodeIndex *, long bias);

/*
Result insertNeighbor(NodeIndex*, uint32_t source, uint32_t dest);

void printGraph(NodeIndex*);
*/
#endif
