#include "ProducerAndConsumer.h"
#include "SysMacros.h"

namespace Zephyr
{
//һ��һ��
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
//����ֿ�ռ�
TBOOL    CProduerAndConsumer::OnProduced()
{

#ifdef _WIN32
    return SetEvent(m_cond);
#else
    pthread_mutex_lock(&m_mutex); //ò��������Բ�Ҫ���ӿ��ٶ�
    m_productsNr = 1;
    bool ret = pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex); //ò��������Բ�Ҫ
    return ret;
#endif
}
//�����ջ�
//ֻ�б������������ߺ�Ż���.
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
        //�����Сʱ���ڣ������������������ˣ�
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

