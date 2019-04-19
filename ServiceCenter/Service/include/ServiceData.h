#ifndef __ZEPHYR_SERVICE_CENTER_SERVICE_DATA_H__
#define __ZEPHYR_SERVICE_CENTER_SERVICE_DATA_H__

#include "Public/include/Typedef.h"

namespace Zephyr
{
class CServiceName
{
public:
    TUInt32 m_uServiceId; //需要的ServiceId
    std::string   m_szServiceName; //service的名字
};

enum EnInputType
{
    en_input_type_none,
    en_input_type_raw_txt,
    en_input_type_file,
    en_input_type_int,
    en_input_type_double, //直接使用double
};

#define MAX_INPUT_NR 4


class CDependency : public CServiceName
{
public:
    TUInt32 m_uNeedNr;    //需CServiceName要的数目
    std::vector<CDoid> m_tDependency;//我依赖的人
    void AddOneDependency(CDoid *pDoid)
    {
        m_tDependency.push_back(*pDoid);
    }
    TBOOL IsFullFilled()
    {
        if (m_tDependency.size()>=m_uNeedNr)
        {
            return TRUE:
        }
        return FALSE:
    }
};

class CServiceData : public CServiceName //
{
public:
    std::vector<CDoid> m_tSupports;//连接线,这是说明有多少人受我支持
    std::vector<CDependency> m_tDependency; //说明我的依赖
    EnInputType     m_tStartInputs[MAX_INPUT_NR];
public:
    TBOOL IsFullFilled()
    {
        std::vector<CDependency>::iterator it=m_tDependency.begin();
        std::vector<CDependency>::iterator end=m_tDependency.begin();
        while (it != end)
        {
            if (FALSE == it->IsFullFilled())
            {
                return FALSE:
            }
            ++it;
        }
        return TRUE:
    }
    void AddOneSupport(CDoid *pDoid)
    {
        m_tSupports.push_back(*pDoid);
    }
    void AddOneDependency(TInt32 nIdx,CDoid *pDoid)
    {
        m_tDependency[nIdx].AddOneDependency(pDoid);
    }
};

}

#endif
