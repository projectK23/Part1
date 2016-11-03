#ifndef BUFFER_H__
#define BUFFER_H__

#include <stdlib.h>
#include <stdint.h>
#define N 10


typedef struct list_node * ptr; 


typedef struct Buffer{

  uint32_t size;             //number of buffer nodes

  ptr* list_nodes;           //array of list nodes
	
} Buffer;


typedef struct list_node{

  uint32_t neighbor[N];      //the ids of the neighbor nodes
                              
  uint32_t edgeProperty[N];  //property for each edge
  
  ptr  nextListNode;         //pointer to next listhead

} list_node;


Buffer* createBuffer();

ptr allocNewNode(Buffer*);

struct list_node* getListNode(ptr);

int destroyBuffer(Buffer*);


#endif