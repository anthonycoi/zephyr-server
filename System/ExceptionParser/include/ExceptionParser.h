#ifndef __ZEPHYR_SYSTEM_EXCEPTION_PARSER_EXCEPTION_PARSER_H__
#define __ZEPHYR_SYSTEM_EXCEPTION_PARSER_EXCEPTION_PARSER_H__


#ifdef WIN32

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif

#include "windows.h"
#include <tchar.h>
#include <eh.h>

#else

#include <dlfcn.h>

#endif




#include <Dbghelp.h>

#include "../../../Public/Interface/Platform/include/IfFinalizer.h"




#define MAX_SYMBOL_NAME		1024

namespace Zephyr
{

enum EXCEPTION_HANDLE_MODE
{
	EXCEPTION_SET_DEFAULT_HANDLER=1,
	EXCEPTION_SET_TRANSLATOR=(1<<1),
	EXCEPTION_USE_API_HOOK=(1<<2),
};

template <class T>
class CStaticObject
{
public:
    static T * GetInstance()
    {
        //if( m_pInstance == NULL )
        //	m_pInstance = new T;
        return &m_Instance;
    }
protected:
    static T m_Instance;
};
template <class T>
T  CStaticObject<T>::m_Instance;



#define MAX_FINALIZER_NR 16

class CExceptionParser :
	public CStaticObject<CExceptionParser>
{
protected:
	struct SYMBOL_INFO_EX:public SYMBOL_INFO
	{
		char NameBuffer[MAX_SYMBOL_NAME];
		SYMBOL_INFO_EX()
		{
			ZeroMemory(this,sizeof(*this));
			MaxNameLen=MAX_SYMBOL_NAME;
		}
	};

	struct ADDRESS_INFO
	{
		DWORD64			Address;
		HINSTANCE		hInstance;
		char			ModuleName[MAX_PATH];
		SYMBOL_INFO_EX	SymbolInfo;
		char			CppFileName[MAX_PATH];
		int				LineNumber;
		ADDRESS_INFO()
		{
			Address=0;
			hInstance=NULL;
			ModuleName[0]=0;
			CppFileName[0]=0;
			LineNumber=-1;
		}
	};

	//CEasyCriticalSection	m_CriticalSection;
	HANDLE					m_hLogFile;
	HANDLE					m_hProcess;
	int						m_ExceptionCount;
	IfFinalizer              *m_ppFinalizers[MAX_FINALIZER_NR];
	int                     m_finalizerNr;
	
public:
	CExceptionParser(void);
	~CExceptionParser(void);

	void Init(UINT HandleMode);

	void ParseException(LPEXCEPTION_POINTERS pException);

	static LONG ExceptionHander(LPEXCEPTION_POINTERS pException);
	static void ExceptionTranslator(UINT Code,LPEXCEPTION_POINTERS pException);

	BOOL WriteDump(LPEXCEPTION_POINTERS pException);
	
    void AddFinalizer(IfFinalizer *pFinalizer);

protected:
	void ParseCallStack(PCONTEXT pContextRecord,UINT MaxLoopCount=16);	

	BOOL GetAddressInfo(DWORD64 Address,ADDRESS_INFO * pAddressInfo);


	static void InvalidParameterHandler(
		const wchar_t * expression,
		const wchar_t * function, 
		const wchar_t * file, 
		unsigned int line,
		uintptr_t pReserved
		);

	static void SignalHandler(int signal);	

	void PrintLog(const char * szFormat,...);
	
	
};


}

#endif
