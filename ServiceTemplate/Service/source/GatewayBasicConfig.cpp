#include "../include/GatewayBasicConfig.h"
#include "Public/Config/include/SettingFile.h"
#include "Public/include/SysMacros.h"
#include "Public/Interface/Platform/include/IfLogger.h"
namespace Zephyr
{

CGatewayBasicConfig::CGatewayBasicConfig()
{
    m_uMaxConnections = 0;
    m_uMaxIncomingConnection4Listner = 0;
    m_uOutPutCacheInKBs = 0;
    m_uInputCacheInKBs = 0;
    m_szLoggerName[0] = 0;
#ifdef _DEBUG
    m_uWriteLoggerMask = log_debug_mode;
    m_uPrint2ScreenLoggerMask = log_debug_mode; 
#else
    m_uWriteLoggerMask = log_release_mode;
    m_uPrint2ScreenLoggerMask = log_release_mode;
#endif
}
    //根据其被加载的ID来获取配置.
TInt32  CGatewayBasicConfig::ReadFile(TUInt16 uVIp,TUInt16 uServiceId)
{
    char szFileName[64];
    sprintf_s(szFileName,sizeof(szFileName),"%d/GatewayConfig%d.ini",uVIp,uServiceId);
    CSettingFile tFile;
    if (tFile.LoadFromFile(szFileName))
    {
        char szMain[] = "MAIN";
        m_uMaxConnections = tFile.GetInteger(szMain,"max_connection",2200);
        m_uMaxIncomingConnection4Listner = tFile.GetInteger(szMain,"max_incoming_connecting_4_listener",32);
        m_uOutPutCacheInKBs = tFile.GetInteger(szMain,"output_cache_in_kbs",64);
        m_uInputCacheInKBs  = tFile.GetInteger(szMain,"input_cache_in_kbs",16);
        const char *psz = tFile.GetString(szMain,"logger_name","gateway_logger");
        strncpy_s(m_szLoggerName,psz,sizeof(m_szLoggerName));
        return SUCCESS;
    }
    return FAIL;
}

}