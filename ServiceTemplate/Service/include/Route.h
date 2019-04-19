#ifndef __ZEPHYR_GATEWAY_ROUTE_H__
#define __ZEPHYR_GATEWAY_ROUTE_H__
#include "Public/include/TypeDef.h"
#include "Public/include/Doid.h"

namespace Zephyr
{

class CRoute
{
public:
    TUInt32 &GetKey()
    {
        return m_uKey;
    }
    TUInt32 m_uKey;
    //���ܻ������������û��ϵ.
    //���ܻ�ע�Ჿ�ֹ���.
    TUInt32 m_uIdBegin;
    TUInt32 m_uIdEnd;
    TUInt32 m_uPriority; //Ȩ�أ�����������ؾ����ʱ��ʹ��
    CDoid   m_tRouteTo;
    CRoute()
    {
        m_uKey = 0;
        m_uIdBegin = 0;
        m_uIdEnd = 0;
        m_uPriority = 0;
    }
    //�����߱�֤��uServiceId��ͬ������rNew��idBegin\idEnd���Ϸ�
    TBOOL Merge(CRoute &rNew)
    {
        if (m_tRouteTo == rNew.m_tRouteTo)
        {
            if (m_uIdBegin > rNew.m_uIdBegin)
            {
                if (m_uIdBegin <= rNew.m_uIdEnd)
                {
                    m_uIdBegin = rNew.m_uIdBegin;
                    if (m_uIdEnd < rNew.m_uIdEnd)
                    {
                        m_uIdEnd = rNew.m_uIdEnd;
                    }
                    //ʹ���µ�Ȩ��
                    m_uPriority = rNew.m_uPriority;
                    return TRUE;
                    //�����ں�
                }
            }
            else if (m_uIdBegin <= rNew.m_uIdEnd)
            {
                //�����ں�
                if (m_uIdEnd > rNew.m_uIdEnd)
                {
                    m_uIdEnd = rNew.m_uIdEnd;
                }
                m_uPriority = rNew.m_uPriority;
                return TRUE;
            }
        }
        return FALSE;
    }
    //�����-1��ƥ�䣬0�����ʾ����Ҫ�κ�ɾ������,1��ʾҪɾ��һ������this��2��ʾ����һ����ֵ��rRmvPart��,3����ʾ��ȫƥ��
    TInt32 Separate(CRoute &rRmvPart)
    {
        if (m_tRouteTo == rRmvPart.m_tRouteTo)
        {
            if (m_uIdBegin < rRmvPart.m_uIdBegin)
            {
                if (m_uIdEnd > rRmvPart.m_uIdBegin)
                {
                    //��ɾ��
                    if (m_uIdEnd > rRmvPart.m_uIdEnd)
                    {
                        //���2��
                        TUInt32 uTmp = m_uIdEnd;
                        m_uIdEnd = rRmvPart.m_uIdBegin;
                        rRmvPart.m_uIdBegin = rRmvPart.m_uIdEnd;
                        rRmvPart.m_uIdEnd = uTmp;
                        return 2;
                    }
                    else
                    {
                        m_uIdEnd = rRmvPart.m_uIdBegin;
                        return 0;
                    }
                }
            }
            else if (m_uIdBegin < rRmvPart.m_uIdEnd)
            {
                if (m_uIdEnd <= rRmvPart.m_uIdEnd)
                {
                    //��ȫƥ��
                    if ((m_uIdBegin == rRmvPart.m_uIdBegin) && (m_uIdEnd == rRmvPart.m_uIdEnd))
                    {
                        return 3;
                    }
                    return 1;
                }
                else
                {
                    m_uIdBegin = rRmvPart.m_uIdEnd;
                    return 0;
                }
            }
        }
        return -1; //����.
    }
};

}
#endif
