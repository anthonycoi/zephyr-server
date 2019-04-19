/*-------------------------------------------------------------
 Copyright (C)| 
 File: TypeDef.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  U must use this file in all platform related code!
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_TYPE_DEFINE_H__
#define __ZEPHYR_TYPE_DEFINE_H__



namespace Zephyr
{
typedef char TChar;
typedef char TInt8;
typedef unsigned char TUChar;
typedef unsigned char TUInt8;
typedef short TInt16;
typedef unsigned short TUInt16;
typedef  int TInt32;
typedef  unsigned int TUInt32;
typedef float TFloat;
typedef double TDouble;
typedef long long TInt64;
typedef unsigned long long TUInt64;
typedef bool TBOOL;

template <class TYPE>
class OctSeq
{
public:
    TYPE m_nBodyLength;
    TUChar *m_pBuffer;
public:
    OctSeq()
    {
        m_nBodyLength = 0;
        m_pBuffer = 0;
    }
    static TInt32 CalFullLength(TInt32 nLength)
    {
        return sizeof(TYPE) + nLength;
    }
    TInt32 GetFullLength()
    {
        return sizeof(TYPE) + m_nBodyLength;
    }
    TYPE GetBodyLength()
    {
        return m_nBodyLength;
    }
    OctSeq(TInt32 nBodyLength,TUChar *pBuffer)
    {
        m_pBuffer = pBuffer;
        m_nBodyLength = nBodyLength;
    }
    TUChar *GetBody()
    {
        return m_pBuffer;
    }
};

template <class LENGHT_TYPE,class TAG_TYPE>
class TLV
{
public:
    LENGHT_TYPE m_nBodyLength;
    TAG_TYPE    m_nTag;
    TUChar      *m_pBuffer;
public:
    TLV()
    {
        m_nBodyLength = 0;
        m_nTag = 0;
        m_pBuffer = 0;
    }
    LENGHT_TYPE GetBodyLength()
    {
        return m_nBodyLength;
    }
    TLV(LENGHT_TYPE nBodyLength,TAG_TYPE tTag,TUChar *pBuffer)
    {
        m_nBodyLength = nBodyLength;
        m_nTag = tTag;
        m_pBuffer = pBuffer;
    }
    TUChar *GetBody()
    {
        return m_pBuffer;
    }
    TAG_TYPE GetTag()
    {
        return m_nTag;
    }
    LENGHT_TYPE GetFullLength()
    {
        return ((sizeof(LENGHT_TYPE)+sizeof(TAG_TYPE)) + m_nBodyLength); 
    }
};



#ifndef FALSE
#define FALSE false 
#endif
#ifndef TRUE
#define TRUE true 
#endif
}








#endif
