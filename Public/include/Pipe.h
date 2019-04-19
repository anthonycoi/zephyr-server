/*-------------------------------------------------------------
 Copyright (C)| 2008-2010 T
 File: Pipe.h
 Author: Zephyr Shannon
 Date: 2009-3-16 16:09:12
 Description: this is a list file for win32 or linux platform
 Others:
 -------------------------------------------------------------*/


#ifndef __ZEPHYR_PIPE_H__
#define __ZEPHYR_PIPE_H__
#include "TypeDef.h"
#include "SysMacros.h"

namespace Zephyr
{

#define MIN_PIPE_LENGTH (128 * 4)

class CPipe
{
private:
    TUChar *m_pMemPool;
    volatile TUChar *m_pHeader;
    volatile TUChar *m_pRear;

    TUInt32 m_memPoolSize;
public:
#ifdef _DEBUG
    TUInt32 m_msgRecvd;
#endif
    TUInt32 GetDataPiece()
    {
        TUChar *pHeader = (TUChar*)m_pHeader;
        TUChar *pRear   = (TUChar*)m_pRear;
        if (pHeader == pRear)
        {
            return 0;
        }
        if (m_pHeader > m_pRear)
        {
            return 1;
        }
        else if(m_pHeader == m_pMemPool)
        {
            return 1;
        }
        return 2;
    }
    TUChar *GetSndPiece(TUInt32 &len)
    {
        len = (TUInt32)((m_pHeader) - (m_pMemPool));
        return m_pMemPool;
    }
    CPipe(TUInt32 pipeSize,TInt32& result)
    {
        if (MIN_PIPE_LENGTH > pipeSize)
        {
            pipeSize = MIN_PIPE_LENGTH;
        }
        result = SUCCESS;
        m_pMemPool = new TUChar[pipeSize];
        if (NULL == m_pMemPool)
        {
            result = OUT_OF_MEM;
            return;
        }
        m_pRear = m_pMemPool;
        m_pHeader = m_pMemPool;
        m_memPoolSize = pipeSize;
        result = SUCCESS;
        #ifdef _DEBUG
        m_msgRecvd = 0;
        #endif
    }
    CPipe()
    {
        m_pMemPool = NULL;
        m_pHeader  = NULL;
        m_pRear    = NULL;
        m_memPoolSize = 0;
        #ifdef _DEBUG
        m_msgRecvd = 0;
        #endif
    }
    TInt32 Init(TInt32 pipeSize)
    {
        TUInt32 uAddr= ((TUInt32)this); //只要最后几位就够了
        if (uAddr % (sizeof(void*)))
        {
            return MEM_NOT_ALIGNED;
        }
        if (MIN_PIPE_LENGTH > pipeSize)
        {
            pipeSize = MIN_PIPE_LENGTH;
        }
        if (!m_pMemPool)
        {
            delete [] m_pMemPool;
            m_pMemPool = NULL;
        }
        m_pMemPool = new TUChar[pipeSize];
        if (NULL == m_pMemPool)
        {
            return OUT_OF_MEM;
        }
        m_pRear = m_pMemPool;
        m_pHeader = m_pMemPool;
        m_memPoolSize = pipeSize;

        #ifdef _DEBUG
        m_msgRecvd = 0;
        #endif
        return SUCCESS;
    }
    TInt32 Init(TUChar *pBuff,TInt32 pipeSize)
    {
        
        m_pMemPool = pBuff;
        if (NULL == m_pMemPool)
        {
            return OUT_OF_MEM;
        }
        m_pRear = m_pMemPool;
        m_pHeader = m_pMemPool;
        m_memPoolSize = pipeSize;

        #ifdef _DEBUG
        m_msgRecvd = 0;
        #endif
        return SUCCESS;
    }
	~CPipe()
	{
		if(m_pMemPool)
		{
			delete[] m_pMemPool;
			m_pMemPool = NULL;
		}
	}

    TUChar *GetDataHeader()
    {
        return (TUChar *)m_pHeader;
    }

    TUChar *GetData(TUInt32& len);


    TInt32 ReturnMsgBuff(TUChar *pMsg,TUInt32 len);

    TUChar *PrepareMsg(TUInt32& len);

    TInt32 ConfirmAddMsg(TUChar *pMem,TUInt32 len);

    TUChar *Peek(TUChar *pBuffer,TUInt32 len);

    TUInt32 GetDataLen();

    TInt32 GetFreeLen();
    
    TInt32 GetAvailFreeLen();

    TBOOL IsInPipe(TUChar *pBuffer)
    {
        if ( (m_pMemPool > pBuffer) && (m_pMemPool + m_memPoolSize <= pBuffer) )
        {
            return false;
        }
        return true;
    }
    void Reset()
    {
        m_pRear = m_pMemPool;
        m_pHeader = m_pMemPool;
        #ifdef _DEBUG
        m_msgRecvd = 0;
        #endif
    }
public:

    TInt32 WriteData(TUChar *pDest,TUInt32 len);
   
    TInt32 ReadData(TUChar *pTo,TUInt32 len);
};

}

#endif
