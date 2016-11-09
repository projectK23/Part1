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
#include "../include/NodeIndex.h"

/**
 * Definitions
 */
int testnum;

NodeIndex* index_p;


/**
 * Test case configuration
 */
void init_test_suite_2(){
	LOGO("test_suite_2: NodeIndex.c procedures");
	init_suite_eval();
	testnum = 1;
}

void clear_test_suite_2(){
	return;
}
/****************************************************
 *  TEST CASES
 */
/**
 * PURPOSE: Insert INIT more nodes so as to double index.
 * CHECK  : Pointers to header are ok, before and after realloc. Size is doubles=d
 */
Boolean TestCaseNodeIndex1(){
	int i;
	for ( i = 0; i < INIT; i++){
		if (  !( insertNode(index_p, i+2, i+2, False) == Success AND i+2 == index_p->end ) ){
			return False;
		}

	}
	return index_p->size == 2*INIT;
}

/**
 * PURPOSE: Insert INIT more nodes, that already exist.
 * CHECK  : Pointers to header are ok, nothing is inserted
 */
Boolean TestCaseNodeIndex2(){
	int i;
	for ( i = 0; i < INIT + 1; i++){
		if (  insertNode(index_p, i+1, i+1, False) != Request_data_found ){
			return False;
		}
	}
	return index_p->end == INIT+1;
}

/**
 * PURPOSE: Get list head pointers with Id for all nodes.
 * CHECK  : Correct pointer is returned
 */
Boolean TestCaseNodeIndex3(){
	int i;
	for ( i = 1; i < INIT + 2; i++){
		if (  getListHead(index_p, i) != i ){
			return False;
		}
	}
	return True;
}

/**
 * PURPOSE: Get list head pointers for not existing Ids.
 * CHECK  : NULL pointer is returned
 */
Boolean TestCaseNodeIndex4(){
	int i;
	for ( i = 1; i < INIT + 2; i++){
		if (  getListHead(index_p, i + 2*INIT ) != -1 ){
			return False;
		}
	}
	return True;
}


/*
 *
 ****************************************************/

/**
 * Run test cases
 */
int test_suite_2(){
	init_test_suite_2();
	TEST(testnum++, "createNodeIndex returns NOT NULL", ( index_p = createNodeIndex() ) != NULL
			AND index_p->size == INIT AND index_p->end == 0 );
	TEST(testnum++, "insert one Node in Node index", insertNode(index_p, 1, 1, False) == Success
			AND index_p->end == 1 );
	TEST(testnum++, "Insert INIT more nodes. Index will double it's size.", TestCaseNodeIndex1() );
	TEST(testnum++, "Insert already existing nodes. Index size will remain the same.", TestCaseNodeIndex2() );
	TEST(testnum++, "Search with nodeId gives correct pointer. .", TestCaseNodeIndex3() );
	TEST(testnum++, "Search not existing nodeId returns 0xffffffff. .", TestCaseNodeIndex4() );
	TEST(testnum++, "Destroy node index clears everything", destroyNodeIndex(index_p) == Success );
	clear_test_suite_2();
	return suite_eval();
}
