/**
 * Standard libs
 */
#include <stdio.h>

/**
 * Unit test definitions
 */
#include "unimacros.h"
#include "testsuitedef.h"

/**
 *  Definition files for function to be tested
 */
#include "testdef.h"

int testnum;

void init_test_suite(){
	LOGO("test_suite_0");
	testnum = 0;
}

void clear_test_suite(){
	return;
}

int test_suite_0(){
	init_test_suite();
	TEST(++testnum, "f(1) returns T", f(1)  );
	TEST(++testnum, "f(2) returns F", !f(2) );
	TEST(++testnum, "f_err(1) returns T", f_err(1) );
	TEST(++testnum, "f_err(2) returns F", !f_err(2) );
	clear_test_suite();
	return 0;
}
