#include "IpMaps.h"
#ifdef _WIN32
#include "winsock2.h"
#endif
#include "stdio.h"
#include "SettingFile.h"
#include "SysMacros.h"
namespace Zephyr
{

CIpMap::CIpMap()
{
    m_nrOfNodes = 0;
    m_nrOfVirtualIp = 0;
    m_localNodeId = 0;
    m_localVirtualIp = 0;
    m_pVirtualIps = NULL;
    m_pRoutes = NULL;
    m_nNrOfMapItem = 0;
}

int CIpMap::ReadIpMapItem4Node(void *pFile,int nVip,CIpMapItem *pItem)
{
    char pMain[16];
    sprintf(pMain,"NODE%d",nVip);
    CSettingFile &file = *((CSettingFile*)pFile);
    const char* pIp = file.GetString(pMain,"myIp");
    unsigned int myIp = 0;
    if (pIp)
    {
        myIp = inet_addr(pIp);
    }
    else
    {
        return -1;
    }
    unsigned int remoteIp = 0;
    pIp = file.GetString(pMain,"remoteIp");
    if (pIp)
    {
        remoteIp = inet_addr(pIp);
    }
    else
    {
        return -1;
    }
    unsigned short myPort = 0;
    unsigned short remotePort = 0;
    myPort = file.GetInteger(pMain,"myPort");
    remotePort = file.GetInteger(pMain,"remotePort");
    if ((remotePort == myPort)&&(remoteIp==myIp))
    {
        return -1;
    }
    pItem->m_tKey.Init(remoteIp,myIp,remotePort,myPort);
    return SUCCESS;
}


TInt32 CIpMap::Init(const TChar *pConfigName,IfConnection *pSelf)
{
    CSettingFile settingFile;
    if (!settingFile.LoadFromFile(pConfigName))
    {
        return NULL_POINTER;
    }
    m_nrOfNodes = settingFile.GetInteger("MAIN","nrOfNodes");
    m_nrOfVirtualIp = settingFile.GetInteger("MAIN","nrOfVirtualIp");
    m_localNodeId    = settingFile.GetInteger("MAIN","localNodeId");
    m_localVirtualIp = settingFile.GetInteger("MAIN","localVirtualIp");

    m_nNrOfMapItem = m_nrOfVirtualIp;
    if (m_nrOfNodes > 1)
    {
        for (int i=0;i<m_nrOfNodes;++i)
        {
            if (i == m_localNodeId)
            {
                continue;
            }
            char buff[64];
            sprintf(buff,"NODE%d",i);
            if (m_localVirtualIp == settingFile.GetInteger(buff,"localVIP"))
            {
                ++m_nNrOfMapItem;
            }
        }
    }
    //if (m_nrOfNodes > 0)
    {
        NEW(m_pVirtualIps,CIpMapItem,(m_nNrOfMapItem));
        if (!m_pVirtualIps)
        {
            return OUT_OF_MEM;
        }
    }
    
    TUInt32 uRemoteIp;
    TUInt32 uMyIp;
    TUInt16 uMyPort;
    TUInt16 uRemotePort;
    char szMain[16];
    sprintf(szMain,"VIP%d",m_localVirtualIp);
    const char* pIp = settingFile.GetString(szMain,"ip");
   
    if (pIp)
    {
        uMyIp = inet_addr(pIp);
    }
    else
    {
        return -1;
    }
    unsigned short uLocalPort = settingFile.GetInteger(szMain,"port");

    
    
    for (int i=0;i<m_nrOfVirtualIp;++i)
    {
        char szMain[16];
        sprintf(szMain,"VIP%d",m_localVirtualIp);

        m_pVirtualIps[i].m_nNodeId = m_localNodeId;
        m_pVirtualIps[i].m_nVirtualIp = i;
        CConPair &iPair = m_pVirtualIps[i].m_tKey;
        pIp = settingFile.GetString(szMain,"ip");
        if (pIp)
        {
            uRemoteIp = inet_addr(pIp);
        }
        else
        {
            return -1;
        }
        if (IsPostive(i)) //是我主動連對端
        {
            //所以端口號是對端的監聽端口
            char szRemoteMain[16];
            sprintf(szRemoteMain,"VIP%d",i);
            uRemotePort = settingFile.GetInteger(szRemoteMain,"port");
            uMyPort     = uRemotePort + m_localVirtualIp - i;
        }
        else //是我被動
        {
            uMyPort   = uLocalPort;
            uRemotePort = uLocalPort + i - m_localVirtualIp;
        }
        iPair.Init(uRemoteIp,uMyIp,uRemotePort,uMyPort);

        if (i == m_localVirtualIp)
        {
            AddListeningExisted(&m_pVirtualIps[i],i);
        }
        //if (i)
        //只监听一个
    }
    int usedNode = 0;

    //获取我的node信息
    if (m_nrOfNodes > 1)
    {
        m_pRoutes = new TUInt32[m_nrOfNodes];
        memset(m_pRoutes,0,(sizeof(TUInt32)*m_nrOfNodes));
        m_pRoutes[m_localNodeId] = m_localVirtualIp; 
        for (int i=0;i<m_nrOfNodes;++i)
        {
            if (i == m_localNodeId)
            {
                m_pRoutes[i] = 0;
                continue;
            }
            char buff[32];
            sprintf(buff,"NODE%d",i);

            if (m_localVirtualIp == settingFile.GetInteger(buff,"localVIP"))
            {
                m_pRoutes[i] = m_nrOfVirtualIp + usedNode;
                if (ReadIpMapItem4Node(&settingFile,i,&m_pVirtualIps[m_pRoutes[i]]) < 0)
                {
                    return -1;
                }
                m_pVirtualIps[m_pRoutes[i]].m_nNodeId = i;
                m_pVirtualIps[m_pRoutes[i]].m_nVirtualIp = 0;
                
                ++usedNode;
            }
            else
            {
                m_pRoutes[i] = settingFile.GetInteger(buff,"localVIP");
            }
            AddListeningExisted(&m_pVirtualIps[m_pRoutes[i]],m_pRoutes[i]);
        }
    }
    //不try了
    
    m_pVirtualIps[m_localVirtualIp].m_pIfConnection = pSelf;
    return SUCCESS;
}

TInt32 CIpMap::InitWithConfig(const TChar *pConfig,IfConnection *pSelf)
{
    CSettingFile settingFile;
    if (!settingFile.Load(pConfig))
    {
        return NULL_POINTER;
    }
    m_nrOfNodes = settingFile.GetInteger("MAIN","nrOfNodes");
    m_nrOfVirtualIp = settingFile.GetInteger("MAIN","nrOfVirtualIp");
    m_localNodeId    = settingFile.GetInteger("MAIN","localNodeId");
    m_localVirtualIp = settingFile.GetInteger("MAIN","localVirtualIp");

    m_nNrOfMapItem = m_nrOfVirtualIp;
    if (m_nrOfNodes > 1)
    {
        for (int i=0;i<m_nrOfNodes;++i)
        {
            if (i == m_localNodeId)
            {
                continue;
            }
            char buff[64];
            sprintf(buff,"NODE%d",i);
            if (m_localVirtualIp == settingFile.GetInteger(buff,"localVIP"))
            {
                ++m_nNrOfMapItem;
            }
        }
    }
    //if (m_nrOfNodes > 0)
    {
        NEW(m_pVirtualIps,CIpMapItem,(m_nNrOfMapItem));
        if (!m_pVirtualIps)
        {
            return OUT_OF_MEM;
        }
    }

    TUInt32 uRemoteIp;
    TUInt32 uMyIp;
    TUInt16 uMyPort;
    TUInt16 uRemotePort;
    char szMain[16];
    sprintf(szMain,"VIP%d",m_localVirtualIp);
    const char* pIp = settingFile.GetString(szMain,"ip");

    if (pIp)
    {
        uMyIp = inet_addr(pIp);
    }
    else
    {
        return -1;
    }
    unsigned short uLocalPort = settingFile.GetInteger(szMain,"port");



    for (int i=0;i<m_nrOfVirtualIp;++i)
    {
        char szMain[16];
        sprintf(szMain,"VIP%d",m_localVirtualIp);

        m_pVirtualIps[i].m_nNodeId = m_localNodeId;
        m_pVirtualIps[i].m_nVirtualIp = i;
        CConPair &iPair = m_pVirtualIps[i].m_tKey;
        pIp = settingFile.GetString(szMain,"ip");
        if (pIp)
        {
            uRemoteIp = inet_addr(pIp);
        }
        else
        {
            return -1;
        }
        if (IsPostive(i)) //是我主動連對端
        {
            //所以端口號是對端的監聽端口
            char szRemoteMain[16];
            sprintf(szRemoteMain,"VIP%d",i);
            uRemotePort = settingFile.GetInteger(szRemoteMain,"port");
            uMyPort     = uRemotePort + m_localVirtualIp - i;
        }
        else //是我被動
        {
            uMyPort   = uLocalPort;
            uRemotePort = uLocalPort + i - m_localVirtualIp;
        }
        iPair.Init(uRemoteIp,uMyIp,uRemotePort,uMyPort);

        if (i == m_localVirtualIp)
        {
            AddListeningExisted(&m_pVirtualIps[i],i);
        }
        //if (i)
        //只监听一个
    }
    int usedNode = 0;

    //获取我的node信息
    if (m_nrOfNodes > 1)
    {
        m_pRoutes = new TUInt32[m_nrOfNodes];
        memset(m_pRoutes,0,(sizeof(TUInt32)*m_nrOfNodes));
        m_pRoutes[m_localNodeId] = m_localVirtualIp; 
        for (int i=0;i<m_nrOfNodes;++i)
        {
            if (i == m_localNodeId)
            {
                m_pRoutes[i] = 0;
                continue;
            }
            char buff[32];
            sprintf(buff,"NODE%d",i);

            if (m_localVirtualIp == settingFile.GetInteger(buff,"localVIP"))
            {
                m_pRoutes[i] = m_nrOfVirtualIp + usedNode;
                if (ReadIpMapItem4Node(&settingFile,i,&m_pVirtualIps[m_pRoutes[i]]) < 0)
                {
                    return -1;
                }
                m_pVirtualIps[m_pRoutes[i]].m_nNodeId = i;
                m_pVirtualIps[m_pRoutes[i]].m_nVirtualIp = 0;

                ++usedNode;
            }
            else
            {
                m_pRoutes[i] = settingFile.GetInteger(buff,"localVIP");
            }
            AddListeningExisted(&m_pVirtualIps[m_pRoutes[i]],m_pRoutes[i]);
        }
    }
    //不try了

    m_pVirtualIps[m_localVirtualIp].m_pIfConnection = pSelf;
    return SUCCESS;
}

}
