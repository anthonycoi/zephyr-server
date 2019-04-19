#include "DBLib.h"




CEasyBuffer::CEasyBuffer(void)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
}

CEasyBuffer::CEasyBuffer(UINT Size)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
	Create(Size);
}

CEasyBuffer::CEasyBuffer(LPVOID pBuff,UINT Size)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
	Create(pBuff,Size);
}

CEasyBuffer::CEasyBuffer(const CEasyBuffer& Buffer)
{
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
	Create(Buffer.GetBufferSize());
	PushBack(Buffer.GetBuffer(),Buffer.GetUsedSize());
}

CEasyBuffer::~CEasyBuffer(void)
{
	Destory();
}

BOOL CEasyBuffer::Create(UINT Size)
{
	Destory();
	m_pBuffer=new BYTE[Size];
	m_BufferSize=Size;
	m_UsedSize=0;
	m_IsSelfBuffer=true;
	return TRUE;
}

BOOL CEasyBuffer::Create(LPVOID pBuff,UINT Size)
{
	Destory();
	m_pBuffer=(BYTE *)pBuff;
	m_BufferSize=Size;
	m_UsedSize=0;
	m_IsSelfBuffer=false;
	return TRUE;
}

void CEasyBuffer::Destory()
{
	if(m_IsSelfBuffer)
		SAFE_DELETE_ARRAY(m_pBuffer);
}

