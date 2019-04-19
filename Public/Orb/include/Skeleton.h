/*-------------------------------------------------------------
 Copyright (C)| 
 File: Skeleton.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: Orb层的Skeleton，代表一个Obj在Orb的注册结果
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_SKELETON_H__
#define __ZEPHYR_SKELETON_H__

#include "../../include/TypeDef.h"
#include "../../Interface/Platform/include/IfSkeleton.h"
#include "../../Interface/App/include/IfObj.h"
#include "../../Interface/Platform/include/IfCommunicator.h"
#include "OrbErrorCode.h"

namespace Zephyr
{

class COrb;

class CSkeleton : public IfSkeleton
{
private:
    DECLARE_CLASS_LIST(CSkeleton);
    
    IfObj           *m_pRegisteredObj;
    IfCommunicator  *m_pIfComm;
    CDoid           m_tDoid;
public:
    CSkeleton();
    virtual ~CSkeleton()
    {

    }
    void RegisterObj(IfObj *pObj,TUInt16 uSrv)
    {
        m_pRegisteredObj = pObj;
        m_tDoid.m_srvId = uSrv;
    }
    void   OnReused(TUInt16 uStep);
    CDoid *GetMyDoid();
    //获得后自己初始化
    virtual CMessageHeader *PrepareMsg(TInt32 bodyLength,TUInt32 methodId,CDoid* destDoid,TInt32 destDoidNum,bool bRearrangeDest) ;
    //发送消息
    virtual TInt32  SendMsg(CMessageHeader *pMsg);

    TInt32    Init(IfCommunicator *pIfComm,CDoid *pDoid);
    
    TInt32    OnInit()
    {
        m_pRegisteredObj = NULL;
        return SUCCESS;
    }

    void      OnFinal()
    {
        m_pRegisteredObj = NULL;
    }

    TBOOL   IsMyMsg(CDoid *pDoid)
    {
        if (m_pRegisteredObj)
        {
            return (pDoid->m_objId == m_tDoid.m_objId);
        }
        return FALSE;
    }
    TInt32  OnRecv(CMessageHeader *pMsg)
    {
        if (m_pRegisteredObj)
        {
            return m_pRegisteredObj->OnRecv(pMsg);
        }
        return OBJ_NOT_REISTERED;
    }
    void OnNetEvent(CConnectionEvent *pEvent)
    {
        if (m_pRegisteredObj)
        {
            m_pRegisteredObj->OnNetEvent(pEvent);
        }
    }
    TInt32 OnRoutine(TUInt32 uRunCnt)
    {
        if (m_pRegisteredObj)
        {
            return m_pRegisteredObj->OnRoutine(uRunCnt);
        }
        return 0;
    }
};


}

#endif
