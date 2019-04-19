#include "DBLib.h"
#include "DBValue.h"

namespace DBLib
{

IMPLEMENT_CLASS_INFO(CDBValue,CNameObject);

CDBValue::CDBValue(void)
{
	m_ValueType=0;	
	m_pData=NULL;	
	m_DataSize=0;
	m_DigitSize=0;
}

CDBValue::CDBValue(const CDBValue& Value)
{
	m_pData=NULL;
	m_DataSize=0;
	m_DigitSize=0;
	SetValue(Value.m_ValueType,Value.m_pData,Value.m_DataSize,Value.m_DigitSize);
}

CDBValue::CDBValue(bool Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(char Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(unsigned char Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(short Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(unsigned short Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(int Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(unsigned int Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(long Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(unsigned long Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(__int64 Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(unsigned __int64 Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(float Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(double Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(const char * Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(const wchar_t * Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
//CDBValue::CDBValue(const SYSTEMTIME& Value)
//{
//	m_pData=NULL;
//	m_DataSize=0;
//	*this=Value;
//}
//CDBValue::CDBValue(const FILETIME& Value)
//{
//	m_pData=NULL;
//	m_DataSize=0;
//	*this=Value;
//}
CDBValue::CDBValue(const DB_DATE& Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(const DB_TIME& Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(const DB_TIMESTAMP& Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}
CDBValue::CDBValue(const DB_GUID& Value)
{
	m_pData=NULL;
	m_DataSize=0;
	*this=Value;
}


CDBValue::~CDBValue(void)
{
	Destory();
}

void CDBValue::Destory()
{
	if(m_pData)
	{	
		SAFE_DELETE_ARRAY(m_pData);
		m_DataSize=0;
		m_DigitSize=0;
		m_ValueType=0;
	}
}

void CDBValue::SetValue(int ValueType,LPCVOID pData,int DataSize,int DitigalSize)
{	
	Destory();
	m_ValueType=ValueType;

	if(pData)
	{

	
		int AllocSize;

		switch(m_ValueType)
		{	
		case DB_TYPE_TINY:
			m_DataSize=1;				
			m_DigitSize=0;
			AllocSize=m_DataSize;
			break;
		case DB_TYPE_SMALLINT:
			m_DataSize=2;		
			m_DigitSize=0;
			AllocSize=m_DataSize;
			break;
		case DB_TYPE_INTEGER:
			m_DataSize=4;
			m_DigitSize=0;
			AllocSize=m_DataSize;
			break;
		case DB_TYPE_BIGINT:
			m_DataSize=8;
			m_DigitSize=0;
			AllocSize=m_DataSize;
			break;
		case DB_TYPE_FLOAT:
			m_DataSize=4;
			m_DigitSize=0;
			AllocSize=m_DataSize;
			break;
		case DB_TYPE_DOUBLE:
			m_DataSize=8;
			m_DigitSize=0;
			AllocSize=m_DataSize;
			break;
		case DB_TYPE_DATE:
			m_DataSize=sizeof(DB_DATE);
			m_DigitSize=0;
			AllocSize=m_DataSize;
			break;
		case DB_TYPE_TIME:
			m_DataSize=sizeof(DB_TIME);
			m_DigitSize=0;
			AllocSize=m_DataSize;
			break;
		case DB_TYPE_TIMESTAMP:
			m_DataSize=sizeof(DB_TIMESTAMP);
			m_DigitSize=0;
			AllocSize=m_DataSize;
			break;
		case DB_TYPE_GUID:
			m_DataSize=sizeof(DB_GUID);
			m_DigitSize=0;
			AllocSize=m_DataSize;		
			break;
		case DB_TYPE_STRING:
		case DB_TYPE_WSTRING:
		case DB_TYPE_BINARY:
			m_DataSize=DataSize;		
			m_DigitSize=0;
			AllocSize=m_DataSize+2;
			break;	
		default:
			return;
		}
		m_pData=new BYTE[AllocSize];
		ZeroMemory(m_pData,AllocSize);
		memcpy(m_pData,pData,m_DataSize);
	}
}

void CDBValue::SetEmptyValue(int ValueType,int DataSize,int DitigalSize)
{
	Destory();
	m_ValueType=ValueType;

	int AllocSize;

	switch(m_ValueType)
	{	
	case DB_TYPE_TINY:
		m_DataSize=1;				
		m_DigitSize=0;
		AllocSize=m_DataSize;
		break;
	case DB_TYPE_SMALLINT:
		m_DataSize=2;		
		m_DigitSize=0;
		AllocSize=m_DataSize;
		break;
	case DB_TYPE_INTEGER:
		m_DataSize=4;
		m_DigitSize=0;
		AllocSize=m_DataSize;
		break;
	case DB_TYPE_BIGINT:
		m_DataSize=8;
		m_DigitSize=0;
		AllocSize=m_DataSize;
		break;
	case DB_TYPE_FLOAT:
		m_DataSize=4;
		m_DigitSize=0;
		AllocSize=m_DataSize;
		break;
	case DB_TYPE_DOUBLE:
		m_DataSize=8;
		m_DigitSize=0;
		AllocSize=m_DataSize;
		break;
	case DB_TYPE_DATE:
		m_DataSize=sizeof(DB_DATE);
		m_DigitSize=0;
		AllocSize=m_DataSize;
		break;
	case DB_TYPE_TIME:
		m_DataSize=sizeof(DB_TIME);
		m_DigitSize=0;
		AllocSize=m_DataSize;
		break;
	case DB_TYPE_TIMESTAMP:
		m_DataSize=sizeof(DB_TIMESTAMP);
		m_DigitSize=0;
		AllocSize=m_DataSize;
		break;
	case DB_TYPE_GUID:
		m_DataSize=sizeof(DB_GUID);
		m_DigitSize=0;
		AllocSize=m_DataSize;		
		break;
	case DB_TYPE_STRING:
	case DB_TYPE_WSTRING:
	case DB_TYPE_BINARY:
		m_DataSize=DataSize;		
		m_DigitSize=0;
		AllocSize=m_DataSize+2;
		break;	
	default:
		return;
	}
	m_pData=new BYTE[AllocSize];
	ZeroMemory(m_pData,AllocSize);		
}

int CDBValue::GetType()
{
	return m_ValueType;
}

int	CDBValue::GetLength()
{
	return m_DataSize;
}

int CDBValue::GetDigitalLength()
{
	return m_DigitSize;
}
bool CDBValue::IsNull()
{
	return m_pData==NULL;
}


CDBValue::operator bool()
{
	bool Default=false;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{	
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return atoi((const char *)m_pData)!=0;
	case DB_TYPE_TINY:
		return (*m_pData)!=0;
	case DB_TYPE_SMALLINT:
		return (*((short *)m_pData))!=0;
	case DB_TYPE_INTEGER:
		return (*((int *)m_pData))!=0;
	case DB_TYPE_BIGINT:
		return (*((__int64 *)m_pData))!=0;
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return Value!=0;
		}		
	case DB_TYPE_DOUBLE:	
		{
			double Value=*((double *)m_pData);
			return Value!=0;
		}		
	}
	return Default;
}
CDBValue::operator char()
{
	char Default=0;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{	
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (char)atoi((const char *)m_pData);
	case DB_TYPE_TINY:
		return *m_pData;
	case DB_TYPE_SMALLINT:
		return (char)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (char)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (char)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return (char)Value;
		}		
	case DB_TYPE_DOUBLE:	
		{
			double Value=*((double *)m_pData);
			return (char)Value;
		}		
	}
	return Default;
}
CDBValue::operator unsigned char()
{
	unsigned char Default=0;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (unsigned char)atoi((const char *)m_pData);
	case DB_TYPE_TINY:
		return *m_pData;
	case DB_TYPE_SMALLINT:
		return (unsigned char)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (unsigned char)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (unsigned char)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return (unsigned char)Value;
		}		
	case DB_TYPE_DOUBLE:
		{
			double Value=*((double *)m_pData);
			return (unsigned char)Value;
		}			
	}
	return Default;
}
CDBValue::operator short()
{
	short Default=0;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{	
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (short)atoi((const char *)m_pData);
	case DB_TYPE_TINY:
		return (short)(*m_pData);
	case DB_TYPE_SMALLINT:
		return (short)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (short)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (short)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return (short)Value;
		}	
	case DB_TYPE_DOUBLE:
		{
			double Value=*((double *)m_pData);
			return (short)Value;
		}	
	}
	return Default;
}
CDBValue::operator unsigned short()
{
	unsigned short Default=0;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{	
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (unsigned short)atoi((const char *)m_pData);
	case DB_TYPE_TINY:
		return (unsigned short)(*m_pData);
	case DB_TYPE_SMALLINT:
		return (unsigned short)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (unsigned short)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (unsigned short)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return (unsigned short)Value;
		}	
	case DB_TYPE_DOUBLE:	
		{
			double Value=*((double *)m_pData);
			return (unsigned short)Value;
		}	
	}
	return Default;
}
CDBValue::operator int()
{
	int Default=0;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (int)atoi((const char *)m_pData);
	case DB_TYPE_TINY:
		return (int)(*m_pData);
	case DB_TYPE_SMALLINT:
		return (int)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (int)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (int)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return (int)Value;
		}
	case DB_TYPE_DOUBLE:
		{
			double Value=*((double *)m_pData);
			return (int)Value;
		}
	}
	return Default;
}
CDBValue::operator unsigned int()
{
	unsigned int Default=0;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (unsigned int)atoi((const char *)m_pData);
	case DB_TYPE_TINY:
		return (unsigned int)(*m_pData);
	case DB_TYPE_SMALLINT:
		return (unsigned int)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (unsigned int)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (unsigned int)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return (unsigned int)Value;
		}
	case DB_TYPE_DOUBLE:
		{
			double Value=*((double *)m_pData);
			return (unsigned int)Value;
		}
	}
	return Default;
}
CDBValue::operator long()
{
	long Default=0;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (long)atoi((const char *)m_pData);
	case DB_TYPE_TINY:
		return (long)(*m_pData);
	case DB_TYPE_SMALLINT:
		return (long)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (long)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (long)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return (long)Value;
		}
	case DB_TYPE_DOUBLE:	
		{
			double Value=*((double *)m_pData);
			return (long)Value;
		}
	}
	return Default;
}
CDBValue::operator unsigned long()
{
	unsigned long Default=0;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (unsigned long)atoi((const char *)m_pData);
	case DB_TYPE_TINY:
		return (unsigned long)(*m_pData);
	case DB_TYPE_SMALLINT:
		return (unsigned long)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (unsigned long)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (unsigned long)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return (unsigned long)Value;
		}
	case DB_TYPE_DOUBLE:	
		{
			double Value=*((double *)m_pData);
			return (unsigned long)Value;
		}
	}
	return Default;
}
CDBValue::operator __int64()
{
	__int64 Default=0;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (__int64)atoi((const char *)m_pData);
	case DB_TYPE_TINY:
		return (__int64)(*m_pData);
	case DB_TYPE_SMALLINT:
		return (__int64)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (__int64)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (__int64)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return (__int64)Value;
		}
	case DB_TYPE_DOUBLE:
		{
			double Value=*((double *)m_pData);
			return (__int64)Value;
		}
	}
	return Default;
}
CDBValue::operator unsigned __int64()
{
	unsigned __int64 Default=0;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (unsigned __int64)atoi((const char *)m_pData);
	case DB_TYPE_TINY:
		return (unsigned __int64)(*m_pData);
	case DB_TYPE_SMALLINT:
		return (unsigned __int64)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (unsigned __int64)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (unsigned __int64)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		{
			float Value=*((float *)m_pData);
			return (unsigned __int64)Value;
		}
	case DB_TYPE_DOUBLE:
		{
			double Value=*((double *)m_pData);
			return (unsigned __int64)Value;
		}
	}
	return Default;
}
CDBValue::operator float()
{
	float Default=0.0f;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (float)atof((const char *)m_pData);
	case DB_TYPE_TINY:
		return (float)(*m_pData);
	case DB_TYPE_SMALLINT:
		return (float)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (float)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (float)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		return (float)(*((float *)m_pData));
	case DB_TYPE_DOUBLE:	
		{
			double Value=*((double *)m_pData);
			return (float)Value;
		}
	}
	return Default;
}
CDBValue::operator double()
{
	double Default=0.0f;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
		return (double)atof((const char *)m_pData);
	case DB_TYPE_TINY:
		return (double)(*m_pData);
	case DB_TYPE_SMALLINT:
		return (double)(*((short *)m_pData));
	case DB_TYPE_INTEGER:
		return (double)(*((int *)m_pData));
	case DB_TYPE_BIGINT:
		return (double)(*((__int64 *)m_pData));
	case DB_TYPE_FLOAT:
		return (double)(*((float *)m_pData));
	case DB_TYPE_DOUBLE:	
		return (double)(*((double *)m_pData));
	}
	return Default;
}
CDBValue::operator const char *()
{
	char * Default=NULL;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
	case DB_TYPE_WSTRING:
		return (const char *)m_pData;
	}
	return Default;
}

