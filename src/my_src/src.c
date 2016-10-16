#include <stdio.h>
#include <testdef.h>

#include "log.h"

/**
 * This function returns false if in is even, else true	
 */
int f(int i)
{
	TRACE_IN
	TRACE_OUT
	return i%2;
}

/**
 * ERROR: This function returns false if in is even, else true	
 */
int f_err(int i)
{
	TRACE_IN
	TRACE_OUT
	return i%2 + 1;
}


