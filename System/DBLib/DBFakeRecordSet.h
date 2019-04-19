#ifndef __DB_LIB_FAKE_RECORD_SET_H__
#define __DB_LIB_FAKE_RECORD_SET_H__


#include "dbinterfaces.h"

namespace DBLib
{

class CDBFakeRecordSet :
	public IDBRecordSet
{
protected:
	CEasyBuffer		m_DataBuffer;
	CDBValue		m_EmptyValue;
public:
	enum GET_DATA_RETURN_CODE
	{
		GET_DATA_NO_MORE_DATA,
		GET_DATA_BUFFER_FULL,
	};

	CDBFakeRecordSet(void);
	virtual ~CDBFakeRecordSet(void);

	virtual int Init(LPCVOID pData,int DataSize);	
	virtual void Destory();	
	virtual int GetRecordSetDataSize();
	virtual LPCVOID GetRecordSetData();
	virtual int GetRecordSetDataEx(int& Pos,LPVOID pBuffer,int& BufferSize);

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
};

}
#endif
