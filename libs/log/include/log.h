#ifndef __log_LIB__
#define __log_LIB__

/**
 * Debug levels
 * 0: Fatal
 * 1: Error logs
 * 2: Error & trace logs
 * 3: All logs
 */


#define TRACE_LOG 1
#define EVERY_LOG 2

#	define FATAL(str) printf("FATAL ERROR : %s\n", (str) );

#if DEBUG_LEVEL == 1 || DEBUG_LEVEL == 2 || DEBUG_LEVEL == 3
#	define ERROR(str) \
	printf("ERROR (File:%s, Line:%d [%s]) : %s\n",  __FILE__, __LINE__, __func__, (str) );
#else
#	define ERROR(str) ;
#endif

#if DEBUG_LEVEL == 2 || DEBUG_LEVEL == 3
#	define TRACE_IN 	printf("In function: %s [%s]\n", __func__, __FILE__ );
#	define TRACE_OUT    printf("--> Out at %d\n",__LINE__ );
#else
#	define TRACE_IN ;
#   define TRACE_OUT ;
#endif

#endif
