#include "MySqlConnection.h"

#ifdef WIN32

#include "MySQL\include\mysql.h"

#else

#include <mysql/mysql.h>

#endif
#include "MySQLRecordSet.h"

namespace DBLib
{

IMPLEMENT_CLASS_INFO(CMySQLConnection,IDBConnection);

CMySQLConnection::CMySQLConnection(void)
{
	m_MySQLHandle=NULL;
	m_pDatabase=NULL;
}

CMySQLConnection::~CMySQLConnection(void)
{
	Destory();
}

int CMySQLConnection::Init(CMySQLDatabase * pDatabase)
{
	m_pDatabase=pDatabase;
	return DBERR_SUCCEED;
}

void CMySQLConnection::Destory()
{
	Disconnect();
	m_pDatabase=NULL;
}

IDBRecordSet * CMySQLConnection::CreateRecordSet(int RecordSetType)
{
	switch(RecordSetType)
	{
	case DB_RS_TYPE_STATIC:
	case DB_RS_TYPE_GENERAL_STATIC:
		return new CDBStaticRecordSet();
	case DB_RS_TYPE_DYNAMIC:
		return new CMySQLRecordSet();
	}
	return NULL;
}

IDBParameterSet * CMySQLConnection::CreateParameterSet(int RecordSetType)
{
	return NULL;
}

int CMySQLConnection::Connect(LPCTSTR ConnectStr)
{
	Disconnect();
	m_MySQLHandle=mysql_init(NULL);
	if(m_MySQLHandle==NULL)
		return DBERR_MYSQL_MYSQLINITFAIL;
	//mysql_options(m_MySQLHandle,MYSQL_SET_CHARSET_NAME,"gb2312");
	CSettingFile StrAnalyzer;
	StrAnalyzer.Load(ConnectStr,';',0);
	const char * Host=StrAnalyzer.GetString(NULL,"Server","");
	const char * User=StrAnalyzer.GetString(NULL,"UID","");
	const char * Password=StrAnalyzer.GetString(NULL,"PWD","");
	const char * FlagsStr=StrAnalyzer.GetString(NULL,"Flag","");
	const char * CharSetStr=StrAnalyzer.GetString(NULL,"CharSet","");
	if(Password[0]==0)
		Password=NULL;
	const char * DB=StrAnalyzer.GetString(NULL,"DB","");
	if(DB[0]==0)
		DB=NULL;
	const char * UnixSocket=StrAnalyzer.GetString(NULL,"UnixSocket","");
	if(UnixSocket[0]==0)
		UnixSocket=NULL;
	int Port=StrAnalyzer.GetInteger(NULL,"Port",3306);
	unsigned int Flags=FetchConnectFlags(FlagsStr);

	if(mysql_real_connect(m_MySQLHandle,Host,User,Password,DB,Port,UnixSocket,
		Flags)==NULL)
	{
		ProcessErrorMsg("连接失败\r\n");
		return DBERR_MYSQL_CONNECTFAIL;
	}

	if(CharSetStr[0])
	{
		if(mysql_set_character_set(m_MySQLHandle,CharSetStr))
			ProcessErrorMsg("设置字符集失败\r\n");
	}

	return DBERR_SUCCEED;
}

int CMySQLConnection::Disconnect()
{
	if(m_MySQLHandle)
	{
		mysql_close(m_MySQLHandle);
		m_MySQLHandle=NULL;
	}
	return DBERR_SUCCEED;
}
BOOL CMySQLConnection::IsConnected()
{
	if(m_MySQLHandle==NULL)
		return FALSE;
	return mysql_ping(m_MySQLHandle)==0;
}

int CMySQLConnection::ExecuteSQL(LPCSTR SQLStr,int StrLen,IDBParameterSet * pParamSet)
{
	if(m_MySQLHandle==NULL)
		return DBERR_MYSQL_WANTCONNECT;
	if(!SQLStr)
	{
		return DBERR_INVALID_PARAM;
	}

	if(StrLen==0)
		StrLen=(int)strlen(SQLStr);
	if(mysql_real_query(m_MySQLHandle,SQLStr,StrLen))
	{
		ProcessErrorMsg("执行SQL失败\r\n");
		return DBERR_EXE_SQL_FAIL;
	}

	return DBERR_SUCCEED;
}

int CMySQLConnection::GetResults(IDBRecordSet * pDBRecordset)
{
	if(!pDBRecordset)
	{
		return DBERR_INVALID_PARAM;
	}
	if(pDBRecordset->IsKindOf(GET_CLASS_INFO(CDBStaticRecordSet)))
	{
		return FetchStaticResult((CDBStaticRecordSet *)pDBRecordset);
	}
	else if(pDBRecordset->IsKindOf(GET_CLASS_INFO(CMySQLRecordSet)))
	{
		return FetchResult((CMySQLRecordSet *)pDBRecordset);
	}
	return DBERR_INVALID_PARAM;

}

int CMySQLConnection::NextResults(IDBRecordSet * pDBRecordset)
{
	if(!mysql_more_results(m_MySQLHandle))
	{
		return DBERR_NO_MORE_RESULTS;
	}
	int RetCode=mysql_next_result(m_MySQLHandle);
	if(RetCode>0)
	{
		ProcessErrorMsg("执行SQL失败\r\n");
		return DBERR_EXE_SQL_FAIL;
	}
	return DBERR_SUCCEED;
}

int CMySQLConnection::GetAffectedRowCount()
{
	int RowCount=(int)mysql_affected_rows(m_MySQLHandle);
	if(RowCount==-1)
	{
		ProcessErrorMsg("获取影响行数失败\r\n");
	}
	return RowCount;
}



int CMySQLConnection::EnableTransaction(BOOL IsEnable)
{
	if(m_MySQLHandle==NULL)
		return DBERR_MYSQL_WANTCONNECT;
	if(mysql_autocommit(m_MySQLHandle,!IsEnable))
	{
		ProcessErrorMsg("设置事务失败\r\n");
		return DBERR_MYSQL_ENABLETRANSACTIONFAIL;
	}
	return DBERR_SUCCEED;
}
int CMySQLConnection::Commit()
{
	if(m_MySQLHandle==NULL)
		return DBERR_MYSQL_WANTCONNECT;
	if(mysql_commit(m_MySQLHandle))
	{
		ProcessErrorMsg("提交事务失败\r\n");
		return DBERR_MYSQL_COMMITFAIL;
	}
	return DBERR_SUCCEED;
}
int CMySQLConnection::RollBack()
{
	if(m_MySQLHandle==NULL)
		return DBERR_MYSQL_WANTCONNECT;
	if(mysql_rollback(m_MySQLHandle))
	{
		ProcessErrorMsg("回滚事务失败\r\n");
		return DBERR_MYSQL_ROLLBACKFAIL;
	}
	return DBERR_SUCCEED;
}

UINT CMySQLConnection::GetLastDatabaseErrorCode()
{
	return mysql_errno(m_MySQLHandle);
}

LPCTSTR CMySQLConnection::GetLastDatabaseErrorString()
{
	return mysql_error(m_MySQLHandle);
}

int CMySQLConnection::TranslateString(const char* szSource,int SrcLen,char* szTarget,int MaxLen)
{
	return (int)mysql_real_escape_string(m_MySQLHandle,szTarget,szSource,SrcLen);
}

unsigned __int64 CMySQLConnection::GetInsertId(void)
{
    return (unsigned __int64)mysql_insert_id(m_MySQLHandle);
}
void CMySQLConnection::ProcessErrorMsg(LPCTSTR Msg)
{
	PrintDBLog(0xff,"%s %s\r\n",Msg,mysql_error(m_MySQLHandle));
}

int CMySQLConnection::FetchStaticResult(CDBStaticRecordSet * pDBRecordset)
{
	CEasyBuffer ResultBuff;


	if(m_MySQLHandle==NULL)
		return DBERR_MYSQL_WANTCONNECT;
	MYSQL_RES_HANDLE hResults=mysql_store_result(m_MySQLHandle);
	if(hResults==NULL)
	{
		if(mysql_errno(m_MySQLHandle)==0)
			return DBERR_NO_RECORDS;
		else
		{
			ProcessErrorMsg("提取结果集失败\r\n");
			return DBERR_FETCH_RESULT_FAIL;
		}
	}

	//计算数据集大小
	int ColNum=mysql_num_fields(hResults);

	if(ColNum<=0)
		return DBERR_NO_RECORDS;

	int RowNum=(int)mysql_num_rows(hResults);
	if(RowNum<=0)
		return DBERR_NO_RECORDS;

	int DataSize=sizeof(int)*2+sizeof(DB_COLUMN_INFO)*ColNum;


	for(int i=0;i<RowNum;i++)
	{
		mysql_data_seek(hResults,i);
		MYSQL_ROW RowData=mysql_fetch_row(hResults);
		ULONG * ValueLen=mysql_fetch_lengths(hResults);
		for(int j=0;j<ColNum;j++)
		{
			DataSize+=sizeof(int)+ValueLen[j]+1;
		}
	}

	DataSize+=1024;
	ResultBuff.Create(DataSize);

	//获取结果集列信息
	ResultBuff.PushBack(&ColNum,sizeof(int));
	int RecordLineLen=0;
	if(ResultBuff.GetFreeSize()<sizeof(DB_COLUMN_INFO)*ColNum)
		return DBERR_BUFFER_OVERFLOW;

	DB_COLUMN_INFO * pColInfos=(DB_COLUMN_INFO *)ResultBuff.GetFreeBuffer();
	ZeroMemory(pColInfos,sizeof(DB_COLUMN_INFO)*ColNum);
	ResultBuff.SetUsedSize(ResultBuff.GetUsedSize()+sizeof(DB_COLUMN_INFO)*ColNum);
	MYSQL_FIELD * pFields=mysql_fetch_fields(hResults);
	for(int i=0;i<ColNum;i++)
	{
		strncpy_s(pColInfos[i].Name,MAX_COLUMN_NAME,pFields[i].name,MAX_COLUMN_NAME);
		pColInfos[i].Name[MAX_COLUMN_NAME-1]=0;
		pColInfos[i].Type=pFields[i].type;
		pColInfos[i].Size=pFields[i].length;
		pColInfos[i].DigitSize=pFields[i].decimals;

	}


	ResultBuff.PushBack(&RowNum,sizeof(int));
	for(int i=0;i<RowNum;i++)
	{
		mysql_data_seek(hResults,i);
		MYSQL_ROW RowData=mysql_fetch_row(hResults);
		ULONG * ValueLen=mysql_fetch_lengths(hResults);
		for(int j=0;j<ColNum;j++)
		{
			if(RowData[j])
			{
				ResultBuff.PushConstBack(int(ValueLen[j]),sizeof(int));
				ResultBuff.PushBack(RowData[j],ValueLen[j]);
			}
			else
			{
				ResultBuff.PushConstBack(-1,sizeof(int));
			}
		}
	}
	mysql_free_result(hResults);
	return pDBRecordset->Init(ResultBuff.GetBuffer(),ResultBuff.GetUsedSize());

}

int CMySQLConnection::FetchResult(CMySQLRecordSet * pDBRecordset)
{
	if(m_MySQLHandle==NULL)
		return DBERR_MYSQL_WANTCONNECT;
	MYSQL_RES_HANDLE hResults=mysql_store_result(m_MySQLHandle);
	if(hResults==NULL)
	{
		if(mysql_errno(m_MySQLHandle)==0)
			return DBERR_NO_RECORDS;
		else
		{
			ProcessErrorMsg("提取结果集失败\r\n");
			return DBERR_FETCH_RESULT_FAIL;
		}
	}
	return pDBRecordset->Init(this,hResults);
}

unsigned int CMySQLConnection::FetchConnectFlags(const char* FlagsStr)
{
	unsigned int Flags=0;
	CStringSplitter Splitter(FlagsStr,'|');
	for(int i=0;i<(int)Splitter.GetCount();i++)
	{
		CEasyString Flag=Splitter[i];
		Flag.Trim(' ');
		if(Flag.CompareNoCase("CLIENT_COMPRESS")==0)
		{
			Flags|=CLIENT_COMPRESS;
		}
		else if(Flag.CompareNoCase("CLIENT_FOUND_ROWS")==0)
		{
			Flags|=CLIENT_FOUND_ROWS;
		}
		else if(Flag.CompareNoCase("CLIENT_IGNORE_SPACE")==0)
		{
			Flags|=CLIENT_IGNORE_SPACE;
		}
		else if(Flag.CompareNoCase("CLIENT_INTERACTIVE")==0)
		{
			Flags|=CLIENT_INTERACTIVE;
		}
		else if(Flag.CompareNoCase("CLIENT_LOCAL_FILES")==0)
		{
			Flags|=CLIENT_LOCAL_FILES;
		}
		else if(Flag.CompareNoCase("CLIENT_MULTI_STATEMENTS")==0)
		{
			Flags|=CLIENT_MULTI_STATEMENTS;
		}
		else if(Flag.CompareNoCase("CLIENT_MULTI_RESULTS")==0)
		{
			Flags|=CLIENT_MULTI_RESULTS;
		}
		else if(Flag.CompareNoCase("CLIENT_NO_SCHEMA")==0)
		{
			Flags|=CLIENT_NO_SCHEMA;
		}
		else if(Flag.CompareNoCase("CLIENT_ODBC")==0)
		{
			Flags|=CLIENT_ODBC;
		}
		else if(Flag.CompareNoCase("CLIENT_SSL")==0)
		{
			Flags|=CLIENT_SSL;
		}

	}
	return Flags;
}

BOOL CMySQLConnection::MySQLValueToDBValue(int ValueType,void *pData,int DataSize,int DitigalSize,CDBValue& DBValue)
{
	switch(ValueType)
	{
	case MYSQL_TYPE_TINY:
		{
			if(pData)
			{
				char Value=(char)atol((const char*)pData);
				DBValue.SetValue(DB_TYPE_TINY,&Value,sizeof(Value),0);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_TINY,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_SHORT:
		{
			if(pData)
			{
				short Value=(short)atol((const char*)pData);
				DBValue.SetValue(DB_TYPE_SMALLINT,&Value,sizeof(Value),0);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_SMALLINT,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_LONG:
		{
			if(pData)
			{
				long Value=(long)atol((const char*)pData);
				DBValue.SetValue(DB_TYPE_INTEGER,&Value,sizeof(Value),0);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_INTEGER,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_INT24:
		{
			if(pData)
			{
				long Value=(long)atol((const char*)pData);
				DBValue.SetValue(DB_TYPE_INTEGER,&Value,sizeof(Value),0);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_INTEGER,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_LONGLONG:
		{
			if(pData)
			{
				INT64 Value=(INT64)_atoi64((const char*)pData);
				DBValue.SetValue(DB_TYPE_BIGINT,&Value,sizeof(Value),0);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_BIGINT,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_DECIMAL:
		{
			if(pData)
			{
				double Value=(double)atof((const char*)pData);
				DBValue.SetValue(DB_TYPE_DOUBLE,&Value,sizeof(Value),DitigalSize);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_DOUBLE,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_FLOAT:
		{
			if(pData)
			{
				float Value=(float)atof((const char*)pData);
				DBValue.SetValue(DB_TYPE_FLOAT,&Value,sizeof(Value),DitigalSize);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_FLOAT,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_DOUBLE:
		{
			if(pData)
			{
				double Value=(double)atof((const char*)pData);
				DBValue.SetValue(DB_TYPE_DOUBLE,&Value,sizeof(Value),DitigalSize);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_DOUBLE,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_NEWDECIMAL:
		{
			if(pData)
			{
				double Value=(double)atof((const char*)pData);
				DBValue.SetValue(DB_TYPE_DOUBLE,&Value,sizeof(Value),DitigalSize);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_DOUBLE,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_BIT:
		{
			DBValue.SetValue(DB_TYPE_BINARY,pData,DataSize,DitigalSize);
			return TRUE;
		}
	case MYSQL_TYPE_TIMESTAMP:
		{
			if(pData)
			{
				DB_TIMESTAMP Value;
				sscanf((const char*)pData,"%hd-%hd-%hd %hd:%hd:%hd.&d",
					&Value.year,&Value.month,&Value.day,
					&Value.hour,&Value.minute,&Value.second,
					&Value.fraction);
				DBValue.SetValue(DB_TYPE_TIMESTAMP,&Value,sizeof(Value),0);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_TIMESTAMP,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_DATE:
		{
			if(pData)
			{
				DB_DATE Value;
				sscanf((const char*)pData,"%hd-%hd-%hd",
					&Value.year,&Value.month,&Value.day);
				DBValue.SetValue(DB_TYPE_DATE,&Value,sizeof(Value),0);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_DATE,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_TIME:
		{
			if(pData)
			{
				DB_TIME Value;
				sscanf((const char*)pData,"%hd:%hd:%hd",
					&Value.hour,&Value.minute,&Value.second);
				DBValue.SetValue(DB_TYPE_TIME,&Value,sizeof(Value),0);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_TIME,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_DATETIME:
		{
			if(pData)
			{
				DB_TIMESTAMP Value;
				sscanf((const char*)pData,"%hd-%hd-%hd %hd:%hd:%hd",
					&Value.year,&Value.month,&Value.day,
					&Value.hour,&Value.minute,&Value.second);
				DBValue.SetValue(DB_TYPE_TIMESTAMP,&Value,sizeof(Value),0);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_TIMESTAMP,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_YEAR:
		{
			if(pData)
			{
				long Value=(long)atol((const char*)pData);
				DBValue.SetValue(DB_TYPE_INTEGER,&Value,sizeof(Value),0);
			}
			else
			{
				DBValue.SetValue(DB_TYPE_INTEGER,NULL,0,0);
			}
			return TRUE;
		}
	case MYSQL_TYPE_SET:
	case MYSQL_TYPE_ENUM:
	case MYSQL_TYPE_STRING:
		{
			DBValue.SetValue(DB_TYPE_STRING,pData,DataSize,DitigalSize);
			return TRUE;
		}
	case MYSQL_TYPE_VAR_STRING:
		{
			DBValue.SetValue(DB_TYPE_STRING,pData,DataSize,DitigalSize);
			return TRUE;
		}
	case MYSQL_TYPE_GEOMETRY:
	case MYSQL_TYPE_BLOB:
		{
			DBValue.SetValue(DB_TYPE_BINARY,pData,DataSize,DitigalSize);
			return TRUE;
		}
	}
	return FALSE;
}

}
