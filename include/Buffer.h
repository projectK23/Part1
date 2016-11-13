#ifndef BUFFER_H__
#define BUFFER_H__

#include "../libs/include/shared_definitions.h"
#include "../libs/include/result_codes.h"

typedef
struct _Slist_node{
	uint32_t neighbor[INIT];      //the ids of the neighbor nodes
	uint32_t edgeProperty[INIT];  //property for each edge
	ptr  nextListNode;       //pointer to next
} list_node;

typedef
struct _SBuffer{
	uint32_t max_size;        // sizeof_buffer (bytes)
	list_node * list_nodes;	          // Start of array of list nodes
	ptr end;        // pointer_to_end
} Buffer;




/***************************************************
 * Purpose : creates a buffer
 * IN      : n/a
 * Returns : pointer to the buffer <-- Success
 *           NULL                  x-- Fail
 */
Buffer* createBuffer();

/***************************************************
 * Purpose : Allocates a new node to the Buffer
 * IN      : Buffer
 * Returns : pointer to the node allocated
 */
ptr allocNewNode(Buffer*);

/***************************************************
 * Purpose : Reverts last allocation
 * IN      : Buffer
 * Returns : n/a
 * Comment : Dangerous !!! :@
 */
void freeLastNode(Buffer*);

/***************************************************
 * Purpose : Gets actual position in buffer
 * IN      : ptr
 * Returns : pointer to list_node
 */
list_node* getListNode(Buffer*, ptr);

/***************************************************
 * Purpose : Allocates a new node to the Buffer
 * IN      : Buffer
 * Returns : pointer to the node allocated
 */
OK_SUCCESS destroyBuffer(Buffer*);


#endif
