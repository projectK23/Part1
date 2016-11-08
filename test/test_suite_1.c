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
#include "../include/Buffer.h"

/**
 * Definitions
 */
int testnum;

Buffer * buf_p;
ptr new_node;


/**
 * Test case configuration
 */
void init_test_suite_1(){
	LOGO("test_suite_1: Buffer.c procedures");
	init_suite_eval();
	testnum = 1;
}

void clear_test_suite_1(){
	buf_p = NULL;
	return;
}
/****************************************************
 *  TEST CASES
 */
/**
 * PURPOSE: allocate INIT more nodes and check that buffer is doubled. check that everything is ok
 */
Boolean TestCaseBuffer1(){
	int i;
	for ( i = 0; i < INIT; i++){
		if (  !( ( new_node = allocNewNode(buf_p) ) != NULL
			AND buf_p->list_nodes + (i+2)*sizeof(list_node)  == buf_p->end ) ){
			return False;
		}
	}
	return buf_p->max_size == 2*INIT*sizeof(list_node);
}


/*
 *
 ****************************************************/

/**
 * Run test cases
 */
int test_suite_1(){
	init_test_suite_1();
	TEST(testnum++, "Create_buffer returns NOT NULL", ( buf_p = createBuffer() ) != NULL
			AND buf_p->max_size == INIT*sizeof(list_node) AND buf_p->list_nodes == buf_p->end );
	TEST(testnum++, "Allocate new node returns not NULL", ( new_node = allocNewNode(buf_p) ) != NULL
			AND buf_p->list_nodes + sizeof(list_node)  == buf_p->end);
	TEST(testnum++, "Buffer was doubled after more INIT insertions", TestCaseBuffer1() );
	TEST(testnum++, "Buffer is destroyed", destroyBuffer( buf_p ) == Success );
	clear_test_suite_1();
	return suite_eval();
}
