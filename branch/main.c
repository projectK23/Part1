#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "NodeIndex.h"


int main(int argc, char *argv[]) { 
	uint32_t source, dest;
	char* flag;
	NodeIndex* index_out;
	NodeIndex* index_in;

	flag= malloc(10*sizeof(char));

	index_in= createNodeIndex();
	index_out= createNodeIndex();

	while (1) {
		scanf(" %s", flag);
		if(strcmp(flag, "S")==0)
			break;
		source= atoi(flag);
		scanf(" %d", &dest);
		insertNode(index_out, source);
		insertNode(index_out, dest);
		insertNode(index_in, source);
		insertNode(index_in, dest);

		if(insert_neighbor(index_out, source, dest) == 0)
		   insert_neighbor(index_in, dest, source);
	}

	printf("\nOutgoing edges\n");
	print_graph(index_out);
	printf("\nIncoming edges\n");
	print_graph(index_in);
	printf("\n");

	while (1) {
		scanf(" %s", flag);
		if(strcmp(flag, "F")==0)
			break;
		scanf(" %d %d",&source, &dest);
		if (strcmp(flag, "A")==0) {
			insertNode(index_out, source);
			insertNode(index_out, dest);
			insertNode(index_in, source);
			insertNode(index_in, dest);
			if(insert_neighbor(index_out, source, dest) == 0)
		   	   insert_neighbor(index_in, dest, source);
		}
	}
	printf("\nOutgoing edges\n");
	print_graph(index_out);
	printf("\nIncoming edges\n");
	print_graph(index_in);
	printf("\n");

	destroyNodeIndex(index_out);
	destroyNodeIndex(index_in);

	return 0;
} 
