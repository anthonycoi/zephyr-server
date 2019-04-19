#ifndef __ZEPHYR_SERVICE_CENTER_SERVICE_DATA_H__
#define __ZEPHYR_SERVICE_CENTER_SERVICE_DATA_H__

#include "Public/include/Typedef.h"

namespace Zephyr
{
class CServiceName
{
public:
    TUInt32 m_uServiceId; //��Ҫ��ServiceId
    std::string   m_szServiceName; //service������
};

enum EnInputType
{
    en_input_type_none,
    en_input_type_raw_txt,
    en_input_type_file,
    en_input_type_int,
    en_input_type_double, //ֱ��ʹ��double
};

#define MAX_INPUT_NR 4


class CDependency : public CServiceName
{
public:
    TUInt32 m_uNeedNr;    //��CServiceNameҪ����Ŀ
    std::vector<CDoid> m_tDependency;//����������
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
    std::vector<CDoid> m_tSupports;//������,����˵���ж���������֧��
    std::vector<CDependency> m_tDependency; //˵���ҵ�����
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
