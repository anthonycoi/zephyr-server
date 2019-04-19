#include "MsgParser.h"

namespace Zephyr
{


TInt32  CMsgParser::OnRecv(TUChar *pNetData,TInt32 dataLen)
{
    CMessageHeader *pMsg = (CMessageHeader*)pNetData;
    TInt32 msgLen = pMsg->GetLength();
#ifdef _DEBUG  
    //为测试而写！！！！
    if (448 != msgLen)
    {
        return 0;
    }
#endif
    //一条条回
    return msgLen;
}

}//end of namespace Zephyr;