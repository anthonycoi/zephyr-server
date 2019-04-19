/*-------------------------------------------------------------
 Copyright (C)| 
 File: Doid.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  �ֲ�ʽ�����ַ
 Others: virtualIp = 0������Ϊ���Ľڵ㣬����ڵ��ͨ��\����������\���������.
         һ��node����һ���ڲ��ڵ㣬һ����һ�������ڣ���Щ����֮����ȫ������
         ��ͬ�ṩĳ�����һ���ڵ��ɴ�Լ254̨�����������ɣ���Щ�ٶ�Ļ�û��Ҫ�ṩֱ�ӵ�ȫ���ӣ�
         ͬʱ������65534��node����ЩNode֮�������ȫ���ӣ�Ҳ�����������ӣ�
         ����Ҫͨ���м�ڵ��ת��������,���Ӻ�ת���㷨����ip..
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/
 
#ifndef _S_DOID_H_
#define _S_DOID_H_


#include "TypeDef.h"
#include "SysMacros.h"

namespace Zephyr
{
//now we use this one, that is much more short.
class CDoid
{

public:
#ifdef _USE_BIG_DOID
    TUInt32 m_objId;                   //obj idx;
    TUInt32  m_srvId;             //srv id, decide the port,ÿ������ip�Ͽ����ܶ��service.

    TUInt32  m_virtualIp;               //һ��������ڵ�����ip
    TUInt32  m_nodeId;           //һ�������Ϊ1���ڵ�
#else
    TUInt32 m_objId;                   //obj idx;
    TUInt8  m_srvId;             //srv id, decide the port,ÿ������ip�Ͽ����ܶ��service.

    TUInt8  m_virtualIp;               //һ��������ڵ�����ip
    TUInt16 m_nodeId;           //һ�������Ϊ1���ڵ�
#endif
public:
	CDoid()
	{
 #ifdef _DEBUG
        m_nodeId = 0xFFFF; 
        m_virtualIp = 0xFF;
        m_srvId = 0xFF;
        m_objId = 0xFFFFFFFF;
 #endif
	}
    CDoid(TUInt16 nNodeId,TUInt8 nVip,TUInt8 nSrvId,TUInt32 nObjId)
    {
        m_nodeId = nNodeId;
        m_virtualIp = nVip;
        m_srvId = nSrvId;
        m_objId = nObjId;
    }
	
    ~CDoid()
    {

    }
    TBOOL IsInOneIp(CDoid &another)
    {
        if (m_nodeId != another.m_nodeId)
        {
            return FALSE;
        }
        if (m_virtualIp != another.m_virtualIp)
        {
            return FALSE;
        }
        return TRUE;
    }
    
	inline bool operator==(const CDoid& rvalue) const 
	{
        return (*((const TUInt64 *)this) == *((const TUInt64 *)(&rvalue)));
		//return ((m_objId == rvalue.m_objId)&&(m_srvId == rvalue.m_srvId)&&(m_virtualIp == rvalue.m_virtualIp)&&(m_nodeId == rvalue.m_nodeId));
	}
    inline bool operator>(const CDoid &rvalue) const
    {
        return (*((const TUInt64 *)this) > *((const TUInt64 *)(&rvalue))); 
    }
    inline bool operator < (const CDoid &rvalue) const
    {
        return (*((const TUInt64 *)this) < *((const TUInt64 *)(&rvalue)));  
    }
	inline TUInt64 ToTUIn64() const
	{
		return *((TUInt64 *)this);
	}

    inline CDoid &operator=(TUInt64& nNewValue)
	{
		*((TUInt64 *)this) = nNewValue;
        return *this;
	}
    //Buff need 32 bytes!
	void ToStr(TChar *pBuff);
};


TInt32 Unmarshall(TUChar *pBuffer,TUInt32 nLen,CDoid *&pDoid);
TInt32 Marshall(TUChar *pBuffer,TUInt32 nLen,CDoid *pDoid);

TInt32 Unmarshall(TUChar *pBuffer,TUInt32 nLen,CDoid& pDoid);
TInt32 Marshall(TUChar *pBuffer,TUInt32 nLen,CDoid& pDoid);

const CDoid NULL_DOID;
}

#endif
