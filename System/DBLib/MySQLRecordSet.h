#ifndef __DB_LIB_MYSQL_RECORDSET_H__
#define __DB_LIB_MYSQL_RECORDSET_H__

#include "dbinterfaces.h"

namespace DBLib
{

class CMySQLConnection;
typedef MYSQL_RES * MYSQL_RES_HANDLE;

class CMySQLRecordSet :
	public IDBRecordSet
{
protected:
	CMySQLConnection *				m_pDBConnection;
	MYSQL_RES_HANDLE				m_hResults;
	std::vector<DB_COLUMN_INFO>		m_pColumnInfos;
	std::vector<CDBValue>			m_RowBuffer;
	int								m_CurRow;
	CDBValue						m_EmptyValue;

	DECLARE_CLASS_INFO(CMySQLRecordSet)
public:
	CMySQLRecordSet(void);
	virtual ~CMySQLRecordSet(void);

	int Init(CMySQLConnection * pDBConnection,MYSQL_RES_HANDLE hResults);
	virtual void Destory();

	virtual int GetRecordCount();
	virtual int GetColumnCount();
	virtual LPCTSTR GetColumnName(int Index);
	virtual int GetIndexByColumnName(LPCTSTR Name);
	virtual DB_COLUMN_INFO * GetColumnInfo(int Index);


	virtual CDBValue& GetField(int Index);
	virtual CDBValue& GetField(LPCTSTR Name);
	virtual int MoveFirst();
	virtual int MoveLast();
	virtual int MoveNext();
	virtual int MovePrevious();
	virtual int MoveTo(int Index);	
	virtual bool IsEOF();
	virtual bool IsBOF();

	virtual bool Close();
protected:
	int FetchRow();
};

}

#endif
