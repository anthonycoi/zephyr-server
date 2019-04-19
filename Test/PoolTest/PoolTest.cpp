// PoolTest.cpp : 定义控制台应用程序的入口点。
//

#include "../../Public/tpl/include/TplPool.h"
#include "time.h"
#include "stdlib.h"
#include <windows.h>

class CTestClass
{
public:
    int m_nIdx;
    int &GetKey()
    {
        return m_nIdx;
    }
};
using namespace Zephyr;
int main()
{
    CTestClass **ppHolder = new CTestClass*[100000];
    CPool<CTestClass> tPool;
    tPool.InitPool(10000);
    int usedNr=0;
    int runCnt=1;

    __int64 nReleaseTime = 0;
    __int64 nGetTime = 0;
    unsigned int timeLast = 0;
    while(++runCnt)
    {
        if(0==(runCnt%100))
        {
            srand(time(0));
#ifdef _WIN32
            //srand(timeGetTime());
#else
            
#endif
            Sleep(15);
        }
        //srand(time(0));
        int ran = 0;
        while (0==ran)
        {
            ran = (rand()%10000);
        }
        if (usedNr<10000)
        {
            ++ran;
        }
        if (usedNr>20000)
        {
            ++ran;
        }
        bool bAdd = false;
        if (runCnt%2)
        {
            bAdd = true;
        }
        if (bAdd)
        {
            //加
            if((ran+usedNr)<100000)
            {
                for(int i=0;i<ran;++i)
                {
                    ppHolder[usedNr]=tPool.GetMem();
                    if (ppHolder[usedNr])
                    {
                        ++usedNr;
                    }
                }
                nGetTime += ran;
            }
        }
        else
        {
            if (ran < usedNr)
            {
                for(int i=0;i<ran;++i)
                {
                    tPool.ReleaseMem(ppHolder[usedNr-1]);
                    --usedNr;
                }
                nReleaseTime += ran;
            }
        }
    }
	return 0;
}

