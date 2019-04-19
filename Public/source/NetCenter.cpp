#include "NetCenter.h"

#include "ConnectionMgr.h"
namespace Zephyr
{

IfNet *CreateNet(IfTaskMgr *pTaskMgr,IfParserFactory *pParserFactory,IfCryptorFactory *pCryptorFactory,TInt32 maxConnectionNr,TUInt32 nSendBuffSize,TUInt32 nRecvBuffSize)
{
    CConnectionMgr *pConnectionMgr = new CConnectionMgr();
    int ret = pConnectionMgr->Init(maxConnectionNr,pTaskMgr,pParserFactory,pCryptorFactory,nSendBuffSize,nRecvBuffSize);
    if (ret < SUCCESS)
    {
        delete pConnectionMgr;
        pConnectionMgr = NULL;
    }
    return pConnectionMgr;
}

TInt32  DestoryNet(IfNet *pIfNet)
{
    CConnectionMgr *pConnectionMgr = (CConnectionMgr *)pIfNet;
    delete pConnectionMgr; 
    return SUCCESS;
}

};