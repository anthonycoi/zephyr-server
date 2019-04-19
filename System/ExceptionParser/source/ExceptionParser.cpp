#include "ExceptionParser.h"
#include <signal.h>
#include <stdio.h>

#ifdef _WIN32
#pragma  comment(lib,"Dbghelp.lib")
#else

#endif

#ifndef _M_IX86
#error "The following code only works for x86!"

#endif


namespace Zephyr
{

void DisableSetUnhandledExceptionFilter()

{

	void *addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")),"SetUnhandledExceptionFilter");

	if (addr) 

	{

		unsigned char code[16];

		int size = 0;

		//xor eax,eax;

		code[size++] = 0x33;

		code[size++] = 0xC0;

		//ret 4

		code[size++] = 0xC2;

		code[size++] = 0x04;

		code[size++] = 0x00;



		DWORD dwOldFlag, dwTempFlag;

		if(VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag))
		{
			if(WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL))
			{
				
			}
			VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
		}

		

	}

}

LPCTSTR GetModulePath()
{
	static char szModulePath[MAX_PATH];
	GetModuleFileName(NULL,szModulePath,MAX_PATH);
	char * pResult=strrchr(szModulePath,'\\');
	if(pResult)
	{
		*pResult=0;
	}
	return szModulePath;
}



LONG CExceptionParser::ExceptionHander(LPEXCEPTION_POINTERS pException)
{
	CExceptionParser::GetInstance()->ParseException(pException);
	return 0;
}

void CExceptionParser::ExceptionTranslator(UINT Code,LPEXCEPTION_POINTERS pException)
{
	CExceptionParser::GetInstance()->ParseException(pException);	
}


CExceptionParser::CExceptionParser(void)
{
	m_hLogFile=INVALID_HANDLE_VALUE;
	m_hProcess=NULL;
	m_ExceptionCount=0;

	m_finalizerNr = 0;
	for (int i =0;i<MAX_FINALIZER_NR;i++)
	{
	    m_ppFinalizers[i] = NULL;
	}
	
}

CExceptionParser::~CExceptionParser(void)
{
	if(m_hProcess)
		SymCleanup(m_hProcess);
}

void CExceptionParser::Init(UINT HandleMode)
{
	

	if(HandleMode&EXCEPTION_SET_DEFAULT_HANDLER)
	{
		SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ExceptionHander);

		

		//_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
		signal(SIGABRT,SignalHandler);
		//_set_invalid_parameter_handler(InvalidParameterHandler);

	}
	if(HandleMode&EXCEPTION_USE_API_HOOK)
	{
		DisableSetUnhandledExceptionFilter();
	}
	if(HandleMode&EXCEPTION_SET_TRANSLATOR)
		_set_se_translator(ExceptionTranslator);
}

