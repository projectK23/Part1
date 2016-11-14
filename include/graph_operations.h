#ifndef __graph_operations
#define __graph_operations

#include <pthread.h>


#include "../libs/include/result_codes.h"
#include "../libs/include/shared_definitions.h"

#include "NodeIndex.h"
#include "Buffer.h"

typedef struct _Sgraph * Graph;


typedef struct _Sgraph{
	NodeIndex *nodeIndexOut;
	NodeIndex *nodeIndexInc;
	Buffer *bufferOut;
	Buffer *bufferInc;
	pthread_mutex_t critical;
	pthread_mutex_t start;
	pthread_mutex_t kill;
	pthread_cond_t start_up;
	pthread_cond_t kill_off;
	pthread_t worker[WORKERS];
	Boolean assign;
	Boolean do_exit;
	int workers_started;
}graph_t;

/******************************************************
 * PURPOSE : Creates a graph structure
 * IN      : n/a
 * OUT     : Pointer to graph <-- SUCCESS
 *           NULL             x-- FAIL
 * COMMENTS: n/a
 */
Graph graphCreate();

/******************************************************
 * PURPOSE : Destroys graph
 * IN      : n/a
 * OUT     : Pointer to graph <-- SUCCESS
 *           NULL             x-- FAIL
 * COMMENTS: n/a
 */
OK_SUCCESS graphDestroy(Graph *graph);


/******************************************************
 * PURPOSE : Insert a node in graph
 * IN      : Graph, node
 * OUT     : Result cause
 * COMMENTS: n/a
 */
OK_SUCCESS insertNodeInGraph(Graph graph, uint32_t nodeId);

/******************************************************
 * PURPOSE : Insert an edge in graph
 * IN      : Graph, source, destination
 * OUT     : Result cause
 * COMMENTS: n/a
 */
OK_SUCCESS insertEdgeInGraph(Graph graph, uint32_t sourceId, uint32_t destId);

/******************************************************
 * FUNCTION: Graph_killWorkers
 * PURPOSE : kills all workers if their task is done
 * IN      : Graph
 * OUT     : n/a
 */
void Graph_killWorkers(Graph graph);

/******************************************************
 * PURPOSE : Search path
 * IN      : void *args
 * OUT     : Result cause, not handled
 * COMMENTS: Path is not hold somewhere.
 *           Posix threads function for b-directional search
 */
void *l_searchPathInGraph(void *arg);

/******************************************************
 * PURPOSE : Search path
 * IN      : Graph, source, target
 * OUT     : Result cause
 * COMMENTS: Path is not hold somewhere.
 *           2 posix threads do b-directional search
 *           main_thread is blocked, until result
 *
 */
int existPathInGraph(Graph graph, uint32_t source, uint32_t target);



#endif
