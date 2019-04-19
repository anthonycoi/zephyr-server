/*-------------------------------------------------------------
 Copyright (C)| 
 File: ProducerAndConsumer.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  生产消费者模型，
 Others: 注意，那个Linux还没尝试编译过.只能有一个消费者，一个生产者.消费者应该可以有多个.
 Function List: 尽量不使用内核操作。同时，尽量让消费者无阻塞.因为消费者处于高层
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
    //申请仓库空间
    TBOOL    OnProduced();
    //申请收货
    //只有遍历所有生产者后才会锁.
    TInt32 RequireFetchProduct(TUInt32 timeout=15);
    //不需要申请收获

};

}

#endif
