#include "DBLib.h"

namespace DBLib
{

IMPLEMENT_CLASS_INFO_STATIC(IDatabase,CNameObject);
IDatabase::IDatabase():CNameObject()
{
}

IDatabase::~IDatabase()
{
}

IMPLEMENT_CLASS_INFO_STATIC(IDBConnection,CNameObject);
IDBConnection::IDBConnection():CNameObject()
{
}

IDBConnection::~IDBConnection()
{
}

IMPLEMENT_CLASS_INFO_STATIC(IDBRecordSet,CNameObject);
IDBRecordSet::IDBRecordSet():CNameObject()
{
}

IDBRecordSet::~IDBRecordSet()
{
}

IMPLEMENT_CLASS_INFO_STATIC(IDBParameterSet,CNameObject);
IDBParameterSet::IDBParameterSet():CNameObject()
{
}

IDBParameterSet::~IDBParameterSet()
{
}

//IMPLEMENT_CLASS_INFO_STATIC(CDBValue,CNameObject);
//CDBValue::CDBValue():CNameObject()
//{
//}
//
//CDBValue::~CDBValue()
//{
//}

}