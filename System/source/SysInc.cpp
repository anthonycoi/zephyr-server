#include "SysInc.h"

namespace Zephyr
{
#ifdef _WIN32
    void SleepT(unsigned int ms)
    {
        Sleep(ms);
    }
#else
    void SleepT(unsigned int ms)
    {
        usleep(ms*1000);
    }
#endif

}