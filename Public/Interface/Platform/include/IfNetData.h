/*-------------------------------------------------------------
 Copyright (C)| 
 File: IfNetData.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  传输层接口数据结构定义
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IF_NET_DATA_H__
#define __ZEPHYR_IF_NET_DATA_H__

#include "../../../include/TypeDef.h"
#include "../../../include/SysMacros.h"

namespace Zephyr
{

class CConPair
{
protected:
    //all in mechina order. and this can be run only in little-endian mode
    TUInt32 m_remoteIp; //这个ip是网络序的，反正差不多，就用了网络序。
    TUInt32 m_myIp;
    TUInt16 m_remotePort; //port是小端在前，为了方便debug
    TUInt16 m_myPort;
    
public:
    CConPair()
    {
        m_remoteIp      = 0;
        m_myIp          = 0;
        m_remotePort    = 0;
        m_myPort        = 0;
    };
    ~CConPair()
    {
        
    }
    void Init(TUInt32 remoteIp,TUInt32 myIp, TUInt16 remotePort,TUInt16 myPort)
    {
        m_remoteIp = remoteIp;
        m_myIp     = myIp;
        m_remotePort = remotePort;
        m_myPort     = myPort;
    }
    TUInt32 GetRemoteIp()
    {
        return m_remoteIp;
    }
    TUInt32 GetMyIp()
    {
        return m_myIp;
    }
    TUInt16 GetRemotePort()
    {
        return m_remotePort;
    }
    TUInt16 GetMyPort()
    {
        return m_myPort;
    }
    bool operator==(CConPair &rvalue)
    {
         return ((m_remoteIp == rvalue.m_remoteIp) && (m_myIp == rvalue.m_myIp) && (m_remotePort == rvalue.m_remotePort) && (m_myPort == rvalue.m_myPort));

    }
    bool operator > (CConPair &rvalue)
    {
         if (m_remoteIp > rvalue.m_remoteIp)
         {
            return true;
         }
         else if (m_remoteIp < rvalue.m_remoteIp)
         {
            return false;
         }
         else
         {
            if (m_remotePort > rvalue.m_remotePort)
            {
                return true;
            }
            else if (m_remotePort < rvalue.m_remotePort)
            {
                return false;
            }
            else
            {
                if (m_myIp > rvalue.m_myIp)
                {
                    return true;
                }
                else if (m_myIp < rvalue.m_myIp)
                {
                    return false;
                }
                else
                {
                    if (m_myPort > rvalue.m_myPort)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
         }
    }
    bool operator < (CConPair &rvalue)
    {
        return !((*this) > rvalue);
    }
};

}
#endif

