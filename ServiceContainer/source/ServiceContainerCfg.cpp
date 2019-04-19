#include "../include/ServiceContainerCfg.h"
#include "../../Public/Config/include/SettingFile.h"
#include "../../Public/include/SysMacros.h"
#include "../../Public/include/TypeDef.h"
#include "stdio.h"
namespace Zephyr
{
//调用者保证pFileName
TInt32 CServiceCfgRead::Read(TChar *pFileName)
{
    CSettingFile settings;
    if (settings.LoadFromFile(pFileName))
    {
        m_tCfg.m_nNrOfOrb = settings.GetInteger("MAIN","nrOfOrb",1);
        m_tCfg.m_pszCommConfigName = settings.GetString("MAIN","szCommConfigName","IpMaps.ini");
        {
            CSettingFile commSetting;
            if (commSetting.LoadFromFile(m_tCfg.m_pszCommConfigName))
            {
                m_tCfg.m_nLocalNodeId = commSetting.GetInteger("MAIN","localNodeId",0);
                m_tCfg.m_nLocalIp     = commSetting.GetInteger("MAIN","localVirtualIp",0);
            }
            else
            {
                printf_s("Load Comm Config %s Failed!",m_tCfg.m_pszCommConfigName);
            }
        }
        m_tCfg.m_nWorkerNr    = settings.GetInteger("MAIN","ThreadNr",(m_tCfg.m_nNrOfOrb+3));
        m_tCfg.m_nCpuNr       = settings.GetInteger("MAIN","cpuNr",m_tCfg.m_nNrOfOrb);
        NEW(m_tCfg.m_pOrbs,TOrbConfig,m_tCfg.m_nNrOfOrb);

        if (!m_tCfg.m_pOrbs)
        {
            printf("Out of mem,create m_tCfg.m_pOrbs failed!");
            return OUT_OF_MEM;
        }
        for (int i=0;i<m_tCfg.m_nNrOfOrb;++i)
        {
            char szBuff[64];
            sprintf(szBuff,"Orb%02d",i);
            m_tCfg.m_pOrbs[i].m_nNrofService = settings.GetInteger(szBuff,"nrOfService",1);
            m_tCfg.m_pOrbs[i].m_nSkeleton = settings.GetInteger(szBuff,"nrOfSkeleton",1);
            NEW(m_tCfg.m_pOrbs[i].m_pServices,TServiceConfig,m_tCfg.m_pOrbs[i].m_nNrofService);
            if (m_tCfg.m_pOrbs[i].m_pServices)
            {
                for (int j=0;j<m_tCfg.m_pOrbs[i].m_nNrofService;++j)
                {
                    sprintf(szBuff,"Service%02d_%02d",i,j);
                    m_tCfg.m_pOrbs[i].m_pServices[j].m_pszServiceDllName = settings.GetString(szBuff,"dllName","");
                    if (0 == m_tCfg.m_pOrbs[i].m_pServices[j].m_pszServiceDllName[0])
                    {
                        printf("Can not find dll!");
                        return NULL_POINTER;
                    }
                }
            }
            else
            {
                printf("Create TServiceConfig failed!");
                return OUT_OF_MEM;
            }
        }
    }
    else
    {
        printf("Open file %s failed!",pFileName);
        return CAN_NOT_OPEN_FILE;
    }
    return SUCCESS;
}

}