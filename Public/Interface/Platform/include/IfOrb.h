/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfOrb.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: ORB�ӿ�,���Է���IfStub
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IF_ORB_H__
#define __ZEPHYR_IF_ORB_H__


#include "../../App/include/IfObj.h"
#include "IfSkeleton.h"
#include "IfCommunicator.h"

namespace Zephyr
{

class IfOrb
{
public:
    //ע��Obj
    virtual IfSkeleton* RegisterObj(IfObj *pIfObj,TInt16 nSrvId) = 0;
    //ע�������ObjIdfx
    //ע��
    virtual void    UnRegisterObj(IfSkeleton *pSkeleton) = 0;
    //����ʱ������ע��Service
    //virtual IfSkeleton* RegiterService(IfObj *pObj,TInt16 nSrvId) = 0;
    //����ΪService׼����, ÿ��comm��ֻ����һ��.Ϊ�������������.
    virtual TInt32 RegisterRun(IfSkeleton *pObj,TUInt32 nGapInMs) = 0;
    //ʱ�����,��ȡһ�ξ����ˣ���������
    virtual const CClock *GetClock() = 0;

    virtual IfCommunicator *GetCommunicator() = 0;
//  ����ŵ�Ӧ�ò�ȥ�� 
//     //����Service
//     CDoid   FindService(TCHAR *pName) = 0;
//     CDoid   FindService(TUInt32 nServiceId) = 0;
    
    
};


}

#endif

