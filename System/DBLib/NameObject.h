#ifndef __DB_LIB_NAME_OBJECT_H__
#define __DB_LIB_NAME_OBJECT_H__



#include <string>
#include <map>

#define OBJECT_TYPE_ID_SEC	0xffff0000
#define OBJECT_TYPE_ID_ID	0x0000ffff

#define MAX_OBJECT_NAME		32

class CNameObject;

typedef CNameObject * (*OBJECT_CREATE_FN)();




enum OBJECT_CLONE_PARAM
{
	OBJECT_CLONE_WITHOUT_CHILD=1,
};


struct CLASS_INFO
{
	LPCTSTR				ClassName;
	CLASS_INFO *		pParentClassInfo;
	OBJECT_CREATE_FN	pObjectCreateFn;
	bool operator==(const CLASS_INFO& ClassInfo)
	{
		return strcmp(ClassName,ClassInfo.ClassName)==0;
	}
	bool operator!=(const CLASS_INFO& ClassInfo)
	{
		return strcmp(ClassName,ClassInfo.ClassName)!=0;
	}
	bool operator==(LPCTSTR Name)
	{
		return strcmp(ClassName,Name)==0;
	}
	bool operator!=(LPCTSTR Name)
	{
		return strcmp(ClassName,Name)!=0;
	}
	bool IsKindOf(const CLASS_INFO& ClassInfo)
	{
		if(*this==ClassInfo)
			return true;
		if(pParentClassInfo)
			return pParentClassInfo->IsKindOf(ClassInfo);
		return false;
	}
	bool IsKindOf(LPCTSTR Name)
	{
		if(*this==Name)
			return true;
		if(pParentClassInfo)
			return pParentClassInfo->IsKindOf(Name);
		return false;
	}
	bool IsKindOfFast(const CLASS_INFO& ClassInfo)
	{
		if(this==&ClassInfo)
			return true;
		if(pParentClassInfo)
			return pParentClassInfo->IsKindOfFast(ClassInfo);
		return false;
	}
	CNameObject * CreateObject()
	{
		if(pObjectCreateFn)
			return (*pObjectCreateFn)();
		else
			return NULL;
	}
};


#define DECLARE_CLASS_INFO(ClassName) \
protected:\
	static CLASS_INFO	m_##ClassName##ClassInfo;\
public:\
	virtual CLASS_INFO& GetClassInfo()\
	{\
		return GetThisClassInfo();\
	}\
	static CNameObject * CreateObject();\
	static CLASS_INFO& GetThisClassInfo();\



#define GET_CLASS_INFO(ClassName) (ClassName::GetThisClassInfo())
#define GET_CLASS_INFO_BY_NAME(ClassName) (*ClassName::GetClassInfo(ClassName))

#define IMPLEMENT_CLASS_INFO(ClassName,ParentClassName) \
	CLASS_INFO	ClassName::m_##ClassName##ClassInfo={#ClassName,&GET_CLASS_INFO(ParentClassName),ClassName::CreateObject};\
	CNameObject * ClassName::CreateObject()\
	{\
		return (ParentClassName *) new ClassName();\
	}\
	CLASS_INFO& ClassName::GetThisClassInfo()\
	{\
		return m_##ClassName##ClassInfo;\
	}

#define DECLARE_CLASS_INFO_STATIC(ClassName) \
protected:\
	static CLASS_INFO	m_##ClassName##ClassInfo;\
public:\
	virtual CLASS_INFO& GetClassInfo()\
	{\
		return GetThisClassInfo();\
	}\
	static CLASS_INFO& GetThisClassInfo();\


#define IMPLEMENT_CLASS_INFO_STATIC(ClassName,ParentClassName) \
	CLASS_INFO	ClassName::m_##ClassName##ClassInfo={#ClassName,&GET_CLASS_INFO(ParentClassName),NULL};\
	CLASS_INFO& ClassName::GetThisClassInfo()\
	{\
		return m_##ClassName##ClassInfo;\
	}






class CUSOFile;


class CNameObject //: public CObject
{
protected:
	UINT			m_UpBoundGarder;
	UINT			m_ID;
	volatile UINT	m_UseRef;
	LPVOID			m_pParam;
	TCHAR			m_Name[MAX_OBJECT_NAME];

	static CLASS_INFO	m_CNameObjectClassInfo;
	
public:
	static CLASS_INFO& GetThisClassInfo();
	static CNameObject * CreateObject();	

	virtual CLASS_INFO& GetClassInfo()
	{
		return GetThisClassInfo();;
	}
	inline bool IsKindOf(const CLASS_INFO& ClassInfo)
	{
		return GetClassInfo().IsKindOf(ClassInfo);
	}
	inline bool IsKindOf(LPCTSTR Name)
	{
		return GetClassInfo().IsKindOf(Name);
	}
	inline bool IsClass(const CLASS_INFO& ClassInfo)
	{
		return GetClassInfo()==ClassInfo;
	}
	inline bool IsClass(LPCTSTR Name)
	{
		return GetClassInfo()==Name;
	}
	inline bool IsKindOfFast(const CLASS_INFO& ClassInfo)
	{
		return GetClassInfo().IsKindOfFast(ClassInfo);
	}
	inline LPCTSTR GetClassName()
	{
		return GetClassInfo().ClassName;
	}

	CNameObject()
	{
		m_ID=0;
		m_UseRef=1;
		m_pParam=NULL;
		m_Name[0]=0;
	}

	virtual ~CNameObject()
	{

	}
	virtual bool Initialize()
	{
		return true;
	}
	virtual void Destory()
	{
	}
	void AddUseRef()
	{
		m_UseRef++;
	}
	void DecUseRef()
	{
		m_UseRef--;
	}
	UINT GetUseRef()
	{
		return m_UseRef;
	}
	virtual void Release()
	{
		DecUseRef();
		if(GetUseRef()<=0)
			delete this;
	}

	void SetName(LPCTSTR Name)
	{
		strncpy_0(m_Name,MAX_OBJECT_NAME,Name,MAX_OBJECT_NAME);
	}

	LPCTSTR GetName()
	{
		return m_Name;
	}

	void SetID(UINT ID)
	{m_ID=ID;}

	UINT GetID()
	{return m_ID;}


	void SetParam(LPVOID pParam)
	{
		m_pParam=pParam;
	}

	LPVOID GetParam()
	{
		return m_pParam;
	}	
	

};
#endif
