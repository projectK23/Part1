/**
 * Standard libs
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>


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
	pthread_mutex_init(&graph->watch_result, NULL );
	pthread_mutex_init(&graph->watch_visit, NULL );
	graph->V = NULL;
	graph->max = 0;
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
	printf("graph = 0x%x\n", graph);
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
	for ( node = 0; node < graph->nodeIndexOut->size; node++){
		printf(" (%d) : %ld - %ld, ",
				node,
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
	for ( node = 0; node < graph->nodeIndexInc->size; node++){
		printf(" (%d) : %ld - %ld, ",
				node,
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
	pthread_mutex_destroy(&(*graph)->watch_visit);
	pthread_mutex_destroy(&(*graph)->watch_result);
	if ( (*graph)->V != NULL )free ( (*graph)->V );
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
			if ( insertNode(i, nodeId, new_node) == Memory_Failure){
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
	LOG("Get positions in buffer")
	if ( ( getListHead(index, source) ) == -1){
		LOG("Insert node")

		if ( insertNodeInBuff(index, buffer, source) != Success ){
			ERROR("Failed to insert edge")
			return Unknown_Failure;
		}
		*force = True;
	}
	if ( *force){
		edgeList_p = getListTail(index, source);
	}else{
		edgeList_p = getListHead(index, source);
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
				LOG("Space successfully allocated")
				ln_p = getListNode(buffer, edgeList_p);
				ln_p->nextListNode = nxt;
				(index->start + source)->lastBatch = ln_p->nextListNode;
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
	if ( ( getListHead(graph->nodeIndexInc, destId) ) == -1)force = True;
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


/******************************************************
 * FUNCTION: Graph_enterCritical
 * PURPOSE : Mutex is locked
 * IN      : Graph
 * OUT     : n/a
 */
void Graph_enterCritical(Graph graph){
	TRACE_IN
	pthread_mutex_lock(&graph->watch_visit);
	TRACE_OUT
}

/******************************************************
 * FUNCTION: Graph_leaveCritical
 * PURPOSE : Mutex is locked
 * IN      : Graph
 * OUT     : n/a
 */
void Graph_leaveCritical(Graph graph){
	TRACE_IN
	pthread_mutex_unlock(&graph->watch_visit);
	TRACE_OUT
}

/******************************************************
 * FUNCTION: Graph_enterCritical
 * PURPOSE : Mutex is locked
 * IN      : Graph
 * OUT     : n/a
 */
void Graph_enterResult(Graph graph){
	TRACE_IN
	pthread_mutex_lock(&graph->watch_result);
	TRACE_OUT
}

/******************************************************
 * FUNCTION: Graph_leaveCritical
 * PURPOSE : Mutex is locked
 * IN      : Graph
 * OUT     : n/a
 */
void Graph_leaveResult(Graph graph){
	TRACE_IN
	pthread_mutex_unlock(&graph->watch_result);
	TRACE_OUT
}

/******************************************************
 * FUNCTION: Graph_returnVal
 * PURPOSE : Sets return value, informs master thread, destroy Q
 * IN      : graph, ret, pointer to Q
 * OUT     : None
 * COMMENTS: --
 */

Boolean Graph_setReturnValue(Graph graph, int ret, Q *q){
	TRACE_IN;
	LOG("Before Graph_enterCritical")
	Graph_enterResult(graph);
	if ( graph->result == -2 )
		graph->result = ret;
	Graph_leaveResult(graph);
	Q_destroy(q);
	TRACE_OUT
	return True;
}

/******************************************************
 * FUNCTION: Graph_returnVal
 * PURPOSE : Sets return value, informs master thread, destroy Q
 * IN      : graph, ret, pointer to Q
 * OUT     : None
 * COMMENTS: --
 */

Boolean Graph_hasReturnValue(Graph graph, Q *q, int thID){
	TRACE_IN;
	Boolean has_result = False;
	LOG("Before Graph_enterCritical")
	Graph_enterResult(graph);
	if ( graph->result != -2 )has_result = True;
	Graph_leaveResult(graph);
	if ( has_result ){
		Q_destroy(q);
	}
	TRACE_OUT
	return has_result;
}

