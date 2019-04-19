/*-------------------------------------------------------------
 Copyright (C)| 
 File: ProducerAndConsumer.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  ����������ģ�ͣ�
 Others: ע�⣬�Ǹ�Linux��û���Ա����.ֻ����һ�������ߣ�һ��������.������Ӧ�ÿ����ж��.
 Function List: ������ʹ���ں˲�����ͬʱ��������������������.��Ϊ�����ߴ��ڸ߲�
 Histroy: 
 -------------------------------------------------------------*/
#ifndef __ZEPHYR_SYSTEM_PRODUCER_AND_CONSUMER_H__
#define __ZEPHYR_SYSTEM_PRODUCER_AND_CONSUMER_H__
#ifdef _WIN32
#include "Windows.h"
#else
#include <pthread.h>
#include <stdint.h>
#endif
#include "Typedef.h"



namespace Zephyr
{

class CProduerAndConsumer
{
private:

    #ifdef _WIN32
    HANDLE  m_cond;
    #else   //Linux
    volatile TUInt32 m_productsNr;
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_cond;
    #endif
public:
    CProduerAndConsumer();
    //����ֿ�ռ�
    TBOOL    OnProduced();
    //�����ջ�
    //ֻ�б������������ߺ�Ż���.
    TInt32 RequireFetchProduct(TUInt32 timeout=15);
    //����Ҫ�����ջ�

};

}

#endif