void CExceptionParser::ParseException(LPEXCEPTION_POINTERS pException)
{
	//CAutoLock Lock(m_CriticalSection);

	LPCTSTR szModulePath=GetModulePath();
	char szLogFileName[MAX_PATH];
	SYSTEMTIME CurTime;	

	GetLocalTime(&CurTime);

	sprintf(szLogFileName,"%s\\Exception%d-%02d-%02d %02d-%02d-%02d.txt",
		(LPCTSTR)szModulePath,
		CurTime.wYear,CurTime.wMonth,CurTime.wDay,
		CurTime.wHour,CurTime.wMinute,CurTime.wSecond);

	m_hLogFile=CreateFile(szLogFileName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);


	m_hProcess=GetCurrentProcess();	
	if(!SymInitialize(m_hProcess,(PSTR)szModulePath,TRUE))
	{

		if(!SymInitialize(m_hProcess,NULL,TRUE))
		{

		}
	}	

	if(m_ExceptionCount>0)
	{
		PrintLog("发生多次异常捕捉%d",m_ExceptionCount);
		return;
	}
	m_ExceptionCount++;
	
	
	PrintLog("-----------------------------------------------------------------");

	PrintLog("程序发生异常:类型: 0x%x  地址: 0x%x", 
		pException->ExceptionRecord->ExceptionCode ,pException->ExceptionRecord->ExceptionAddress);

	if(pException->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)
	{
		PrintLog("地址:0x%x%s",
			pException->ExceptionRecord->ExceptionInformation[1],
			pException->ExceptionRecord->ExceptionInformation[0]?"不可写":"不可读");
	}
	

	static ADDRESS_INFO AddressInfo;

	GetAddressInfo((DWORD64)pException->ExceptionRecord->ExceptionAddress,&AddressInfo);
	PrintLog("地址描述:函数(%s),文件(%s)(%d)",AddressInfo.SymbolInfo.Name,AddressInfo.CppFileName,AddressInfo.LineNumber);

	PrintLog("调用堆栈:");

	
	ParseCallStack(pException->ContextRecord);

	PrintLog("-----------------------------------------------------------------");

	WriteDump(pException);
	CloseHandle(m_hLogFile);
	
	FILE *pFile = fopen("ExceptionDump.dump","a+");
	if(pFile)
	{
        char tmp[] = "Begin write exception dump for the subsystem!";
        tmp[sizeof(tmp)-1] = '\n';
        fwrite(tmp,sizeof(tmp),1,pFile);
        fflush(pFile);
        for(int i=0;i<m_finalizerNr;i++)
        {
            m_ppFinalizers[i]->OnException(pFile);
        }
        fclose(pFile);
        pFile = NULL;
	}
	
}

void CExceptionParser::ParseCallStack(PCONTEXT pContextRecord,UINT MaxLoopCount)
{
	if(m_hProcess==NULL)
		return;


	static HINSTANCE LastInstance=NULL;

	static ADDRESS_INFO AddressInfo;

	static STACKFRAME64 StackFrame;

	ZeroMemory(&StackFrame,sizeof(StackFrame));

	StackFrame.AddrPC.Offset=pContextRecord->Eip;
	StackFrame.AddrPC.Mode=AddrModeFlat;
	StackFrame.AddrStack.Offset=pContextRecord->Esp;
	StackFrame.AddrStack.Mode=AddrModeFlat;
	StackFrame.AddrFrame.Offset=pContextRecord->Ebp;
	StackFrame.AddrFrame.Mode=AddrModeFlat;

	while(MaxLoopCount&&StackWalk64(IMAGE_FILE_MACHINE_I386,m_hProcess,GetCurrentThread(),
		&StackFrame,pContextRecord,0,SymFunctionTableAccess64,SymGetModuleBase64,0))
	{
		if(StackFrame.AddrFrame.Offset==0)
			break;
		if(GetAddressInfo(StackFrame.AddrPC.Offset,&AddressInfo))
		{
			if(LastInstance!=AddressInfo.hInstance)
			{	
				LastInstance=AddressInfo.hInstance;
				PrintLog("调用模块:%s",AddressInfo.ModuleName);
			}
			PrintLog("调用地址:0x%X",(DWORD)StackFrame.AddrPC.Offset);
			PrintLog("地址描述:函数(%s),文件(%s)(%d)",AddressInfo.SymbolInfo.Name,AddressInfo.CppFileName,AddressInfo.LineNumber);
		}				
		
		MaxLoopCount--;
	}
}

