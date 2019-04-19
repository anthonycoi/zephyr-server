/*-------------------------------------------------------------
 Copyright (C)| 
 File: Doid.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  分布式对象地址
 Others: virtualIp = 0用来作为中心节点，负责节点间通信\命名服务器\管理服务器.
         一个node就是一个内部节点，一般在一个网段内，这些机器之间是全练级，
         共同提供某项服务，一个节点由大约254台机器机器构成，这些再多的话没必要提供直接的全连接，
         同时可以有65534个node，这些Node之间可以试全连接，也可以是弱连接，
         即需要通过中间节点的转发而连接,连接和转发算法类似ip..
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
    TUInt32  m_srvId;             //srv id, decide the port,每个虚拟ip上可以跑多个service.

    TUInt32  m_virtualIp;               //一组服务器内的虚拟ip
    TUInt32  m_nodeId;           //一组服务器为1个节点
#else
    TUInt32 m_objId;                   //obj idx;
    TUInt8  m_srvId;             //srv id, decide the port,每个虚拟ip上可以跑多个service.

    TUInt8  m_virtualIp;               //一组服务器内的虚拟ip
    TUInt16 m_nodeId;           //一组服务器为1个节点
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
