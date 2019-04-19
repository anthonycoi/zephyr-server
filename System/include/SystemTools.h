#ifndef __ZEPHYR_SYSTEM_TOOLS_H__
#define __ZEPHYR_SYSTEM_TOOLS_H__

#include <time.h>
#include "../../Public/include/TypeDef.h"
#include "../../Public/include/SysMacros.h"
#include <stdlib.h>


#define CLOCKS_PER_MILLISEC		(CLOCKS_PER_SEC/1000)

#ifdef WIN32
namespace Zephyr
{


inline unsigned int GetTickCount()
{
	return clock()/CLOCKS_PER_MILLISEC;
}
#else
inline unsigned int GetTickCount()
{
	struct timeval CurTime;
	gettimeofday(&CurTime,NULL);
	return CurTime.tv_sec*1000+CurTime.tv_usec/1000;	
}
#endif

#define EXCHANGE(i1,i2)\
{\
	tmp = (i1);\
	(i1) = (i2);\
	(i2) = tmp;\
}\


inline unsigned int sntohl(unsigned int nl)
{
	char *pInt = (char*)&nl;
	char tmp;
	EXCHANGE(pInt[0],pInt[3]);
	EXCHANGE(pInt[1],pInt[2]);
	return nl;
}

inline int ctoa(unsigned char c,char *pBuf, int len)
{
	if ((NULL == pBuf) || (len <4))
	{
		return FAIL;
	}
	unsigned char base = 100;
	int usedLen = 0;
	for (int i=0;i<2;i++)
	{
		unsigned char result = c / base;
		c -= result * base;
		if (result != 0)
		{
			result += '0';

			pBuf[usedLen] = result;

			usedLen ++;
		}
		base /= 10;
	}
	pBuf[usedLen] = c + '0';
	usedLen ++;
	pBuf[usedLen] = 0;

	return usedLen + 1;
}


//for little end use only
inline int IntIpToStr(unsigned int Intip, char *pBuffer,int buffLen)
{
	int ipLen = 0;
	unsigned char *psz = (unsigned char *)&Intip;
	for(int i=0;i<4;i++)
	{

		//something strange because this function doesn't work properly.so I had to write it my own.
		//char *pszDig = _itoa(digI,pBuffer + ipLen,(buffLen-ipLen));
		int usedLen = ctoa(psz[i],pBuffer+ipLen,buffLen);
		if (usedLen != FAIL)
		{
			ipLen += usedLen;
			pBuffer[ipLen -1] = '.';
			pBuffer[ipLen] = '\0';
			buffLen -= usedLen;
			buffLen = buffLen;
		}
		else
		{
			return FAIL;
		}
	}
	pBuffer[ipLen - 1] = '\0';
	return SUCCESS;
}

inline char * strncpy_0(char * pszDest,const char * pszSrc,size_t nLen)
{
	if(pszSrc)
	{
		strncpy(pszDest,pszSrc,nLen);
		pszDest[nLen-1] = 0;
	}
	else
	{
		pszDest[0]=0;
	}
	return pszDest;
}

inline int GetRand(int Min,int Max)
{
	return (rand()%(Max-Min+1))+Min;
}


#define MAXTIME	(unsigned long(0xffffffff))

// unsigned long	GetTimeToTime(unsigned long t1,unsigned long t2 )
// {
// 	return (t1<=t2?(t2-t1):(MAXTIME-t1+t2));
// }

inline unsigned long	GetTimeToTime(unsigned long t1,unsigned long t2 )
{
	return (t1<=t2?(t2-t1):(MAXTIME-t1+t2));
}

#ifndef ZeroMemory
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#endif


#ifdef WIN32

#define atoi64 _atoi64

#else

#define atoi64 atoll

#define strnicmp strncasecmp

#define stricmp strcasecmp

#endif




struct NO_CASE_CHAR_NAME
{
	char	szCharName[64];
	NO_CASE_CHAR_NAME()
	{
		szCharName[0]=0;
	}
	NO_CASE_CHAR_NAME(const char * szName)
	{
		strncpy_0(szCharName,szName,64);
	}
	bool operator>(const char * szName)
	{
		return strnicmp(szCharName,szName,64)>0;
	}
	bool operator<(const char * szName)
	{
		return strnicmp(szCharName,szName,64)<0;
	}
	bool operator>(const NO_CASE_CHAR_NAME& Name)
	{
		return strnicmp(szCharName,Name.szCharName,64)>0;
	}
	bool operator<(const NO_CASE_CHAR_NAME& Name)
	{
		return strnicmp(szCharName,Name.szCharName,64)<0;
	}

};

}

#endif
