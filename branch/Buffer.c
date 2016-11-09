#include "Buffer.h"


Buffer* createBuffer() 
{
    Buffer* buffer;
      
    buffer = malloc(sizeof(Buffer));
    buffer->list_nodes= malloc(sizeof(ptr));
    buffer->size = 0;
  
    return buffer;
}


ptr allocNewNode(Buffer* buffer)
{
	int i;
	ptr node;

    node = malloc(sizeof(list_node));
    node->nextListNode= NULL;
    for(i=0; i<N; i++){
    	node->neighbor[i]=-1;
    }
	buffer->size++;
	buffer->list_nodes= realloc(buffer->list_nodes, (buffer->size)*sizeof(ptr));
    buffer->list_nodes[buffer->size-1]= node;

    return node;
}


struct list_node* getListNode(ptr node){ return node;}


OK_SUCCESS destroyBuffer(Buffer* buffer)
{
	int i;

	for(i=0; i<buffer->size; i++) {
        free((buffer)->list_nodes[i]);
	}
	free(buffer->list_nodes);
	free(buffer);
	buffer=NULL;

	return 0;
}
