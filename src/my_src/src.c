#include <stdio.h>
#include <testdef.h>

/**
 * This function returns false if in is even, else true	
 */
int f(int i){ return i%2; }

/**
 * ERROR: This function returns false if in is even, else true	
 */
int f_err(int i){ return i%2 + 1; }


