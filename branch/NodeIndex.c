#include "NodeIndex.h" 




NodeIndex* createNodeIndex() 
{
    NodeIndex* nodeindex;
      
    nodeindex = malloc(sizeof(NodeIndex));
    nodeindex->buffer= createBuffer();
    nodeindex->size = 0;
    nodeindex->start = NULL;
    nodeindex->end= NULL;
  
    return nodeindex;
} 


int insertNode(NodeIndex* nodeindex, uint32_t nodeId)
{
	node_ptr node;

	if ((nodeindex)->start == NULL){
        node = malloc(sizeof(Index_node));
        node->Id= nodeId;
        node->listhead= allocNewNode((nodeindex)->buffer); 
        node->next= NULL;
	    (nodeindex)->start= node;
	    (nodeindex)->end= node;
	    (nodeindex)->size++;
    }
    else {
    	node= (nodeindex)->start;
	    while (node != NULL) {
		    if (node->Id == nodeId)
			   return 1;
		    node= node->next;
	    }
	    node = malloc(sizeof(Index_node));
	    node->Id= nodeId;
	    node->listhead= allocNewNode((nodeindex)->buffer);
        node->next= NULL;
	    (nodeindex)->end->next= node;
	    (nodeindex)->end= node;
	    (nodeindex)->size++;
    }
	return 0;
}


ptr getListHead( NodeIndex* nodeindex, uint32_t nodeId )
{
	node_ptr node;

	node= (nodeindex)->start;
	while (node != NULL) {
		if (node->Id == nodeId)
			return node->listhead;
		node= node->next;
	}
	return NULL;
}


int destroyNodeIndex( NodeIndex* nodeindex)
{
	node_ptr node1, node2;
	
	node1= (nodeindex)->start;
	while(node1!=NULL) {
	    node2= node1;
        node1= node1->next;
        free(node2);
	}
	destroyBuffer((nodeindex)->buffer);
	free(nodeindex);
	(nodeindex)=NULL;

	return 0;
}


int insert_neighbor(NodeIndex* nodeindex, uint32_t source, uint32_t dest)
{
	int i, stop=0;
	ptr list;

	list= getListHead(nodeindex, source);
	while (1) {
	    for(i=0; i<N; i++){
	    	if (list->neighbor[i] == dest)
	    		return 1;
	    	else if (list->neighbor[i] == -1) {
	    		list->neighbor[i]= dest;
	    		stop=1;
	    		break;
	    	}
		}
		if (stop == 1) break;
		if(list->nextListNode == NULL)
			list->nextListNode= allocNewNode((nodeindex)->buffer);
	    list= list->nextListNode;
	}
	return 0;
}


void print_graph(NodeIndex* nodeindex)
{
	int i;
	node_ptr node;
	ptr list;
	
	node= (nodeindex)->start;
	while(node != NULL) {
		printf("%d: ",node->Id);
		list= node->listhead;
		do {
	    	for(i=0; i<N; i++){
	    		if (list->neighbor[i] == -1)
	    			break;
	    		else {
	    			printf("%d ", list->neighbor[i]);
	    		}
			}
	    	list= list->nextListNode;
		} while(list != NULL);
		printf("\n");
		node= node->next;
	}
}