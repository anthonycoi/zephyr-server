/*-------------------------------------------------------------
 Copyright (C)| 
 File: Message.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  �������Ϣ
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/
#ifndef S_MESSAGE_H
#define S_MESSAGE_H


#include "TypeDef.h"


#include "Doid.h"
#include "SysMacros.h"
#include <string.h>
namespace Zephyr
{

enum EnConnectEvent
{
    en_connection_is_broken_event             = 0,
    en_connection_is_established_event        = 1,
};

struct CConnectionEvent
{
    unsigned short m_nNodeId;
    unsigned short m_nVip;
    unsigned int   m_nEvent;
};



//the following was used for srv

enum EnByteOrder
{
    little_endian         = 0,
    big_endian            = 1,
};




class CMessageHeader
{
public:
    union UnMsgInfo
    {
        struct 
        {
			//���256k�ֽڣ�����.
			//ת�����ͻ��˵�ʱ��ֱ�Ӵ����m_msgInfo��ʼ����������.
			//ֱ��������bodyLength
            TUInt64    m_msgBodyLength:18;
            TUInt64    m_nrOfBroadcastDoid:6;
            //Ҳ��Ҫ��.
			//TUInt32    m_byteOrder:1;
            
			TUInt64	   m_timeStamp:16;

            //the 2 bits indicated transport priority, (default yes,that means we will retry the sending.)
            TUInt64    m_priority:1;
            //the 2nd bit indicate whether if we should report the exceptions (Default yes)
            TUInt64    m_reportExcep:1;
            //the 3rd bits indicated if it is marked to be traced. Reserved.
            TUInt64    m_needTrace:1;
			/*
            //the 4th bits indicated if it needs synchronization,that means the skeleton will hang up.  Reserved.
            TUInt32    m_needSyn:1;
            // no use.
            TUInt32    m_reserved:1;
			*/
            //the 5th bits indicated if it is a system call and it should not be reported to the application.
            //1 for net layer,heartbeat, 2 for system calls,like for time synchronization. service heartbeat. service managerment and so on.
            TUInt64    m_systemCall:1;
           
            //1024��ServiceӦ��Ҳ���˰�
			//����10��bit, �ӿ�2,����8���� // һ������ֻҪ4���ӿڣ�Ӧ���㹻�ˣ�(Service/Session) * 2(Reply)��,(ÿ���ӿ�256������Ӧ��Ҳ�㹻�ˣ�һ���಻Ӧ���ṩ���෽����)
			TUInt64    m_methodId:20;
        };
        TUInt64 m_data;
    };
private: //�Ĺ����ģ���Ҫ�鷳��
	
	CDoid m_destDoid;          //the destination obj id(may have a lot, max 256)
    //the 1st  bit indicate the byte-orders of this msg.
    //the following 7 bits indicate how many dest doid does this header has;
    //the rest 24 bits stored the length of this msg body
    //TUInt64 m_msgInfo;
	//�������copy��.
	CDoid m_srcDoid;           //the source obj id
    
    UnMsgInfo m_msgInfo;

	//��Ҫ��,timeStamp�ŵ�m_data��
    //TUInt16 m_timeStamp;
    //TUChar m_checkSum[2];



