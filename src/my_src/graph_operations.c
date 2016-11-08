/**
 * Standard libs
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


/**
 * Local libs
 */
#include "log.h"
#include "shared_definitions.h"
#include "result_codes.h"

/**
 * Local files
 */
#include "graph_operations.h"
#include "NodeIndex.h"
#include "queue.h"

/******************************************************
 * PURPOSE : Creates a graph structure
 * IN      : n/a
 * OUT     : Pointer to graph <-- SUCCESS
 *           NULL             x-- FAIL
 * COMMENTS: n/a
 */
Graph graphCreate(){
	TRACE_IN
	Graph graph = NULL;
	if ( (graph = malloc( sizeof(graph_t) ) ) == NULL ){
		FATAL("Unable to initialize graph")
		return NULL;
	}else{
		LOG("Memory for graph successfully allocated")
		graph->bufferOut = NULL;
		graph->bufferInc = NULL;
		graph->nodeIndexOut = NULL;
		graph->nodeIndexInc = NULL;
		graph->startBufferOut = NULL;
		graph->startBufferInc = NULL;

	}
	if ( ( graph->nodeIndexOut = createNodeIndex() ) == NULL){
		FATAL("Index can not be initialized.")
		goto destroy_members;
	}
	if ( ( graph->nodeIndexInc = createNodeIndex() ) == NULL){
		FATAL("Index can not be initialized.")
		goto destroy_members;
	}
	if ( ( graph->bufferOut = createBuffer() ) == NULL ){
		FATAL("Buffer could not be initialized")
		goto destroy_members;
	}else{
		LOG("BufferOut successfully created")
		graph->startBufferOut = graph->bufferOut->list_nodes;
	}
	if ( ( graph->bufferInc = createBuffer() ) == NULL ){
		FATAL("Buffer could not be initialized")
		goto destroy_members;
	}else{
		LOG("BufferInc successfully created")
		graph->startBufferInc = graph->bufferInc->list_nodes;
	}
	TRACE_OUT
	return graph;
destroy_members:
	destroyNodeIndex(graph->nodeIndexOut);
	destroyNodeIndex(graph->nodeIndexInc);
	destroyBuffer(graph->bufferOut);
	destroyBuffer(graph->bufferInc);
	free(graph);
	return NULL;
}

/******************************************************
 * PURPOSE : Print graph
 * IN      : Graph
 * OUT     : void
 * COMMENTS: Only for debugging reasons
 */
void printGraph(Graph graph){
	ptr node;
	int i;
	printf("-------- GRAPH ---------\n");
	printf("graph = %ld\n", (long)graph);
	printf("bufOut(start: %ld, end: %ld, size :%u)\n",
			(long)graph->bufferOut->list_nodes, (long)graph->bufferOut->end, graph->bufferOut->max_size);
	for(node = graph->bufferOut->list_nodes; node < graph->bufferOut->end; node += sizeof(list_node)){
		printf(" (0x%x) ", node);
		for ( i = 0; i < INIT; i++){
				printf("0x%x ", ((list_node*)node)->neighbor[i] );
		}
		printf(" | 0x%x | \n",  ((list_node*)node)->nextListNode);
	}
	printf("NodeIndexOut (start: 0x%x, end: 0x%x, size :%u)\n",
			graph->nodeIndexOut->start, graph->nodeIndexOut->end, graph->nodeIndexOut->size);
	for ( node = graph->nodeIndexOut->start; node < graph->nodeIndexOut->end; node += sizeof(Index_node)){
		printf(" (0x%x) : 0x%x 0x%x, ", node, ((Index_node*)node)->Id, ((Index_node*)node)->posAtBuff);
	}
	printf("\n\n");
	printf("bufInc(start: 0x%x, end: 0x%x, size :%u\n",
			graph->bufferInc->list_nodes, graph->bufferInc->end, graph->bufferInc->max_size);
	for(node = graph->bufferInc->list_nodes; node < graph->bufferInc->end; node += sizeof(list_node)){
		printf(" (0x%x) ", node);
		for ( i = 0; i < INIT; i++){
				printf("0x%x ", ((list_node*)node)->neighbor[i] );
		}
		printf(" | 0x%x | \n",  ((list_node*)node)->nextListNode);
	}
	printf("NodeIndexInc (start: 0x%x, end: 0x%x, size :%u)\n",
			(long)graph->nodeIndexInc->start, (long)graph->nodeIndexInc->end, graph->nodeIndexInc->size);
	for ( node = graph->nodeIndexInc->start; node < graph->nodeIndexInc->end; node += sizeof(Index_node)){
		printf(" (0x%x) : 0x%x 0x%x, ", node, ((Index_node*)node)->Id, ((Index_node*)node)->posAtBuff);
	}

	printf("\n------------------------\n\n");

}

