/*-------------------------------------------------------------
 Copyright (C)| 
 File: MsgLisnter.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: ��Ϣ������.���ǰ���Ϣ��Դ�ֳ�3�ࣺ1����̬�㲥��ţ�������Ƶ������̬��ͼƵ��(0.5G)
                                                2����̬�����ţ��羲̬��ͼ�ϵ�NPC���̶�ˢ�Ĺ���������(0.5G)
                                                3����̬�㲥��ţ�����ɡ�С������(0.5G)
                                                4����̬�����ţ��������̬��ͼ�ϵ�NPC(1G)
                                                5�������������Ʒ��(1G)

 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/


#ifndef __ZEPHYR_GATEWAY_MSG_LISTENER__
#define __ZEPHYR_GATEWAY_MSG_LISTENER__

class CListener
{
public:
    TUInt32 m_uListnerId;
    void    *m_pListener;
    CListener()
    {
        m_uListnerId = 0;
        m_pListener = 0;
    }
};

class CMsgSender
{
public:
    TUInt32 m_uSender;  
    TplMap<CListener>   m_tLisnters;
    CMsgSender()
    {
        m_uSender = 0;
    }
};

#endif
