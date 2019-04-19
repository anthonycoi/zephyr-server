#ifndef __DB_LIB_MYSQL_CONNECTION_H__
#define __DB_LIB_MYSQL_CONNECTION_H__

#include "DBLib.h"


namespace DBLib
{

typedef MYSQL * MYSQL_HANDLE;
typedef MYSQL_RES * MYSQL_RES_HANDLE;

class CMySQLDatabase;

class CMySQLConnection :
	public IDBConnection
{
protected:
	MYSQL_HANDLE		m_MySQLHandle;
	CMySQLDatabase *	m_pDatabase;

	DECLARE_CLASS_INFO(CMySQLConnection)
public:
	CMySQLConnection(void);
	virtual ~CMySQLConnection(void);

	int Init(CMySQLDatabase * pDatabase);
	virtual void Destory();

	virtual IDBRecordSet * CreateRecordSet(int RecordSetType);
	virtual IDBParameterSet * CreateParameterSet(int RecordSetType);

	virtual int Connect(LPCTSTR ConnectStr);
	virtual int Disconnect();
	virtual BOOL IsConnected();
	virtual int ExecuteSQL(const char* SQLStr,int StrLen=0,IDBParameterSet * pParamSet=NULL);		
	virtual int GetResults(IDBRecordSet * pDBRecordset);
	virtual int NextResults(IDBRecordSet * pDBRecordset);	
	virtual int GetAffectedRowCount();
	virtual int EnableTransaction(BOOL IsEnable);
	virtual int Commit();
	virtual int RollBack();

	virtual unsigned int GetLastDatabaseErrorCode();
	virtual const char* GetLastDatabaseErrorString();

	virtual int TranslateString(const char* szSource,int SrcLen,char *szTarget,int MaxLen);

    virtual unsigned __int64 GetInsertId(void);
	static int MySQLValueToDBValue(int ValueType,void *pData,int DataSize,int DitigalSize,CDBValue& DBValue);

	void ProcessErrorMsg(const char* Msg);
protected:
	int FetchStaticResult(CDBStaticRecordSet * pDBRecordset);
	int FetchResult(CMySQLRecordSet * pDBRecordset);
	unsigned int FetchConnectFlags(const char* FlagsStr);
};

}

#endif
