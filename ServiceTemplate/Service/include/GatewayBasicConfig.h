#ifndef __ZEPHYR_GATEWAY_BASIC_CONFIG_H__
#define __ZEPHYR_GATEWAY_BASIC_CONFIG_H__
#include "Public/include/TypeDef.h"
namespace Zephyr
{


class CGatewayBasicConfig
{
public:
    TUInt16 m_uMaxConnections;
    TUInt16 m_uMaxIncomingConnection4Listner;
    TUInt16 m_uOutPutCacheInKBs;
    TUInt16 m_uInputCacheInKBs;
    TUInt32 m_uWriteLoggerMask;
    TUInt32 m_uPrint2ScreenLoggerMask;
    TChar   m_szLoggerName[32];
    CGatewayBasicConfig();
    //根据其被加载的ID来获取配置.
    TInt32  ReadFile(TUInt16 uVip,TUInt16 uServiceId);
};
}
#endif
