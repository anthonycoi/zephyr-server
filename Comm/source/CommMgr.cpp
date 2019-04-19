#include "CommMgr.h"
#include "SettingFile.h"
#include "../Public/include/NetCenter.h"
#include "../System/include\SysInc.h"
#include "include/CommLogger.h"
#include "CommMsg.h"
namespace Zephyr
{


TInt32 CCommMgr::Init(TInt32 nrOfWorkerThread,IfTaskMgr *pTaskMgr,IfLoggerManager *pIfLogMgr,const TChar *pConfigName)
{
    //��ʼ������ײ�
    
    //�ȶ�ȡ����
    if (MAX_COMM_NR < nrOfWorkerThread)
    {
        //Ҳ����32��
        printf("Max %d comm for one CommMgr",MAX_COMM_NR);
        return OUT_OF_RANGE;
    }
    TInt32 ret = m_ipMaps.Init(pConfigName,this);
    m_tClock.Update();
    m_uLastCheckTime = m_tClock.GetLocalTime();
    if (SUCCESS > ret)
    {
        return ret;
    }
    try
    {
        m_pParserFactory = new CMsgParserFactory(m_ipMaps.m_nrOfVirtualIp + 5);
    }
    catch (...)
    {
    }
    if (!m_pParserFactory)
    {
#ifdef _DEBUG
        printf("Can not allocate mem for m_pParserFactory!");
#endif
        return OUT_OF_MEM;
    }
    CSettingFile settingFile;
    TInt32 inPipeSize = 128*1024;
    TInt32 outPipeSize = 256*1024;
    TInt32 maxMsgSize = 256*1024;
    if (!settingFile.LoadFromFile("commSetting.ini"))
    {

    }
    else
    {
        inPipeSize = settingFile.GetInteger("MAIN","inPipeSize",inPipeSize);
        outPipeSize = settingFile.GetInteger("MAIN","outPipeSize",outPipeSize);
        maxMsgSize = settingFile.GetInteger("MAIN","maxMsgSize",maxMsgSize);
    }
    m_pNet = CreateNet(pTaskMgr,m_pParserFactory,NULL,(m_ipMaps.m_nNrOfMapItem+1),outPipeSize,inPipeSize/*���5��*/);
    if (!m_pNet)
    {
#ifdef _DEBUG
        printf("Create m_pNet Failed!");
#endif
        return OUT_OF_MEM;
    }
    int nRet = m_connectionPool.InitPool(m_ipMaps.m_nrOfVirtualIp + m_ipMaps.m_nrOfNodes);
    if (nRet)
    {

    }

    
    NEW(m_pBuff,TUChar,maxMsgSize);
    if (!m_pBuff)
    {
#ifdef _DEBUG
        printf("Can not allocated memory for m_pBuff");
#endif
        return OUT_OF_MEM;
    }


    //�ٿ��Ƿ���Ҫ����������node.
    m_pLoggerMgr = pIfLogMgr;
    ret = m_pLoggerMgr->AddLogger("CommLogger",-1);
    if (ret < SUCCESS)
    {
        return ret;
    }
    m_pLogger = m_pLoggerMgr->GetLogger(ret);
    g_pCommLogger = m_pLogger;
    pTaskMgr->AddTask(this);

    m_nNrOfComm = nrOfWorkerThread;
    NEW(m_pCommunicators,CCommunicator,nrOfWorkerThread);
    if (!m_pCommunicators)
    {
#ifdef _DEBUG
        printf("Allocate mem 4 m_pCommunicators Failed!");
#endif
        //OnFinal();
        return OUT_OF_MEM;
    }

    for (int i=0;i<nrOfWorkerThread;++i)
    {
        int ret = m_pCommunicators[i].Init(&m_tClock,inPipeSize,outPipeSize,maxMsgSize);
        m_pCommunicators[i].InitEventPool(m_ipMaps.m_nrOfVirtualIp);
        if (ret < SUCCESS)
        {
#ifdef _DEBUG
            printf("Can not init Communicator %d",i);
#endif
            return ret;
        }
    }



//     NEW(m_ppConnections,CCommConnection*,m_ipMaps.m_nrOfVirtualIp + 5);
//     if (!m_ppConnections)
//     {
// #ifdef _DEBUG
//         printf("Can not allocate memory for m_ppConnections");
// #endif
//         return OUT_OF_MEM;
//     }
    //��ʼ����һ��port

	//��������vip���Լ�С�����л�����ÿ̨���������������˳��.����ֻ�����������Լ�vipС�Ļ���
    //if (m_ipMaps.m_nrOfNodes > 1)
    {
        for (TUInt32 i = 0;i<m_ipMaps.m_nNrOfMapItem;++i)
        {
            if (m_ipMaps.IsPostive(i))
            {
                CCommConnection *pConnection = m_connectionPool.GetMem();
                if (!pConnection)
                {
#ifdef _DEBUG
                    printf("Can not get Comm Connection");
#endif
                    return OUT_OF_MEM;
                }
                CIpMapItem *pIp = m_ipMaps.GetConnection(i);
                TInt32 rtn = m_pNet->Connect(pIp->m_tKey.GetRemoteIp(),pIp->m_tKey.GetMyIp(),pIp->m_tKey.GetRemotePort(),pIp->m_tKey.GetMyPort(),pConnection);
                if (rtn < SUCCESS)
                {
#ifdef _DEBUG
                    printf("Connection Failed!");
#endif
                    m_connectionPool.ReleaseMem(pConnection);
                    return rtn;
                }
                //m_ppConnections[i] = pConnection;
                pConnection->SetAllInfo(this,pIp);
                pIp->OnConnecting(pConnection,m_tClock.GetLocalTime());
                //���ͬʱ���ӹ��������
                if (0 == (i % 32))
                {
                    printf("Connecting...");
#ifdef _WIN32
                    Sleep(50);
#else  
                    usleep(15000);
#endif
                    m_pNet->Run(128);
                }
            }
        }
        for (TUInt32 i=0;i<m_ipMaps.m_vListening.size();++i)
        {
            CIpMapItem *pIpMap = m_ipMaps.GetConnection(m_ipMaps.m_vListening[i]);
            //TChar pIps[64];
            
            pIpMap->m_pListeningItem = m_pNet->Listen(pIpMap->m_tKey.GetMyIp(),pIpMap->m_tKey.GetMyPort(),32,((IfListenerCallBack*)this));
            if (!pIpMap->m_pListeningItem)
            {
                printf("Listening to %u:%d failed!",pIpMap->m_tKey.GetMyIp(),(int)pIpMap->m_tKey.GetMyPort());
            }
        }
    }
    

    //��ȡ���ã����м���Service, ��Ҫ������������Service.
    
    return SUCCESS;
}


TInt32 CCommMgr::InitWithConfig(TInt32 nrOfWorkerThread,IfTaskMgr *pTaskMgr,IfLoggerManager *pIfLogMgr,const TChar *pConfigStr)
{
    //�ȶ�ȡ����
    if (MAX_COMM_NR < nrOfWorkerThread)
    {
        //Ҳ����32��
        printf("Max %d comm for one CommMgr",MAX_COMM_NR);
        return OUT_OF_RANGE;
    }
    TInt32 ret = m_ipMaps.InitWithConfig(pConfigStr,this);
    m_tClock.Update();
    m_uLastCheckTime = m_tClock.GetLocalTime();
    if (SUCCESS > ret)
    {
        return ret;
    }
    try
    {
        m_pParserFactory = new CMsgParserFactory(m_ipMaps.m_nrOfVirtualIp + 5);
    }
    catch (...)
    {
    }
    if (!m_pParserFactory)
    {
#ifdef _DEBUG
        printf("Can not allocate mem for m_pParserFactory!");
#endif
        return OUT_OF_MEM;
    }
    CSettingFile settingFile;
    TInt32 inPipeSize = 128*1024;
    TInt32 outPipeSize = 256*1024;
    TInt32 maxMsgSize = 256*1024;
    if (!settingFile.LoadFromFile("commSetting.ini"))
    {

    }
    else
    {
        inPipeSize = settingFile.GetInteger("MAIN","inPipeSize",inPipeSize);
        outPipeSize = settingFile.GetInteger("MAIN","outPipeSize",outPipeSize);
        maxMsgSize = settingFile.GetInteger("MAIN","maxMsgSize",maxMsgSize);
    }
    m_pNet = CreateNet(pTaskMgr,m_pParserFactory,NULL,(m_ipMaps.m_nNrOfMapItem+1),outPipeSize,inPipeSize/*���5��*/);
    if (!m_pNet)
    {
#ifdef _DEBUG
        printf("Create m_pNet Failed!");
#endif
        return OUT_OF_MEM;
    }
    int nRet = m_connectionPool.InitPool(m_ipMaps.m_nrOfVirtualIp + m_ipMaps.m_nrOfNodes);
    if (nRet)
    {

    }


    NEW(m_pBuff,TUChar,maxMsgSize);
    if (!m_pBuff)
    {
#ifdef _DEBUG
        printf("Can not allocated memory for m_pBuff");
#endif
        return OUT_OF_MEM;
    }


    //�ٿ��Ƿ���Ҫ����������node.
    m_pLoggerMgr = pIfLogMgr;
    ret = m_pLoggerMgr->AddLogger("CommLogger",-1);
    if (ret < SUCCESS)
    {
        return ret;
    }
    m_pLogger = m_pLoggerMgr->GetLogger(ret);
    g_pCommLogger = m_pLogger;
    pTaskMgr->AddTask(this);

    m_nNrOfComm = nrOfWorkerThread;
    NEW(m_pCommunicators,CCommunicator,nrOfWorkerThread);
    if (!m_pCommunicators)
    {
#ifdef _DEBUG
        printf("Allocate mem 4 m_pCommunicators Failed!");
#endif
        //OnFinal();
        return OUT_OF_MEM;
    }

    for (int i=0;i<nrOfWorkerThread;++i)
    {
        int ret = m_pCommunicators[i].Init(&m_tClock,inPipeSize,outPipeSize,maxMsgSize);
        m_pCommunicators[i].InitEventPool(m_ipMaps.m_nrOfVirtualIp);
        if (ret < SUCCESS)
        {
#ifdef _DEBUG
            printf("Can not init Communicator %d",i);
#endif
            return ret;
        }
    }



    //     NEW(m_ppConnections,CCommConnection*,m_ipMaps.m_nrOfVirtualIp + 5);
    //     if (!m_ppConnections)
    //     {
    // #ifdef _DEBUG
    //         printf("Can not allocate memory for m_ppConnections");
    // #endif
    //         return OUT_OF_MEM;
    //     }
    //��ʼ����һ��port

    //��������vip���Լ�С�����л�����ÿ̨���������������˳��.����ֻ�����������Լ�vipС�Ļ���
    //if (m_ipMaps.m_nrOfNodes > 1)
    {
        for (TUInt32 i = 0;i<m_ipMaps.m_nNrOfMapItem;++i)
        {
            if (m_ipMaps.IsPostive(i))
            {
                CCommConnection *pConnection = m_connectionPool.GetMem();
                if (!pConnection)
                {
#ifdef _DEBUG
                    printf("Can not get Comm Connection");
#endif
                    return OUT_OF_MEM;
                }
                CIpMapItem *pIp = m_ipMaps.GetConnection(i);
                TInt32 rtn = m_pNet->Connect(pIp->m_tKey.GetRemoteIp(),pIp->m_tKey.GetMyIp(),pIp->m_tKey.GetRemotePort(),pIp->m_tKey.GetMyPort(),pConnection);
                if (rtn < SUCCESS)
                {
#ifdef _DEBUG
                    printf("Connection Failed!");
#endif
                    m_connectionPool.ReleaseMem(pConnection);
                    return rtn;
                }
                //m_ppConnections[i] = pConnection;
                pConnection->SetAllInfo(this,pIp);
                pIp->OnConnecting(pConnection,m_tClock.GetLocalTime());
                //���ͬʱ���ӹ��������
                if (0 == (i % 32))
                {
                    printf("Connecting...");
#ifdef _WIN32
                    Sleep(50);
#else  
                    usleep(15000);
#endif
                    m_pNet->Run(128);
                }
            }
        }
        for (TUInt32 i=0;i<m_ipMaps.m_vListening.size();++i)
        {
            CIpMapItem *pIpMap = m_ipMaps.GetConnection(m_ipMaps.m_vListening[i]);
            //TChar pIps[64];

            pIpMap->m_pListeningItem = m_pNet->Listen(pIpMap->m_tKey.GetMyIp(),pIpMap->m_tKey.GetMyPort(),32,((IfListenerCallBack*)this));
            if (!pIpMap->m_pListeningItem)
            {
                printf("Listening to %u:%d failed!",pIpMap->m_tKey.GetMyIp(),(int)pIpMap->m_tKey.GetMyPort());
            }
        }
    }


    //��ȡ���ã����м���Service, ��Ҫ������������Service.

    return SUCCESS;
}


IfCommunicator *CCommMgr::GetComm(TUInt16& nSrvBegin,TUInt16& nSrvEnd,TInt32 nCommIdx)
{
    if (nCommIdx < m_nNrOfComm)
    {
        nSrvBegin = nCommIdx * MAX_SERVICE_NR_PER_COMM;
        nSrvEnd   = (nCommIdx + 1) * MAX_SERVICE_NR_PER_COMM - 1;
        return m_pCommunicators + nCommIdx;
    }
    return NULL;
}

TInt32 CCommMgr::Run(const TInt32 threadId,const TInt32 runCnt)
{
    int usedCnt = 0;
    //�ȵ���ʱ��
    m_tClock.Update();
    //������Ϣ���ײ�buffС
    usedCnt += m_pNet->Run(runCnt);
    //��ת����Ϣ
    for (TUInt32 i = 0;i<m_nNrOfComm;++i)
    {
 //       usedCnt += DistributeSrvMsg(i);
        CMessageHeader *pMsg = m_pCommunicators[i].GetAppMsg(m_pBuff);
        if (!pMsg)
        {
            continue;
        }
        //if(CheckNetState(pMsg))
        {
            SendAppMsg(pMsg);
            if (m_pBuff != (TUChar *)pMsg)
            {
                 m_pCommunicators[i].AppMsgSent(pMsg);   
            }
            ++usedCnt;
        }
    }

    //���Net�¼�
    
    //10���Ӽ��һ��
    TUInt32 uTimeNow = m_tClock.GetLocalTime();
    if ((uTimeNow-10000) > m_uLastCheckTime)
    {
        m_uLastCheckTime = uTimeNow;
        for (TUInt32 i = 0;i<m_ipMaps.m_nNrOfMapItem;++i)
        {
            CIpMapItem *pItem = m_ipMaps.GetConnection(i);
            TUInt32 nGap = m_tClock.GetTimeGap(pItem->m_uLastUsedTime);
            if (nGap > 10000)
            {
                if ((pItem->m_pIfConnection) && (!m_ipMaps.IsLocal(pItem)))
                {
                    //����������Ϣ
                    char buff[sizeof(CMessageHeader)];
                    CMessageHeader *pMsg = (CMessageHeader*)(buff);
                    CDoid src(m_ipMaps.m_localNodeId,m_ipMaps.m_localVirtualIp,0,0);
                    CDoid dest(pItem->m_nNodeId,pItem->m_nVirtualIp,0,0);
                    pMsg->Init(0,_COMM_CONNECTION_HEART_BEAT_MSG_ID_,src,&dest,1,false);
                    pMsg->SetSystemCall();
                    pItem->m_pIfConnection->SendMsg((TUChar*)pMsg,sizeof(CMessageHeader));
                }
                else
                {
                    //����
                    if (pItem->m_pCommConnection)
                    {
                        //������������
                        continue;
                    }
                    if (m_ipMaps.IsPostive(i))
                    {
                        CCommConnection *pConnection = m_connectionPool.GetMem();
                        if (!pConnection)
                        {
#ifdef _DEBUG
                            printf("Can not get Comm Connection");
#endif
                            return usedCnt;
                        }
                        TInt32 rtn = m_pNet->Connect(pItem->m_tKey.GetRemoteIp(),pItem->m_tKey.GetMyIp(),pItem->m_tKey.GetRemotePort(),pItem->m_tKey.GetMyPort(),pConnection);
                        if (rtn < SUCCESS)
                        {
#ifdef _DEBUG
                            printf("Connection Failed!");
#endif
                            m_connectionPool.ReleaseMem(pConnection);
                            continue;
                        }
                        //m_ppConnections[i] = pConnection;
                        pConnection->SetAllInfo(this,pItem);
                        pItem->OnConnecting(pConnection,m_tClock.GetLocalTime());
                        //���ͬʱ���ӹ��������
                        if (0 == (i % 32))
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
    return usedCnt;
}

TBOOL CCommMgr::CheckNetState(CMessageHeader *pMsg)
{
    //������ζ��ɹ�
    return TRUE;
    //������Ҫ��������ip,����node��ͬ
//     int nrOfDest = pMsg->GetBroadcastDoidNr();
//     //�й㲥
//     if (nrOfDest)
//     { 
//         CDoid *pDoid = pMsg->GetDestDoidByIdx();
//         //int idx = m_ipMaps.GetConnectionIdx(pDoid);
//         IfConnection *pConn = m_ipMaps.RouteTo(pDoid);
//         int nrToSend = 1;
//         int i = 1;
//         while(i <= nrOfDest)
//         {
//             CDoid *pNext = pMsg->GetDestDoidByIdx(i);
//             if (pDoid->IsInOneIp(*pNext))
//             {
//                 ++i;
//                 ++nrToSend;
//             }
//             else
//             {
//                 //��ͬ��
//                 TUInt32 needLen = pMsg->GetBodyLength() + sizeof(CMessageHeader) + (sizeof(CDoid) * (nrToSend));
//                 //С��0������.
//                 //if (idx > 0)
//                 {
//                     //connectionû���ϣ��ǾͲ�����.
//                     if (pConn)
//                     {
//                         if (pConn->GetFreeBuffLength() < needLen)
//                         {
//                             return FALSE;
//                         }
//                     }
//                 }
//                 pDoid = pNext;
//                 pConn = m_ipMaps.RouteTo(pDoid);
//                 nrToSend = 1;
//                 ++i;
//             }
//         }
// 
//         TUInt32 needLen = pMsg->GetBodyLength() + (sizeof(CMessageHeader) + sizeof(CDoid) * (nrToSend));
//         //С��0������.
//         //if (idx > 0)
//         {
//             //connectionû���ϣ��ǾͲ�����.
//             if (pConn)
//             {
//                 if (pConn->GetFreeBuffLength() < needLen)
//                 {
//                     return FALSE;
//                 }
//             }
//         }
//     }
//     else //û�й㲥
//     {
//         //int idx = m_ipMaps.GetConnectionIdx(pMsg->GetDestDoidByIdx());
//         IfConnection *pConn = m_ipMaps.RouteTo(pMsg->GetDestDoidByIdx(0));
//         if (pConn)
//         {
//             TUInt32 freeLen = pConn->GetFreeBuffLength();
//             if (pMsg->GetLength() <= freeLen)
//             {
//                 return TRUE;
//             }
//         }
//         return FALSE;
//     }
}


void CCommMgr::SendAppMsg(CMessageHeader *pMsg)
{
    //������Ҫ��������ip,����node��ͬ
    int nrOfDest = pMsg->GetBroadcastDoidNr();
    //�й㲥
    if (nrOfDest)
    { 
        CDoid *pDoid = pMsg->GetDestDoidByIdx();
        CIpMapItem *pItem =  m_ipMaps.RouteTo(pDoid);
        IfConnection *pConn;
        if (pItem)
        {
            pConn = pItem->m_pIfConnection;
            if (pConn)
            {
                pItem->m_uLastUsedTime = m_tClock.GetLocalTime();
            }
        }
        else
        {
            pConn = NULL;
        }
        int from = 0;
        int i = 1;
        while(i <= nrOfDest)
        {
            CDoid *pNext = pMsg->GetDestDoidByIdx(i);

            int nRet = FAIL;
            if (pDoid->IsInOneIp(*pNext))
            {
                ++i;
            }
            else
            {
                //��ͬ��
               /* TUInt32 needLen = pMsg->GetBodyLength() + (sizeof(CMessageHeader) * (nrToSend));*/
                //С��0������.
                pMsg->ReInitMsg4Send(from,(i-1));
                //if (idx > 0)
                {
                    //connectionû���ϣ��ǾͲ�����.
                    if (pConn)
                    {
                        nRet = pConn->SendMsg((TUChar*)pMsg,pMsg->GetLength());
                    }
                }
                if (nRet < SUCCESS)
                {
                    RecordOneMsg(pMsg);
                }
                pDoid = pNext;
                CIpMapItem *pIpMapItem = m_ipMaps.RouteTo(pDoid);
                if (pIpMapItem)
                {
                    pConn = pIpMapItem->m_pIfConnection;
                    if (pConn)
                    {
                        pIpMapItem->m_uLastUsedTime = m_tClock.GetLocalTime();
                    }
                }
                else
                {
                    pConn = NULL;
                }
                from = i;
                ++i;
                
            }
        }
        
        int nRet = FAIL;
        if (pConn)
        {
            if (from)
            {
                pMsg->ReInitMsg4Send(from,nrOfDest);
            }
            //if (m_ppConnections[idx])
            {
                nRet = pConn->SendMsg((TUChar*)pMsg,pMsg->GetLength());
            }
        }
        if (nRet < SUCCESS) //connectionû����,��¼��Ϣ
        {
            RecordOneMsg(pMsg);
        }
        pMsg->SetBroadcastDoid(nrOfDest);
    }
    else //û�й㲥
    {
        CIpMapItem *pIpMapItem = m_ipMaps.RouteTo(pMsg->GetDestDoidByIdx());
        IfConnection *pConn;
        if (pIpMapItem)
        {
            pConn = pIpMapItem->m_pIfConnection;
            if (pConn)
            {
                pIpMapItem->m_uLastUsedTime = m_tClock.GetLocalTime();
            }
        }
        else
        {
            pConn = NULL;
        }
        int nRet = FAIL;
        if (pConn)
        {
            nRet = pConn->SendMsg((TUChar*)pMsg,pMsg->GetLength());
        }
        if (nRet < SUCCESS)
        {
            RecordOneMsg(pMsg);
        }
        //return FALSE;
    }

}

//�յ���ʱ����붼��CMessagerHeader�ĸ�ʽ���ɵ����߱�֤
TInt32 CCommMgr::SendMsg(TUChar *pBuff,TUInt32 buffLen)
{
    CMessageHeader *pMsg = (CMessageHeader*)pBuff;
    TUInt32 usedLen = 0;
    TUInt32 msgLen = pMsg->GetLength();
    while (msgLen >= buffLen)
    {
        usedLen += msgLen;
        buffLen -= msgLen;
        if (pMsg->IsSystemCall())
        {
            HandleOneSystemMsg(pMsg);
        }
        else
        {
            HandleOneNetMsg(pMsg);
        }
        //�����ǿ����ݰ�
        if (sizeof(CMessageHeader) <= buffLen)
        {
            pMsg = (CMessageHeader*)(pBuff + usedLen);
        }
        else
        {
            break;
        }
    }
    return usedLen;
}
//��Զ�����
TUInt32 CCommMgr::GetFreeBuffLength()
{
    return 0x7FFFFFFF;
}
//���ر������ӱ��ص�
//��ȡ������Ϣ
CConPair *CCommMgr::GetConnectionInfo()
{
    return &m_cLoopBack;;
}
//�����Ƿ���ҪNegla�㷨������
TInt32 CCommMgr::NeedNoDelay(const char chOpt)
{
    return SUCCESS;
}
//��ȡ����״̬������
EnConnectionState CCommMgr::GetConnctionState()
{
    return connection_is_using;
}
//���Ի�ȡ��δ���͵����ݵĳ��ȣ�����
TInt32 CCommMgr::GetPendingDataLen()
{
    return 0;
}

//��CCommMgr��˵���������
TInt32 CCommMgr::Disconnect()
{
    return SUCCESS;
}

CIpMapItem *CCommMgr::GetIpMapInfo(CConPair *pPair)
{
    return m_ipMaps.GetIpMapInfo(pPair);
}

void   CCommMgr::OnConnected(CCommConnection *pConnection)
{
    CCommConnection *pLast = m_ipMaps.OnConnected(pConnection,m_tClock.GetLocalTime());
    CConnectionEvent event;
    event.m_nNodeId = pConnection->GetNodeId();
    event.m_nVip    = pConnection->GetVirtualIp();
    event.m_nEvent  = en_connection_is_established_event;
    for (TUInt32 i=0;i<m_nNrOfComm;++i)
    {
        m_pCommunicators[i].AddNetEvent(event);
    }
    if (pLast)
    {
        if (pLast != pConnection)
        {
            pLast->Disconnect();
            m_connectionPool.ReleaseMem(pLast);
        }
    }
}
void   CCommMgr::OnDisconnected(CCommConnection *pConnection,TBOOL bIsNegative)
{
    //��ipmap��
    m_ipMaps.OnDisconnected(pConnection,m_tClock.GetLocalTime());

    m_connectionPool.ReleaseMem(pConnection);

    if (bIsNegative)
    {
        CConnectionEvent event;
        event.m_nNodeId = pConnection->GetNodeId();
        event.m_nVip    = pConnection->GetVirtualIp();
        event.m_nEvent  = en_connection_is_broken_event;
        for (TUInt32 i=0;i<m_nNrOfComm;++i)
        {
            m_pCommunicators[i].AddNetEvent(event);
        }
    }
}


IfConnectionCallBack *CCommMgr::OnNewConnection(CConPair *pPair)
{
    CIpMapItem *pItem = GetIpMapInfo(pPair);
    if (!pItem)
    {
        return NULL;
    }
    //
    CCommConnection *pConn = m_connectionPool.GetMem();
    pConn->SetAllInfo(this,pItem);
    return pConn;
}


void CCommMgr::HandleOneNetMsg(CMessageHeader *pMsg)
{
    int i = 0;
    int from = 0;
    int to = 1;
    CDoid *pDoid = pMsg->GetDestDoidByIdx(i);
    CCommunicator *pComm = GetIfComm(pDoid->m_srvId);
    if (pComm)
    {
        int msgDoidNr = pMsg->GetBroadcastDoidNr();
        if (msgDoidNr)
        {
            pDoid = pMsg->GetBroadcastDoids();
            for (i=1;i<= msgDoidNr;++i)
            {
                if (pComm != GetIfComm(pDoid->m_srvId))
                {
                    //send last msg
                    pMsg->ReInitMsg4Send(from,i-1);
                    pComm->AddNetMsg(pMsg);
                    pMsg->SetBroadcastDoid(msgDoidNr);
                    from = i;
                    pComm = GetIfComm(pDoid->m_srvId);
                    if (!pComm)
                    {
                        pMsg->SetBroadcastDoid(msgDoidNr);
                        RecordOneMsg(pMsg);
                        return;
                    }
                }
                else
                {
                    //pComm = GetIfComm(pDoid->m_srvId);
                }
                ++pDoid;
            }
        }
        
        if (!pComm)
        {
            pMsg->SetBroadcastDoid(msgDoidNr);
            RecordOneMsg(pMsg);
            return;
        }
        //���from�ı��������Ҫ����init Msg.
        if (from)
        {
            pMsg->ReInitMsg4Send(from,(msgDoidNr));
        }
        
        pComm->AddNetMsg(pMsg); //���������ʧ�ܣ�������¼�����治�õ���,CommMgr�ǹ�Comm�ģ�����Comm�ڲ����߼�
        pMsg->SetBroadcastDoid(msgDoidNr);
        return;
    }
    else
    {
        RecordOneMsg(pMsg);
        return;
    }
}

void CCommMgr::HandleOneSystemMsg(CMessageHeader *pMsg)
{
    switch(pMsg->GetMethodId())
    {
    case _COMM_CONNECTION_WELCOME_MSG_ID_:
        {

        }
        break;
    case _COMM_CONNECTION_HEART_BEAT_MSG_ID_:
        {   
            CDoid *pSrc = pMsg->GetSrcDoid();
            CIpMapItem *pItem =  m_ipMaps.RouteTo(pSrc);
            if (pItem)
            {
                pItem->m_uLastUsedTime = m_tClock.GetLocalTime();
            }
        }
        break;
    case _COMM_CONNECTION_TIME_SYNCHRONIZE_MSG_ID:
        {

        }
        break;
    case _COMM_EXCEPTION_MSG_ID:
        {

        }
        break;
    }
}

}