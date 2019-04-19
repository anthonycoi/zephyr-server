/*-------------------------------------------------------------
 Copyright (C)| 2008-2010 TDGame Info. Co. Ltd.
 File: SysInc.h
 Author: Yipeng.Zhang
 Date: 2009-3-16 16:09:12
 Description: this is a list file for win32 or linux platform
 Others:
 -------------------------------------------------------------*/

#ifndef __SYS_INCLUDE_H__
#define __SYS_INCLUDE_H__

//#include "TypeDef.h"

// win include
#ifndef WIN32
#error you must specify WIN32 specification
#endif

#ifdef WIN32
#ifndef _WIN32_WINNT 
# define _WIN32_WINNT 0x500 
#endif

#include <windows.h>
#include <process.h>
#include <winerror.h>
#define I64_FMT "%I64d"
// linux include
#else
#include <pthread.h>
#define NOERROR 0
#define E_FAIL  0x80005
#define SOCKET_ERROR -1
typedef struct hostent HOSTENT;
typedef struct in_addr IN_ADDR;
#endif

namespace Zephyr
{
#ifdef _WIN32
    void SleepT(unsigned int ms);
#else
    void SleepT(unsigned int ms);
#endif

}

#endif
