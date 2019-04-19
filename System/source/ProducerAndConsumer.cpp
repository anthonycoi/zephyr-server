#include "ProducerAndConsumer.h"
#include "SysMacros.h"

namespace Zephyr
{
//一对一的
CProduerAndConsumer::CProduerAndConsumer()
{
#ifdef _WIN32
    m_cond = CreateEvent(NULL,false,false,NULL);
#else
    m_productsNr = 0;
    pthread_mutex_init(&m_mutex,NULL);
    pthread_cond_init(&m_cond,NULL);
#endif`
}
//申请仓库空间
TBOOL    CProduerAndConsumer::OnProduced()
{

#ifdef _WIN32
    return SetEvent(m_cond);
#else
    pthread_mutex_lock(&m_mutex); //貌似这个可以不要，加快速度
    m_productsNr = 1;
    bool ret = pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex); //貌似这个可以不要
    return ret;
#endif
}
//申请收货
//只有遍历所有生产者后才会锁.
TInt32 CProduerAndConsumer::RequireFetchProduct(TUInt32 timeout)
{
    
#ifdef _WIN32
    int ret = WaitForSingleObject(m_cond,timeout);
    if (WAIT_TIMEOUT == ret)
    {
        return TIME_OUT;
    }
    if (WAIT_OBJECT_0 == ret)
    {
        return SUCCESS;
    }
#else

    struct timeval tv;
    struct timezone tz;


    //if()
    {
        struct timespec ts;

        //unsigned long long endtime = ((long long)tv.tv_sec) * 1000000 + tv.tv_usec+1000*timeout;

        ts.tv_sec = endtime/1000000;
        ts.tv_nsec = (endtime%1000000)*1000;

        pthread_mutex_lock(&m_mutex);
        //在这段小时间内，生产者又生产东西了！
        if (m_productsNr)
        {
            int result = m_productsNr;
            m_productsNr = 0;
            pthread_mutex_unlock(&m_mutex);
            return result;
        }
        int retcode= pthread_cond_reltimedwait_np(&m_cond,&m_mutex,&ts);
        //if (m_productsNr)
        {
            int result = (int)m_productsNr;
            m_productsNr = 0;
            pthread_mutex_unlock(&m_mutex);
            return result;
        }
    }
#endif

    return SUCCESS;
}


}

