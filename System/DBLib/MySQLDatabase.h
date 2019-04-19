#ifndef __DB_LIB_MYSQL_DATABASE_H__
#define __DB_LIB_MYSQL_DATABASE_H__

#include "dbinterfaces.h"


//#pragma comment(lib,"mysqlclient.lib")

	



namespace DBLib
{

enum DB_MYSQL_ERROR_INFO
{
	DBERR_MYSQL_MYSQLINITFAIL=DBERR_MAX,
	DBERR_MYSQL_CONNECTFAIL,
	DBERR_MYSQL_WANTCONNECT,
	DBERR_MYSQL_COMMITFAIL,
	DBERR_MYSQL_ROLLBACKFAIL,
	DBERR_MYSQL_ENABLETRANSACTIONFAIL,
	DBERR_MYSQL_MAX,
};


class CMySQLDatabase :
	public IDatabase
{
protected:	

	DECLARE_CLASS_INFO(CMySQLDatabase)
public:
	CMySQLDatabase(void);
	virtual ~CMySQLDatabase(void);	

	virtual IDBConnection * CreateConnection();
	virtual int DeleteConnection(IDBConnection * pDBConnection);	
	
};



}

#endif