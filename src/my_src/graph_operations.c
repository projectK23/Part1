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
	}
	if ( ( graph->bufferInc = createBuffer() ) == NULL ){
		FATAL("Buffer could not be initialized")
		goto destroy_members;
	}else{
		LOG("BufferInc successfully created")
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
	printf("bufOut(start: 0x%x, end: 0x%x, size :%u)\n",
			graph->bufferOut->list_nodes,
			graph->bufferOut->list_nodes + graph->bufferOut->end,
			graph->bufferOut->max_size);
	for(node = 0; node < graph->bufferOut->end; node ++){
		printf(" (%d) ", node);
		for ( i = 0; i < INIT; i++){
				printf("%u ", (graph->bufferOut->list_nodes + node)->neighbor[i] );
		}
		printf(" | %ld |\n",  (graph->bufferOut->list_nodes + node)->nextListNode);
	}
	printf("NodeIndexOut :\n");
	for ( node = 0; node < graph->nodeIndexOut->end; node++){
		printf(" (%d) : %d - %ld - %ld, ",
				node,
				(graph->nodeIndexOut->start + node)->Id,
				(graph->nodeIndexOut->start + node)->posAtBuff,
				(graph->nodeIndexOut->start + node)->lastBatch );
	}
	printf("\n\n");
	printf("bufInc(start: 0x%x, end: 0x%x, size :%u)\n",
			graph->bufferInc->list_nodes,
			graph->bufferInc->list_nodes + graph->bufferOut->end,
			graph->bufferInc->max_size);
	for(node = 0; node < graph->bufferInc->end; node ++){
		printf(" (%d) ", node);
		for ( i = 0; i < INIT; i++){
				printf("%u ", (graph->bufferInc->list_nodes + node)->neighbor[i] );
		}
		printf(" | %ld |\n",  (graph->bufferInc->list_nodes + node)->nextListNode);
	}
	printf("NodeIndexInc :\n");
	for ( node = 0; node < graph->nodeIndexInc->end; node++){
		printf(" (%d) : %d - %ld - %ld, ",
				node,
				(graph->nodeIndexInc->start + node)->Id,
				(graph->nodeIndexInc->start + node)->posAtBuff,
				(graph->nodeIndexInc->start + node)->lastBatch);
	}

	printf("\n------------------------\n\n");

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

	if ( destroyBuffer( (*graph)->bufferInc) != Success ){
		ERROR("Resources of bufferInc in graph was not released successfully")
		success = False;
	}
	(*graph)->bufferInc = NULL;

	free( *graph );
	*graph = NULL;
	TRACE_OUT
	if ( success) return Success;
	return Unknown_Failure;
}


/******************************************************
 * PURPOSE : Insert a node in sub graph
 * IN      : NodeIndex *, Buffer *, uint32_t
 * OUT     : Result cause
 * COMMENTS: Local, created for simplicity
 */

OK_SUCCESS insertNodeInBuff(NodeIndex * i, Buffer * b, uint32_t nodeId){
	TRACE_IN
	ptr new_node;
	if ( i == NULL || b == NULL){
		ERROR("Null pointers for graph given")
		TRACE_OUT
		return Unknown_Failure;
	}
	if ( getListHead(i, nodeId) == -1){
		LOG("Node was not inside nodeIndexOut")
		if ( (new_node = allocNewNode(b) ) == -1 ){
			ERROR("Allocation in buffer failed");
			TRACE_OUT
			return Unknown_Failure;
		}else{
			LOG("Allocation in buffer was successful")
			if ( insertNode(i, nodeId, new_node, True) == Memory_Failure){
				ERROR("Unable to insert node in NodeIndex. Revert last insertion in buffer")
				freeLastNode(b);
				TRACE_OUT
				return Memory_Failure;
			}
		}
	}else{
		LOG("Node exists in graph")
		TRACE_OUT
		return Request_data_found;
	}
	return Success;
}

/******************************************************
 * PURPOSE : Insert an edge in sub graph
 * IN      : NodeIndex *, Buffer *, uin32_t nodeId
 * OUT     : Result cause
 * COMMENTS: Local, created for simplicity
 */

