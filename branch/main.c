#include <string.h>
#include "NodeIndex.h"


int main(int argc, char *argv[]) {
	uint32_t source, dest;
	char* op;
	NodeIndex* index_out;
	NodeIndex* index_in;

	op= malloc(10*sizeof(char));

	index_in= createNodeIndex();
	index_out= createNodeIndex();

	while (1) {
		scanf(" %s", op);
		if(strcmp(op, "S")==0)
			break;
		source= atoi(op);
		scanf(" %d", &dest);
		if (source == dest) continue;
		if (insertNode(index_out, source) == 0)
			insertNode(index_in, source);
		if (insertNode(index_out, dest) == 0)
		    insertNode(index_in, dest);
		if (insert_neighbor(index_out, source, dest) == 0)
		    insert_neighbor(index_in, dest, source);
	}

	while (1) {
		scanf(" %s", op);
		if(strcmp(op, "F")==0)
			break;
		scanf(" %d %d",&source, &dest);
		if (strcmp(op, "A")==0) {
			if (source == dest) continue;
			if (insertNode(index_out, source) == 0)
				insertNode(index_in, source);
			if (insertNode(index_out, dest) == 0)
		    	insertNode(index_in, dest);
			if (insert_neighbor(index_out, source, dest) == 0)
		    	insert_neighbor(index_in, dest, source);
		}
	}

	printf("\nOutgoing edges\n");
	print_graph(index_out);
	printf("\nIncoming edges\n");
	print_graph(index_in);

	destroyNodeIndex(index_out);
	destroyNodeIndex(index_in);

	return 0;
} 
