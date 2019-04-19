/*-------------------------------------------------------------
 Copyright (C)| 
 File: CommConnection.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  ´«Êä²ãÁ¬½Ó³Ø
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/
#ifndef __ZEPHYR_COMM_COMM_CONNECTION_POOL_H__
#define __ZEPHYR_COMM_COMM_CONNECTION_POOL_H__

#include "CommConnection.h"

namespace Zephyr
{
class CComConnectionPool : public IfListenerCallBack
{
private:
    ItemClassPool<CCommConnection> m_pool;
public:
    virtual IfConnectionCallBack *OnNewConnection(CConPair *pPair);
    CCommConnection *GetConnection();
    void ReleaseConnection(CCommConnection *pConnection);
};
    
}


#endif
