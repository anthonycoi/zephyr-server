#include "TypeDef.h"
#include "SysInc.h"
//Task
#include "TaskCenter.h"
#include "SysMacros.h"
#include "IfLoggerMgr.h"
#include "ExceptionParser.h"
#include <iostream>
using namespace Zephyr;
using namespace std;
int main()
{
    IfTaskMgr *pTaskMgr = CreateTaskMgr();
    pTaskMgr->StartWorking(4);
    CExceptionParser parser;
    
    char stop = 'n';
    while(('y' != stop) || ('Y' != stop))
    {
        cout<<"Do U wanna stop?"<<endl;
        cin>>stop;
    }
    pTaskMgr->StopWorking();
    ReleaseTaskMgr(pTaskMgr);
    return SUCCESS;
}