/******************************************************
 * PURPOSE : Update graph pointers
 * IN      : Pointer to graph
 * OUT     : n/a
 * COMMENTS: Inside a function, so as to void code replication
 */
void updateGraphPointers(Graph graph, Boolean Out){
	TRACE_IN
	if ( Out){
		if ( graph->startBufferOut != graph->bufferOut->list_nodes){
			LOG("Add bias in nodeIndexOut and bufferOut")
			updateIndex(graph->nodeIndexOut, graph->bufferOut->list_nodes - graph->startBufferOut);
			updateBufferIndex(graph->bufferOut, graph->bufferOut->list_nodes - graph->startBufferOut);
			graph->startBufferOut = graph->bufferOut->list_nodes;
		}
		TRACE_OUT
	}else{
		if ( graph->startBufferOut != graph->bufferOut->list_nodes){
			LOG("Add bias in nodeIndexOut and bufferOut")
			updateIndex(graph->nodeIndexOut, graph->bufferOut->list_nodes - graph->startBufferOut);
			updateBufferIndex(graph->bufferOut, graph->bufferOut->list_nodes - graph->startBufferOut);
			graph->startBufferOut = graph->bufferOut->list_nodes;
		}
		TRACE_OUT
	}
}


/******************************************************
 * PURPOSE : Destroys graph
 * IN      : Pointer to graph
 * OUT     : Result   <-- SUCCESS
 *           NULL     x-- FAIL
 * COMMENTS: n/a
 */
OK_SUCCESS graphDestroy(Graph *graph){
	TRACE_IN
	Boolean success = True;
	if ( *graph == NULL){
		LOG("Graph is not initialized");
		TRACE_OUT
		return Success;
	}

	if ( destroyNodeIndex( (*graph)->nodeIndexOut) != Success ){
		ERROR("Resources of nodeIndexOut in graph was not released successfully")
		success = False;
	}
	(*graph)->nodeIndexOut = NULL;

	if ( destroyNodeIndex( (*graph)->nodeIndexInc) != Success ){
		ERROR("Resources of nodeIndexInc in graph was not released successfully")
		success = False;
	}
	(*graph)->nodeIndexOut = NULL;

	if ( destroyBuffer( (*graph)->bufferOut) != Success ){
		ERROR("Resources of nodeIndexInc in graph was not released successfully")
		success = False;
	}
	(*graph)->bufferOut = NULL;
	(*graph)->startBufferOut = NULL;

	if ( destroyBuffer( (*graph)->bufferInc) != Success ){
		ERROR("Resources of bufferInc in graph was not released successfully")
		success = False;
	}
	(*graph)->bufferInc = NULL;
	(*graph)->startBufferOut = NULL;

	free( *graph );
	*graph = NULL;
	TRACE_OUT
	if ( success) return Success;
	return Unknown_Failure;
}


/******************************************************
 * PURPOSE : Insert a node in graph
 * IN      : NodeIndex *nodeIndex, uint32_t
 * OUT     : Result cause
 * COMMENTS: n/a
 */
