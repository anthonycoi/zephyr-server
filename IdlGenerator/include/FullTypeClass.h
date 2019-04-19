#ifndef __ZEPHYR_IDL_GENERATOR_FULL_TYPE_CLASS_H__
#define __ZEPHYR_IDL_GENERATOR_FULL_TYPE_CLASS_H__

#include "./baseElement.h"

namespace Zephyr
{

class CFullTypeDef : public CBaseElement
{
public:
    DECLARE_STATIC_CLASS_POOL(CFullTypeDef);

public:
    enum EnPrefix
    {
        en_no_prefix = 0,
        en_const_prefix         = 0x01,
        en_static_prefix        = 0x02,
        en_mutable_prefix       = 0x04,
        en_volatile_prefix      = 0x08,
    };
    enum EnOperator
    {
        en_no_opr              = 0x00,
        en_star_operator       = 0x01,
        en_ref_operator        = 0x02,
    };
    unsigned int m_uPrefix;
    unsigned char m_tOprs[4]; //4¸ö
    CBaseElement *m_pType;
    std::string  m_szFull;
    std::string  m_szRawTxt;
    std::string  m_szRawNoPrefix;
    std::vector<CFullTypeDef*> m_pTplSubs;
public:
    std::string &GetRawTxt()
    {
        return m_szRawTxt;
    }
    CFullTypeDef();

    void AddPrefix(EnPrefix);
    void AddOpr(EnOperator enOpr);

    virtual TInt32 Process(char **ppElements,EnType *pTypes,int nProcess2,int nTotalEles);
    virtual const char *GetHierachyName();
};

}

#endif
