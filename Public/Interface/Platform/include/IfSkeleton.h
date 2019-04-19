/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfSkeleton.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: IfSkeleton�ӿ�,�������һ��������ܷ���Ϣ��,�����˶�����ORB��ע����
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IF_STUB__
#define __ZEPHYR_IF_STUB__

#include "../../../include/Message.h"
#include "../../../include/Clock.h"
namespace Zephyr
{



class IfSkeleton
{
public:
    virtual CDoid *GetMyDoid() = 0;
    //��ú��Լ���ʼ��
    virtual CMessageHeader *PrepareMsg(TInt32 bodyLength,TUInt32 methodId,CDoid* destDoid,TInt32 destDoidNum,bool bRearrangeDest)  = 0;
    //������Ϣ
    virtual TInt32  SendMsg(CMessageHeader *pMsg) = 0;

    //����ΪService׼����,ÿ��comm��ֻ����һ��.Ϊ�������������.��Ҫ�����ֱ�ӵ�IfOrbȥע��.
    //virtual TInt32 RegisterRun(TInt32 nRunGapInMs) = 0;
};

}

#endif
