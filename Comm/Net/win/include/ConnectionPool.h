/*-------------------------------------------------------------
 Copyright (C)| 
 File: ConnectionPool.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  Á¬½Ó³Ø
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/
#ifndef __ZEPHYR_COMM_NET_CONNECTION_POOL_H__
#define __ZEPHYR_COMM_NET_CONNECTION_POOL_H__
#include "TypeDef.h"
#include "SysMacros.h"
#include "Connection.h"
#include "TplPool.h"
namespace Zephyr
{
    ItemClassPool<CConnection>;
}

#endif
