#include "NetApi.h"
#include <winsock2.h>
namespace Zephyr
{

TInt32 SetSocketOptions(SOCKET socket)
{
    LINGER lingerStruct;
    lingerStruct.l_onoff = 0;
    lingerStruct.l_linger = 0;
    //generally close the socket,and do not block the app.
    //use api's orignal types
    int ret = setsockopt(socket, SOL_SOCKET, SO_LINGER,
        (char *)&lingerStruct, sizeof(lingerStruct));
    if (ret == SOCKET_ERROR)
    {
        int errcode = WSAGetLastError();
        return -errcode;
    }

    //-------------------------
    // Set the socket I/O mode: In this case FIONBIO
    // enables or disables the blocking mode for the 
    // socket based on the numerical value of iMode.
    // If argp = 0, blocking is enabled; 
    // If argp != 0, non-blocking mode is enabled.

    unsigned long argp   =   1;
    
    ret = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&argp, sizeof(argp));
    if (ret == SOCKET_ERROR)
    {
        int errcode = WSAGetLastError();
        return -errcode;
    }
    
    ret   =   ioctlsocket(socket,FIONBIO,(unsigned   long*)&argp);
    if(ret   ==   SOCKET_ERROR)   
    {   
        int errcode = WSAGetLastError();
        return -errcode;
    }

    return SUCCESS;
}

TInt32 GetConnPair(SOCKET socket,CConPair &pair)
{
    sockaddr_in remoteAdd,myAdd;
    int     remoteNameLen = sizeof(sockaddr_in);
    int     myNameLen = remoteNameLen;
    int ret = getpeername(socket,(sockaddr *)&remoteAdd,&remoteNameLen);
    if (ret == SOCKET_ERROR)
    {
        return (-((TInt32)WSAGetLastError()));
    }

    ret = getsockname(socket,(sockaddr *)&myAdd,&myNameLen);
    if (ret == SOCKET_ERROR)
    {
        return (-((TInt32)WSAGetLastError()));
    }
    //myAdd.sin_addr.s_addr

    pair.Init(remoteAdd.sin_addr.s_addr,myAdd.sin_addr.s_addr,ntohs(remoteAdd.sin_port),ntohs(myAdd.sin_port));
    return SUCCESS;
}

};