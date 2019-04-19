#include "../../Public/include/Timer.h"
#include <iostream>
#include <windows.h>
using namespace Zephyr;
using namespace std;


unsigned long long setTime  = 0;
unsigned long long releaseTime = 0;
unsigned long long killTime = 0;
unsigned long long totalGap = 0;
class CSche : public IfScheduler
{
public:
    unsigned int     m_nIdx;
    unsigned int     m_nTrigerTime;
    CSche            *m_pPool;
    CTimer           *m_pTimer;
    void *           m_pIf;
public:
    virtual TInt32 OnScheduler(void *pTimer,TUInt64 nTimeNow)
    {
        ++releaseTime;
        if (nTimeNow >= m_nTrigerTime)
        {
            totalGap += (nTimeNow-m_nTrigerTime);
        }
        else
        {
            totalGap += (m_nTrigerTime-nTimeNow);
        }
        m_pIf = NULL;
        ResetTimer(nTimeNow);
        return 0;
    }
    void ResetTimer(unsigned int nTimeNow)
    {
        if (m_pIf)
        {
            m_pTimer->KillTimer(m_pIf);
            ++killTime;
            m_pIf = NULL;
        }
        int nRandTime = rand()%10000;
        m_pIf = m_pTimer->SetTimer(nRandTime,0,this,nTimeNow);
        m_nTrigerTime = nTimeNow + nRandTime;
        if (!m_pIf)
        {
            cout<<"NULL !\n\t"<<endl;
        }
        else
        {
            ++setTime;
        }
    }
};



int main()
{
    CTimer timer;
    cout<<"Input Nr of Timer"<<endl;
    int nTimerSize = 10000;
    cin>>nTimerSize;
    if (nTimerSize< 10000)
    {
        nTimerSize = 10000;
    }
    timer.Init(nTimerSize);
    CSche *p = new CSche[nTimerSize];
    int nTime = 0;;
    for (int i =0;i<nTimerSize;++i)
    {
        p[i].m_nIdx = i;
        p[i].m_nTrigerTime = 0;
        p[i].m_pPool = p;
        p[i].m_pTimer = &timer;
        p[i].m_pIf = NULL;
    }
    int nTimeNow = timeGetTime();
    int nTimeBegin = nTimeNow;
    while(1)
    {
        for (int i=0;i<nTimerSize;)
        {

            if (i%1000)
            {
                nTimeNow = timeGetTime();
                
                timer.Run(nTimeNow);
            }
            p[i].ResetTimer(nTimeNow);
            ++i;
        }
        nTimeNow = timeGetTime();
        srand(nTimeNow);
        cout<<"test set"<<setTime<<"Trigger "<<releaseTime<<"Kill "<<killTime<<"In"<<(nTimeNow-nTimeBegin)<<"total gap:"<<totalGap<<"avg"<<(setTime/totalGap)<<endl;
    }
    

    return 0;
}