/******************************************************
 * FUNCTION: Graph_getReturnVal
 * PURPOSE : Sets return value, informs master thread, destroy Q
 * IN      : graph, ret, pointer to Q
 * OUT     : None
 * COMMENTS: --
 */

int Graph_getReturnValue(Graph graph){
	TRACE_IN;
	int ret;
	LOG("Before Graph_enterCritical")
	Graph_enterResult(graph);
	ret = graph->result;
	Graph_leaveResult(graph);
	free(graph->V);
	TRACE_OUT
	return ret;
}

/******************************************************
 * LOCAL FUNCTIONS
 */
Boolean initQueue(Graph graph, Q *q){
	if ( ( *q = Q_init() ) == NULL){
		ERROR("q_init failed")
		return !Graph_setReturnValue(graph, -1, NULL);
	}
	return True;
}

Boolean setNodeVisited(Graph graph, data_t node, Q *q, int thID){
	Boolean ret;
	LOG("Before Graph_enterCritical")
	Graph_enterCritical(graph);
	if ( graph->V[node.nodeId].length != -1){
		LOG("Node found in visited graph")
		if ( graph->V[node.nodeId].direction != thID){
			Graph_setReturnValue(graph, graph->V[node.nodeId].length+node.step-1, q);
			Graph_leaveCritical(graph);
			ret = False;
		}else{
			Graph_leaveCritical(graph);
			ret = True;
		}
	}else{
		graph->V[node.nodeId].direction = thID;
		graph->V[node.nodeId].length = node.step;
		Graph_leaveCritical(graph);
		if ( Q_push(*q, node) != Success ){
			ERROR("q_init failed")
			Graph_setReturnValue(graph, -1, q);
			ret = False;
		}else ret = True;
	}
	return ret;
}


/******************************************************
 * FUNCTION: l_searchPathInGraph
 * PURPOSE : Search path
 * IN      : void *args
 * OUT     : Result cause, not handled
 * COMMENTS: Path is not hold somewhere.
 *           thread function for b-directional search
 */

void *l_searchPathInGraph(void *arg){
	TRACE_IN
	Least_path_req worker = *(Least_path_req*)arg;
	Graph graph = worker.graph;
	uint32_t source = worker.source;
	uint32_t target = worker.target;
	int thID = worker.thID;

	Q q = NULL;
	data_t node;
	ptr edgeList_p;
	list_node *ln_p;
	int i, current_step;
	Boolean work = True;


	if ( !initQueue(graph, &q)  )goto exit_thread;
	node.nodeId = worker.source;
	node.step = 0;
	if ( !setNodeVisited(graph, node, &q, thID))goto exit_thread;

	while( !Graph_hasReturnValue(graph, &q, thID) && !Q_isEmpty(q) ){
		if ( Q_pop(q, &node) != Success ){
			FATAL("Error in search algorithm (Q1). Probable software error.")
			Graph_setReturnValue(graph, -1, &q);
			break;
		}
		node.step += 1;
		if ( thID == 0 ){
			if ( ( edgeList_p = getListHead(graph->nodeIndexOut, node.nodeId) ) == -1 )
			{
				LOG("Released paths")
				continue;
			}
		}else{
			if ( ( edgeList_p = getListHead(graph->nodeIndexInc, node.nodeId) ) == -1 )
			{
				LOG("Released paths")
				continue;
			}
		}
		if ( thID == 0)
			ln_p = getListNode(graph->bufferOut, edgeList_p);
		else
			ln_p = getListNode(graph->bufferInc, edgeList_p);
		while(work){
			for ( i = 0; i < INIT; i++){
				if ( ln_p->neighbor[i] == -1 )break;
				if (ln_p->neighbor[i] == worker.target ){
					work = !Graph_setReturnValue(graph, node.step, &q);
					break;
				}
				node.nodeId = ln_p->neighbor[i];
				if ( !(work=setNodeVisited(graph, node, &q, thID) ) )break;
			}
			if ( !work )break;
			if ( ln_p->nextListNode == -1)break;
			if ( thID == 0 )
				ln_p = getListNode(graph->bufferOut, ln_p->nextListNode);
			else
				ln_p = getListNode(graph->bufferInc, ln_p->nextListNode);
		}
	}
	Graph_setReturnValue(graph, -1, &q);
exit_thread:
	LOG("Worker exited")
	TRACE_OUT
	pthread_exit(NULL);
}
/******************************************************
 * PURPOSE : Start_up task
 * IN      : Graph, source, target
 * OUT     : void
 * COMMENTS: n/a
 */
