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
 * Common variables for thread synchronization
 */
Boolean do_exit = False;


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
	if (pthread_mutex_init(&graph->critical, NULL) != 0)
    {
        ERROR("Mutex init failed");
		goto destroy_members;
    }
	if (pthread_mutex_init(&graph->start, NULL) != 0)
    {
        ERROR("Mutex init failed");
        pthread_mutex_destroy (&graph->critical);
		goto destroy_members;
    }
	if (pthread_mutex_init(&graph->kill, NULL) != 0)
    {
        ERROR("Mutex init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
		goto destroy_members;
    }
	if (pthread_cond_init(&graph->start_up, NULL) != 0 ){
        ERROR("Conditional variable init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
		goto destroy_members;
	}
	if (pthread_cond_init(&graph->start_up, NULL) != 0 ){
        ERROR("Conditional variable init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
		goto destroy_members;
	}
	if (pthread_cond_init(&graph->kill_off, NULL) != 0 ){
        ERROR("Conditional variable init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
    	pthread_cond_destroy( &graph->start_up );
		goto destroy_members;
	}
	if (pthread_cond_init(&graph->finish, NULL) != 0 ){
        ERROR("Conditional variable init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
    	pthread_cond_destroy( &graph->start_up );
    	pthread_cond_destroy( &graph->kill_off );
		goto destroy_members;
	}
	int i, j;
	graph->assign = False;
	graph->do_exit = False;
	graph->workers_started = 0;
	for ( i = 0; i < WORKERS; i++){
		if ( pthread_create(&(graph->worker[i]), NULL, &l_searchPathInGraph, (void *)graph) ){
			ERROR("pthread_create failed\n");
			Graph_killWorkers(graph);
			for ( j =  0; j < i; j++)
				pthread_join( graph->worker[i], NULL);
			pthread_mutex_destroy (&graph->critical);
			pthread_mutex_destroy (&graph->start);
	        pthread_mutex_destroy (&graph->kill);
	    	pthread_cond_destroy( &graph->start_up );
	    	pthread_cond_destroy( &graph->kill_off );
			goto destroy_members;
		}
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
	Graph_killWorkers(*graph);
	int i;
	for ( i = 0; i < WORKERS; i++) {
		pthread_join( (*graph)->worker[i], NULL);
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
	pthread_mutex_destroy( &(*graph)->critical );
	pthread_mutex_destroy( &(*graph)->start );
	pthread_mutex_destroy( &(*graph)->kill );
	pthread_cond_destroy( &(*graph)->start_up );
	pthread_cond_destroy( &(*graph)->kill_off);
	pthread_cond_destroy( &(*graph)->finish);
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


/***************************
 * LOCAL:
 */
typedef struct args{
	uint32_t source;
	uint32_t target;
}Arguments;

Arguments out, inc;

/******************************************************
 * FUNCTION: Graph_enterCritical
 * PURPOSE : Mutex is locked
 * IN      : Graph
 * OUT     : n/a
 */
void Graph_enterCritical(Graph graph){
	TRACE_IN
	pthread_mutex_lock(&graph->critical);
	TRACE_OUT
}

/******************************************************
 * FUNCTION: Graph_enterCritical
 * PURPOSE : Mutex is locked
 * IN      : Graph
 * OUT     : n/a
 */
void Graph_leaveCritical(Graph graph){
	TRACE_IN
	pthread_mutex_unlock(&graph->critical);
	TRACE_OUT
}

/******************************************************
 * FUNCTION: Graph_killWorkers
 * PURPOSE : kills all workers if their task is done
 * IN      : Graph
 * OUT     : n/a
 */
void Graph_killWorkers(Graph graph){
	TRACE_IN
	pthread_mutex_lock(&graph->kill);
	while (graph->workers_started < 2 )
		pthread_cond_wait(&graph->kill_off, &graph->kill);
	pthread_mutex_unlock(&graph->kill);
	pthread_mutex_lock(&graph->start);
	graph->do_exit = True;
	pthread_cond_broadcast(&graph->start_up);
	pthread_mutex_unlock(&graph->start);
	TRACE_OUT
}


/******************************************************
 * FUNCTION: Graph_workersWaitForTask
 * PURPOSE : workers wait until a new task is been assigned to them
 * IN      : Graph, pointer to arguments of task
 * OUT     : n/a
 */
void Graph_workersWaitForTask(Graph graph, int i, Arguments *args){
	TRACE_IN
	Boolean start = False;
	while ( !start ){
		pthread_mutex_lock(&graph->start);
		pthread_mutex_lock(&graph->kill);
		if ( graph->workers_started < WORKERS)graph->workers_started ++;
		pthread_mutex_unlock(&graph->kill);
		pthread_cond_signal(&graph->kill_off);
		pthread_cond_wait(&graph->start_up, &graph->start);
		start = True;
		if ( i == 0){
			(*args).source = out.source;
			(*args).target = out.target;
		}else{
			(*args).source = inc.source;
			(*args).target = inc.target;
		}
		pthread_mutex_unlock(&graph->start);
	}
	TRACE_OUT
}

#define OUTGOING  0
#define INCOMING  1
#define UNDEFINED 0xFF


int retValue;

/******************************************************
 * FUNCTION: Graph_returnVal
 * PURPOSE : Sets return value, informs master thread, destroy Q
 * IN      : graph, ret, pointer to Q
 * OUT     : None
 * COMMENTS: --
 */

void Graph_setReturnVal(Graph graph, int ret, Q *q){
	TRACE_IN;
printf("Graph_setReturnVal : ret = %d\n", ret);
	Graph_enterCritical(graph);
	if ( finished == 0 )retValue = ret;
printf("Set!!\n");
	finished++;
	Graph_leaveCritical(graph);
	Q_destroy(q);
	if ( finished == 2 )
		pthread_cond_signal(&graph->finish);
	TRACE_OUT
}

/******************************************************
 * FUNCTION: Graph_returnVal
 * PURPOSE : Sets return value, informs master thread, destroy Q
 * IN      : graph, ret, pointer to Q
 * OUT     : None
 * COMMENTS: --
 */

Boolean Graph_hasReturnVal(Graph graph, Q *q){
	TRACE_IN;
	Boolean result = False;
	Graph_enterCritical(graph);
	if ( retValue != -2 )finished++;
	if ( finished == 2 ){
		result = True;
		Q_destroy(q);
	}
	Graph_leaveCritical(graph);
	if ( finished == 2 )
		pthread_cond_signal(&graph->finish);
	TRACE_OUT
	return result;
}

/******************************************************
 * FUNCTION: Graph_getReturnVal
 * PURPOSE : Sets return value, informs master thread, destroy Q
 * IN      : graph, ret, pointer to Q
 * OUT     : None
 * COMMENTS: --
 */

int Graph_getReturnVal(Graph graph){
	TRACE_IN;
	int ret;
	Graph_enterCritical(graph);
	while ( graph->masterWaitsForResult ){
		pthread_cond_wait(&graph->finish, &graph->critical);
	}
	Graph_leaveCritical(graph);
	ret = retValue;
	retValue = -2;
	free(V);
	TRACE_OUT
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
	int thID;
	Graph graph = (Graph)arg;
	Graph_enterCritical(graph);
	if ( graph->assign == False){
		thID = 0;
		graph->assign = True;
	}else{
		thID = 1;
	}
	Graph_leaveCritical(graph);
	printf("Slave %d started working.\n", thID);
	Arguments worker;
	Q q = NULL;
	data_t node;
	ptr edgeList_p;
	list_node *ln_p;
	int i, current_step;
	Boolean work;
	while(1)
	{
		work = True;
		Graph_workersWaitForTask(graph, thID, &worker);
		if ( graph->do_exit )break;
		printf("Thread %d starts task Q %d %d\n", thID, worker.source, worker.target );
		if ( ( q = Q_init() ) == NULL){
			ERROR("q_init failed")
			TRACE_OUT
			Graph_setReturnVal(graph, -1, NULL);
		}
		node.nodeId = worker.source;
		node.step = 0;
		Graph_enterCritical(graph);
		if ( V[node.nodeId].length != -1){
			LOG("Node found in visited graph")
			Graph_setReturnVal(graph, V[node.nodeId].length, &q);
			Graph_leaveCritical(graph);
			work = False;
			continue;
		}else{
			V[node.nodeId].direction = thID;
			V[node.nodeId].length = node.step;
			Graph_leaveCritical(graph);
			if ( Q_push(q, node) != Success ){
				ERROR("q_init failed")
				Graph_setReturnVal(graph, -1, &q);
				work = False;
				continue;
			}
		}
		while( !Graph_hasReturnVal(graph, &q) && !Q_isEmpty(q) ){
			if ( Q_pop(q, &node) != Success ){
				FATAL("Error in search algorithm (Q1). Probable software error.")
				Graph_setReturnVal(graph, -1, &q);
				break;
			}
			node.step += 1;
printf("Thread %d popped node=%d (%d) from q\n", thID, node.nodeId, node.step);
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
printf("Thread %d : edgeList_p=%d\n", thID, edgeList_p);
			if ( thID == 0)
				ln_p = getListNode(graph->bufferOut, edgeList_p);
			else
				ln_p = getListNode(graph->bufferInc, edgeList_p);
			/////////////////////////////
			while(1){
				for ( i = 0; i < INIT; i++){
					if ( ln_p->neighbor[i] == -1 )break;
printf("Thread %d : Neighb=%d\n", thID, ln_p->neighbor[i]);

					Graph_enterCritical(graph);
					if ( V[ln_p->neighbor[i]].length != -1){
						LOG("Node found in visited graph")
printf("Thread %d node found in visited graph\n", thID);
printf("Thread %d found visited: V[%d]=%d.%d\n", thID, ln_p->neighbor[i], V[ln_p->neighbor[i]].length, V[ln_p->neighbor[i]].direction  );
						if ( V[ln_p->neighbor[i]].direction != thID ){
							LOG("Path found")
printf("Thread %d found path. Node(%d.%d) was visited by other thread.\n", thID, V[ln_p->neighbor[i]].length, V[ln_p->neighbor[i]].direction);
							Graph_leaveCritical(graph);
							Graph_setReturnVal(graph, V[ln_p->neighbor[i]].length+node.step, &q);
							work = False;
							break;
						}else{
							LOG("Node already visited\n")
							Graph_leaveCritical(graph);
							continue;
						}
					}else if (ln_p->neighbor[i] == worker.target ){
						Graph_leaveCritical(graph);
						Graph_setReturnVal(graph, node.step, &q);
					}else{
						V[ln_p->neighbor[i]].direction = thID;
						V[ln_p->neighbor[i]].length = node.step;
						node.nodeId = ln_p->neighbor[i];
printf("Thread %d sets  V[%d]=%d. %d\n", thID, ln_p->neighbor[i], V[ln_p->neighbor[i]].length, V[ln_p->neighbor[i]].direction);
						Graph_leaveCritical(graph);
						if ( Q_push(q, node) != Success ){
							ERROR("q_init failed")
							Graph_setReturnVal(graph, -1, &q);
							work = False;
							break;
						}
					}
				}
				if ( !work )break;
printf("Thread %d : nextListNode = %d\n", thID, ln_p->nextListNode);
				if ( ln_p->nextListNode == -1)break;
				if ( thID == 0 )
					ln_p = getListNode(graph->bufferOut, ln_p->nextListNode);
				else
					ln_p = getListNode(graph->bufferInc, ln_p->nextListNode);
			}
		}
		if ( !Graph_hasReturnVal(graph, &q) )
		{
			LOG("Path was not found")
			Graph_setReturnVal(graph, -1, &q);
		}
	}
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
	if ( ( V = malloc(max_size*sizeof(visitFlags) ) ) == NULL ){
		ERROR("Failed to allocate space V buffer")
		TRACE_OUT
		return Memory_Failure;
	}
	memset(V, 0xff, max_size*sizeof(visitFlags) );
	int i;
	for ( i = 0; i < max_size; i++){
		printf("V[%d]=%d.%d\n", i, V[i].length, V[i].direction);
	}
	pthread_mutex_lock(&graph->start);
	graph->masterWaitsForResult = True;
	finished = 0;
	retValue = -2;
	out.source = source;
	out.target = target;
	inc.source = target;
	inc.target = source;
	pthread_cond_broadcast(&graph->start_up);
	pthread_mutex_unlock(&graph->start);
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

int existPathInGraph(Graph graph, uint32_t source, uint32_t target){
	TRACE_IN
	printGraph(graph);
	if ( source == target){
		LOG("source == target in search")
		TRACE_OUT
		return 0;
	}
	int ret;
	if ( Graph_startUpTask(graph, source, target) == Memory_Failure){
		ERROR("Task failed to start due to memory issues")
		TRACE_OUT
		return -1;
	}
	ret = Graph_getReturnVal(graph);
	char c;
	printf("Ret = %d\n", ret);
	scanf("%c", &c);
	TRACE_OUT
	return ret;
}

