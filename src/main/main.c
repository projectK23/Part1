/**
 * Include standard libs
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Include local libs
 */
#include "log.h"
#include "result_codes.h"
#include "shared_definitions.h"

/**
 * Include local i\fs
 */
#include "graph_operations.h"

/**
 * GLOBAL DEFS
 */
#if DEBUG_LEVEL == 0
	char fileB1[512];
	char fileB2[512];
#else
	char *fileB1 = "../datasets/tiny/tinyGraph.txt";
	char *fileB2 = "../datasets/tiny/tinyWorkload_FINAL.txt";
#endif

	char op[128];
	uint32_t source, dest;
	Graph graph;

/**
 * PURPOSE : Initializes structures
 * RETURNS : Status of calls
 */
OK_SUCCESS initialize(){
	TRACE_IN

	LOG("Buffers for reading commands")
	memset(op, '\0', 128);

#if DEBUG_LEVEL == 0
	memset(fileB1, 0, 512);
	memset(fileB2, 0, 512);
#endif

	LOG("Initialize graph")
	if ( (graph = graphCreate() ) == NULL ){
		FATAL("Graph was not initialized")
		TRACE_OUT
		return Unknown_Failure;
	}
	TRACE_OUT
	return Success;
}


/**
 * PURPOSE : Handle 1st blast
 * RETURNS : Status
 */
OK_SUCCESS handleBlast1(){
	TRACE_IN
	FILE *fp;
	OK_SUCCESS ret;

	if ( (fp = fopen(fileB1, "r") ) == NULL ){
		FATAL("Unable to open file for reading")
		TRACE_OUT
		return File_does_not_exist;
	}
	unsigned short counter = 0;
	unsigned int thousands = 0;
	LOG("Read data from blast1 file")
	char c;
	while( fgets(op, 127, fp) != NULL ){
		if ( strcmp(op, "s") == 0 || strcmp(op, "S") == 0 ){
			LOG("First blast completed!")
			break;
		}
		sscanf(op, "%d %d", &source, &dest);
#if DEBUG_LEVEL > 1
		printf("Insertion in graph  %s\n", op);
//		scanf("%c", &c);
#endif
		insertEdgeInGraph(graph, source, dest);
		counter++;
		if ( !(counter % 1000) ){
			thousands ++;
			counter = 0;
			printf("%d thousands edges inserted in graph\n", thousands);
		}
#if DEGUG_LEVEL > 1
//		scanf("%c", &c);
#endif
	}
	fclose(fp);
	printf("%d edges inserted in graph\n", thousands * 1000 + counter);
	TRACE_OUT
	return Success;
}

/**
 * PURPOSE : Handle 2nd blast
 * RETURNS : Status
 */
#define __add__    'A'
#define __querry__ 'Q'
OK_SUCCESS handleBlast2(){
	TRACE_IN
	FILE *fp;
	OK_SUCCESS ret;
	char action;
	if ( (fp = fopen(fileB2, "r") ) == NULL ){
		FATAL("Unable to open file for reading")
		TRACE_OUT
		return File_does_not_exist;
	}

	LOG("Read data from blast2 file")
	while( fgets(op, 127, fp) != NULL ){
		if ( op[0] == 'F' || op[0] == 'f' ){
			LOG("First burst completed!")
			continue;
		}
		sscanf(op, "%c %d %d", &action, &source, &dest);
		switch( action ){

			case __add__:
#if DEBUG_LEVEL != 0
				printf("%s", op);
#endif
				if ( ( ret = insertEdgeInGraph(graph, source, dest) ) != Success ){
					ERROR("Edge failed to get in graph. Ignore action")
					printf("RET = %d\n", ret);
				}
				break;

			case __querry__:
#if DEBUG_LEVEL != 0
				op[strlen(op)-1] = '\0';
				printf("%s\t:--> ", op);
#endif
				printf("%d\n", existPathInGraph(graph, source, dest) );
				break;

			default:
				ERROR("Unknown command. Ignore")
				break;
		}


	}
	fclose(fp);
	TRACE_OUT
	return Success;
}


int main(int argc, char *argv[]){
	TRACE_IN
	system ("banner K23");

	LOG("Initialize structures")
	if ( initialize() != Success ){
		FATAL("Failed to initialize structures")
		TRACE_OUT
		return 1;
	}

	LOG("Handle first blast")
#if DEBUG_LEVEL == 0
	if ( argc != 3 )
	{
		ERROR("No file arguments provided")
		printf("Give file for Blast 1: ");
		scanf("%s", fileB1 );
		printf("Give file for Blast 2: ");
		scanf("%s", fileB2 );
	}else{
		strcpy(fileB1, argv[1]) ;
		strcpy(fileB2, argv[2]);
	}
#endif
	if ( handleBlast1() != Success ){
		FATAL("Failed to init");
		TRACE_OUT
		return 1;
	}
	if ( handleBlast2() != Success ){
		FATAL("Failed to do actions");
		TRACE_OUT
		return 1;
	}


	if ( graphDestroy(&graph) != Success ){
		ERROR("Unable destroy graph")
	}
	TRACE_OUT
	return 0;
}
