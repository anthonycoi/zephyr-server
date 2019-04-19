#include "IfTask.h"
#include "..\..\System\include\ProducerAndConsumer.h"
namespace Zephyr
{


TInt32  IfTask::Wait4Event(void *pProducerAndConsumer,TInt32 timeout)
{
    return ((CProduerAndConsumer*)pProducerAndConsumer)->RequireFetchProduct(timeout);
}

void IfTask::OnNewEvent(void *pProducerAndConsumer)
{
    ((CProduerAndConsumer*)pProducerAndConsumer)->OnProduced();
}


}
