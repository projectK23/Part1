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
#include "../include/queue.h"

/**
 * Definitions
 */
int testnum;

Q q;


/**
 * Test case configuration
 */
void init_test_suite_4(){
	LOGO("test_suite_4: queue.c procedures");
	init_suite_eval();
	testnum = 1;
}

void clear_test_suite_4(){
	return;
}
/****************************************************
 *  TEST CASES
 */

Boolean TestCaseQueue1(){
	int i, itest;
	for ( i = 0; i < 10; i++){
		if ( Q_push(q, i) != Success )return False;
	}
	if ( Q_size(q) != 10 )return False;
	for ( i = 0; i < 10; i++){
		if ( Q_pop(q, &itest) != Success AND itest != i)return False;
	}
	return True;
}


/*
 *
 ****************************************************/

/**
 * Run test cases
 */
int test_suite_4(){
	init_test_suite_4();
	TEST(testnum++, "Q_init returns NOT NULL and queue is empty", ( q = Q_init() ) != NULL AND Q_isEmpty(q));
	TEST(testnum++, "Add 10 data in queue, pop 10 data from queue, queue is empty", TestCaseQueue1() AND Q_isEmpty(q));
	Q_destroy(&q);
	TEST(testnum++, "Q_destroy sets q = NULL", q == NULL);
	clear_test_suite_2();
	return suite_eval();
}
