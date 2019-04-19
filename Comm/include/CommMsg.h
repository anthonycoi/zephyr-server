/*-------------------------------------------------------------
 Copyright (C)| 
 File: CommMsg.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  传输层消息定义，在这儿定义心跳消息、时间同步消息、异常消息
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/


#ifndef __ZEPHYR_COMM_COMM_MSG_H__
#define __ZEPHYR_COMM_COMM_MSG_H__

//Welcome 
#define _COMM_CONNECTION_WELCOME_MSG_ID_ 0x00000001

//心跳消息
#define _COMM_CONNECTION_HEART_BEAT_MSG_ID_ 0x00000002


//时间同步消息
#define _COMM_CONNECTION_TIME_SYNCHRONIZE_MSG_ID 0x00000003

//异常消息
#define _COMM_EXCEPTION_MSG_ID              0x00000004



#endif
