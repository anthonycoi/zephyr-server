#ifndef __ZEPHYR_COMM_NET_NET_EVENT_QUEUE_H__
#define __ZEPHYR_COMM_NET_NET_EVENT_QUEUE_H__\



#include "TypeDef.h"
#include "SysMacros.h"
#include "Pipe.h"
#include "IfTask.h"
#include "../../../../System/include/ProducerAndConsumer.h"
#include "stdio.h"


namespace Zephyr
{

struct TIOEvent
{
    TUInt16                     m_connectionIdx;
    TUInt16                     m_connectionEvents;
    TUInt32                     m_seqNum;
};

template<class TEVENT>
class CIoEventQueue
{
private:
    CPipe   m_pipe;
    CProduerAndConsumer m_tPAndC;
    volatile TInt32 m_nBlocked;
public:
    TInt32  Init(TInt32 maxEventNr)
    {
        return m_pipe.Init(maxEventNr * sizeof(TEVENT));
    }
    TInt32  AddEvent(TEVENT &event,TInt32 nBlockTime)
    {
        TUInt32 len = sizeof(TEVENT);
        TUChar *pMsg = m_pipe.PrepareMsg(len);
        if(NULL == pMsg)
        {
            //����������,���Ҫ�ȴ�
            if (nBlockTime)
            {
                m_nBlocked = 1;//�ȸ�block��־һ��

                //��0�ȴ�ȡһ�Σ���ι�����������һ���������ź�,ͬʱ����
                m_tPAndC.RequireFetchProduct(0);
                {
                    len = sizeof(TEVENT);
                    pMsg = m_pipe.PrepareMsg(len);
                }
                if (pMsg)
                {
                    m_nBlocked = 0;
                }
                else
                {
                    m_tPAndC.RequireFetchProduct(nBlockTime);
                    m_nBlocked = 0;
                    len = sizeof(TEVENT);
                    pMsg = m_pipe.PrepareMsg(len);
                }
            }
        }
        if (pMsg)
        {
            //��Ӧ�ã�
#ifdef _DEBUG
            if (len < sizeof(TEVENT))
            {
                printf("Something wrong?!");
                return NOT_INITIALIZED;
            }
#endif
            return m_pipe.WriteData((TUChar*)&event,sizeof(TEVENT));
        }
        return OUT_OF_MEM;
    }
    TEVENT *GetEvent()
    {
        TUInt32 len = sizeof(TEVENT);
        TUChar *pData = m_pipe.GetData(len);

        
        if (pData)
        {
            if (len < sizeof(TEVENT))
            {
                return NULL;
            }
        }
        return (TEVENT*)pData;
    }
    void ConfirmHandleEvent(TEVENT *pEvent)
    {
        m_pipe.ReturnMsgBuff((TUChar*)pEvent,sizeof(TEVENT));
        if (m_nBlocked)
        {
            if (m_pipe.GetFreeLen() >= sizeof(TEVENT))
            {
                m_tPAndC.OnProduced();
            }
        }
        
    }
};

class CNetEventQueues
{
private:
    CIoEventQueue<TIOEvent> m_netQueue;
    CIoEventQueue<TIOEvent> m_appQueue;
public:
    TInt32      Init(TInt32 maxEventNr)
    {
        TInt32 rtn = m_netQueue.Init(maxEventNr);
        if (SUCCESS > rtn)
        {
            //����Ͳ����ڴ����ʧ�ܺ�Ĵ����ˣ�Ӧ�ö��ɹ���.ʧ���˾Ͱѳ������
            return rtn;
        }
        return m_appQueue.Init(maxEventNr);
    }
    TInt32      AddNetEvent(TIOEvent &event,TInt32 nBlockTime)
    {
        //TInt32 rtn = 
        return m_netQueue.AddEvent(event,nBlockTime);
    }
    TIOEvent    *GetNetEvent()
    {
        return m_netQueue.GetEvent();
    }
    void        ConfirmHandleNetEvent(TIOEvent *pEvent)
    {
        m_netQueue.ConfirmHandleEvent(pEvent);
    }

    TInt32      AddAppEvent(TIOEvent &event,TInt32 nBlockTime)
    {
        return m_appQueue.AddEvent(event,nBlockTime);
    }
    TIOEvent    *GetAppEvent()
    {
        return m_appQueue.GetEvent();
    }
    void        ConfirmHandleAppEvent(TIOEvent *pEvent)
    {
        m_appQueue.ConfirmHandleEvent(pEvent);
    }
};


};


#endif
