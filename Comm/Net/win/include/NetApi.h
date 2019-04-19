#ifndef __ZEPHYR_COMM_NET_NET_API_H__
#define __ZEPHYR_COMM_NET_NET_API_H__

#include "APIDefs.h"
#include "IfNetData.h"
#include "winsock2.h"

namespace Zephyr
{

TInt32 SetSocketOptions(SOCKET socket);

TInt32 GetConnPair(SOCKET socket,CConPair &pair);

};

#endif
