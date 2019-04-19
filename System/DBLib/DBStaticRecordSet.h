#ifndef __DB_LIB_STATIC_RECORD_SET_H__
#define __DB_LIB_STATIC_RECORD_SET_H__

#include "dbinterfaces.h"
#include <vector>

namespace DBLib
{



class CDBStaticRecordSet :
	public IDBRecordSet
{
protected:
	typedef std::vector<CDBValue> CDBValueRow;	
	std::vector<DB_COLUMN_INFO>		m_pColumnInfos;
	std::vector<CDBValueRow>		m_Records;
	std::vector<CDBValue>			m_Params;
	int								m_CurRow;
	CDBValue						m_EmptyValue;

	DECLARE_CLASS_INFO(CDBStaticRecordSet)
public:
	CDBStaticRecordSet(void);
	virtual ~CDBStaticRecordSet(void);	

	virtual int Init(LPCVOID pData,int DataSize);	
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

};

}

#endif
