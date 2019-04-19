#ifndef __ZEPHYR_SERVICE_CONTAINER_CFG_H__
#define __ZEPHYR_SERVICE_CONTAINER_CFG_H__

#include "../../Public/include/TypeDef.h"
#include "../../Public/include/SysMacros.h"
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Zephyr
{

class IfOrb;
class IfTaskMgr;
class IfLoggerManager;
class CService;

//注意pIfTaskMgr\pIfLoggerMgr都只能在初始化时候调用，不要在运行时调用.pStubCenter可以重复使用
#define SERVICE_INIT_FUN_NAME	"InitService"
    typedef CService* (*SERVICE_INIT_FUN)(IfOrb* pStubCenter,IfTaskMgr *pIfTaskMgr,IfLoggerManager *pIfLoggerMgr);

#define SERVICE_RELEASE_FUN_NAME "ReleaseService"
    typedef TInt32 (*SERVICE_RELEASE_FUN)(CService*);



struct TServiceConfig
{
    //这个Service的dll
    std::string m_pszServiceDllName;
#ifdef _WIN32
    HMODULE				m_pPluginModuleHandle;
#else
    void*               m_pPluginModuleHandle
#endif
    SERVICE_INIT_FUN	    m_pInitFun;
    SERVICE_RELEASE_FUN	    m_pReleaseFun;
    void *                  m_pService;
    TServiceConfig()
    {
        m_pPluginModuleHandle = NULL;
        m_pInitFun = NULL;
        m_pReleaseFun = NULL;
        m_pService = NULL;
    }
};


//一个Service可以有多个
struct TOrbConfig
{
    TOrbConfig()
    {
        m_nNrofService = 0;
        m_nSkeleton = 0;
        m_pServices = NULL;
    }
    ~TOrbConfig()
    {
        if (m_pServices)
        {
            delete [] m_pServices;
            m_pServices = NULL;
        }
    }
    TInt32 m_nNrofService;
    TUInt32 m_nSkeleton;
    TServiceConfig *m_pServices; 
};

struct TServiceContainerCfg
{
    TServiceContainerCfg()
    {
        m_nNrOfOrb = 0;
        m_nLocalIp= 0;
        m_nLocalNodeId = 0;
        m_pszCommConfigName = NULL;
        m_pOrbs = NULL;
        m_nWorkerNr = 0;
        m_nCpuNr = 0;
    }
    ~TServiceContainerCfg()
    {
        if (m_pOrbs)
        {
            delete [] m_pOrbs;
        }
    }

    const TChar *m_pszCommConfigName;
    TOrbConfig *m_pOrbs;
    TInt32 m_nNrOfOrb;
    TInt16  m_nLocalIp;
    TInt16  m_nLocalNodeId;
    TInt16  m_nWorkerNr;
    TInt16  m_nCpuNr;
};

class CServiceCfgRead
{
public:
    TServiceContainerCfg m_tCfg;
    //从这儿读取.
    TInt32 Read(TChar *pFileName);

};


}
#endif