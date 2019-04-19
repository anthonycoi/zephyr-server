#include "IfLogger.h"
#include "..\Public\include\Message.h"
namespace Zephyr
{


extern IfLogger *g_pCommLogger;

#define COMM_DEBUG_LOG(LOG_IDX,LOG_STR,...) g_pCommLogger->WriteLog((LOG_IDX),log_debug,LOG_STR,__VA_ARGS__);

#define COMM_RUN_LOG(LOG_IDX,LOG_STR,...) g_pCommLogger->WriteLog(LOG_IDX,log_run,LOG_STR,__VA_ARGS__);

#define COMM_EXCPETION_LOG(LOG_IDX,LOG_STR,...) g_pCommLogger->WriteLog(LOG_IDX,log_critical,LOG_STR,__VA_ARGS__);



#define COMM_CONNECTION_REINITED 1

#define COMM_DROP_MSG 2

#define COMM_DROP_MSG_BODY 3

void RecordOneMsg(CMessageHeader *pMsg);
}