CDBValue::operator const wchar_t *()
{
	wchar_t * Default=NULL;
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{		
	case DB_TYPE_BINARY:
	case DB_TYPE_STRING:
	case DB_TYPE_WSTRING:
		return (const wchar_t *)m_pData;
	}
	return Default;
}


//CDBValue::operator SYSTEMTIME()
//{
//	SYSTEMTIME_EX Default;	
//	if(m_pData==NULL)
//		return Default;
//	switch(m_ValueType)
//	{
//	case DB_TYPE_DATE:
//		Default=*((DB_DATE *)m_pData);
//		break;
//	case DB_TYPE_TIME:
//		Default=*((DB_TIME *)m_pData);		
//		break;
//	case DB_TYPE_TIMESTAMP:
//		Default=*((DB_TIMESTAMP *)m_pData);		
//		break;
//	}
//	
//	return Default;
//}
//CDBValue::operator FILETIME()
//{
//	FILETIME_EX Default;
//	if(m_pData==NULL)
//		return Default;
//	switch(m_ValueType)
//	{
//	case DB_TYPE_DATE:
//		Default=*((DB_DATE *)m_pData);
//		break;
//	case DB_TYPE_TIME:
//		Default=*((DB_TIME *)m_pData);		
//		break;
//	case DB_TYPE_TIMESTAMP:
//		Default=*((DB_TIMESTAMP *)m_pData);		
//		break;
//	}
//	return Default;
//}
CDBValue::operator DB_DATE()
{
	DB_DATE Default;
	ZeroMemory(&Default,sizeof(Default));
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{
	case DB_TYPE_DATE:
		return *((DB_DATE *)m_pData);
		break;	
	case DB_TYPE_TIMESTAMP:
		Default=*((DB_TIMESTAMP *)m_pData);		
		break;
	}
	return Default;
}
CDBValue::operator DB_TIME()
{
	DB_TIME Default;
	ZeroMemory(&Default,sizeof(Default));
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{	
	case DB_TYPE_TIME:
		return *((DB_TIME *)m_pData);		
		break;
	case DB_TYPE_TIMESTAMP:
		Default=*((DB_TIMESTAMP *)m_pData);		
		break;
	}
	return Default;
}
CDBValue::operator DB_TIMESTAMP()
{
	DB_TIMESTAMP Default;
	ZeroMemory(&Default,sizeof(Default));
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{
	case DB_TYPE_DATE:
		Default=*((DB_DATE *)m_pData);
		break;
	case DB_TYPE_TIME:
		Default=*((DB_TIME *)m_pData);		
		break;
	case DB_TYPE_TIMESTAMP:
		Default=*((DB_TIMESTAMP *)m_pData);		
		break;
	}
	return Default;
}
CDBValue::operator DB_GUID()
{
	DB_GUID Default;
	ZeroMemory(&Default,sizeof(Default));
	if(m_pData==NULL)
		return Default;
	switch(m_ValueType)
	{
	case DB_TYPE_GUID:
		return *((DB_GUID *)m_pData);
	}
	return Default;	
}
CDBValue::operator const void *()
{
	return m_pData;
}

