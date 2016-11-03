#ifndef NODEINDEX_H__
#define NODEINDEX_H__

#include "stdio.h"
#include "Buffer.h"


typedef struct Index_node* node_ptr;


typedef struct NodeIndex{

  Buffer*  buffer;          //pointer to buffer

  uint32_t size;            //number of Index nodes

  node_ptr start;           //pointer to first Index node

  node_ptr end;             //pointer to last Index node
	
} NodeIndex;


typedef struct Index_node{

  uint32_t Id;              //node Id                          
  
  ptr listhead;             //pointer to listhead

  node_ptr next;            //pointer to next index node

} Index_node;


NodeIndex* createNodeIndex();

OK_SUCCESS insertNode(NodeIndex*, uint32_t nodeId);

ptr getListHead(NodeIndex*, uint32_t nodeId);

OK_SUCCESS destroyNodeIndex(NodeIndex*);

OK_SUCCESS insertNeighbor(NodeIndex*, uint32_t source, uint32_t dest);

void printGraph(NodeIndex*);


#endif