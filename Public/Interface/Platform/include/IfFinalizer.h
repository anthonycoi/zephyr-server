#ifndef __ZEPHYR_SYSTEM_EXCEPTION_IF_FINALIZER_H__
#define __ZEPHYR_SYSTEM_EXCEPTION_IF_FINALIZER_H__

#include <stdio.h>

namespace Zephyr
{

class IfFinalizer
{
public:
    virtual void OnException(FILE *pDump = NULL) = 0;
};

}

#endif