void CDBValue::operator=(const CDBValue& Value)
{
	Destory();
	SetValue(Value.m_ValueType,Value.m_pData,Value.m_DataSize,Value.m_DigitSize);
}

void CDBValue::operator=(bool Value)
{
	Destory();	
	SetValue(DB_TYPE_TINY,&Value,1,0);
}

void CDBValue::operator=(char Value)
{
	Destory();	
	SetValue(DB_TYPE_TINY,&Value,1,0);
}

void CDBValue::operator=(unsigned char Value)
{
	Destory();	
	SetValue(DB_TYPE_TINY,&Value,1,0);
}

void CDBValue::operator=(short Value)
{
	Destory();	
	SetValue(DB_TYPE_SMALLINT,&Value,2,0);
}

void CDBValue::operator=(unsigned short Value)
{
	Destory();
	SetValue(DB_TYPE_SMALLINT,&Value,2,0);
}

void CDBValue::operator=(int Value)
{
	Destory();	
	SetValue(DB_TYPE_INTEGER,&Value,4,0);
}

void CDBValue::operator=(unsigned int Value)
{
	Destory();
	SetValue(DB_TYPE_INTEGER,&Value,4,0);
}

void CDBValue::operator=(long Value)
{
	Destory();
	SetValue(DB_TYPE_INTEGER,&Value,4,0);
}

