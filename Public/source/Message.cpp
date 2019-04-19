#include "Message.h"
#include "sysMacros.h"
#include <string.h>
#include "Tpl/include/tplmap.h"
#include <algorithm>



namespace Zephyr
{

TInt32 CMessageHeader::Init(TUInt32 bodyLength,TUInt32 methodId,CDoid srcId,CDoid* pDestDoids,TUInt32 destDoidNum,bool bNeedRearrangeDestDoid)
{
    if ((destDoidNum > 63) || (NULL == pDestDoids) || (bodyLength < 0) || (methodId > 0x3FFFF))
    {
        return INPUT_PARA_ERROR;
    }
    m_msgInfo.m_data = 0;
    //TUInt32 length = sizeof(CMessageHeader) + sizeof(CDoid) * (destDoidNum -1);
    m_msgInfo.m_msgBodyLength = bodyLength;
    m_msgInfo.m_methodId  = methodId;
    m_srcDoid   = srcId;
    m_msgInfo.m_nrOfBroadcastDoid = (destDoidNum-1);
    
    if (destDoidNum > 1)
    {	
        if (bNeedRearrangeDestDoid)
        {
            std::sort(pDestDoids,pDestDoids+destDoidNum);
        }
        memcpy((void*)GetBroadcastDoids(),(void*)(pDestDoids+1),(sizeof(CDoid)*(destDoidNum-1)));
    }
    m_destDoid  = *pDestDoids;
    /* do it when actived this message!
    for(int i =0;i<(sizeof(SCTDMessageHeader)-2);i+=2)
    {
        m_checkSum[0] = m_checkSum[0] ^ pBuffer[i];
        m_checkSum[1] = m_checkSum[1] ^ pBuffer[i+1];
    }
    */
    return SUCCESS;
}


TInt32 CMessageHeader::ResetBodyLength(TUInt32 bodyLength)
{
	TUInt32 destDoidNum = m_msgInfo.m_nrOfBroadcastDoid;
	TUInt32 OldBodyLength = m_msgInfo.m_msgBodyLength;
	
	if(bodyLength>OldBodyLength)
	{
	    return OUT_OF_RANGE;
	}
	else
	{
		CDoid  *pOldDest = GetDestDoidByIdx(1);
		m_msgInfo.m_msgBodyLength = bodyLength;
		
		CDoid  *pNewDest = GetDestDoidByIdx(1);
		if (destDoidNum > 1)
		{
			memmove(pNewDest,pOldDest,(sizeof(CDoid)*(destDoidNum)));
		}
		return SUCCESS;
	}
}
//不检查有效性，有调用者负责
void CMessageHeader::ReInitMsg4Send(TInt32 fromDest,TInt32 to)
{
    if (to >= fromDest)
    {
        int nrOfBroadcastDoid = to - fromDest;
        if (0 != fromDest)
        {
            memcpy(&m_destDoid,GetDestDoidByIdx(fromDest),sizeof(CDoid));
        }
        else
        {
            m_msgInfo.m_nrOfBroadcastDoid  = nrOfBroadcastDoid;
            return ;
        }
        ++fromDest;
        if(nrOfBroadcastDoid)
        {
            memmove(((TUChar*)this + sizeof(CMessageHeader) + GetBodyLength()),GetDestDoidByIdx(fromDest),sizeof(CDoid)*(nrOfBroadcastDoid));
        }
        m_msgInfo.m_nrOfBroadcastDoid  = nrOfBroadcastDoid;
    }
    else
    {
#ifdef _DEBUG
        printf("FromDest is not less than to!");
#endif
    }
}

}
//to make sure you could only create the message by msgPool.
