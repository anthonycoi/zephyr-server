#include "MsgParser.h"

namespace Zephyr
{


TInt32  CMsgParser::OnRecv(TUChar *pNetData,TInt32 dataLen)
{
    CMessageHeader *pMsg = (CMessageHeader*)pNetData;
    TInt32 msgLen = pMsg->GetLength();
#ifdef _DEBUG  
    //Ϊ���Զ�д��������
    if (448 != msgLen)
    {
        return 0;
    }
#endif
    //һ������
    return msgLen;
}

}//end of namespace Zephyr;