#include "DBLib.h"


namespace DBLib
{

IMPLEMENT_CLASS_INFO(CMySQLDatabase,IDatabase);

CMySQLDatabase::CMySQLDatabase(void):IDatabase()
{
	mysql_library_init(0,NULL,NULL);
}

CMySQLDatabase::~CMySQLDatabase(void)
{
	mysql_library_end();
}


IDBConnection * CMySQLDatabase::CreateConnection()
{
	CMySQLConnection * pConnection=new CMySQLConnection();
	if(pConnection->Init(this)==DBERR_SUCCEED)
		return pConnection;
	pConnection->Release();
	return NULL;
}

int CMySQLDatabase::DeleteConnection(IDBConnection * pDBConnection)
{
	if(pDBConnection)
	{
		pDBConnection->Release();
		return DBERR_SUCCEED;
	}
	return DBERR_INVALID_CONNECTION;
}

}