OK_SUCCESS insertNodeInGraph(Graph graph, uint32_t nodeId){
	TRACE_IN
	ptr new_node;

	LOG("Insert Outgoing")
	if ( getListHead(graph->nodeIndexOut, nodeId) == NULL){
		LOG("Node was not inside nodeIndexOut")
		if ( (new_node = allocNewNode(graph->bufferOut) ) == NULL ){
			ERROR("Allocation in buffer failed");
			TRACE_OUT
			return Unknown_Failure;
		}else{
			LOG("Allocation in buffer was successful")
			updateGraphPointers(graph, True);
			if ( insertNode(graph->nodeIndexOut, nodeId, new_node, True) == Memory_Failure){
				ERROR("Unable to insert node in NodeIndex. Revert last insertion in buffer")
				freeLastNode(graph->bufferOut);
				TRACE_OUT
				return Memory_Failure;
			}
		}
	}else{
		LOG("Node already exists in graph")
		if ( getListHead(graph->nodeIndexInc, nodeId) == NULL){
			LOG("..but does not exist in nodeIndexInc list")
			FATAL("Error in construction of graph!!");
			TRACE_OUT
			return Unknown_Failure;
		}
		LOG("Node exists in graph")
		TRACE_OUT
		return Request_data_found;
	}

	LOG("Insert Incoming")
	if ( getListHead(graph->nodeIndexInc, nodeId) == NULL){
		LOG("Node was not inside nodeIndexInc")
		if ( (new_node = allocNewNode(graph->bufferInc) ) == NULL ){
			ERROR("Allocation in buffer failed");
			TRACE_OUT
			return Unknown_Failure;
		}else{
			LOG("Allocation in buffer was successful")
			updateGraphPointers(graph, False);
			if ( insertNode(graph->nodeIndexInc, nodeId, new_node, True) == Memory_Failure){
				ERROR("Unable to insert node in NodeIndex. Revert last insertion in buffer")
				freeLastNode(graph->bufferInc);
				TRACE_OUT
				return Memory_Failure;
			}
		}
	}else{
		ERROR("Node already existed in list with the incoming edges, but inserted in the list with the outgoing")
		printf("Node with Id =%u exists in the incoming nodeIndex, but not in outgoing.", nodeId);
		LOG("Node exists in graph")
		TRACE_OUT
		return Request_data_found;
	}

	TRACE_OUT
	return Success;
}

/******************************************************
 * PURPOSE : Insert an edge in graph
 * IN      : Graph, source, destination
 * OUT     : Result cause
 * COMMENTS: n/a
 */
