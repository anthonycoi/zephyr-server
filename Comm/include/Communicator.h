#ifndef __ZEPHYR_COMM_COMMUNICATOR_H__
#define __ZEPHYR_COMM_COMMUNICATOR_H__

#include "TypeDef.h"
#include "IfCommunicator.h"
#include "Pipe.h"
#include "../../Public/include/Clock.h"
#include "../../Public/Interface/Platform/include/IfTask.h"
#include "../../System/include/ProducerAndConsumer.h"
#include "../Net/win/include/NetEventQueue.h"

namespace Zephyr
{

class CCommMgr;

class CCommunicator : public IfCommunicator
{
protected:
    CPipe   m_inPipe;
    CPipe   m_outPipe;
    TUChar* m_pBuff;
    TUInt32 m_buffSize;

    CClock *m_pClock;
    TUInt64  m_nLastBlockTimes;
    
    CIoEventQueue<CConnectionEvent> m_tNetEventQueue; 
    TUInt32 m_uLastBlockedTime;
    
public:
    CCommunicator();
    ~CCommunicator();
    TInt32 Init(CClock *pTimeSystem,TUInt32 inPipeSize,TUInt32 outPipeSize,TUInt32 maxMessageSize);
    TInt32 InitEventPool(TUInt32 maxEventNr);
public:

    //add a new idx , that indicate the pipe to use!
    virtual CMessageHeader *GetMsg(TUInt32 needRetMsgBuff = 1);

    virtual void ReturnMsgBuff(CMessageHeader *pMsg);

    virtual CMessageHeader *PrepareMsg(TInt32 bodyLength,TUInt32 methodId,CDoid srcId,CDoid* pDestDoid,TInt32 destDoidNum,bool bRearrangeDest);

    //virtual int SendMsg(SCTDMessageHeader *,bool bNeedCopy = false) = 0;
    //do not need the para bNeedCopy any more, I will check it!`
    virtual TInt32 SendMsg(CMessageHeader *pMsg);

    
    virtual TInt32 GetNetEvent(CConnectionEvent& result);

 /*   virtual int GetEvent(CConnectionEvent &event);*/

    virtual const CClock *GetClock();
    //注意，有阻塞
    void AddNetEvent(CConnectionEvent event);
    
protected:
    //供commMgr使用.
    CMessageHeader *GetAppMsg(TUChar *pBuff)
    {
        //if (m_)
        TUInt32 dataLen = m_outPipe.GetDataLen();
        if (dataLen > 0)
        {
            CMessageHeader *pMsg = (CMessageHeader*)m_outPipe.Peek(pBuff,sizeof(CMessageHeader));
            if (pMsg->GetLength() > dataLen)
            {
                return NULL;
            }
            TUInt32 len = pMsg->GetLength();
            dataLen = len;
            //这个其实肯定有
            TUChar *pData = m_outPipe.GetData(len);
            
            if (dataLen > len)
            {
                m_outPipe.ReadData(pBuff,dataLen);
                pMsg = (CMessageHeader *)pBuff;
            }
            else
            {
                pMsg = (CMessageHeader*)pData;
            }
            return pMsg;
        }
        return NULL;
    }
    void AppMsgSent(CMessageHeader *pMsg)
    {
        m_outPipe.ReturnMsgBuff((TUChar*)pMsg,pMsg->GetLength());
    }

    TInt32 AddNetMsg(CMessageHeader *pMsg);

friend class CCommMgr;
};

}

#endif
