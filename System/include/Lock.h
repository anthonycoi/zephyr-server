/*-------------------------------------------------------------
 Copyright (C)| 2008-2010 TDGame Info. Co. Ltd.
 File: Lock.h
 Author: Yipeng.Zhang
 Date: 2009-3-6 9:43:44
 Description:我们把它放在System目录下，是不希望应用层使用锁，实际上应用层都应改是单线程的，
             线程间的交互走平台的远程调用，不需要锁
 Others:
 -------------------------------------------------------------*/

#ifndef __THREAD_LOCK_H__
#define __THREAD_LOCK_H__

//#include <SysMacros.h>
#include "SysInc.h"

namespace Zephyr
{


#ifdef WIN32
class Win32Lock
{
protected:
	CRITICAL_SECTION m_cs;
public:
    void Lock() 
        { 
            EnterCriticalSection(&m_cs);
        }
	bool TryLock()
        { 
		    return (TryEnterCriticalSection(&m_cs)==TRUE); 
	    }
	void Unlock()
        {
            LeaveCriticalSection(&m_cs);
        }

	Win32Lock()
        { 
            InitializeCriticalSection(&m_cs);
        }
	~Win32Lock() 
        {
            DeleteCriticalSection(&m_cs);
        }
};
typedef Win32Lock CLock;

#else
class LinuxLock
{
protected:
    pthread_mutex_t m_mutex;
public:
	void Lock() 
        { 
            pthread_mutex_lock(&m_mutex); 
        }
	bool TryLock() 
        { 
            return (pthread_mutex_trylock(&m_mutex)==0);
        }
	void Unlock() 
        { 
            pthread_mutex_unlock(&m_mutex);
        }

    LinuxLock() 
        { 
            pthread_mutex_init(&m_mutex,NULL); 
        }
	~ LinuxLock() 
        { 
            pthread_mutex_destroy(&m_mutex); 
        }
};

typedef LinuxLock  CLock;
#endif

class CAutoLock
{
protected:
	CLock& m_lock;
public:
    CAutoLock(CLock& lock):m_lock(lock) 
    { 
        m_lock.Lock();
    }
	~CAutoLock() 
    {
        m_lock.Unlock();
    }
};

class CAutoLockP
{
protected:
	CLock		*m_pLock;
public:
	CAutoLockP(CLock& Lock)
	{
		m_pLock=&Lock;
		m_pLock->Lock();
	}
	~CAutoLockP()
	{
		if(m_pLock)
		{
			m_pLock->Unlock();
		}
	}
};

};

#endif

