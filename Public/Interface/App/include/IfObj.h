/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfObj.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: ORB�ص��ӿ�,��IfSkeleton�����
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IF_SKELETON_H__
#define __ZEPHYR_IF_SKELETON_H__
#include "../../../include/Message.h"
#include "../../../include/TypeDef.h"
namespace Zephyr
{

class IfObj
{
public:
    //���յ���Ϣ��ʱ�򣬻ص����,�����0�����ʾ��Ϣ������
    virtual TInt32  OnRecv(CMessageHeader *pMsg) = 0;
    
//     //��ʱ����ʱ��
//     virtual TInt32  OnTimer(TInt32 nTimerIdx,void *pData,TInt32 nTimeGap,TUInt32 nTimeNow) = 0;

    //�����Ǹ�Serviceʹ�õ�
    //���ڵĻص�������ע��ѭ��ʱ�䣬��ֻ����һ��
    virtual TInt32  OnRoutine(TUInt32 nRunCnt) = 0;
    //����ʱ��
    virtual TInt32  OnNetEvent(CConnectionEvent *pEvent) = 0;

};

}

#endif
