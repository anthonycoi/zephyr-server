#include "DBLib.h"
#include "DBStaticRecordSet.h"

namespace DBLib
{

IMPLEMENT_CLASS_INFO(CDBStaticRecordSet,IDBRecordSet);

CDBStaticRecordSet::CDBStaticRecordSet(void):IDBRecordSet()
{	
	m_CurRow=0;
}

CDBStaticRecordSet::~CDBStaticRecordSet(void)
{
	Destory();
}


int CDBStaticRecordSet::Init(LPCVOID pData,int DataSize)
{
	CEasyBuffer RecordBuffer;

	Destory();

	RecordBuffer.Create((LPVOID)pData,DataSize);
	RecordBuffer.SetUsedSize(DataSize);
	int ColNum=0;
	UINT PeekPtr=0;
	RecordBuffer.Peek(PeekPtr,&ColNum,sizeof(int));
	if(ColNum<=0)
		return DBERR_NO_RECORDS;
	m_pColumnInfos.resize(ColNum);
	for(int i=0;i<ColNum;i++)
	{
		RecordBuffer.Peek(PeekPtr,&(m_pColumnInfos[i]),sizeof(DB_COLUMN_INFO));
	}
	int RecordNum=0;
	RecordBuffer.Peek(PeekPtr,&RecordNum,sizeof(int));
	if(RecordNum<=0)
		return DBERR_NO_RECORDS;
	m_Records.resize(RecordNum);
	for(int i=0;i<RecordNum;i++)
	{
		m_Records[i].resize(ColNum);		
		for(int j=0;j<ColNum;j++)
		{
			int ValueSize=0;
			RecordBuffer.Peek(PeekPtr,&ValueSize,sizeof(int));			
			if(ValueSize<0)
			{
				m_Records[i][j].SetValue(m_pColumnInfos[j].Type,
					NULL,0,m_pColumnInfos[j].DigitSize);
				ValueSize=0;
			}
			else
			{
				m_Records[i][j].SetValue(m_pColumnInfos[j].Type,
					(BYTE *)RecordBuffer.GetBuffer()+PeekPtr,
					ValueSize,m_pColumnInfos[j].DigitSize);
			}
			PeekPtr+=ValueSize;			
		}		
	}
	

	return DBERR_SUCCEED;
}


void CDBStaticRecordSet::Destory()
{
	m_pColumnInfos.clear();
	m_Records.clear();
	m_CurRow=0;
}


int CDBStaticRecordSet::GetRecordCount()
{
	return (int)m_Records.size();
}

int CDBStaticRecordSet::GetColumnCount()
{
	return (int)m_pColumnInfos.size();
}

LPCTSTR CDBStaticRecordSet::GetColumnName(int Index)
{
	if(Index>=0&&Index<(int)m_pColumnInfos.size())
	{
		return m_pColumnInfos[Index].Name;
	}
	return NULL;
}

int CDBStaticRecordSet::GetIndexByColumnName(LPCTSTR Name)
{
	for(int i=0;i<(int)m_pColumnInfos.size();i++)
	{
		if(_strnicmp(m_pColumnInfos[i].Name,Name,MAX_COLUMN_NAME)==0)
			return i;
	}
	return -1;
}

DB_COLUMN_INFO * CDBStaticRecordSet::GetColumnInfo(int Index)
{
	if(Index>=0&&Index<(int)m_pColumnInfos.size())
	{
		return &(m_pColumnInfos[Index]);
	}
	return NULL;
}


CDBValue& CDBStaticRecordSet::GetField(int Index)
{
	if(m_CurRow>=0&&m_CurRow<(int)m_Records.size())
	{	
		CDBValueRow& Record=m_Records[m_CurRow];
		if(Index>=0&&Index<(int)Record.size())
		{
			return Record[Index];
		}
	}
	return m_EmptyValue;
}

CDBValue& CDBStaticRecordSet::GetField(LPCTSTR Name)
{
	if(m_CurRow>=0&&m_CurRow<(int)m_Records.size())
	{	
		CDBValueRow& Record=m_Records[m_CurRow];
		int Index=GetIndexByColumnName(Name);
		if(Index>=0)
		{
			return Record[Index];
		}
	}
	return m_EmptyValue;
}

int CDBStaticRecordSet::MoveFirst()
{
	if((int)m_Records.size()<=0)
		return DBERR_NO_RECORDS;
	m_CurRow=0;
	return DBERR_SUCCEED;
}

int CDBStaticRecordSet::MoveLast()
{
	if((int)m_Records.size()<=0)
		return DBERR_NO_RECORDS;
	m_CurRow=(int)m_Records.size();
	return DBERR_SUCCEED;
}

int CDBStaticRecordSet::MoveNext()
{
	m_CurRow++;

	if((int)m_Records.size()<=0)
		return DBERR_NO_RECORDS;
	if(m_CurRow>=(int)m_Records.size())
		return DBERR_IS_RECORDSET_TAIL;	

	return DBERR_SUCCEED;
}

int CDBStaticRecordSet::MovePrevious()
{
	m_CurRow--;

	if((int)m_Records.size()<=0)
		return DBERR_NO_RECORDS;
	if(m_CurRow<0)
		return DBERR_IS_RECORDSET_HEAD;
	
	return DBERR_SUCCEED;
}

int CDBStaticRecordSet::MoveTo(int Index)
{
	if((int)m_Records.size()<=0)
		return DBERR_NO_RECORDS;
	if(Index<0||Index>=(int)m_Records.size())
		return DBERR_INVALID_RECORD_POSITION;
	m_CurRow=Index;
	return DBERR_SUCCEED;
}

bool CDBStaticRecordSet::IsEOF()
{
	if(GetRecordCount())
	{
		return m_CurRow>=GetRecordCount();
	}
	else
		return true;
}

bool CDBStaticRecordSet::IsBOF()
{
	if(GetRecordCount())
	{
		return m_CurRow<0;
	}
	else
		return true;
}

bool CDBStaticRecordSet::Close()
{
	return true;
}

}