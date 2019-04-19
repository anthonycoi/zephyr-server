#include "CommTester.h"
#include "../Config/include/SettingFile.h"

CDoid *CCommTester::sm_pDoids = NULL;

TInt32 CCommTester::Run(const TInt32 threadId,const TInt32 runCnt)
{
    if(m_nBeginTime) //先跳过一段时间
    {
        unsigned long long timeNow = m_pComms->GetClock()->GetPlatformTime();
        if (timeNow > m_nBeginTime)
        {
            if ((timeNow-m_nBeginTime) >  10000)
            {
                m_nBeginTime = 0;
                //发消息
               
            }
        }
    }
    CConnectionEvent event;
    
    int eventNr = m_pComms->GetNetEvent(event);
    int usedCnt = 0;
    while (eventNr>=0)
    {
        if (event.m_nEvent == en_connection_is_established_event)
        {
            char szBuffer[64];
            m_tSvrDoid.ToStr(szBuffer);
            printf("connected from(%s) to (%u,%u)\n\r",szBuffer,(TUInt32)event.m_nNodeId,(TUInt32)event.m_nVip);
            ++m_bIsConnected;
        }
        else
        {
            //断链了
            char szBuffer[64];
            m_tSvrDoid.ToStr(szBuffer);
            printf("connection breaked from(%s) to (%u,%u)\n\r",szBuffer,(TUInt32)event.m_nNodeId,(TUInt32)event.m_nVip);
            --m_bIsConnected;
            m_nSendTime = 0;
        }
        ++usedCnt;
        eventNr = m_pComms->GetNetEvent(event);
    }
    if (m_bIsConnected < m_nIpNr)
    {
        return usedCnt;
    }
    if (0 == m_nSendTime) //最新连接，要发初始消息
    {
        m_nLastGetAllRespTime = m_pComms->GetClock()->GetLocalTime();
        SendAllMessage();
        ++m_nSendTime;
    }
    
    for (int i=usedCnt;i<runCnt;++i)
    {
        CMessageHeader *pMsg = m_pComms->GetMsg();
        while (pMsg)
        {
            int id = pMsg->GetMethodId();
            if (id != 400)
            {
                printf("Msg Len Incorrect!");
            }
            if (id != pMsg->GetBodyLength())
            {
                printf("Msg incorrect!");
            }
            else
            {
                TUChar *pD = pMsg->GetBody();
                for (unsigned int i = 0;i<id ;++i)
                {
                    TUChar c = i;
                    if (c != pD[i])
                    {
                        printf("Msg incorrect!");
                    }
                }
            }
            OnRecv(pMsg->GetSrcDoid());

            ++usedCnt;
            
            m_pComms->ReturnMsgBuff(pMsg);
            if (usedCnt >= runCnt)
            {
                return usedCnt;
            }
            if (0==(m_nMsgReced%m_nDoidNr))
            {
                
                int gap = m_pComms->GetClock()->GetTimeGap(m_nLastGetAllRespTime);
                m_nLastGetAllRespTime = m_pComms->GetClock()->GetLocalTime();
                m_nTotalSendGap += gap;
                if (gap < m_nMinSendGap)
                {
                    m_nMinSendGap = gap;
                }
                if (gap > m_nMaxSendGap)
                {
                    m_nMaxSendGap = gap;
                }
                ++m_nSendTime ;
                if (0 == (m_nSendTime%10000))
                {
                    printf("MinSendGap:%u,MaxSendGap:%u,totalSendGap:%u,sendTime:%u,averageSendGap:%u \n\t",m_nMinSendGap,m_nMaxSendGap,m_nTotalSendGap,m_nSendTime,(m_nTotalSendGap/m_nSendTime));
                }
                SendAllMessage();
            }
            pMsg = m_pComms->GetMsg();
        }
        //是m_nSrvNr就群发
    }
    //CheckAll();
    return usedCnt;
}

void CCommTester::OnStartTestOne(int nInitMsgNr,int nInitMsgLen,int srvNr,int nIpNr,int nNodeNr)
{
    
    {
        int totalNr = srvNr;
        if (nIpNr>0)
        {
            totalNr *= nIpNr;
        }
        else
        {
            nIpNr = 1;
        }
        if (nNodeNr>0)
        {
            totalNr *= nNodeNr;
        }
        else
        {
            nNodeNr = 1;
        }
        if (!sm_pDoids)
        {
            sm_pDoids = new CDoid[totalNr];
            for (int n=0;n<nNodeNr;++n)
            {
                for (int ip=0;ip<nIpNr;++ip)
                {
                    for (int i = 0;i<srvNr;++i)
                    {
                        int offset = n*nIpNr*srvNr+ip*srvNr+i;
                        sm_pDoids[offset].m_nodeId = n;
                        sm_pDoids[offset].m_virtualIp = ip;
                        sm_pDoids[offset].m_srvId = i * MAX_SERVICE_NR_PER_COMM;
                        sm_pDoids[offset].m_objId = 0;
                    }
                }
            }
        }
        m_nDoidNr = totalNr;
    }
   

    m_pMsgRecords = new unsigned char[m_nDoidNr];
    memset(m_pMsgRecords,0,m_nDoidNr);
    m_nInitSendMgrNr = nInitMsgNr;
    m_nInitSendMgrLen  = nInitMsgLen;
    m_nSrvNr = srvNr;
    m_nIpNr = nIpNr;
    m_nNodeNr = nNodeNr;
    m_nBeginTime = m_pComms->GetClock()->GetPlatformTime();
    if (0 == m_nBeginTime) //确保不为0
    {
        m_nBeginTime = 1;
    }
    
    //for (int i = 0;i<nInitMsgNr;++i)
    
}

