#ifndef __ZEPHYR_PUBLIC_TYPE_MARSHALLER_H__
#define __ZEPHYR_PUBLIC_TYPE_MARSHALLER_H__

#include "TypeDef.h"
#include "SysMacros.h"
namespace Zephyr
{
#ifndef _USE_LINK_2_MARSHALL

#define DECLARE_MARSHALLERS(TYPE) \
    inline TInt32 Marshall(TUChar *pBuffer,TInt32 uBufferLen,TYPE tType) \
    { \
    if (uBufferLen >= sizeof( TYPE )) \
    { \
    *(( TYPE *)pBuffer) = tType; \
    return sizeof( TYPE ); \
} \
    return OUT_OF_MEM; \
} \
    TInt32 GetLength(TYPE c) \
    { \
    return sizeof(TYPE); \
}

    DECLARE_MARSHALLERS(TChar) 

        //IMPL_MARSHALLERS(TInt8) 

    DECLARE_MARSHALLERS(TUChar) 

        //IMPL_MARSHALLERS(TUInt8) 

    DECLARE_MARSHALLERS(TInt16) 

    DECLARE_MARSHALLERS(TUInt16) 

    DECLARE_MARSHALLERS(TInt32) 

    DECLARE_MARSHALLERS(TUInt32) 

    DECLARE_MARSHALLERS(TFloat) 

    DECLARE_MARSHALLERS(TDouble) 

    DECLARE_MARSHALLERS(TInt64) 

    DECLARE_MARSHALLERS(TUInt64) 

    //DECLARE_MARSHALLERS(TBOOL)  
#else

// 
 TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TUChar ucChar);
// 
// TInt32 GetLength(TUChar c);
// 
 TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TChar ucChar);
// 
// TInt32 GetLength(TChar c);
// // inline TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TInt8 nInt8);
// // inline TInt32 Unmarshall(TUChar *pBuffer,TInt32 uBuffLen,TInt8& nInt8);
// // 
// // inline TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TUInt8 uInt8);
// // inline TInt32 Unmarshall(TUChar *pBuffer,TInt32 uBuffLen,TUInt8& uInt8);
// 
// 
 TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TInt16 nInt16);
// 
// TInt32 GetLength(TInt16 c);
// 
 TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TUInt16 uInt16);
// 
// TInt32 GetLength(TUInt16 c);
// 
 TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TInt32 nInt32);
// 
// TInt32 GetLength(TInt32 c);
// 
 TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TUInt32 uInt32);
// 
// TInt32 GetLength(TUInt32 c);
// 
 TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TFloat fFloat);
// 
// TInt32 GetLength(TFloat c);
// 
 TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TDouble dDouble);
// 
// TInt32 GetLength(TDouble c);
// 
 TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TInt64 llInt64);
// 
// TInt32 GetLength(TInt64 c);
// 
 TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TInt64 ullInt64);
// 
// TInt32 GetLength(TInt64 c);
// 
// //inline TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TBOOL bBool); //为什么会和TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,CDoid *pDoid)冲突呢？！
// 
// TInt32 GetLength(TBOOL c);
#endif

TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,const TChar *psz);

template<class TYPE>
TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,OctSeq<TYPE> &tOctSeq)
{
    TInt32 nLength = tOctSeq.GetFullLength();
    if (nLength > uBuffLen)
    {
        return OUT_OF_MEM;
    }
    *((TYPE*)pBuffer) = tOctSeq.m_nBodyLength;
    memcpy((pBuffer+sizeof(TYPE)),tOctSeq.m_pBuffer,tOctSeq.m_nBodyLength);
    return nLength;
}

template<class LENGTH_TYPE,class TAG_TYPE>
TInt32 Marshall(TUChar *pBuffer,TInt32 uBuffLen,TLV<LENGTH_TYPE,TAG_TYPE> &tTLV)
{
    TInt32 nLength = tTLV.GetFullLength();
    if (nLength > uBuffLen)
    {
        return OUT_OF_MEM;
    }
    *((LENGTH_TYPE*)pBuffer) = tTLV.m_nBodyLength;
    pBuffer += sizeof(LENGTH_TYPE);
    *((TAG_TYPE*)pBuffer) = tTLV.m_nTag;
    pBuffer += sizeof(TAG_TYPE);
    memcpy(pBuffer,tTLV.m_pBuffer,tTLV.m_nBodyLength);
    return nLength;
}

}



#endif
