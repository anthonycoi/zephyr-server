// ServiceContainer.cpp : 定义控制台应用程序的入口点。
//
//这个是用来加载DLLservice的，需要提供findService

#include "Public/include/SysMacros.h"
#include "Public/include/TypeDef.h"
#include "Public/Orb/include//Orb.h"
#include "Public/include/LogCenter.h"
#include "Public/include/CommCenter.h"
#include "Public/include/TaskCenter.h"
#include "System/ExceptionParser/include/ExceptionParser.h"
#include "../include/ServiceContainerCfg.h"
#include "System/Logger/include/LoggerManager.h"
#include "Public/Orb//include/Service.h"
#include "System/include/SysInc.h"
using namespace Zephyr;



#ifdef WIN32

HMODULE LoadDynamicLib(const char * LibFileName)
{
    return LoadLibrary(LibFileName);
}

void ReleaseDynamicLib(HMODULE LibHandle)
{
    FreeLibrary(LibHandle);
}

void * GetDynamicLibFunction(HMODULE LibHandle,char * FunctionName)
{
    return GetProcAddress(LibHandle,FunctionName);
}

char * dlerror()
{
    static char ErrBuff[20];
    sprintf(ErrBuff,"%d",GetLastError());
    return ErrBuff;
}

#else


void * LoadDynamicLib(const char * LibFileName)
{
    return dlopen(LibFileName,RTLD_NOW);
}

void ReleaseDynamicLib(void * LibHandle)
{
    dlclose(LibHandle);
}

void * GetDynamicLibFunction(void * LibHandle,char * FunctionName)
{
    return dlsym(LibHandle,FunctionName);
}

#endif


