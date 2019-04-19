#ifndef __DB_LIB_DB_TRANSACTION_H__
#define __DB_LIB_DB_TRANSACTION_H__

//#include "DBLib.h"

namespace DBLib
{

class CDBTransaction 
{
protected:
	bool		m_IsSucceed;

	
public:
	CDBTransaction(void);
	virtual ~CDBTransaction(void);

	virtual bool OnExecute(void * pConnection);
	virtual void OnFinish();

	void SetResult(bool IsSucceed)
	{
		m_IsSucceed=IsSucceed;
	}
};

}

#endif
