/*-------------------------------------------------------------
 Copyright (C)| 
 File: IocpOverlappedDataHeader.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description: 连接层，标示一个连接池
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/

#ifndef __ZEPHYR_IOCP_OVERLAPPED_DATA_HEADER_H__
#define __ZEPHYR_IOCP_OVERLAPPED_DATA_HEADER_H__

#include "Typedef.h"
#include "SysMacros.h"
#include <winsock2.h>

namespace Zephyr
{

enum EnOperation
{
    overlapped_not_in_using              = 0x00,
    overlapped_try_reading               = 0x01,
    overlapped_try_writing               = 0x02,
    overlapped_try_reading_with_zero_mem = (0x04 | 0x01),    //没buff,就发空read下去，保持监听，或者没有
    overlapped_try_writing_with_zero_mem = (0x04 | 0x02),    //没buff,就发空write下去，保持监听，
};

class CConnection;
class CIocpOverlappedDataHeader
{
public:
    OVERLAPPED			m_ol;
    CConnection         *m_pConnection;
    WSABUF              m_wsaBuff;
    TUInt16             m_operation;
	TUInt16				m_seqNum;
public:
    void Init(CConnection *pConnection,TUInt32 seq)
    {
        m_pConnection = pConnection;
        m_operation = overlapped_not_in_using;
        m_seqNum    = seq;
        ZeroMemory(&m_ol, sizeof(OVERLAPPED));
    }
	bool InUsing()
    {
        if (m_operation != overlapped_not_in_using)
        {
            return true;
        }
        return false;
    }
    bool IsFree()
    {
        if (m_operation == overlapped_not_in_using)
        {
            return true;
        }
        return false;
    }
    void SetBuff(TChar *pChar,TUInt32 len)
    {
        m_wsaBuff.buf = pChar;
        m_wsaBuff.len  = len;
    }
    void FreeHeader()
    {
        m_operation = overlapped_not_in_using;
        //m_seqNum ++;
        ZeroMemory(&m_ol, sizeof(OVERLAPPED));
    }
    void PrepZeroByteRead()
    {
        m_wsaBuff.len = 0;
        m_operation = overlapped_try_reading_with_zero_mem;
    }
    
};






}

#endif


 