/*************************************************
  Copyright (C), 1988-2010, Zephyr Platform
  File name:      tplList.h
  Author:         Zephyr Shannon
  Version:        V1.2
  Date:           12-22-2009
  Description:    List template
  Others:         none
  Function List:  
  History:
*************************************************/
#ifndef __ZEPHYR_PUBLIC_TPL_LIST_TEMPLATE_H__
#define __ZEPHYR_PUBLIC_TPL_LIST_TEMPLATE_H__

#include "../../include/SysMacros.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
#include "../../include/TypeDef.h"

namespace Zephyr 
{


//¼Ù¶¨
template<class BASE_CLASS>
class CListNode : public BASE_CLASS
{
public:
    DECLARE_CLASS_LIST(CListNode);
};

template<class BASE_CLASS>
class CList
{
private:
    CListNode<BASE_CLASS> *m_pHeader;
    CListNode<BASE_CLASS> *m_pRear;
    TUInt32 m_uSize;
public:
    CList()
    {
        m_pHeader = NULL;
        m_pRear   = NULL;
        m_uSize = 0;
    }
    CListNode<BASE_CLASS> *pop_front()
    {
        if (m_pHeader)
        {
            CListNode<BASE_CLASS> *pRtn = m_pHeader;
            if (m_pHeader == m_pRear)
            {
                m_pHeader = NULL;
                m_pRear   = NULL;
            }
            else
            {
                m_pHeader = m_pHeader->GetNext();
                pRtn->Detach();
            }
            --m_uSize;
            return pRtn;
        }
        return NULL;
    }
    CListNode<BASE_CLASS> *pop_back()
    {
        if (m_pRear)
        {
            CListNode<BASE_CLASS> *pRtn = m_pRear;
            if (m_pHeader == m_pRear)
            {
#ifdef _DEBUG
                if ((m_pRear->GetNext())||(m_pRear->GetPrev()))
                {
                    printf("ListNode Incorrect!");
                }
#endif
                m_pHeader = NULL;
                m_pRear   = NULL;
            }
            else
            {
                m_pRear = m_pRear->GetPrev();
                pRtn->Detach();
            }
            --m_uSize;
            return pRtn;
        }
        return NULL;
    }
    
    CListNode<BASE_CLASS> *header()
    { 
        return m_pHeader;
    }
    CListNode<BASE_CLASS> *rear()
    {
        return m_pRear;
    }
    void push_front(CListNode<BASE_CLASS> *p)
    {
        if (m_pHeader)
        {
            p->AttachList(m_pHeader);
            m_pHeader = p;
        }
        else
        {
            m_pHeader = p;
            m_pRear   = p;
        }
        ++m_uSize;
    }
    void push_back(CListNode<BASE_CLASS> *p)
    {
        if (m_pRear)
        {
            m_pRear->AttachList(p);
            m_pRear = p;
        }
        else
        {
            m_pHeader = p;
            m_pRear   = p;
        }
        ++m_uSize;
    }
    void Detach(CListNode<BASE_CLASS> *p)
    {
        if (p == m_pHeader)
        {
            pop_front();
        }
        else if (p == m_pRear)
        {
            pop_back();
        }
        else
        {
            p->Detach();
        }
        --m_uSize;
    }
    void HandleOver(CList *pTo,CListNode<BASE_CLASS> *p)
    {
        Detach(p);
        pTo->push_front(p);
    }
    void clear()
    {
        m_pHeader = NULL;
        m_pRear   = NULL;
        m_uSize = 0;
    }
    TUInt32 size()
    {
        return m_uSize;
    }
};

}

#endif