OK_SUCCESS Graph_startUpTask(Graph graph, uint32_t source, uint32_t target){
	TRACE_IN;
	uint32_t max_size = graph->nodeIndexOut->size > graph->nodeIndexInc->size?
			graph->nodeIndexOut->size : graph->nodeIndexInc->size;
	if ( ( graph->V = malloc(max_size*sizeof(visitFlags) ) ) == NULL ){
		ERROR("Failed to allocate space V buffer")
		TRACE_OUT
		return Memory_Failure;
	}
	memset(graph->V, 0xff, max_size*sizeof(visitFlags) );
	Least_path_req out, inc;
	out.graph = graph;
	out.source = source;
	out.target = target;
	out.thID = 0;
	inc.graph = graph;
	inc.source = target;
	inc.target = source;
	inc.thID = 1;
	Boolean ret1 = True, ret2 = True;
	graph->result = -2;
	if ( pthread_create(&graph->worker[0], NULL, l_searchPathInGraph, (void *)&out) != 0){
		printf("Worker 0 not created");
		ret1 =  False;
	}
	if ( pthread_create(&graph->worker[1], NULL, l_searchPathInGraph, (void *)&inc) != 0 ){
		printf("Worker 1 not created");
		ret2 =  False;
	}
	if ( !ret1 && !ret2 ){
		ERROR("Treads not created");
		graph->result = -1;
		return Threads_not_created;
	}
	if ( ret1 )pthread_join(graph->worker[0], NULL);
//printf("Thread 1 joined");
	if ( ret2 )pthread_join(graph->worker[1], NULL);
//printf("Thread 2 joined");
	TRACE_OUT
	return Success;
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

int existPathInGraphThreaded(Graph graph, uint32_t source, uint32_t target){
	TRACE_IN
#if DEBUG_LEVEL > 1
printGraph(graph);
#endif
	if ( source == target){
		LOG("source == target in search")
		TRACE_OUT
		return 0;
	}
	int ret;
	if ( Graph_startUpTask(graph, source, target) != Success){
		ERROR("Task failed due to unknown reasons")
		TRACE_OUT
		return -1;
	}
	ret = Graph_getReturnValue(graph);
#if DEBUG_LEVEL > 1
	printf("Ret = %d\n", ret);
	char c;
//	scanf("%c", &c);
#endif

	TRACE_OUT
	return ret;
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
#if DEBUG_LEVEL > 1
printGraph(graph);
#endif
	if ( source == target){
		LOG("source == target in search")
		TRACE_OUT
		return 0;
	}
	int ret = -1;
	data_t node, nodeInc, nodeOut;
	ptr edgeListInc_p, edgeListOut_p, l_edgeListInc_p, l_edgeListOut_p;
	list_node *lnInc_p, *lnOut_p;
	int num_batch_Inc, num_batch_Out;
	uint32_t max_size = graph->nodeIndexOut->size > graph->nodeIndexInc->size?
			graph->nodeIndexOut->size : graph->nodeIndexInc->size;
	if ( graph->max  <  max_size){
		if ( ( graph->V = malloc(max_size*sizeof(visitFlags) ) ) == NULL ){
			ERROR("Failed to allocate space V buffer")
			TRACE_OUT
			return Memory_Failure;
		}
		graph->max = max_size;
	}
	memset(graph->V, 0xff, (graph->max)*sizeof(visitFlags) );
	Q qinc, qout;
	if ( ( qinc = Q_init() ) == NULL){
		ERROR("q_init failed")
		return -1;
	}
	if ( ( qout = Q_init() ) == NULL){
		ERROR("q_init failed")
		return -1;
	}
	node.step = 0;
	node.nodeId = source;
	graph->V[node.nodeId].length = 0;
	graph->V[node.nodeId].direction = 0;
	if ( Q_push(qout, node) != Success){
		ERROR("Q_push failed")
		goto return_point;
	}
	node.nodeId = target;
	graph->V[node.nodeId].length = 0;
	graph->V[node.nodeId].direction = 1;
	if ( Q_push(qinc, node) != Success ){
		ERROR("Q_push failed")
		goto return_point;
	}
	Boolean used_inc = True, used_out = True, found = False;
	int i;
	while( !found ){
		if ( used_inc ){
			if ( Q_isEmpty(qinc) )break;
			if ( Q_pop(qinc, &nodeInc) != Success ){
				FATAL("Error in search algorithm (Q1). Probable software error.")
				break;
			}
			nodeInc.step += 1;
			edgeListInc_p = getListHead(graph->nodeIndexInc, nodeInc.nodeId);
			num_batch_Inc = 0;
			l_edgeListInc_p = edgeListInc_p;
			while (  l_edgeListInc_p != -1 ){
				lnInc_p = getListNode(graph->bufferInc, l_edgeListInc_p);
				l_edgeListInc_p = lnInc_p->nextListNode;
				num_batch_Inc ++ ;
			}
			used_inc = False;
		}
		if ( used_out ){
			if ( Q_isEmpty(qout) ){
				break;
			}
			if ( Q_pop(qout, &nodeOut) != Success ){
				FATAL("Error in search algorithm (Q1). Probable software error.")
				break;
			}
			nodeOut.step += 1;
			edgeListOut_p = getListHead(graph->nodeIndexOut, nodeOut.nodeId);
			num_batch_Out = 0;
			l_edgeListOut_p = edgeListOut_p;
			while (  l_edgeListOut_p != -1 ){
				lnOut_p = getListNode(graph->bufferOut, l_edgeListOut_p);
				l_edgeListOut_p = lnOut_p->nextListNode;
				num_batch_Out ++ ;
			}
			used_out = False;
		}

		if ( num_batch_Inc < num_batch_Out){
			used_inc = True;
			while( !found && edgeListInc_p != -1 ){
				lnInc_p = getListNode(graph->bufferInc, edgeListInc_p);
				for ( i = 0; i < INIT; i++){
					if ( lnInc_p->neighbor[i] == -1 )break;
					if (lnInc_p->neighbor[i] == source ){
						ret = nodeInc.step;
						found = True;
						break;
					}
					nodeInc.nodeId = lnInc_p->neighbor[i];
					if ( graph->V[nodeInc.nodeId].direction == 0 ){
						ret = graph->V[nodeInc.nodeId].length + nodeInc.step;
						found = True;
						break;
					}else if ( graph->V[nodeInc.nodeId].direction == 1 ){
						continue;
					}else{
						graph->V[nodeInc.nodeId].length = nodeInc.step;
						graph->V[nodeInc.nodeId].direction = 1;
						if ( Q_push(qinc, nodeInc) != Success ){
							ERROR("Q_push failed")
							goto return_point;
						}
					}
				}
				edgeListInc_p = lnInc_p->nextListNode;
			}
		}else{
			used_out = True;

			while( !found && edgeListOut_p != -1 ){
				lnOut_p = getListNode(graph->bufferOut, edgeListOut_p);
				for ( i = 0; i < INIT; i++){
					if ( lnOut_p->neighbor[i] == -1 ){
						break;
					}
					if ( lnOut_p->neighbor[i] == target){
						ret = nodeOut.step;
						found = True;
						break;
					}
					nodeOut.nodeId = lnOut_p->neighbor[i];
					if ( graph->V[nodeOut.nodeId].direction == 1 ){
						ret = graph->V[nodeOut.nodeId].length + nodeOut.step;
						found = True;
						break;
					}else if ( graph->V[nodeOut.nodeId].direction == 0 ){
						continue;
					}else{
						graph->V[nodeOut.nodeId].length = nodeOut.step;
						graph->V[nodeOut.nodeId].direction = 0;
						if ( Q_push(qout, nodeOut) != Success ){
							ERROR("Q_push failed")
							goto return_point;
						}
					}
				}
				edgeListOut_p = lnOut_p->nextListNode;
			}

		}
	}
return_point:
#if DEBUG_LEVEL > 1
	printf("Ret = %d\n", ret);
	char c;
	scanf("%c", &c);
#endif

	Q_destroy(&qinc);
	Q_destroy(&qout);
	TRACE_OUT
	return ret;
}
