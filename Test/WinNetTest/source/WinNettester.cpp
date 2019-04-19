#include "WinNetTester.h"
#include "Mmsystem.h"
#include "time.h"
CWinNetTester::CWinNetTester(IfTaskMgr *pTaskMgr,int maxConnectionNr)
{
    m_pNet = CreateNet(pTaskMgr,&m_parserFactory,NULL,maxConnectionNr);
    if(!m_pNet)
    {
        return;
    }
}

CWinNetTester::~CWinNetTester()
{
    
}

TInt32 CWinNetTester::OnInit()
{
    m_lastRunTime = timeGetTime();
    return SUCCESS;
}

TInt32 CWinNetTester::Init(const char *pMyIp,const char *pRemoteIp,unsigned short myPort,unsigned short remotePort,int passiveConnectionNr,int connectionNr,int initSendMsg)
{
    m_ConnectionPool.Init(connectionNr);
    strcpy(m_myIp,pMyIp);
    strcpy(m_remoteIp,pRemoteIp);
    m_myPort = myPort;
    m_remotePort = remotePort;
    int ret = (int)m_pNet->Listen(pMyIp,myPort,connectionNr,&m_ConnectionPool);
    if (NULL == ret)
    {
        return ret;
    }
    for (int i=0;i<passiveConnectionNr;++i)
    {
        CAppConnection *pConnection = m_ConnectionPool.GetConnection();
        pConnection->SetPassvieSendNr(initSendMsg);
        bool tryAgain = true;
        while(tryAgain)
        {
            //这里有个bug，如果一开始连接没成功，那么后面就不会重连.
            int ret = m_pNet->Connect(pRemoteIp,pMyIp,remotePort,0,pConnection);
            if (SUCCESS > ret)
            {
                m_pNet->Run(128);
                Sleep(15);
            }
            else
            {
                tryAgain = false;
            }
        }
        if (39 == (i %40))
        {
            Sleep(15);
            m_pNet->Run(128);
        }
    }
    return SUCCESS;
}


TInt32 CWinNetTester::OnFianl()
{
    return SUCCESS;
}

TInt32 CWinNetTester::Run(const TInt32 threadId,const TInt32 runCnt)
{
    int usedCnt = m_pNet->Run(runCnt);
    srand(time(NULL));
    if (!usedCnt)
    {
        //使用了反而速度降慢了？
//         int waitRtl; = Wait4Event();
//         if (TIME_OUT != waitRtl)
        {
            usedCnt = m_pNet->Run(runCnt);
            if (0 == usedCnt)
            {
                //printf("Wrong!");
            }
        }
        //printf("no work to do!");
    }
    unsigned long timeNow = timeGetTime();
    if ((timeNow - m_lastRunTime) > 1000)
    {
        m_lastRunTime = timeNow;
        int PoolNr = m_ConnectionPool.GetConnectionNr();
        for (int i = 0;i<PoolNr;++i)
        {
            CAppConnection *pConn = m_ConnectionPool.GetConnectionByIdx(i);
            if (pConn)
            {
                int ret =  pConn->Run();
                if (SUCCESS < ret)
                {
                    usedCnt += ret;
                }
                
                if (-5 == ret)
                {
                    ret = m_pNet->Connect(m_remoteIp,m_myIp,m_remotePort,0,pConn);
                    if(ret < SUCCESS)
                    {
                        pConn->m_actived = 3;
                    }
                }
                if (-4 == ret)
                {
                    //m_ConnectionPool.ReleaseConnection(pConn);
                }
            }
        }
    }
    return usedCnt;
}
