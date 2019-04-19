#include "TypeDef.h"
#include "SysInc.h"
//Task
#include "TaskCenter.h"
#include "SysMacros.h"
#include "LoggerManager.h"
#include "ExceptionParser.h"
#include <iostream>
#include "LoggerTester.h"
using namespace Zephyr;
using namespace std;
#define NUM_OF_TESTER 4
int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);	
    //_CrtSetBreakAlloc(249);
    CExceptionParser::GetInstance()->Init(EXCEPTION_SET_DEFAULT_HANDLER|EXCEPTION_USE_API_HOOK);
    
    IfTaskMgr *pTaskMgr = CreateTaskMgr();
    CloggeRTester *pTester = new CloggeRTester[NUM_OF_TESTER];
    int ret = CLoggerManager::InitInstance();
    if (ret < SUCCESS)
    {
        return ret;
    }
    CLoggerManager *pLoggerMgr = CLoggerManager::Instance();
    for (int i = 0;i<NUM_OF_TESTER;i++)
    {
        char name[32];
        sprintf(name,"Test%d""%d",i,i);
        ret = pLoggerMgr->AddLogger(name,i,log_run);
        if (ret < SUCCESS)
        {
            printf("Add Logger failed with error code %d",ret);
            return ret;
        }
        IfLogger *pLogger = pLoggerMgr->GetLogger(ret);
        pTester[i].SetLogger(pLogger);
        pTaskMgr->AddTask(pTester+i,normal_task);
    } 
    CExceptionParser::GetInstance()->AddFinalizer(CLoggerManager::Instance()->GetLogFinalizer());
    pTaskMgr->AddTask(pLoggerMgr,normal_task);
    
    pTaskMgr->StartWorking(8,1);
    //CExceptionParser parser;
    
    char stop = 'n';
    while(('y' != stop) && ('Y' != stop))
    {
        cout<<"Do U wanna stop?"<<endl;
        cin>>stop;
    }
    pTaskMgr->StopWorking();
    ReleaseTaskMgr(pTaskMgr);

    int *p = 0;
    *p = 4;
    return SUCCESS;
}