OK_SUCCESS insertEdgeInBuff(NodeIndex * index, Buffer * buffer, uint32_t source, uint32_t target, Boolean *force){
	TRACE_IN
	ptr edgeList_p;
	OK_SUCCESS ret;
	uint32_t serial;
	LOG("Get positions in buffer. Check that graph is ok. If not try to recover graph")
	if ( ( serial = getSerial(index, source) ) == -1){
		LOG("Insert node")
		if ( insertNodeInBuff(index, buffer, source) != Success ){
			ERROR("Failed to insert edge")
			return Unknown_Failure;
		}
		*force = True;
	}
	if ( serial == -1 ){ // --> Not existed, just inserted, so this is the last inserted
		LOG("Now inserted")
		edgeList_p = (index->start + index->end - 1 )->posAtBuff;
	}else{
		LOG("Node existed")
		if ( *force ){
			LOG("Get last batch")
			edgeList_p =  (index->start + serial)->lastBatch;
		}else{
			edgeList_p =  (index->start + serial)->posAtBuff;
		}
	}
	list_node *ln_p = getListNode(buffer, edgeList_p);
	int i;
	Boolean inside = False;

	LOG("Add edge in list if does not exist already")
	ptr nxt;
	while (1){
		for( i = 0; i < INIT; i++){
			if ( ln_p->neighbor[i] == 0xffffffff){
				LOG("Empty room exists in last batch")
				ln_p->neighbor[i] = target;
				inside = True;
				break;
			}else if ( !(*force) && ln_p->neighbor[i] == target ){
				LOG("Edge already exists in graph")
				TRACE_OUT
				return Request_data_found;
			}
		}
		if ( inside )break;
		if ( ln_p->nextListNode == -1 ){
			if ( (nxt = allocNewNode(buffer) ) != -1 ){
				ln_p = getListNode(buffer, edgeList_p);
				ln_p->nextListNode = nxt;
				LOG("Space successfully allocated")
				(index->start + serial)->lastBatch = ln_p->nextListNode;
			}else{
				ERROR("Failed to allocate space for list batch")
				TRACE_OUT
				return Memory_Failure;
			}
		}
		edgeList_p = ln_p->nextListNode;
		ln_p = getListNode(buffer, edgeList_p);
	}
	*force = True;
	TRACE_OUT
	return Success;
}

/******************************************************
 * PURPOSE : Insert a node in graph
 * IN      : Graph, nodeId
 * OUT     : Result cause
 * COMMENTS: n/a
 */
