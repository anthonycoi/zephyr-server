#ifndef __DB_LIB_DB_LIB_H__
#define __DB_LIB_DB_LIB_H__

typedef const char * LPCTSTR;
typedef char * LPTSTR;




#ifdef WIN32

#ifndef WINVER				// 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。
#define WINVER 0x0400		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0400		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINDOWS		// 允许使用 Windows 98 或更高版本的特定功能。
#define _WIN32_WINDOWS 0x0410 //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE			// 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0400	//为 IE 5.0 及更新版本改变为适当的值。
#endif

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料


#define USE_CRT_DETAIL_NEW

#include <WinSock2.h>
#include <MSWSock.h>

#endif

#include "../../System/include/SystemTools.h"
#include <vector>
#include "EasyBuffer.h"
//#include "TDThread.h"
#include "../../System/include/Lock.h"
#include "ThreadSafeList.h"
#include "EasyTimer.h"
#include "../../Public/Config/include/SettingFile.h"
#include "EasyString.h"
#include "StringSplitter.h"
#include "NameObject.h"

#include "IfDBLib.h"

#define DBLIB_VERSION	"2.0.0.0"


typedef unsigned long ulong;


#include "DBValue.h"

#include "DBInterfaces.h"

#include "DBStaticRecordSet.h"
#include "DBFakeRecordSet.h"


#ifdef WIN32

#include "MySQL\include\mysql.h"

#else

#include <mysql/mysql.h>

#endif


void PrintDBLog(int logLvl,const char* __pszFormat,...);




#include "MySQLRecordSet.h"
#include "MySQLConnection.h"
#include "MySQLDatabase.h"

#include "DBTransaction.h"
#include "DBTransationWorkThread.h"
#include "DBTransationManager.h"

using namespace DBLib;

#endif
