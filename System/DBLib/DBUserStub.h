#ifndef __DB_LIB_DB_USER_STUB_H__
#define __DB_LIB_DB_USER_STUB_H__

#include "DBLib.h"


class CDBUserStub
{
private:
    CDBTransationManager *m_pTransMgr;
    CDBTransationWorkThread *m_pWorkThread;
    int m_iPeddingTrans;
    int m_iLocked;
public:

};
#endif
