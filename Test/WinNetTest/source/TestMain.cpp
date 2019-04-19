#include "TypeDef.h"
#include "SysInc.h"
//Task
#include "TaskCenter.h"
#include "SysMacros.h"
#include "IfLoggerMgr.h"
#include "ExceptionParser.h"
#include <iostream>
#include "WinNetTester.h"
#include "Config\include\SettingFile.h"
using namespace Zephyr;
using namespace std;


int main()
{
    IfTaskMgr *pTaskMgr = CreateTaskMgr();
    
    
    CSettingFile setting;
    if(!setting.LoadFromFile("Setting.ini"))
    {
        return -1;
    }
    
    const char *pMyIp = setting.GetString("MAIN","myIp");
    const char *pRemoteIp = setting.GetString("MAIN","remoteIp");
    unsigned short myPort = setting.GetInteger("MAIN","myPort",12437);
    unsigned short remotePort = setting.GetInteger("MAIN","remotePort",12436);
    int passiveConnectionNr = setting.GetInteger("MAIN","passiveConnectionNr",64);
    int maxConnectionNr = setting.GetInteger("MAIN","maxConnectionNr",128);
    int initSendMsg = setting.GetInteger("MAIN","initSendMsg",10);
    int averageMsglen = setting.GetInteger("MAIN","averageMsglen",150);
    CWinNetTester *pNetTester = new CWinNetTester(pTaskMgr,maxConnectionNr);
    pNetTester->Init(pMyIp,pRemoteIp,myPort,remotePort,passiveConnectionNr,maxConnectionNr,initSendMsg);
    pNetTester->OnInit();
    CAppConnection::SetAverageMsgLen(averageMsglen);
    pTaskMgr->AddTask(pNetTester);
    pTaskMgr->StartWorking(8,1);
    //CExceptionParser parser;
    char stop = 'n';
    while(('y' != stop) && ('Y' != stop))
    {
        cout<<"Do U wanna stop?"<<endl;
        cin>>stop;
    }
    pNetTester->OnFianl();
    pTaskMgr->StopWorking();
    ReleaseTaskMgr(pTaskMgr);
    return SUCCESS;
}