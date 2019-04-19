#include "DBLib.h"


namespace DBLib
{


CDBTransaction::CDBTransaction(void)
{
	m_IsSucceed=false;
}

CDBTransaction::~CDBTransaction(void)
{
}

bool CDBTransaction::OnExecute(void * pConnection)
{
	return false;
}

void CDBTransaction::OnFinish()
{
}

}
