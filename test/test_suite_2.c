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
 * PURPOSE: Insert more nodes at STEP + area.
 * CHECK  : Pointers to header are ok, before and after realloc.
 */
Boolean TestCaseNodeIndex1(){
	int i;
	for ( i = 0; i < 5; i++){
		if (  !( insertNode(index_p, STEP+i, STEP+i) == Success ) ){
			return False;
		}

	}
	printf("size == %d\n", index_p->size);
	return index_p->size == 2*STEP;
}

/**
 * PURPOSE: Insert INIT more nodes, that already exist.
 * CHECK  : Pointers to header are ok, nothing is inserted
 */
Boolean TestCaseNodeIndex2(){
	int i;
	for ( i = 0; i < 5; i++){
		if (  insertNode(index_p, STEP+i, i+1) != Request_data_found ){
			return False;
		}
	}
	return True;
}

/**
 * PURPOSE: Get list head pointers with Id for all nodes.
 * CHECK  : Correct pointer is returned
 */
Boolean TestCaseNodeIndex3(){
	int i;
	for ( i = 1; i < 5; i++){
		if (  getListHead(index_p, STEP+i) != STEP+i ){
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
		if (  getListHead(index_p, 50 + 2*INIT ) != -1 ){
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
			AND index_p->size == STEP );
	TEST(testnum++, "insert one Node in Node index", insertNode(index_p, 1, 1) == Success );
	TEST(testnum++, "Insert more nodes further than step area. Index will reallocate space.", TestCaseNodeIndex1() );
	TEST(testnum++, "Insert already existing nodes. Index size will remain the same.", TestCaseNodeIndex2() );
	TEST(testnum++, "Search with nodeId gives correct pointer. .", TestCaseNodeIndex3() );
	TEST(testnum++, "Search not existing nodeId returns 0xffffffff. .", TestCaseNodeIndex4() );
	TEST(testnum++, "Destroy node index clears everything", destroyNodeIndex(index_p) == Success );
	clear_test_suite_2();
	return suite_eval();
}
