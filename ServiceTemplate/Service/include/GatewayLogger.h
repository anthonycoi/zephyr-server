#ifndef __ZEPHYR_GATEWAY_LOGGER_H__
#define __ZEPHYR_GATEWAY_LOGGER_H__

#include "Public/Interface/Platform/include/IfLogger.h"

enum EnGWLogId
{
    //前65536个保留给系统，这些错误是需要写critical日志的，属于系统错误，而下面的是运行错误，说明输入参数有误.
    //所以查日志只要查日志号小于65536的，其它都是属于使用错误。
    en_gw_error_begin    = 0x00010000, //每个模块预留64k个，绝对够了
    en_error_service_id,
    en_allocate_route_mem_failed,
    en_route_info_not_found,
    en_start_listing,
    en_start_listening_failed,
    en_restart_listening,
    en_stop_listening,
    en_listening_not_started,
    en_no_much_service_id_found,
    en_system_id_unmatch, //系统ID不对，说明漏消息了
    en_on_disconnected,
};


#endif