OK_SUCCESS insertEdgeInGraph(Graph graph, uint32_t sourceId, uint32_t destId){
	TRACE_IN
	ptr sourceEdgeList, destEdgeList, new_node;
	LOG("Get positions in buffer. Check that graph is ok. If not try to recover graph")
	if ( ( sourceEdgeList = getListHead(graph->nodeIndexOut, sourceId) ) == NULL){
		ERROR("Node was not inside nodeIndexOut")
		printf("Error for sourceId= %u\n", sourceId);
		LOG("Insert sourceId in graph for recovery")
		if ( getListHead(graph->nodeIndexInc, sourceId) == NULL){
			ERROR("Node does not exist in graph.")
			if ( insertNodeInGraph(graph, sourceId) != Success ){
				ERROR("Graph failed to recover");
				TRACE_OUT
				return Unknown_Failure;
			}else if ( ( sourceEdgeList = getListHead(graph->nodeIndexOut, sourceId) ) == NULL ){
				TRACE_OUT
				return Unknown_Failure;
			}
		}else{
			ERROR("sourceId node exists in list with incoming edges")
			if ( (new_node = allocNewNode(graph->bufferOut) ) == NULL ){
				ERROR("Allocation in buffer failed");
				TRACE_OUT
				return Unknown_Failure;
			}else{
				LOG("Allocation in buffer was successful")
				updateGraphPointers(graph, True);
				if ( insertNode(graph->nodeIndexOut, sourceId, new_node, True) == Memory_Failure){
					ERROR("Unable to insert node in NodeIndex. Revert last insertion in buffer")
					freeLastNode(graph->bufferOut);
					TRACE_OUT
					return Memory_Failure;
				}
			}
		}
	}else{
		LOG("Node exists in nodeIndexOut")
		if ( getListHead(graph->nodeIndexInc, sourceId) == NULL){
			ERROR("Node does not exist in index for incoming nodes. Try to recover")
			if ( (new_node = allocNewNode(graph->bufferInc) ) == NULL ){
				ERROR("Allocation in buffer failed");
				TRACE_OUT
				return Unknown_Failure;
			}else{
				LOG("Allocation in buffer was successful")
				updateGraphPointers(graph, False);
				if ( insertNode(graph->nodeIndexInc, sourceId, new_node, True) == Memory_Failure){
					ERROR("Unable to insert node in NodeIndex. Revert last insertion in buffer")
					freeLastNode(graph->bufferInc);
					TRACE_OUT
					return Memory_Failure;
				}
			}
		}
	}


	if ( ( destEdgeList = getListHead(graph->nodeIndexInc, destId) ) == NULL){
		ERROR("Node was not inside nodeIndexInc")
		printf("Error for destId= %u\n", destId);
		LOG("Insert destId in graph for recovery")
		if ( getListHead(graph->nodeIndexOut, destId) == NULL){
			ERROR("Node does not exist at all in graph.")
			if ( insertNodeInGraph(graph, sourceId) != Success ){
				ERROR("Graph failed to recover");
				TRACE_OUT
				return Unknown_Failure;
			}else if ( ( destEdgeList = getListHead(graph->nodeIndexOut, sourceId) ) == NULL ){
				TRACE_OUT
				return Unknown_Failure;
			}
		}else{
			ERROR("destId node exists in list with outgoing edges")
			if ( (new_node = allocNewNode(graph->bufferInc) ) == NULL ){
				ERROR("Allocation in buffer failed");
				TRACE_OUT
				return Unknown_Failure;
			}else{
				LOG("Allocation in buffer was successful")
				updateGraphPointers(graph, False);
				if ( insertNode(graph->nodeIndexInc, destId, new_node, True) == Memory_Failure){
					ERROR("Unable to insert node in NodeIndex. Revert last insertion in buffer")
					freeLastNode(graph->bufferInc);
					TRACE_OUT
					return Memory_Failure;
				}
			}
		}
	}else{
		LOG("Node exists in nodeIndexInc")
		if ( getListHead(graph->nodeIndexOut, destId) == NULL){
			ERROR("Node does not exist in index for outgoing nodes. Try to recover")
			if ( (new_node = allocNewNode(graph->bufferOut) ) == NULL ){
				ERROR("Allocation in buffer failed");
				TRACE_OUT
				return Unknown_Failure;
			}else{
				LOG("Allocation in buffer was successful")
				updateGraphPointers(graph, True);
				if ( insertNode(graph->nodeIndexOut, destId, new_node, True) == Memory_Failure){
					ERROR("Unable to insert node in NodeIndex. Revert last insertion in buffer")
					freeLastNode(graph->bufferOut);
					TRACE_OUT
					return Memory_Failure;
				}
			}
		}
	}

#if DEBUG_LEVEL == 3
	printGraph(graph);
#endif


	list_node *lnOut, *lnInc;
	int i;
	Boolean hasRoom;

	LOG("Go in last batch for outgoing edges")
	lnOut = getListNode(sourceEdgeList);
	while(lnOut->nextListNode != NULL){
		lnOut = lnOut->nextListNode;
	}
	hasRoom = False;
	for( i = 0; i < INIT; i++){
		if ( lnOut->neighbor[i] == 0xffffffff){
			LOG("Empty room exists in last batch")
			lnOut->neighbor[i] = destId;
			hasRoom = True;
			break;
		}
	}
	if ( !hasRoom ){
		LOG("Allocate batch in bufferOut")
		if ( (lnOut->nextListNode = allocNewNode(graph->bufferOut) ) != NULL ){
			updateGraphPointers(graph, True);
			lnOut = lnOut->nextListNode;
			lnOut->neighbor[0] = destId;
		}else{
			ERROR("Failed to allocate space for list batch")
			TRACE_OUT
			return Memory_Failure;
		}
	}
	LOG("Go in last batch for outgoing edges")
	lnInc = getListNode(destEdgeList);

printf("lnInc = 0x%x\n", lnInc);

	while(lnInc->nextListNode != NULL){
		printf("--> 0x%x\n", lnInc->nextListNode);
		lnInc = lnInc->nextListNode;
	}
	hasRoom = False;
	for( i = 0; i < INIT; i++){
		if ( lnInc->neighbor[i] == 0xffffffff){
			LOG("Empty room exists in last batch")
			hasRoom = True;
			break;
		}
	}
	if ( !hasRoom ){
		LOG("Allocate batch in bufferInc")
		if ( (lnInc->nextListNode = allocNewNode(graph->bufferInc) ) != NULL ){
			updateGraphPointers(graph, False);
			lnInc = lnInc->nextListNode;
			i = 0;
		}else{
			ERROR("Failed to allocate space for list batch")
			TRACE_OUT
			return Memory_Failure;
		}
	}
	lnInc->neighbor[i] = sourceId;
	TRACE_OUT
	return Success;
}


