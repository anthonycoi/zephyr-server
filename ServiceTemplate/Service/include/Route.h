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
    //可能会有溢出，但是没关系.
    //可能会注册部分功能.
    TUInt32 m_uIdBegin;
    TUInt32 m_uIdEnd;
    TUInt32 m_uPriority; //权重，这个再做负载均衡的时候使用
    CDoid   m_tRouteTo;
    CRoute()
    {
        m_uKey = 0;
        m_uIdBegin = 0;
        m_uIdEnd = 0;
        m_uPriority = 0;
    }
    //调用者保证其uServiceId相同，并且rNew的idBegin\idEnd都合法
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
                    //使用新的权重
                    m_uPriority = rNew.m_uPriority;
                    return TRUE;
                    //可以融合
                }
            }
            else if (m_uIdBegin <= rNew.m_uIdEnd)
            {
                //可以融合
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
    //如果是-1不匹配，0，则表示不需要任何删减操作,1表示要删除一个，即this，2表示增加一个（值即rRmvPart）,3，表示完全匹配
    TInt32 Separate(CRoute &rRmvPart)
    {
        if (m_tRouteTo == rRmvPart.m_tRouteTo)
        {
            if (m_uIdBegin < rRmvPart.m_uIdBegin)
            {
                if (m_uIdEnd > rRmvPart.m_uIdBegin)
                {
                    //有删减
                    if (m_uIdEnd > rRmvPart.m_uIdEnd)
                    {
                        //变成2个
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
                    //完全匹配
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
        return -1; //不对.
    }
};

}
#endif
