#ifndef __DB_LIB_DB_LIB_H__
#define __DB_LIB_DB_LIB_H__

typedef const char * LPCTSTR;
typedef char * LPTSTR;




#ifdef WIN32

#ifndef WINVER				// ����ʹ�� Windows 95 �� Windows NT 4 ����߰汾���ض����ܡ�
#define WINVER 0x0400		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ�� Windows NT 4 ����߰汾���ض����ܡ�
#define _WIN32_WINNT 0x0400		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_WINDOWS		// ����ʹ�� Windows 98 ����߰汾���ض����ܡ�
#define _WIN32_WINDOWS 0x0410 //Ϊ Windows Me �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ�� IE 4.0 ����߰汾���ض����ܡ�
#define _WIN32_IE 0x0400	//Ϊ IE 5.0 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����


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
