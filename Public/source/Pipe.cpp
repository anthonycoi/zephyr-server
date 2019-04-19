
#include "Pipe.h"
#include <memory.h>
#include "SysMacros.h"


namespace Zephyr
{

TUChar *CPipe::GetData(TUInt32& len)
{
    if (GetDataLen() < len)
    {
    	return NULL;
    }
	TUChar *pRear = (TUChar *)(m_pRear);
	TUChar *pHeader = (TUChar *)(m_pHeader);
    if (pRear <= pHeader)
    {
        len = pHeader - pRear;
    }
    else 
    {
        len = (m_pMemPool + m_memPoolSize - pRear);
    }
	
    return pRear;


}

TInt32 CPipe::ReturnMsgBuff(TUChar *pMsg,TUInt32 len)
{
	TUInt32 dataLen = GetDataLen();
	if (len > dataLen)
    {
        return INPUT_PARA_ERROR;
    }
    if (m_pRear != pMsg)
    {
        return SUCCESS;
    }
    TUChar * pPipeRear = (m_pMemPool + m_memPoolSize);
    TUChar *pNewRear = (TUChar*)m_pRear;
    pNewRear += len;
    //m_pRear += len;
#ifdef _DEBUG
    if (pNewRear > pPipeRear)
    {
        pNewRear = m_pMemPool + (pPipeRear - m_pRear);
        //cout<<"Error in CPipe::PrepareMsg, wrong format!"<<endl;
        return INPUT_PARA_ERROR;
    }
#endif
    if (pNewRear == pPipeRear)
    {
        m_pRear = m_pMemPool;
    }
    else
    {
        m_pRear = pNewRear;
    }
    return SUCCESS;

}



TUChar *CPipe::PrepareMsg(TUInt32& len)
{
    TInt32 nFreeLen = GetFreeLen();
	if (len > nFreeLen)
	{
		len = 0;
        //cout<<"Error in CPipe::PrepareMsg, out of memory!"<<endl;
		return NULL;
	}
	TUChar *pRear = (TUChar *)(m_pRear);
	TUChar *pHeader = (TUChar *)(m_pHeader);
    if (pRear > pHeader)
    {
        //这儿不能留一个字节，不然就会阻塞在pipe尾部
        len = (pRear - pHeader)-1;
    }
    else
    {
        //这儿不能留一个字节，不然就会阻塞在pipe尾部
        len = (m_pMemPool + m_memPoolSize - pHeader);
    }

    //这个就是
    if (len > nFreeLen) 
    {
        len = nFreeLen;
    }
#ifdef _DEBUG
    if (!len)
    {
        if (m_pRear > m_pHeader)
        {
            len = (m_pRear - m_pHeader);
        }
        else
        {
            len = (m_pMemPool + m_memPoolSize - m_pHeader);
        }
    }
#endif
    return pHeader;
}

TInt32 CPipe::ConfirmAddMsg(TUChar *pMem,TUInt32 len)
{
    #ifdef _DEBUG
        m_msgRecvd += len;
	#endif
    if (pMem != m_pHeader)
    {
        //cout<<"Error in CPipe::ConfirmAddMsg, wrong pMem!"<<endl;
        return FAIL;
    }

    TUChar *pPipeRear = (m_pMemPool + m_memPoolSize);
    TUChar *pNewHeader = (TUChar*)m_pHeader;
    
    pNewHeader += len;
#ifdef _DEBUG
    if (m_pHeader > pPipeRear)
    {
        //some thing wrong!
        //cout<<"something wrong in ConfirmAddMsg"<<endl;
        m_pHeader = m_pMemPool;

		return FAIL;

    }
#endif
    if (pNewHeader == pPipeRear)
    {
        m_pHeader = m_pMemPool;
    }
    else
    {
        m_pHeader = pNewHeader;
    }
	return SUCCESS;
}

TUChar *CPipe::Peek(TUChar *pBuffer,TUInt32 len)
{
    TUInt32 result = GetDataLen();
    if (result < len)
    {
        return NULL;
    }
    TUInt32 buffLen = 0;
    if (m_pHeader >= m_pRear)
    {
        buffLen = m_pHeader - m_pRear;
        if (len <= buffLen)
        {
            //memcpy(pBuffer,m_pRear,len);
            return (TUChar*)(m_pRear);
        }
        else
        {
            //not enough data;something wrong
            return NULL;
        }
    }
    else
    {
        buffLen = m_pMemPool + m_memPoolSize - m_pRear;
        if (buffLen >= len)
        {
            //memcpy(pBuffer,(const void*)(m_pRear),len);
            return (TUChar*)(m_pRear);
        }
        else
        {
            memcpy(pBuffer,(const void*)(m_pRear),buffLen);
            memcpy(pBuffer + buffLen,m_pMemPool,(len - buffLen));
            return pBuffer;
        }
    }
}

TUInt32 CPipe::GetDataLen()
{
	TUChar *pHeader = (TUChar*)m_pHeader;
	TUChar *pRear   = (TUChar*)m_pRear;
    if ((pHeader >= pRear))
    {
        return (TUInt32)(pHeader - pRear);
    }
    return (m_memPoolSize - (pRear - pHeader));
}


TInt32 CPipe::GetFreeLen()
{
    TUChar *pHeader = (TUChar*)m_pHeader;
	TUChar *pRear   = (TUChar*)m_pRear;
	TInt32 result = 0;
    if (pHeader < pRear)
    {
        //to make sure that m_pHeader would never catch up with m_pUsed;
        result = ( ( pRear - pHeader ) - 1 );
    }
	else
	{
		result = ((m_memPoolSize - (pHeader - pRear)) - 1 );
	}
#ifdef _DEBUG
    if (result < 0)
    {
        if (m_pHeader < m_pRear)
        {
            //to make sure that m_pHeader would never catch up with m_pUsed;
            result = ( ( m_pRear - m_pHeader ) - 1 );
        }
        result = ((m_memPoolSize - (m_pHeader - m_pRear)) - 1 );
    }
#endif
    return result;
}

TInt32 CPipe::GetAvailFreeLen()
{
    TInt32 len;
    TUChar *pRear = (TUChar *)(m_pRear);
    TUChar *pHeader = (TUChar *)(m_pHeader);
    if (pRear > pHeader)
    {
        len = (pRear - pHeader);
    }
    else
    {
        len = (m_pMemPool + m_memPoolSize - pHeader);
    }
    return len;
}

TInt32 CPipe::WriteData(TUChar *pSrc,TUInt32 len)
{
    TInt32 freeLen = GetFreeLen();
	if (freeLen <= 0)
	{
		return OUT_OF_MEM;
	}
	if (freeLen < len)
	{
		return OUT_OF_MEM;
	}
    TUChar *pRear = (TUChar *)(m_pRear);
	TUChar *pHeader = (TUChar *)(m_pHeader);
#ifdef _DEBUG
    m_msgRecvd += len;
	bool needRecycle = false;
#endif
    if (pRear > pHeader)
    {
        freeLen = (pRear - pHeader);
    }
    else
    {
#ifdef _DEBUG
		needRecycle = true;
#endif
        freeLen = (m_pMemPool + m_memPoolSize - pHeader);
    }

	
	if (freeLen >= len)
	{
		memcpy(pHeader,pSrc,len);
		pHeader += len;
		if (pHeader == (m_pMemPool + m_memPoolSize))
		{
			m_pHeader = m_pMemPool;
		}
		else
		{
			m_pHeader = pHeader;
		}
	}
	else
	{
#ifdef _DEBUG
		if (!needRecycle)
		{
			return FAIL;
		}
#endif
		memcpy(pHeader,pSrc,freeLen);
		TInt32 remainLen = len - freeLen;
		memcpy(m_pMemPool,(pSrc+freeLen),remainLen);
		m_pHeader =  m_pMemPool + remainLen;
	}
    return len;
}

//读数据到pData
TInt32 CPipe::ReadData(TUChar *pTo,TUInt32 len)
{
    TUInt32 dataLen = GetDataLen();
    if (len > dataLen)
    {
        return OUT_OF_RANGE;
    }
    TUChar *pRear = (TUChar *)(m_pRear);
    TUChar *pHeader = (TUChar *)(m_pHeader);
    
#ifdef _DEBUG
    bool needRecycle = false;
#endif
    if (pRear < pHeader)
    {
        dataLen = (pHeader - pRear);
    }
    else
    {
#ifdef _DEBUG
        needRecycle = true;
#endif
        dataLen = (m_pMemPool + m_memPoolSize - pRear);
    }
    
    if (dataLen >= len)
    {
        memcpy(pTo,pRear,len);
        pRear += len;
        if (pRear == (m_pMemPool + m_memPoolSize))
        {
            m_pRear = m_pMemPool;
        }
        else
        {
            m_pRear = pRear;
        }
    }
    else
    {
#ifdef _DEBUG
        if (!needRecycle)
        {
            return FAIL;
        }
#endif
        memcpy(pTo,pRear,dataLen);
        TInt32 remainLen = len - dataLen;
        memcpy((pTo+dataLen),m_pMemPool,remainLen);
        m_pRear =  m_pMemPool + remainLen;
    }
    return len;
}


}