/***************************
 * LOCAL:
 */
int l_existPathInGraph(NodeIndex *index, uint32_t source, uint32_t target){
	TRACE_IN
	if ( source == target){
		LOG("source == target in search")
		TRACE_OUT
		return 0;
	}

	Q q;
	data_t node;
	ptr list_node_ptr;
	list_node *list_node_p;
	int i;

	if ( ( q = Q_init() ) == NULL){
		ERROR("q_init failed")
		TRACE_OUT
		return -1;
	}
	node.nodeId = source;
	node.step = 0;
	if ( Q_push(q, node) != Success ){
		ERROR("q_init failed")
		goto search_error;
	}
	while( !Q_isEmpty(q) ){
		if ( Q_pop(q, &node) != Success ){
			FATAL("Error in search algorithm (Q1). Probable software error.")
			goto search_error;
		}
		if ( node.nodeId == target ){
			LOG("Found path")
			goto search_found;
		}
		if ( ( list_node_ptr = getListHead(index, node.nodeId) ) == NULL ){
			FATAL("Error in search algorithm (I). Probable software error.")
			LOG("Unsafe search results")
			continue;
		}
		list_node_p = getListNode(list_node_ptr);
		node.step += 1;
		do{
			for ( i = 0; i < INIT; i++){
				if ( list_node_p->neighbor[i] == -1 )break;
				node.nodeId = list_node_p->neighbor[i];
				if ( Q_push(q, node) != Success){
					FATAL("Error in search algorithm (Q2). Probable software error.")
					goto search_error;
				}
//printf("neigh = %d\n", list_node_p->neighbor[i]);
			}

		}while(  (list_node_p = (list_node* )list_node_p->nextListNode) != NULL );
	}
	TRACE_OUT
	LOG("Path not found, normal execution")
	Q_destroy(&q);
	return -1;
search_found:
	TRACE_OUT
	Q_destroy(&q);
	return node.step;

search_error:
	Q_destroy(&q);
	TRACE_OUT
	return -1;
}


/******************************************************
 * PURPOSE : Search path
 * IN      : Graph, source, target
 * OUT     : Result cause
 * COMMENTS: Path is not hold somewhere.
 *           2 posix threads do b-directional search
 *           main_thread is blocked, until result
 *
 */
int existPathInGraph(Graph graph, uint32_t source, uint32_t target){
	TRACE_IN
	TRACE_OUT
	return l_existPathInGraph(graph->nodeIndexOut, source, target);
}


