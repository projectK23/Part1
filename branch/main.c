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
		if (!strcmp(op, "S")) break;
		source= atoi(op);
		scanf(" %d", &dest);
		if (source == dest) continue;
		if (insertNode(index_out, source))
		    insertNode(index_in, source);
		if (insertNode(index_out, dest))
		    insertNode(index_in, dest);
		if (insertNeighbor(index_out, source, dest))
		    insertNeighbor(index_in, dest, source);
	}

	while (1) {
		scanf(" %s", op);
		if (!strcmp(op, "F")) break;
		scanf(" %d %d", &source, &dest);
		if (!strcmp(op, "A")) {
			if (source == dest) continue;
			if (insertNode(index_out, source))
			    insertNode(index_in, source);
			if (insertNode(index_out, dest))
		            insertNode(index_in, dest);
			if (insertNeighbor(index_out, source, dest))
		    	    insertNeighbor(index_in, dest, source);
		}
	}

	printf("\nOutgoing edges\n");
	printGraph(index_out);
	printf("\nIncoming edges\n");
	printGraph(index_in);
	printf("\n");

	destroyNodeIndex(index_out);
	destroyNodeIndex(index_in);

	return 0;
}
