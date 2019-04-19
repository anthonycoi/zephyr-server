/*************************************************
  Copyright (C), 1988-1999
  File name:      SettingFile.h
  Author:         陆志华
  Version:        V1.2
  Date:           12-22-2009
  Description:    读取.ini
  Others:         none
  Function List:  
  History:
*************************************************/
//#include "TypeDef.h"
//#include "SystemTools.h"
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <string>

typedef long long LONGLONG;
inline void TrimStr(std::string& str,char TrimChar)
{
	int Start=0,End=-1;
	for(int i=0;i<(int)str.size();i++)
	{
		if(str[i]!=TrimChar)
		{
			Start=i;
			break;
		}
	}

	for(int i=(int)str.size()-1;i>=0;i--)
	{
		if(str[i]!=TrimChar)
		{
			End=i;
			break;
		}
	}

	str=str.substr(Start,End-Start+1);

}

class CSettingFile
{
protected:
	std::vector<std::string> m_SettingStrings;
	int	m_FileChannel;
public:
	CSettingFile(int FileChannel=0)
	{
		m_FileChannel=FileChannel;
	}
    bool LoadFromFile(const char * FileName,unsigned long long uPwd1,unsigned long long uPwd2)
    {
        FILE * pFile=NULL;
        pFile=fopen(FileName,"rb");
        if(NULL == pFile)
        {
            fprintf(stderr, "Open setting file:%s failed:%s \n\r",FileName, strerror(errno));
        }


        if(pFile)
        {
            fseek(pFile,0,SEEK_END);
            int nFileSize=(int)ftell(pFile);
            nFileSize += 8;
            nFileSize <<= 3;
            nFileSize >>= 3;

            char * pStr=new char[nFileSize];
            memset(pStr,0,nFileSize);
            fseek(pFile,0,SEEK_SET);
            fread(pStr,1,nFileSize,pFile);
            fclose(pFile);
            for (int i=0;i<(nFileSize);i+=8)
            {
                unsigned long long *pRaw = (unsigned long long *)(pStr+i);
                *pRaw ^= uPwd2;
                uPwd2 ^= *pRaw;
            }
            for (int i=0;i<(nFileSize);i+=8)
            {
                unsigned long long *pRaw = (unsigned long long *)pStr+i;
                *pRaw ^= uPwd1;
                uPwd1 ^= *pRaw;
            }
            pStr[nFileSize]=0;
            bool ret=Load(pStr);
            delete[] pStr;
            return ret;
        }
        return false;
    }

	bool LoadFromFile(const char * FileName)
	{
		FILE * pFile=NULL;
		pFile=fopen(FileName,"rb");
        if(NULL == pFile)
        {
            fprintf(stderr, "Open setting file:%s failed:%s \n\r",FileName, strerror(errno));
        }


		if(pFile)
		{
			fseek(pFile,0,SEEK_END);
			int FileSize=(int)ftell(pFile);
			char * Str=new char[FileSize+1];
			fseek(pFile,0,SEEK_SET);
			fread(Str,1,FileSize,pFile);
			fclose(pFile);
			Str[FileSize]=0;
			bool ret=Load(Str);
			delete[] Str;
			return ret;
		}
		return false;
	}

	bool Load(const char * SettingString,char LineDelimiter='\r',char CommentHead=';')
	{
		char * Source;
		char * Line;
		int Len=(int)strlen(SettingString)+1;
		char * pBuff=new char[Len];
		Source=pBuff;
		strcpy(Source,SettingString);
		m_SettingStrings.clear();
		while(Source)
		{
			char * Find=(char *)FindNextLine(Source,LineDelimiter);
			if(Find)
			{
				Find[0]=0;
				Find++;
				Line=Source;
				Source=Find;
			}
			else
			{
				Line=Source;
				Source=NULL;
			}
			Find=(char *)FindNextLine(Line,CommentHead);
			if(Find)
				Find[0]=0;
			std::string temp=Line;
			TrimStr(temp,' ');
			TrimStr(temp,LineDelimiter);
			TrimStr(temp,'\r');
			TrimStr(temp,'\n');
			if(!temp.empty())
				m_SettingStrings.push_back(temp);
		}
		delete[] pBuff;
		return true;
	}

	const char * GetString( const char * pszSection, const char * pszItemName, const char * pszDefValue = NULL )
	{
		const char * p = GetSettingString( pszSection, pszItemName );
		if( p == NULL )return pszDefValue;
		return p;
	}
	int	GetInteger( const char * pszSection, const char * pszItemName, int DefValue = 0 )
	{
		const char * p = GetSettingString( pszSection, pszItemName );
		if( p == NULL )return DefValue;
		return atoi(p);
	}
	LONGLONG	GetInteger64( const char * pszSection, const char * pszItemName, int DefValue = 0 )
	{
		const char * p = GetSettingString( pszSection, pszItemName );
		if( p == NULL )return DefValue;
		return _atoi64(p);
	}
	double GetDouble(const char * pszSection, const char * pszItemName, double DefValue = 0 )
	{
		const char * p = GetSettingString( pszSection, pszItemName );
		if( p == NULL ) return DefValue;
		return atof(p);
	}
protected:
	int	FindSectionLine( const char * pszSection )
	{
		if( pszSection == NULL )
			return 0;
		int linecount = (int)m_SettingStrings.size();
		int sectionlength = (int)strlen( pszSection );
		for( int i = 0;i < linecount;i ++ )
		{
			const char * p = m_SettingStrings[i].c_str();

			if( *p == '[' && *(p+sectionlength+1)==']' )
			{
				if( strnicmp( p+1, pszSection, sectionlength ) == 0 )
					return (i+1);
			}
		}
		return -1;
	}
	const char * GetSettingString(  const char * pszSection, const char * pszItemName )
	{
		int startindex = 0;
		if( pszSection != NULL )
		{
			startindex = FindSectionLine( pszSection );
			if( startindex == -1 )return NULL;
		}

		int itemnamelength = (int)strlen( pszItemName );
		if( itemnamelength == 0 )return NULL;

		int linecount = (int)m_SettingStrings.size();
		for( int i = startindex;i < linecount;i ++ )
		{
			const char * p = m_SettingStrings[i].c_str();
			//	如果到达下一个section， 返回错误
			if( *p == '[' )return NULL;
			//	如果是ItemName=这样的句式，进入进一步搜索
			if( *(p+itemnamelength) == '=' )
			{
				if( strnicmp( p, pszItemName, itemnamelength ) == 0 )
				{
					if( *(p+itemnamelength+1) == '\"' )
					{
						char * pret = (char *)(p+itemnamelength+2);
						int length = (int)strlen( pret );
						if( *( pret + length-1 ) == '\"' )
							*( pret + length-1 ) = 0;
						return pret;
					}
					return (p+itemnamelength+1);
				}
			}
		}
		return NULL;

	}
protected:
	const char * FindNextLine(const char * pStr,char LineDelimiter)
	{
		bool InStr=false;
		while(*pStr)
		{
			if(*pStr=='"')
				InStr=!InStr;
			if(*pStr==LineDelimiter&&(!InStr))
				return pStr;
			pStr++;

		}
		return NULL;
	}
};
