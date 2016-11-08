#ifndef __graph_operations
#define __graph_operations


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
	ptr startBufferOut;
	ptr startBufferInc;
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
 * PURPOSE : Search path
 * IN      : Graph, source, target
 * OUT     : Result cause
 * COMMENTS: Path is not hold somewhere.
 */
int existPathInGraph(Graph graph, uint32_t source, uint32_t target);


/******************************************************
 * PURPOSE : Search path
 * IN      : Graph, source, target
 * OUT     : Result cause
 * COMMENTS: Path is not hold somewhere.
 *           2 posix threads do b-directional search
 *           main_thread is blocked, until result
 *
 */
int existPathInGraphBD(Graph graph, uint32_t source, uint32_t target);



#endif