int main(int argc, char* argv[])
{
    //先读配置
    CServiceCfgRead tRead;
    
    if (argc>1)
    {
        if (tRead.Read(argv[1]) < SUCCESS)
        {
            printf("Read ServiceContainer Config:%s failed",argv[0]);
            return FAIL;
        }
    }
    else
    {
        if (tRead.Read("DefaultConfig.ini") < SUCCESS)
        {
            printf("Read Default ServiceContainer Config failed");;
            return FAIL;
        }
    }

    //创建TaskMgr
    IfTaskMgr *pTaskMgr = CreateTaskMgr();
    if (!pTaskMgr)
    {
        printf("Create TaskMgr failed!");
        return OUT_OF_MEM;
    }

    CExceptionParser parser;

    //创建LoggerMgr
    IfLoggerManager *pLogMgr = CreateLogSys(pTaskMgr);
    if (!pLogMgr)
    {
        printf("Create LogMgr failed!");
        return OUT_OF_MEM;
    }
    //创建Comm
    IfCommunicatorMgr *pMgr = CreateCommMgr(tRead.m_tCfg.m_nNrOfOrb,pTaskMgr,pLogMgr,tRead.m_tCfg.m_pszCommConfigName);
    if (!pMgr)
    {
        printf("Create CommMgr failed!");
        return OUT_OF_MEM;
    }
    //创建Orb
    COrb *pOrb = NULL;
    try 
    {
        pOrb = new COrb[tRead.m_tCfg.m_nNrOfOrb];
    }
    catch(...)
    {
        printf("Create Orb[%d] failed!",tRead.m_tCfg.m_nNrOfOrb);
        return OUT_OF_MEM;
    }
    for (int i=0;i<tRead.m_tCfg.m_nNrOfOrb;++i)
    {
        TUInt16 nSrvBegin,nSrvEnd;
        IfCommunicator *pIfComm = pMgr->GetComm(nSrvBegin,nSrvEnd,i);
        if (pIfComm)
        {
            CDoid doid;
            doid.m_objId = 0;
            doid.m_srvId = nSrvBegin;
            doid.m_virtualIp = tRead.m_tCfg.m_nLocalIp;
            doid.m_nodeId    = tRead.m_tCfg.m_nLocalNodeId;
            char szBuffer[128];
            sprintf_s(szBuffer,sizeof(szBuffer),"OrbLogger%d",i);
            TInt32 nRet = pLogMgr->AddLogger(szBuffer,-1,LOG_WRITE_MODE,LOG_PRINT_SCREEN_MODE);
            if (nRet < SUCCESS)
            {
                return nRet;
            }
            IfLogger *pLogger = pLogMgr->GetLogger(nRet);
            nRet = pOrb[i].Init(pIfComm,&doid,tRead.m_tCfg.m_pOrbs[i].m_nSkeleton,pLogger);
            if (nRet < SUCCESS)
            {
                printf("Orb[%d] init failed! stub nr:%d\n\r",i,tRead.m_tCfg.m_pOrbs[i].m_nSkeleton);
                return nRet;
            }
        }
        else
        {
            printf("Get IfComm[%d] failed!\n\r",i);
            return NULL_POINTER;
        }
    }
    //加载ServiceMgr,这个再写好了


    //加载Service
    for (int i=0;i<tRead.m_tCfg.m_nNrOfOrb;++i)
    {
        for (int j=0;j<tRead.m_tCfg.m_pOrbs[i].m_nNrofService;++j)
        {
            bool bSuccessfull = false;
            tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pPluginModuleHandle = LoadDynamicLib(tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pszServiceDllName.c_str());
            if (tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pPluginModuleHandle)
            {
                tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pInitFun = (SERVICE_INIT_FUN)GetDynamicLibFunction(tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pPluginModuleHandle,SERVICE_INIT_FUN_NAME);
                tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pReleaseFun=(SERVICE_RELEASE_FUN)GetDynamicLibFunction(tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pPluginModuleHandle,SERVICE_RELEASE_FUN_NAME);
                if ((tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pInitFun)&&(tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pReleaseFun))
                {
                    printf("Plugin [%s] InitFun=%p ReleaseFun=%p\n\r",
                        tRead.m_tCfg.m_pOrbs[i].m_pServices->m_pszServiceDllName.c_str(),
                        tRead.m_tCfg.m_pOrbs[i].m_pServices->m_pInitFun,
                        tRead.m_tCfg.m_pOrbs[i].m_pServices->m_pReleaseFun);
                    bSuccessfull = true;
                }
            }
            if (!bSuccessfull)
            {
                printf("Load DLL:%s failed!\n\r",tRead.m_tCfg.m_pOrbs[i].m_pServices->m_pszServiceDllName);
            }
        }
    }
    for (int i=0;i<tRead.m_tCfg.m_nNrOfOrb;++i)
    {
        TInt32 nRegisterSuccess = SUCCESS;
        for (int j=0;j<tRead.m_tCfg.m_pOrbs[i].m_nNrofService;++j)
        {
            CService *pService = (*(tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pInitFun))(pOrb+i,pTaskMgr,pLogMgr);
            if(pService)
            {
                tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pService = pService;
                IfSkeleton *pSkeleton = pOrb[i].RegiterService(pService,pService->GetServiceId());
                if (pSkeleton)
                {
                    pService->SetSkeleton(pSkeleton);
                    TInt32 nRet = pService->OnInit();
                    if (nRet < SUCCESS)
                    {
                        printf("Init Service %s Failed!\n\r",tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pszServiceDllName.c_str());
                        return nRet;
                    }
                    printf_s("Init Plugin [%s] Succeed\n\r",tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pszServiceDllName.c_str());
                }
            }
            else
            {
                return FAIL; //所有资源由系统释放
            }
        }
        nRegisterSuccess = pTaskMgr->AddTask(pOrb+i);
    }
    pTaskMgr->AddTask((CLoggerManager*)pLogMgr);
    pTaskMgr->StartWorking(tRead.m_tCfg.m_nWorkerNr,tRead.m_tCfg.m_nCpuNr);
    TChar cStop = 'n';
    while((cStop!='y')&&(cStop!='Y'))
    {
        printf_s("Starting wait 4 Net...\n\r");
        SleepT(15000);
        printf_s("Do U wanna stop? press (y)");
        std::cin>>cStop;
    }
    printf("Stopping the Service!\n\r");
    
    for (int i=0;i<tRead.m_tCfg.m_nNrOfOrb;++i)
    {
        TInt32 nRegisterSuccess = SUCCESS;
        for (int j=0;j<tRead.m_tCfg.m_pOrbs[i].m_nNrofService;++j)
        {
            if (tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pService)
            {
                CService *pService = (CService *)tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pService;
                pOrb[j].StopService(pService->GetServiceId());
                (*(tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pReleaseFun))(pService);
                ReleaseDynamicLib(tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pPluginModuleHandle);
                tRead.m_tCfg.m_pOrbs[i].m_pServices[j].m_pPluginModuleHandle = NULL;
            }
        }
    }
    pTaskMgr->StopWorking();
    printf("All Worker Stopped! Now Finalize the Services!\n\r");
    //完了
	return 0;
}