void CDBValue::operator=(unsigned long Value)
{
	Destory();
	SetValue(DB_TYPE_INTEGER,&Value,4,0);
}

void CDBValue::operator=(__int64 Value)
{
	Destory();	
	SetValue(DB_TYPE_BIGINT,&Value,8,0);
}

void CDBValue::operator=(unsigned __int64 Value)
{
	Destory();
	SetValue(DB_TYPE_BIGINT,&Value,8,0);
}

void CDBValue::operator=(float Value)
{
	Destory();	
	SetValue(DB_TYPE_FLOAT,&Value,4,0);
}

void CDBValue::operator=(double Value)
{
	Destory();	
	SetValue(DB_TYPE_DOUBLE,&Value,8,0);
}

void CDBValue::operator=(const char * Value)
{
	Destory();	
	SetValue(DB_TYPE_STRING,Value,(int)strlen(Value),0);
}

void CDBValue::operator=(const wchar_t * Value)
{
	Destory();	
	SetValue(DB_TYPE_WSTRING,Value,(int)wcslen(Value)*2,0);
}

//void CDBValue::operator=(const SYSTEMTIME& Value)
//{
//	Destory();	
//	DB_TIMESTAMP TimeStamp(Value);
//	SetValue(DB_TYPE_TIMESTAMP,&TimeStamp,sizeof(DB_TIMESTAMP),0);
//}
//
//void CDBValue::operator=(const FILETIME& Value)
//{
//	Destory();
//	DB_TIMESTAMP TimeStamp(Value);	
//	SetValue(DB_TYPE_TIMESTAMP,&TimeStamp,sizeof(DB_TIMESTAMP),0);
//}

void CDBValue::operator=(const DB_DATE& Value)
{
	Destory();	
	SetValue(DB_TYPE_DATE,&Value,sizeof(DB_DATE),0);
}

void CDBValue::operator=(const DB_TIME& Value)
{
	Destory();	
	SetValue(DB_TYPE_TIME,&Value,sizeof(DB_TIME),0);
}

void CDBValue::operator=(const DB_TIMESTAMP& Value)
{
	Destory();	
	SetValue(DB_TYPE_TIMESTAMP,&Value,sizeof(DB_TIMESTAMP),0);
}

void CDBValue::operator=(const DB_GUID& Value)
{
	Destory();	
	SetValue(DB_TYPE_GUID,&Value,sizeof(DB_GUID),0);
}



}