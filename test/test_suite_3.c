/**
 * Standard libs
 */
#include <stdio.h>

/**
 * Unit test definitions
 */
#include "unimacros.h"
#include "testsuitedef.h"
#include "../libs/include/result_codes.h"

/**
 *  Definition files for function to be tested
 */
#include "../include/graph_operations.h"

/**
 * Definitions
 */
int testnum;

Graph graph;


/**
 * Test case configuration
 */
void init_test_suite_3(){
	LOGO("test_suite_3: graph_operations.c procedures");
	init_suite_eval();
	testnum = 1;
}

void clear_test_suite_3(){
	return;
}
/****************************************************
 *  TEST CASES
 */


/*
 *
 ****************************************************/

/**
 * Run test cases
 */
int test_suite_3(){
	init_test_suite_3();
	TEST(testnum++, "createGraph returns NOT NULL", ( graph = graphCreate() ) != NULL );
	TEST(testnum++, "graphDestroy returns Success, graph is set to NULL", graphDestroy(&graph) == Success AND graph == NULL);
	clear_test_suite_2();
	return suite_eval();
}