void CCommTester::OnStartTestOne(int nInitMsgNr,int nInitMsgLen,int srvNr)
{
    if (!sm_pDoids)
    {
        sm_pDoids = new CDoid[srvNr];
        for (int i = 0;i<srvNr;++i)
        {
            sm_pDoids[i] = m_tSvrDoid;
            sm_pDoids[i].m_srvId = i;
        }
    }
    m_nInitSendMgrNr = nInitMsgNr;
    m_nInitSendMgrLen  = nInitMsgLen;
    m_nSrvNr = srvNr;
    CDoid dest = m_tSvrDoid;
    //for (int i = 0;i<nInitMsgNr;++i)
//     {
//         dest.m_srvId = ((dest.m_srvId + 1) % srvNr);
//         CMessageHeader *pMsg = m_pComms->PrepareMsg(m_nInitSendMgrLen,m_nInitSendMgrLen,m_tSvrDoid,&dest,1,false);
//         if (!pMsg)
//         {
//             ++m_nFailedTime;
//             return;
//         }
//         TUChar *pBuff = pMsg->GetBody();
//         for (int i = 0;i<nInitMsgLen;++i)
//         {
//             pBuff[i] = i;
//         }
//         m_pComms->SendMsg(pMsg);
//     }
}

int CCommTester::Init(IfCommunicatorMgr *pMgr,CDoid *pSrvDoid)
{
    m_tSvrDoid = * pSrvDoid;
    m_pCommMgr = pMgr;
    TUInt16 nSrvBegin,nSrvEnd;
    m_pComms = pMgr->GetComm(nSrvBegin,nSrvEnd,(pSrvDoid->m_srvId/MAX_SERVICE_NR_PER_COMM));
    if (!m_pComms)
    {
        printf("Register worker failed!");
        return OUT_OF_MEM;
    }
    m_nSuccessTime = 0;
    m_nFailedTime  = 0;
    m_nSrvNr       = 0;
    m_nInitSendMgrNr = 0;
    m_nInitSendMgrLen = 0;
    m_nMsgReced = 0;
    m_nLastSendTime = 0;
    m_bIsConnected = 1; 
    m_nLastSendTime = m_pComms->GetClock()->GetLocalTime();
    m_nLastGetAllRespTime = 0;
    m_nMinSendGap = 10000000;
    m_nMaxSendGap = 0;
    m_nSendTime = 0;
    m_nTotalSendGap = 0;

    return SUCCESS;
}


void CCommTester::SendAllMessage()
{
    //return;
    CMessageHeader *pMsg = m_pComms->PrepareMsg(m_nInitSendMgrLen,m_nInitSendMgrLen,m_tSvrDoid,sm_pDoids,m_nDoidNr,true);
    if (pMsg)
    {
        unsigned char *pBuff = (unsigned char*)pMsg->GetBody();
        for (unsigned int i=0;i<m_nInitSendMgrLen;++i)
        {
            pBuff[i] = (unsigned char)i;
        }
        m_pComms->SendMsg(pMsg);
        m_nLastSendTime = m_pComms->GetClock()->GetLocalTime();
        memset(m_pMsgRecords,0,m_nDoidNr);
        m_nMsgReced = 0;
    }
}

void CCommTester::CheckAll()
{
    if (m_pComms->GetClock()->GetTimeGap(m_nLastSendTime) > 5000)
    {
        m_nLastSendTime = m_pComms->GetClock()->GetLocalTime();
        SendAllMessage();
    }
}

void CCommTester::OnRecv(CDoid *pSrcDoid)
{
    int nIdx = pSrcDoid->m_nodeId * m_nIpNr* m_nSrvNr + pSrcDoid->m_virtualIp*m_nSrvNr+(pSrcDoid->m_srvId/MAX_SERVICE_NR_PER_COMM);
    if (0 == m_pMsgRecords[nIdx])
    {
        m_pMsgRecords[nIdx] = 1;
        m_nMsgReced += 1;
    }
}