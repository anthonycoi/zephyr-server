#include "../include/TypeUnmarshaller.h"
#include "../include/SysMacros.h"
#include <string.h>
namespace Zephyr
{
#ifdef _USE_LINK_2_MARSHALL
#define IMPL_UNMARSHALLERS(TYPE) \
TInt32 Unmarshall(TUChar *pBuffer,TInt32 uBufferLen,TYPE &tType) \
{ \
    if (uBufferLen >= sizeof(TYPE)) \
    { \
        tType = *((TYPE*)pBuffer); \
        return sizeof(TYPE); \
    } \
    return OUT_OF_MEM; \
}

// 
 IMPL_UNMARSHALLERS(TChar) 
 
  //IMPL_MARSHALLERS(TInt8) 
 
 IMPL_UNMARSHALLERS(TUChar) 
 
  //IMPL_MARSHALLERS(TUInt8) 
 
 IMPL_UNMARSHALLERS(TInt16) 
 
 IMPL_UNMARSHALLERS(TUInt16) 
 
 IMPL_UNMARSHALLERS(TInt32) 
 
 IMPL_UNMARSHALLERS(TUInt32) 
 
 IMPL_UNMARSHALLERS(TFloat) 
 
 IMPL_UNMARSHALLERS(TDouble) 
 
 IMPL_UNMARSHALLERS(TInt64) 
 
 IMPL_UNMARSHALLERS(TUInt64) 
 
 IMPL_UNMARSHALLERS(TBOOL) 
#endif

TInt32 Unmarshall(TUChar *pBuffer,TInt32 uBuffLen,TChar *&psz)
{
     TInt32 nRet = strnlen((const char *)pBuffer,uBuffLen);
     if (nRet == uBuffLen)
     {
         pBuffer[nRet-1] = '\0';
     }
     psz = (TChar*)pBuffer;
     return nRet;
}

}
