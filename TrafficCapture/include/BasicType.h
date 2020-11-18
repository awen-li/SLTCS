/***********************************************************
 * Author: Wen Li
 * Date  : 3/25/2019
 * Describe: basic type define 
 * History:
   <1> 3/25/2019 , create
************************************************************/

#ifndef _BASICTYPE_H_
#define _BASICTYPE_H_ 

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <deque>
#include <iostream>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <unordered_map> 
#include <algorithm>  
#include <list>
#include <vector>
#include <cstddef> 



typedef unsigned char    BYTE;
typedef char             CHAR;
typedef unsigned int     DWORD;
typedef int              INT;
typedef unsigned short   WORD;
typedef unsigned long    ULONG;
typedef signed int       SDWORD;
typedef signed short     SWORD;
typedef void             VOID;
typedef unsigned int     BOOL;



#define M_SUCCESS          (0)
#define M_FAIL             (1)
#define M_TRUE             (1)
#define M_FALSE            (0)


#define CLOCK_IN_SEC()  (time((time_t*)NULL))


#define PACKET_SIZE         (1500)

#define USER_IP_PROPERTITY  ("ip.properties")


#define CAPTURE_NUM         (20)
#define FORMAT_LEN          (512)



#endif
