#include <stdio.h>
#include <string.h>

#include "testsuitedef.h"
#include "unimacros.h"


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

	/* Run test cases */
	test_suite_0();
	return evaluate();
}