OK_SUCCESS insertNodeInGraph(Graph graph, uint32_t nodeId){
	TRACE_IN
	OK_SUCCESS ret = insertNodeInBuff(graph->nodeIndexOut,graph->bufferOut, nodeId);
	switch ( ret ){
		case Success:
			LOG("Node inserted in out")
			break;
		case Request_data_found:
			LOG("Node existed in out")
			break;
		default:
			ERROR("Failed to insert data in Out")
			TRACE_OUT
			return Unknown_Failure;
	}

	switch ( insertNodeInBuff(graph->nodeIndexInc,graph->bufferInc, nodeId) ){
		case Success:
			if ( ret == Request_data_found){
				ERROR("Possible error: Node existed in Out buffer but not in Inc")
			}
			LOG("Node inserted in inc")
			break;
		case Request_data_found:
			if ( ret == Success){
				ERROR("Possible error: Node existed in Inc buffer but not in Out")
			}
			LOG("Node existed in inc")
			break;
		default:
			ERROR("Failed to insert data in Inc")
			TRACE_OUT
			return Unknown_Failure;
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
	Boolean force = False;
	if ( getSerial(graph->nodeIndexInc, destId) == -1 )force = True;
	OK_SUCCESS ret = insertEdgeInBuff(graph->nodeIndexOut, graph->bufferOut, sourceId, destId, &force);
	switch ( ret ){
		case Success:
			if ( insertEdgeInBuff(graph->nodeIndexInc, graph->bufferInc, destId, sourceId, &force) != Success ){
				ERROR("Edge was not inserted in graph");
				TRACE_OUT
				return Unknown_Failure;
			}
			break;
		case Request_data_found:
			break;
		default:
			ERROR("Edge was not inserted in graph");
			TRACE_OUT
			return Unknown_Failure;
		}
#if DEBUG_LEVEL > 1
	printGraph(graph);
#endif
	TRACE_OUT
	return Success;
}


/***************************
 * LOCAL:
 */
struct args{
	NodeIndex *i;
	Buffer *b;
	uint32_t source;
	uint32_t target;
	Boolean theaded;
};

uint32_t visit;
pthread_mutex_t lock;


void *l_existPathInGraph(void *arg){
	TRACE_IN
	Boolean found;
	Buffer *buf = ((struct args *)arg)->b;
	NodeIndex *index = ((struct args *)arg)->i;
	uint32_t source = ((struct args *)arg)->source;
	uint32_t target = ((struct args *)arg)->target;
	Boolean threaded = ((struct args *)arg)->theaded;
	if ( source == target){
		LOG("source == target in search")
		TRACE_OUT
		return 0;
	}

	Q q;
	data_t node;
	ptr edgeList_p;
	list_node *ln_p;
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
		if ( threaded ){
			pthread_mutex_lock(&lock);
			if ( node.nodeId == visit )
				found = True;
			pthread_mutex_unlock(&lock);

		}else{
			if ( node.nodeId == target ){
				LOG("Found path")
				goto search_found;
			}
		}
		if ( ( edgeList_p = getListHead(index, node.nodeId) ) == -1 ){
			FATAL("Error in search algorithm (I). Probable software error.")
			LOG("Unsafe search results")
			continue;
		}
		ln_p = getListNode(buf, edgeList_p);
		node.step += 1;

		while(1){
			for ( i = 0; i < INIT; i++){
				if ( ln_p->neighbor[i] == -1 )break;
				node.nodeId = ln_p->neighbor[i];
				if ( Q_push(q, node) != Success){
					FATAL("Error in search algorithm (Q2). Probable software error.")
					goto search_error;
				}
			}
			if ( ln_p->nextListNode == -1)break;
			ln_p = getListNode(buf, ln_p->nextListNode);
		}
	}

	TRACE_OUT
	LOG("Path not found, normal execution")
	Q_destroy(&q);
	return -1;
search_found:
	TRACE_OUT
	Q_destroy(&q);
	return (void *)node.step;

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
 *           If thread functions fail, we fallback as single thread process
 *           main_thread is blocked, until result
 */

int existPathInGraph(Graph graph, uint32_t source, uint32_t target){
	TRACE_IN

	struct args A_out, A_inc;
	A_out.i = graph->nodeIndexOut;
	A_out.b = graph->bufferOut;
	A_out.source = source;
	A_out.target = target;
/*	A_inc.i = graph->nodeIndexInc;
	A_inc.b = graph->bufferInc;
	A_inc.source = target;
	A_inc.target = source;

	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        ERROR("Mutex init failed");
        A_out.theaded = False;
        TRACE_OUT;
        return (int)l_existPathInGraph((void *)(&A_out) );
    }
	visit = -1;
	pthread_t tid_Out, tid_Inc;
	int ret_Out, ret_Inc;
	A_out.theaded = True;
	if ( ret_Out = pthread_create(&tid_Out, NULL, &l_existPathInGraph, &A_out) ){
		ERROR("pthread_create failed\n");*/
		A_out.theaded = False;
		TRACE_OUT
		return (int)l_existPathInGraph((void *)(&A_out) );
	}
/*	A_inc.theaded = True;
	if ( ret_Inc = pthread_create(&tid_Inc, NULL, &l_existPathInGraph, &A_inc) ){
		ERROR("pthread_create failed\n");
		pthread_mutex_lock(&lock);
		visit = target;
		pthread_mutex_unlock(&lock);

	}
	pthread_mutex_destroy(&lock);
	TRACE_OUT
	return -1;
}
*/

