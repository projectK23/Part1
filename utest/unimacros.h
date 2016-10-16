#ifndef __my_UNI__
#define __my_UNI__


#define __logprintf printf
#define LOGO(str) \
		printf("\n\n\n##############################\nTEST %s\n##############################\n", (str) )


void TEST(int testnum, char *description,int eval);

/**
 * evaluates the number of failed test cases
 */
void init_suite_eval();
int suite_eval();


/**
 * evaluates the number of passed tests
 */
int evaluate();

/**
 *  set test mode
 *  i = 1 --> virtual zero (all test cases should pass)
 *  i = 0 --> regression   (evaluates pass rate)
 */
void SET_TEST_MODE(int i);

#endif