public:
    //��MessageId
    static TUInt32 GetServiceID(TUInt32 uMethodId)
    {
        return (uMethodId&0x000FFF00);
    }

	inline TUInt32 GetBodyLength()
	{
		return m_msgInfo.m_msgBodyLength;
	}

    inline TUChar *GetBody()
    {
        return (TUChar*)((TUChar*)this + sizeof (CMessageHeader));
    }
	//���������Ϣ�ĳ���
    inline TUInt32 GetLength()
    {
        return  m_msgInfo.m_msgBodyLength + sizeof(CMessageHeader) + (m_msgInfo.m_nrOfBroadcastDoid) * sizeof(CDoid);
    }

    inline void SetPriority(TUInt32 prioity)
    {
        m_msgInfo.m_priority = prioity;
    }
    inline TUInt32 GetPriority()
    {
        return m_msgInfo.m_priority;
    }

	inline TUInt32 GetBroadcastDoidNr()
    {
        return m_msgInfo.m_nrOfBroadcastDoid;
    }
    inline TInt32 SetBroadcastDoid(TUInt32 num)
    {
        if (num > 63)
        {
            return OUT_OF_RANGE;
        }
        m_msgInfo.m_nrOfBroadcastDoid = num;
        return num;
    }
    inline CDoid  *GetDestDoidByIdx(TUInt32 idx = 0)
    {
        if (0 == idx)
        {
            return &m_destDoid;
        }
        else
        {
            //�������,��ס�����߱����Լ���֤idx�Ŀɿ���
            //if (idx < m_msgInfo.m_nrOfDestDoid)
            {
                return (CDoid  *) ((TUChar*)this + sizeof(CMessageHeader) + GetBodyLength() + sizeof(CDoid) * (idx - 1));
            }
        }
    }
    inline CDoid *GetBroadcastDoids()
    {
        if (m_msgInfo.m_nrOfBroadcastDoid)
        {
            return (CDoid  *) ((char*)this + sizeof(CMessageHeader) +  m_msgInfo.m_msgBodyLength);
        }
        return NULL;
    }
   
    
    //client only need know these things above.

    inline CDoid *GetSrcDoid()
    {
        return &m_srcDoid;
    }

    inline TUInt32 GetMethodId()
    {
        return (m_msgInfo.m_methodId);
    }

	inline void SetTimeStamp(TUInt16 timeStamp)
	{
		m_msgInfo.m_timeStamp = timeStamp;
	}
	inline TUInt16 GetTimeStamp()
	{
		return m_msgInfo.m_timeStamp;
	}
    /*inline void PrepareToSend(TUChar checkCode1,TUChar checkCode2)
    {
        TUChar* pBuffer = (TUChar*)this;
		m_checkSum[0] = checkCode1;
		m_checkSum[1] = checkCode2;
		int len = (sizeof(CMessageHeader)-2);
        for(int i =0;i<len;i+=2)
        {
            m_checkSum[0] = m_checkSum[0] ^ pBuffer[i];
            m_checkSum[1] = m_checkSum[1] ^ pBuffer[i+1];
        }
    }*/
    
    //���һ����������˼��DestDoid�Ƿ���Ҫ����
    TInt32 Init(TUInt32 bodyLength,TUInt32 methodId,CDoid srcId,CDoid* pDestDoids,TUInt32 destDoidNum,bool bNeedRearrangeDestDoid);

	//��ʹ��GetCompackedLen()��ó���
    void Compack4Client(TUChar *pBuff)
    {
        memcpy(pBuff,&m_msgInfo,(GetBodyLength() + sizeof(m_msgInfo)));
    }
    TInt32 GetCompackedLen()
    {
        //6bytes for compacked header
        return GetBodyLength() + sizeof(m_msgInfo);
    }
    //ֻ�ܴ�0��ʼ��������Ϣͷ�����ҵ���
    //�ú�����[fromDest,to)�Ĺ㲥��ַ���Ƶ�[0,(to-fromDest))��ַ
    void ReInitMsg4Send(TInt32 fromDest,TInt32 to);
    //ʹ��
//     inline TUInt32 RemarkHead(CDoid *pDoid,TUChar checkCode1,TUChar checkCode2)
//     {
//         m_destDoid = *pDoid;
//         m_msgInfo.m_nrOfDestDoid = 1;
//         PrepareToSend(checkCode1,checkCode2);
//     }
    
//private:
    //can be called by the communicator only. these are internal info;
    //no use now
    /*bool IsOnExtension()
    {
        return false;
    }
    */
    //no use now
    //int IsSliced();
    TInt32 ResetBodyLength(TUInt32 bodyLength);
    void SetSystemCall()
    {
        m_msgInfo.m_systemCall = 1;
    }
    TBOOL IsSystemCall()
    {
        if (m_msgInfo.m_systemCall)
        {
            return TRUE;
        }
        return FALSE;
    }
private:
	//to make sure you could only create the message by msgPool.
	CMessageHeader()
	{
	    m_msgInfo.m_data = 0;
	};
    //set to limit the "=" operation.
	CMessageHeader &operator=(CMessageHeader& msg){};

};



//make sure len > sizeof(SCTDMessageHeader)
}
#endif

