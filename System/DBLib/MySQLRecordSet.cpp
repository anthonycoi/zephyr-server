#include "DBLib.h"


namespace DBLib
{

IMPLEMENT_CLASS_INFO(CMySQLRecordSet,IDBRecordSet);

CMySQLRecordSet::CMySQLRecordSet(void)
{
	m_pDBConnection=NULL;
	m_hResults=NULL;
	m_CurRow=0;
}

CMySQLRecordSet::~CMySQLRecordSet(void)
{
	Destory();
}

int CMySQLRecordSet::Init(CMySQLConnection * pDBConnection,MYSQL_RES_HANDLE hResults)
{
	if(pDBConnection==NULL||hResults==NULL)
		return DBERR_INVALID_PARAM;
	Destory();
	m_pDBConnection=pDBConnection;
	m_hResults=hResults;
	int ColNum=GetColumnCount();
	m_pColumnInfos.resize(ColNum);
	m_RowBuffer.resize(ColNum);
	MYSQL_FIELD * pFields=mysql_fetch_fields(hResults);
	for(int i=0;i<ColNum;i++)
	{
		strncpy_s(m_pColumnInfos[i].Name,MAX_COLUMN_NAME,pFields[i].name,MAX_COLUMN_NAME);
		m_pColumnInfos[i].Name[MAX_COLUMN_NAME-1]=0;
		m_pColumnInfos[i].Type=pFields[i].type;
		m_pColumnInfos[i].Size=pFields[i].length;
		m_pColumnInfos[i].DigitSize=pFields[i].decimals;

	}

	int Ret=FetchRow();
	if(Ret==DBERR_SUCCEED)
	{
		m_CurRow=0;
	}
	else
	{
		m_CurRow=-1;
	}
	return Ret;
}

void CMySQLRecordSet::Destory()
{
	if(m_hResults)
	{
		mysql_free_result(m_hResults);
		m_hResults=NULL;
	}
	m_pDBConnection=NULL;
	m_CurRow=0;
	m_pColumnInfos.clear();
	m_RowBuffer.clear();
}

int CMySQLRecordSet::GetRecordCount()
{
	if(m_hResults)
		return (int)mysql_num_rows(m_hResults);
	else
		return 0;
}

int CMySQLRecordSet::GetColumnCount()
{
	if(m_hResults)
		return (int)mysql_num_fields(m_hResults);
	else
		return 0;
}

LPCTSTR CMySQLRecordSet::GetColumnName(int Index)
{
	if(Index>=0&&Index<(int)m_pColumnInfos.size())
	{
		return m_pColumnInfos[Index].Name;
	}
	return NULL;
}

int CMySQLRecordSet::GetIndexByColumnName(LPCTSTR Name)
{
	for(int i=0;i<(int)m_pColumnInfos.size();i++)
	{
		if(_strnicmp(m_pColumnInfos[i].Name,Name,MAX_COLUMN_NAME)==0)
			return i;
	}
	return -1;
}

DB_COLUMN_INFO * CMySQLRecordSet::GetColumnInfo(int Index)
{
	if(Index>=0&&Index<(int)m_pColumnInfos.size())
	{
		return &(m_pColumnInfos[Index]);
	}
	return NULL;
}


CDBValue& CMySQLRecordSet::GetField(int Index)
{
	if(Index>=0&&Index<(int)m_RowBuffer.size())
	{
		return m_RowBuffer[Index];
	}
	return m_EmptyValue;
}

CDBValue& CMySQLRecordSet::GetField(LPCTSTR Name)
{
	int Index=GetIndexByColumnName(Name);
	if(Index>=0)
	{
		return m_RowBuffer[Index];
	}
	return m_EmptyValue;
}

int CMySQLRecordSet::MoveFirst()
{
	if(m_hResults)
	{
		if(m_CurRow>0)
		{
			m_CurRow=0;
			mysql_data_seek(m_hResults,m_CurRow);
			FetchRow();
		}
		return DBERR_SUCCEED;
	}
	else
	{
		return DBERR_NO_RECORDS;
	}

}

int CMySQLRecordSet::MoveLast()
{
	if(m_hResults)
	{
		if(m_CurRow<GetRecordCount()-1)
		{
			m_CurRow=GetRecordCount()-1;
			mysql_data_seek(m_hResults,m_CurRow);
			FetchRow();
		}
		return DBERR_SUCCEED;
	}
	else
	{
		return DBERR_NO_RECORDS;
	}
}

int CMySQLRecordSet::MoveNext()
{
	if(m_hResults)
	{
		m_CurRow++;
		if(m_CurRow<GetRecordCount())
		{

			mysql_data_seek(m_hResults,m_CurRow);
			FetchRow();
			return DBERR_SUCCEED;
		}
		else
		{
			return DBERR_IS_RECORDSET_TAIL;
		}
	}
	else
	{
		return DBERR_NO_RECORDS;
	}
}

int CMySQLRecordSet::MovePrevious()
{
	if(m_hResults)
	{
		m_CurRow--;
		if(m_CurRow>=0)
		{
			mysql_data_seek(m_hResults,m_CurRow);
			FetchRow();
			return DBERR_SUCCEED;
		}
		else
		{
			return DBERR_IS_RECORDSET_HEAD;
		}
	}
	else
	{
		return DBERR_NO_RECORDS;
	}
}

int CMySQLRecordSet::MoveTo(int Index)
{
	if(m_hResults)
	{
		if(Index>=0||Index<GetRecordCount()-1)
		{
			if(m_CurRow!=Index)
			{
				m_CurRow=Index;
				mysql_data_seek(m_hResults,m_CurRow);
				FetchRow();
			}
			return DBERR_SUCCEED;
		}
		else
		{
			return DBERR_INVALID_RECORD_POSITION;
		}
	}
	else
	{
		return DBERR_NO_RECORDS;
	}
}

bool CMySQLRecordSet::IsEOF()
{
	if(m_hResults)
	{
		return m_CurRow>=GetRecordCount()||GetRecordCount()<=0;
	}
	else
		return true;

}

bool CMySQLRecordSet::IsBOF()
{
	if(m_hResults)
	{
		return m_CurRow<0||GetRecordCount()<=0;
	}
	else
		return true;
}

bool CMySQLRecordSet::Close()
{
	return true;
}

int CMySQLRecordSet::FetchRow()
{
	int ColNum=GetColumnCount();
	MYSQL_ROW RowData=mysql_fetch_row(m_hResults);
	ULONG * ValueLen=mysql_fetch_lengths(m_hResults);
	if(RowData&&ValueLen)
	{
		for(int j=0;j<ColNum;j++)
		{
			CMySQLConnection::MySQLValueToDBValue(
				m_pColumnInfos[j].Type,RowData[j],
				ValueLen[j],m_pColumnInfos[j].DigitSize,
				m_RowBuffer[j]);
		}
		return DBERR_SUCCEED;
	}
	else
	{
		return DBERR_NO_RECORDS;
	}
}

}
