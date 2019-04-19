#include "DBLib.h"
#include "NameObject.h"

CLASS_INFO	CNameObject::m_CNameObjectClassInfo={"CNameObject",NULL,CNameObject::CreateObject};




CNameObject * CNameObject::CreateObject()
{
	return new CNameObject();
}


CLASS_INFO& CNameObject::GetThisClassInfo()
{
	return m_CNameObjectClassInfo;
}



