#include <stdio.h>
#include <assert.h>

#include "unimacros.h"

int num_suite_failed = 0;
int test_result;
int num_passed = 0;
int num_failed = 0;
int v0;

/**
 * TEST function
 */
void TEST(int testnum, char *description,int eval){
	if ( (eval) ){ \
		__logprintf("TEST %d (%s) PASSED!\n", testnum, description);
		num_passed++;
	}else {
		__logprintf("TEST %d (%s) failed\n", testnum, description);
		num_failed++;
		num_suite_failed++;
		if (v0)assert(0);
	}
}

/**
 * evaluates the number of failed test cases
 */
void init_suite_eval(){
	num_suite_failed = 0;
}
int suite_eval(){
	return num_suite_failed;
}


int evaluate(){
	int total = num_passed+num_failed;
	__logprintf("\n\nTotal results: ");
	__logprintf("%2.2f %% (%d:%d) of test cases passed\n",
			(float)(100*num_passed)/(float)(total), num_failed, total );
	return num_failed;
}

void SET_TEST_MODE(int i){
	v0 = i;
}
