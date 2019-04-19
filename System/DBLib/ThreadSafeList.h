#ifndef __DB_LIB_THREAD_SAFE_LIST_H__
#define __DB_LIB_THREAD_SAFE_LIST_H__
#include "DBLib.h"

template < class T>
class CThreadSafeList
{
protected:
	struct StorageNode
	{
		StorageNode * pPrev;
		StorageNode * pNext;
		bool IsFree;
		T Object;
	};
	StorageNode *	m_pObjectBuff;
	StorageNode *	m_pFreeList;
	StorageNode *	m_pObjectList;
	StorageNode *	m_pObjectListTail;
	volatile UINT	m_ObjectCount;
	volatile UINT	m_ObjectBuffSize;
    Zephyr::CLock	m_EasyCriticalSection;
public:
	CThreadSafeList(void)
		:m_EasyCriticalSection()
	{
		CAutoLock Lock(m_EasyCriticalSection);
		m_pObjectBuff=NULL;
		m_pFreeList=NULL;
		m_pObjectList=NULL;
		m_pObjectListTail=NULL;
		m_ObjectCount=0;
		m_ObjectBuffSize=0;
	}
	CThreadSafeList(int Size)
		:m_EasyCriticalSection()
	{
		CAutoLock Lock(m_EasyCriticalSection);
		m_pObjectBuff=NULL;
		m_pFreeList=NULL;
		m_pObjectList=NULL;
		m_pObjectListTail=NULL;
		m_ObjectCount=0;
		m_ObjectBuffSize=0;
		Create(Size);
	}
	virtual ~CThreadSafeList(void)
	{
		Destory();
	}
	BOOL Create(int Size)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		Destory();
		m_ObjectBuffSize=Size;
		m_pObjectBuff=new StorageNode[m_ObjectBuffSize];
		Clear();
		return TRUE;
	}
	UINT GetBufferSize()
	{
		return m_ObjectBuffSize;
	}
	BOOL IsCreated()
	{
		return m_pObjectBuff!=NULL;
	}
	void Destory()
	{
		CAutoLockP Lock(m_EasyCriticalSection);
		SAFE_DELETE_ARRAY(m_pObjectBuff);
		m_pFreeList=NULL;
		m_pObjectList=NULL;
		m_pObjectListTail=NULL;
		m_ObjectCount=0;
		m_ObjectBuffSize=0;
	}

	void Clear()
	{
		CAutoLockP Lock(m_EasyCriticalSection);
		for(UINT i=0;i<m_ObjectBuffSize;i++)
		{
			if(i==0)
			{
				m_pObjectBuff[i].pPrev=NULL;
			}
			else
			{
				m_pObjectBuff[i].pPrev=&(m_pObjectBuff[i-1]);
			}

			if(i==m_ObjectBuffSize-1)
			{
				m_pObjectBuff[i].pNext=NULL;
			}
			else
			{
				m_pObjectBuff[i].pNext=&(m_pObjectBuff[i+1]);
			}
			m_pObjectBuff[i].IsFree=true;
		}
		m_pFreeList=m_pObjectBuff;
		m_pObjectList=NULL;
		m_pObjectListTail=NULL;
		m_ObjectCount=0;
	}

	T * PushFront()
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(m_pFreeList)
		{
			StorageNode * pNode;

			pNode=m_pFreeList;
			m_pFreeList=pNode->pNext;
			if(m_pFreeList)
				m_pFreeList->pPrev=NULL;
			pNode->pPrev=NULL;
			pNode->pNext=m_pObjectList;
			if(m_pObjectList)
				m_pObjectList->pPrev=pNode;
			else
				m_pObjectListTail=pNode;
			m_pObjectList=pNode;
			pNode->IsFree=false;
			m_ObjectCount++;
			return &(pNode->Object);
		}
		return NULL;
	}

	BOOL PushFront(T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(m_pFreeList)
		{
			StorageNode * pNode;

			pNode=m_pFreeList;
			m_pFreeList=pNode->pNext;
			if(m_pFreeList)
				m_pFreeList->pPrev=NULL;
			pNode->pPrev=NULL;
			pNode->pNext=m_pObjectList;
			if(m_pObjectList)
				m_pObjectList->pPrev=pNode;
			else
				m_pObjectListTail=pNode;
			m_pObjectList=pNode;
			pNode->IsFree=false;
			pNode->Object=Object;
			m_ObjectCount++;
			return TRUE;
		}
		return FALSE;
	}

	T* PushBack()
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(m_pFreeList)
		{
			StorageNode * pNode;

			pNode=m_pFreeList;
			m_pFreeList=pNode->pNext;
			if(m_pFreeList)
				m_pFreeList->pPrev=NULL;
			pNode->pPrev=m_pObjectListTail;
			pNode->pNext=NULL;
			if(m_pObjectListTail)
				m_pObjectListTail->pNext=pNode;
			else
				m_pObjectList=pNode;
			m_pObjectListTail=pNode;
			pNode->IsFree=false;
			m_ObjectCount++;
			return &(pNode->Object);
		}
		return NULL;
	}

	BOOL PushBack(T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(m_pFreeList)
		{
			StorageNode * pNode;

			pNode=m_pFreeList;
			m_pFreeList=pNode->pNext;
			if(m_pFreeList)
				m_pFreeList->pPrev=NULL;
			pNode->pPrev=m_pObjectListTail;
			pNode->pNext=NULL;
			if(m_pObjectListTail)
				m_pObjectListTail->pNext=pNode;
			else
				m_pObjectList=pNode;
			m_pObjectListTail=pNode;
			pNode->IsFree=false;
			pNode->Object=Object;
			m_ObjectCount++;
			return TRUE;
		}
		return FALSE;
	}

	BOOL PopFront(T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(m_pObjectList)
		{
			Object=m_pObjectList->Object;
			DeleteNode(m_pObjectList);
			return TRUE;
		}
		return FALSE;
	}

	BOOL PopBack(T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(m_pObjectListTail)
		{
			Object=m_pObjectListTail->Object;
			DeleteNode(m_pObjectListTail);
			return TRUE;
		}
		return FALSE;
	}

	BOOL DeleteObject(LPVOID Pos)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(Pos)
		{
			DeleteNode((StorageNode *)Pos);
			return TRUE;
		}
		return FALSE;
	}

	UINT GetObjectCount()
	{
		return m_ObjectCount;
	}

	LPVOID GetFirstObjectPos()
	{
		return m_pObjectList;
	}

	LPVOID GetLastObjectPos()
	{
		return m_pObjectListTail;
	}
	BOOL GetHead(T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(m_pObjectList)
		{
			Object=m_pObjectList->Object;
			return TRUE;
		}
		return FALSE;
	}
	BOOL GetTail(T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(m_pObjectListTail)
		{
			Object=m_pObjectListTail->Object;
			return TRUE;
		}
		return FALSE;
	}

	T * GetNext(LPVOID& Pos)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;
			Pos=pNode->pNext;
			return &(pNode->Object);
		}
		return NULL;
	}

	T * GetPrev(LPVOID& Pos)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		if(Pos)
		{
			StorageNode * pNode=(StorageNode *)Pos;
			Pos=pNode->pPrev;
			return &(pNode->Object);
		}
		return NULL;
	}

protected:
	void DeleteNode(StorageNode * pNode)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		pNode->IsFree=true;
		if(pNode->pPrev)
		{
			pNode->pPrev->pNext=pNode->pNext;
			if(pNode->pNext)
				pNode->pNext->pPrev=pNode->pPrev;
			else
				m_pObjectListTail=pNode->pPrev;
		}
		else
		{
			m_pObjectList=pNode->pNext;
			if(m_pObjectList)
				m_pObjectList->pPrev=NULL;
			else
				m_pObjectListTail=NULL;
		}
		pNode->pPrev=NULL;
		pNode->pNext=m_pFreeList;
		if(m_pFreeList)
			m_pFreeList->pPrev=pNode;
		m_pFreeList=pNode;
		m_ObjectCount--;
	}
};

#endif
