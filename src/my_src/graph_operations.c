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
	graph->state = START;
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
	if (pthread_mutex_init(&graph->finish, NULL) != 0)
    {
        ERROR("Mutex init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
		goto destroy_members;
    }
	if (pthread_mutex_init(&graph->check_state, NULL) != 0)
    {
        ERROR("Mutex init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
        pthread_mutex_destroy (&graph->finish);
		goto destroy_members;
    }
	if (pthread_cond_init(&graph->start_up, NULL) != 0 ){
        ERROR("Conditional variable init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
        pthread_mutex_destroy (&graph->finish);
		goto destroy_members;
	}
	if (pthread_cond_init(&graph->kill_off, NULL) != 0 ){
        ERROR("Conditional variable init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
        pthread_mutex_destroy (&graph->finish);
        pthread_mutex_destroy (&graph->check_state);
    	pthread_cond_destroy( &graph->start_up );
		goto destroy_members;
	}
	if (pthread_cond_init(&graph->finish_out, NULL) != 0 ){
        ERROR("Conditional variable init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
        pthread_mutex_destroy (&graph->finish);
        pthread_mutex_destroy (&graph->check_state);
        pthread_cond_destroy( &graph->start_up );
    	pthread_cond_destroy( &graph->kill_off );
		goto destroy_members;
	}
	if (pthread_cond_init(&graph->ready, NULL) != 0 ){
        ERROR("Conditional variable init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
        pthread_mutex_destroy (&graph->finish);
        pthread_mutex_destroy (&graph->check_state);
    	pthread_cond_destroy( &graph->start_up );
    	pthread_cond_destroy( &graph->kill_off );
    	pthread_cond_destroy(&graph->finish_out) ;
		goto destroy_members;
	}
	if (pthread_cond_init(&graph->change_state, NULL) != 0 ){
        ERROR("Conditional variable init failed");
        pthread_mutex_destroy (&graph->critical);
        pthread_mutex_destroy (&graph->start);
        pthread_mutex_destroy (&graph->kill);
        pthread_mutex_destroy (&graph->finish);
        pthread_mutex_destroy (&graph->check_state);
        pthread_cond_destroy( &graph->start_up );
    	pthread_cond_destroy( &graph->kill_off );
    	pthread_cond_destroy(&graph->finish_out) ;
    	pthread_cond_destroy(&graph->ready) ;
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
	        pthread_mutex_destroy (&graph->finish);
	        pthread_mutex_destroy (&graph->check_state);
	        pthread_cond_destroy( &graph->start_up );
	    	pthread_cond_destroy( &graph->start_up );
	    	pthread_cond_destroy( &graph->kill_off );
	    	pthread_cond_destroy( &graph->finish_out);
	    	pthread_cond_destroy( &graph->ready);
			goto destroy_members;
		}
	}
	Graph_changeState(graph, WAIT);
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
 * PURPOSE : Sets state in graph
 * IN      : graph, state
 * OUT     : n/a
 * COMMENTS: n/a
 */
void Graph_changeState(Graph graph, Graph_state st){
	TRACE_IN
	pthread_mutex_lock(&graph->check_state);
	if ( graph->state != st){
		graph->state = st;
		pthread_cond_broadcast(&graph->change_state);
	}else{
		LOG("We are in the same state");
	}
	pthread_mutex_unlock(&graph->check_state);
	TRACE_OUT
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
	Graph_enterCritical(*graph);
	while( (*graph)->state == WORK )
	Graph_leaveCritical(*graph);
	if ( )
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
	pthread_mutex_destroy( &(*graph)->finish );
	pthread_cond_destroy( &(*graph)->start_up );
	pthread_cond_destroy( &(*graph)->kill_off);
	pthread_cond_destroy( &(*graph)->finish_out);
	pthread_cond_destroy( &graph->ready);
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
	while (graph->workers_started <  )
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
void Graph_workersWaitForTask(Graph graph, int i, Last_path_req *args){
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
			(*args).source = graph->out.source;
			(*args).target = graph->out.target;
		}else{
			(*args).source = graph->inc.source;
			(*args).target = graph->inc.target;
		}
		pthread_mutex_unlock(&graph->start);
	}
	TRACE_OUT
}


/******************************************************
 * FUNCTION: Graph_returnVal
 * PURPOSE : Sets return value, informs master thread, destroy Q
 * IN      : graph, ret, pointer to Q
 * OUT     : None
 * COMMENTS: --
 */

Boolean Graph_setReturnValue(Graph graph, int ret, Q *q, int thID){
	TRACE_IN;
	pthread_mutex_lock(&graph->finish);
	if ( graph->workers_finished == 0 )graph->result = ret;
	graph->workers_finished++;
//printf("******** KER set (%d): graph->workers_finished=%d\n", thID, graph->workers_finished);
	if ( graph->workers_finished == 2 ){
		graph->masterWaitsForResult = False;
		pthread_mutex_unlock(&graph->finish);
		pthread_cond_signal(&graph->finish_out);
	}else pthread_mutex_unlock(&graph->finish);
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
	pthread_mutex_lock(&graph->finish);
	if ( graph->result != -2 )graph->workers_finished++;
//	printf("******** KER has (%d): graph->workers_finished=%d\n", thID, graph->workers_finished);

	if ( graph->workers_finished == 2 ){
		graph->masterWaitsForResult = False;
		pthread_mutex_unlock(&graph->finish);
		pthread_cond_signal(&graph->finish_out);
		Q_destroy(q);
		has_result = True;
	}else pthread_mutex_unlock(&graph->finish);
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
	pthread_mutex_lock(&graph->finish);
	while ( graph->masterWaitsForResult ){
		pthread_cond_wait(&graph->finish_out, &graph->finish);
	}
	pthread_mutex_unlock(&graph->finish);
	ret = graph->result;
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
		return !Graph_setReturnValue(graph, -1, NULL, 3);
	}
	return True;
}

Boolean isNodeVisited(Graph graph, data_t node, Q *q, int thID){
	Boolean ret;
	Graph_enterCritical(graph);
	if ( graph->V[node.nodeId].length != -1){
		LOG("Node found in visited graph")
		if ( graph->V[node.nodeId].direction != thID){
			Graph_setReturnValue(graph, graph->V[node.nodeId].length, q, thID);
			ret = True;
		}else ret = False;
	}else ret = False;
	Graph_leaveCritical(graph);
	return ret;
}

Boolean setNodeVisited(Graph graph, data_t node, Q *q, int thID){
	Boolean ret;
	Graph_enterCritical(graph);
	if ( graph->V[node.nodeId].length != -1){
		LOG("Node found in visited graph")
		if ( graph->V[node.nodeId].direction != thID){
			Graph_setReturnValue(graph, graph->V[node.nodeId].length+node.step, q, thID);
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
			Graph_setReturnValue(graph, -1, q, thID);
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
	Last_path_req worker;
	Q q = NULL;
	data_t node;
	ptr edgeList_p;
	list_node *ln_p;
	int i, current_step;
	Boolean work;
	while(1)
	{
		printf("Thread with ID : %d at start\n", thID);
		work = True;
		Graph_workersWaitForTask(graph, thID, &worker);
		if ( graph->do_exit )break;
		printf("Thread %d starts task Q %d %d\n", thID, worker.source, worker.target );
		if ( !initQueue(graph, &q)  )continue;

		node.nodeId = worker.source;
		node.step = 0;

		if ( !setNodeVisited(graph, node, &q, thID))continue;

		while( (work = !Graph_hasReturnValue(graph, &q, thID)) && !Q_isEmpty(q) ){
			if ( Q_pop(q, &node) != Success ){
				FATAL("Error in search algorithm (Q1). Probable software error.")
				work = Graph_setReturnValue(graph, -1, &q, thID);
				break;
			}
			node.step += 1;
//printf("Thread %d popped node=%d (%d) from q\n", thID, node.nodeId, node.step);
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
//printf("Thread %d : edgeList_p=%d\n", thID, edgeList_p);
			if ( thID == 0)
				ln_p = getListNode(graph->bufferOut, edgeList_p);
			else
				ln_p = getListNode(graph->bufferInc, edgeList_p);
			/////////////////////////////
			while(work){
				for ( i = 0; i < INIT; i++){
					if ( ln_p->neighbor[i] == -1 )break;
//printf("Thread %d : Neighb=%d\n", thID, ln_p->neighbor[i]);
					if (ln_p->neighbor[i] == worker.target ){
						work = !(Graph_setReturnValue(graph, node.step, &q, thID) );
						break;
					}
					node.nodeId = ln_p->neighbor[i];
					if ( !(work=setNodeVisited(graph, node, &q, thID) ) )break;
				}
				if ( !work )break;
//printf("Thread %d : nextListNode = %d\n", thID, ln_p->nextListNode);
				if ( ln_p->nextListNode == -1)break;
				if ( thID == 0 )
					ln_p = getListNode(graph->bufferOut, ln_p->nextListNode);
				else
					ln_p = getListNode(graph->bufferInc, ln_p->nextListNode);
			}
			if(!work)break;
		}
		if (!work)continue;
		if ( !Graph_hasReturnValue(graph, &q, thID) )
		{
			LOG("Path was not found")
			Graph_setReturnValue(graph, -1, &q, thID);
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
	if ( ( graph->V = malloc(max_size*sizeof(visitFlags) ) ) == NULL ){
		ERROR("Failed to allocate space V buffer")
		TRACE_OUT
		return Memory_Failure;
	}
	memset(graph->V, 0xff, max_size*sizeof(visitFlags) );
	int i;
/*	for ( i = 0; i < max_size; i++){
		printf("V[%d]=%d.%d\n", i, graph->V[i].length, graph->V[i].direction);
	}
*/	pthread_mutex_lock(&graph->start);
	graph->masterWaitsForResult = True;
	while( graph->workers_finished ){
		pthread_cond_wait(&graph->ready);
	}
	graph->result = -2;
	graph->out.source = source;
	graph->out.target = target;
	graph->inc.source = target;
	graph->inc.target = source;
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
//printGraph(graph);
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
	ret = Graph_getReturnValue(graph);
	char c;
	printf("Ret = %d\n", ret);
	TRACE_OUT
	return ret;
}

