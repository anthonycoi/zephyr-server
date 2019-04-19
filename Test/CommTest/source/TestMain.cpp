#include "TypeDef.h"
#include "SysInc.h"
//Task
#include "TaskCenter.h"
#include "SysMacros.h"
#include "IfLoggerMgr.h"
#include "ExceptionParser.h"
#include "CommTester.h"
#include "LogCenter.h"
#include "CommCenter.h"
#include <iostream>
#include <List>

#include "../../../Public//Config/include/SettingFile.h"

using namespace Zephyr;
using namespace std;
class CTest
{
public:
    int a;
};
class CTestB : public CTest
{
    //CTestB();
public:

    int b;
};
int main()
{
    

    IfTaskMgr *pTaskMgr = CreateTaskMgr();


    CExceptionParser parser;
    IfLoggerManager *pLogMgr = CreateLogSys(pTaskMgr);
    int initSendMsgNr = 0;
    int initSendMsgLen = 0;
    CDoid initSendMsgDoid;
    CSettingFile setting;
    int nrOfTester = 0;
    int nrOfServiceContainer = 0;
    if (setting.LoadFromFile("CommTestConfig.ini"))
    {
        initSendMsgNr = setting.GetInteger("MAIN","initSendMsgNr",0);
        initSendMsgLen = setting.GetInteger("MAIN","initSendMsgLen",0);
        initSendMsgDoid.m_nodeId = setting.GetInteger("MAIN","nodeId",0);
        initSendMsgDoid.m_virtualIp = setting.GetInteger("MAIN","m_virtualIp",0);
        initSendMsgDoid.m_srvId    = setting.GetInteger("MAIN","m_srvId",0);
        initSendMsgDoid.m_objId  = setting.GetInteger("MAIN","m_objId",0);
        nrOfTester   = setting.GetInteger("MAIN","testCommNum",1);
        nrOfServiceContainer = setting.GetInteger("MAIN","serviceCommNum",2);
    }
    else
    {
        printf("Can not open file CommTestConfig.ini");
        return FAIL;
    }
    IfCommunicatorMgr *pMgr = CreateCommMgr(nrOfTester,pTaskMgr,pLogMgr,"IpMaps.ini");
    
    
    


    CCommTester* pCommTests = new CCommTester[nrOfTester];
    for (int i=0;i<nrOfTester;++i)
    {
        initSendMsgDoid.m_srvId = i*MAX_SERVICE_NR_PER_COMM;
        pCommTests[i].Init(pMgr,&initSendMsgDoid);
        pCommTests[i].OnStartTestOne(initSendMsgNr,initSendMsgLen,nrOfTester,nrOfServiceContainer,1);
        pTaskMgr->AddTask(pCommTests+i);
    }

    pTaskMgr->StartWorking(4,2);
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