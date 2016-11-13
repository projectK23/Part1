#ifndef __shared
#define __shared

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;

/*#if DEBUG_LEVEL == 0
#	define INIT 100
#	define STEP 1000
#else*/
#	define INIT 3
#	define STEP 5
//#endif

#define WORKERS 2

typedef unsigned int ptr;


#endif
