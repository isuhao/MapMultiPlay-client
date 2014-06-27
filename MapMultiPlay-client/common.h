#pragma once
#if DEBUG
#include <cstdio>
#define MM_LOG(s) \
	printf(s);printf("\n");
#define MM_LOGN(s,...) \
	printf(s,__VA_ARGS__);printf("\n");
#else
#define MM_LOG(s)
#define MM_LOGN(s,...)
#endif

#if DEBUG
#include <cassert>
#define MM_ASSERT(a,s) \
	do{\
		if(!(a)){printf(s);assert(false);}\
	}while(0)
#define MM_ASSERTN(a,s,...) \
	do{\
		if(!(a)){printf(s,__VA_ARGS__);assert(false);}\
	}while(0)
#else
#define MM_ASSERT(a,s)
#define MM_ASSERTN(a,s,...)
#endif