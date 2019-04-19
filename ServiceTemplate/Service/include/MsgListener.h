/*-------------------------------------------------------------
 Copyright (C)| 
 File: MsgLisnter.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 消息监听者.我们把消息来源分成3类：1、静态广播编号，如世界频道、静态地图频道(0.5G)
                                                2、静态对象编号，如静态地图上的NPC、固定刷的怪物、地形物等(0.5G)
                                                3、动态广播编号，如帮派、小队聊天(0.5G)
                                                4、动态对象编号，如人物、动态地图上的NPC(1G)
                                                5、地形物、掉落物品等(1G)

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
