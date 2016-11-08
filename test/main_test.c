#include <stdio.h>
#include <string.h>

#include "testsuitedef.h"
#include "unimacros.h"


#define PRINT_RESULT( ev ) \
	printf("\n... Test suite finished. %d test cases failed\n", (ev));

/* The suite config.
 * Should returns zero on success, non-zero otherwise.
 */
int main(int argc , char *argv[]){
	/* Configure test */
	if ( argc == 1){
		printf("No argument: test for VIRTUAL 0");
		SET_TEST_MODE(1);
	}else if( strncmp(argv[1], "reg", 3) == 0)
		SET_TEST_MODE(0);
	else SET_TEST_MODE(1);

	/* Run suite test cases */
	PRINT_RESULT( test_suite_1() )
	PRINT_RESULT( test_suite_2() )
	PRINT_RESULT( test_suite_3() )

	/* evaluate result */
	return evaluate();
}
