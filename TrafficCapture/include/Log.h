
#ifndef _LOG_H_
#define _LOG_H_
#include <BasicType.h>


#ifdef _DEBUG_
#define DebugLog(format, ...) printf(format, ##__VA_ARGS__)
#else
#define DebugLog(format, ...) 
#endif



#endif 
