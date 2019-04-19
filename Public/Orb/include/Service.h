#include "./DistributedObject.h"
#include "../../include/TypeDef.h"
#include "../../include/SysMacros.h"
#include "../../include/Timer.h"
#include "../../include/Clock.h"
#include "./Session.h"
#include "../../Interface/Platform/include/IfOrb.h"
namespace Zephyr
{

class CService : public CObject
{
protected:
    CTimer m_tTimer;
    const CClock  *m_pClock;
    IfOrb   *m_pIfOrb; //只有service才能注销
    IfCommunicator *m_pIfComm;
   
    TUInt32 m_nServiceId;


public:
    TUInt32 GetServiceId()
    {
        return m_nServiceId;
    }
    CService()
    {
        m_pClock = NULL;
        m_pIfOrb = NULL;
        m_pIfComm = NULL;
        m_nServiceId = 0;
    }
    virtual ~CService();
    TInt32 Init(TInt32 nInitTimerNr,CClock *pClock,IfCommunicator *pIfComm,IfOrb *pOrb,CDoid *pDoid);
    void *SetTimer(TUInt32 uGapInMs,TInt32 nRepeatTime,IfScheduler *pScheduler)
    {
        return m_tTimer.SetTimer(uGapInMs,nRepeatTime,pScheduler,m_pClock->GetPlatformTime());
    }
    //删除定时器pTimer是SetTimer返回的结果
    IfScheduler *KillTimer(void *pTimer)
    {
        return m_tTimer.KillTimer(pTimer);
    }

    const CClock *GetClock()
    {
        return m_pClock;
    }
    
    TInt32 RegisterSession(CSession *pSession);

};

}