BOOL CExceptionParser::GetAddressInfo(DWORD64 Address,ADDRESS_INFO * pAddressInfo)
{
	if(m_hProcess==NULL)
		return false;
	

	pAddressInfo->Address=Address;

	//获取地址对应的模块信息
	MEMORY_BASIC_INFORMATION mbi ;
	ZeroMemory(&mbi,sizeof(mbi));
	VirtualQuery((LPCVOID)Address, &mbi, sizeof( mbi ) ) ;			
	HINSTANCE hInstance = (HINSTANCE)mbi.AllocationBase ;

	if(hInstance)
	{	
		if(pAddressInfo->hInstance!=hInstance)
		{
			pAddressInfo->hInstance=hInstance;			
			GetModuleFileName( hInstance, pAddressInfo->ModuleName, MAX_PATH ) ;			
		}			
	}		

	DWORD64 Displacement=0;

	if(SymFromAddr(m_hProcess,Address,&Displacement,&(pAddressInfo->SymbolInfo)))
	{
		IMAGEHLP_LINE64 LineInfo;
		ZeroMemory(&LineInfo,sizeof(LineInfo));
		LineInfo.SizeOfStruct=sizeof(LineInfo);

		DWORD LineDisplacement=0;

		SymGetLineFromAddr64(m_hProcess,Address,&LineDisplacement,&LineInfo);
		if(LineInfo.FileName)
		{
			strncpy(pAddressInfo->CppFileName,LineInfo.FileName,MAX_PATH);
			pAddressInfo->LineNumber=LineInfo.LineNumber;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CExceptionParser::WriteDump(LPEXCEPTION_POINTERS pException)
{
	HANDLE hDumpFile;

//    int idx = m_ppFinalizers[0]->GetIdx();
	LPCTSTR szModulePath=GetModulePath();
	char szDumpFileName[MAX_PATH];
	SYSTEMTIME CurTime;

	PrintLog("开始制作Dump文件");

	GetLocalTime(&CurTime);

	sprintf(szDumpFileName,"%s\\Dump%d-%02d-%02d %02d-%02d-%02d.dmp",
		(LPCTSTR)szModulePath,
		CurTime.wYear,CurTime.wMonth,CurTime.wDay,
		CurTime.wHour,CurTime.wMinute,CurTime.wSecond);

	hDumpFile=CreateFile(szDumpFileName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hDumpFile!=INVALID_HANDLE_VALUE)
	{		
		PrintLog("开始写入Dump文件%s",szDumpFileName);

		static DWORD ProcessID=GetCurrentProcessId();

		static MINIDUMP_EXCEPTION_INFORMATION ExceptionInfo;
		static MINIDUMP_EXCEPTION_INFORMATION * pExceptionInfo=NULL;

		if(pException)
		{
			ExceptionInfo.ThreadId=GetCurrentThreadId();
			ExceptionInfo.ExceptionPointers=pException;
			ExceptionInfo.ClientPointers=true;
			pExceptionInfo=&ExceptionInfo;
		}
		
		

		if(MiniDumpWriteDump(m_hProcess,ProcessID,hDumpFile,
			MiniDumpNormal,pExceptionInfo,NULL,NULL))
		{
			PrintLog("写入Dump文件成功%s",(LPCTSTR)szDumpFileName);
			CloseHandle(hDumpFile);
			return TRUE;
		}
		else
		{
			PrintLog("写入Dump文件失败%s",(LPCTSTR)szDumpFileName);
		}
		CloseHandle(hDumpFile);
		
	}
	else
	{
		PrintLog("打开Dump文件失败%s",(LPCTSTR)szDumpFileName);
	}

	return FALSE;
}

void CExceptionParser::InvalidParameterHandler(const wchar_t * expression,const wchar_t * function,const wchar_t * file,unsigned int line,uintptr_t pReserved)
{
	//PrintLog("非法的调用参数");
}

void CExceptionParser::SignalHandler(int signal)
{
	//PrintLog("系统提示%d",signal);
}

void CExceptionParser::PrintLog(const char * szFormat,...)
{
	static char szLog[5000];
	va_list vl;

	va_start(vl,szFormat);
	vsprintf(szLog,szFormat,vl);
	strcat(szLog,"\r\n");
	DWORD WriteSize;
	WriteFile(m_hLogFile,szLog,(DWORD)strlen(szLog),&WriteSize,NULL);
	va_end(vl);
}

void CExceptionParser::AddFinalizer(IfFinalizer *pFinalizer)
{
    if (m_finalizerNr < MAX_FINALIZER_NR)
    {
        m_ppFinalizers[m_finalizerNr] = pFinalizer;
        m_finalizerNr ++;
    }
}


}
