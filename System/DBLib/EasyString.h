#pragma once

#ifdef WIN32
inline size_t AnsiToUnicode(const char * SrcStr,size_t SrcLen,wchar_t * DestStr,size_t DestLen)
{	
	return (size_t)MultiByteToWideChar(CP_ACP,0,SrcStr,(int)SrcLen,(LPWSTR)DestStr,(int)DestLen);
}

inline size_t UnicodeToAnsi(const wchar_t * SrcStr,size_t SrcLen,char * DestStr,size_t DestLen)
{
	return (size_t)WideCharToMultiByte(CP_ACP,0,(LPWSTR)SrcStr,(int)SrcLen,DestStr,(int)DestLen,NULL,NULL);
}
#else
inline size_t AnsiToUnicode(const char * SrcStr,size_t SrcLen,wchar_t * DestStr,size_t DestLen)
{
	return mbstowcs(DestStr,SrcStr,SrcLen);
}

inline size_t UnicodeToAnsi(const wchar_t * SrcStr,size_t SrcLen,char * DestStr,size_t DestLen)
{
	return wcstombs(DestStr,SrcStr,SrcLen);
}
#endif

inline int strncpy_0(char *strDest,size_t DestSize, const char *strSource, size_t count)
{
	int Ret=strncpy_s(strDest,DestSize,strSource,count-1);
	strDest[count-1]=0;
	return Ret;
}

inline int strncpy_0( wchar_t *strDest,size_t DestSize, const wchar_t *strSource, size_t count )
{
	int Ret=wcsncpy_s(strDest,DestSize,strSource,count-1);
	strDest[count-1]=0;
	return Ret;
}

inline int CompareString(const char * pStr1,const char * pStr2)
{
	return strcmp(pStr1,pStr2);
}
inline int CompareString(const wchar_t * pStr1,const wchar_t * pStr2)
{
	return wcscmp(pStr1,pStr2);
}
inline int CompareStringNoCase(const char * pStr1,const char * pStr2)
{
	return _stricmp(pStr1,pStr2);
}
inline int CompareStringNoCase(const wchar_t * pStr1,const wchar_t * pStr2)
{
	return _wcsicmp(pStr1,pStr2);
}

template <typename T>
inline char * StringTrimRightT(T * pStr,T TrimChar)
{
	char * pStrTail=pStr;
	while(*pStrTail)
	{
		pStrTail++;
	}
	if(pStrTail==pStr)
		return pStr;
	pStrTail--;
	while(pStrTail!=pStr&&(*pStrTail==TrimChar))
	{
		*pStrTail=0;
	}
	return pStr;
}

template < typename T>
inline char * StringTrimLeftT(T * pStr,T TrimChar)
{
	T * ResultStr=pStr;
	while(*ResultStr==TrimChar)
	{
		ResultStr++;
	}
	return ResultStr;
}

template < typename T>
inline char * StringTrimT(T * pStr,T TrimChar)
{
	T * ResultStr=pStr;
	while(*ResultStr==TrimChar)
	{
		ResultStr++;
	}
	pStr=ResultStr;


	char * pStrTail=pStr;
	while(*pStrTail)
	{
		pStrTail++;
	}
	if(pStrTail==pStr)
		return pStr;
	pStrTail--;
	while(pStrTail!=pStr&&(*pStrTail==TrimChar))
	{
		*pStrTail=0;
	}
	return pStr;
}

#define StringTrimLeft	StringTrimLeftT<TCHAR>
#define StringTrimLeftA	StringTrimLeftT<char>
#define StringTrimLeftW	StringTrimLeftT<wchar_t>

#define StringTrimRight		StringTrimRightT<TCHAR>
#define StringTrimRightA	StringTrimRightT<char>
#define StringTrimRightW	StringTrimRightT<wchar_t>

#define StringTrim	StringTrimT<TCHAR>
#define StringTrimA	StringTrimT<char>
#define StringTrimW	StringTrimT<wchar_t>


template < typename T>
class CEasyStringT
{
protected:
	T*		m_pBuffer;
	size_t	m_BufferSize;
	size_t	m_StringLength;
public:
	CEasyStringT()
	{
		m_pBuffer=new T[1];
		m_pBuffer[0]=0;
		m_BufferSize=1;
		m_StringLength=0;
	}
	CEasyStringT(const char * pStr)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(pStr,GetStrLen(pStr));
	}
	CEasyStringT(const wchar_t * pStr)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(pStr,GetStrLen(pStr));
	}
	CEasyStringT(const char * pStr,size_t Size)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(pStr,Size);
	}
	CEasyStringT(const wchar_t * pStr,size_t Size)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(pStr,Size);
	}
	CEasyStringT(const char Char)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(&Char,1);
	}
	CEasyStringT(const wchar_t Char)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(&Char,1);
	}
	CEasyStringT(const CEasyStringT<char>& Str)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(Str.m_pBuffer,Str.m_StringLength);
	}
	CEasyStringT(const CEasyStringT<wchar_t>& Str)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(Str.m_pBuffer,Str.m_StringLength);
	}
	~CEasyStringT()
	{
		SAFE_DELETE_ARRAY(m_pBuffer);
	}
	void Clear()
	{
		Resize(0,false);
	}
	void Resize(size_t Size,bool ReserveData=true)
	{
		T * pNewBuffer=new T[Size+1];
		size_t NewStringLen=0;
		if(m_pBuffer&&ReserveData)
		{
			if(m_StringLength>Size)
				NewStringLen=Size;
			else
				NewStringLen=m_StringLength;
			memcpy(pNewBuffer,m_pBuffer,sizeof(T)*NewStringLen);
		}
		pNewBuffer[NewStringLen]=0;
		pNewBuffer[Size]=0;
		m_BufferSize=Size+1;
		m_StringLength=NewStringLen;
		SAFE_DELETE_ARRAY(m_pBuffer);
		m_pBuffer=pNewBuffer;
	}
	void TrimBuffer(size_t Size=0)
	{
		if(Size<=0)
		{
			Size=m_BufferSize-1;
			for(size_t i=0;i<m_BufferSize;i++)
			{
				if(m_pBuffer[i]==0)
				{
					Size=i;
					break;
				}
			}
		}
		m_StringLength=Size;
		Resize(m_StringLength);
	}
	void SetString(const char * pStr,size_t Size);
	void SetString(const wchar_t * pStr,size_t Size);
	void AppendString(const char * pStr,size_t Size);
	void AppendString(const wchar_t * pStr,size_t Size);
	size_t GetLength() const
	{
		return m_StringLength;
	}
	void SetLength(size_t Len)
	{
		if(Len<m_BufferSize)
		{
			m_StringLength=Len;
			m_pBuffer[m_StringLength]=0;
		}
	}
	size_t GetBufferSize() const
	{
		return m_BufferSize;
	}
	bool IsEmpty() const
	{
		return m_StringLength==0;
	}
	const T* GetBuffer() const
	{
		return m_pBuffer;
	}
	operator const T*() const
	{
		return m_pBuffer;
	}
	operator T*() const
	{
		return m_pBuffer;
	}
	T& operator[](int Index) const
	{
		return m_pBuffer[Index];
	}
	CEasyStringT<T>& operator=(const char* pStr)
	{
		SetString(pStr,GetStrLen(pStr));
		return *this;
	}
	CEasyStringT<T>& operator=(const wchar_t* pStr)
	{
		SetString(pStr,GetStrLen(pStr));
		return *this;
	}
	CEasyStringT<T>& operator=(char Char)
	{
		SetString(&Char,1);
		return *this;
	}
	CEasyStringT<T>& operator=(wchar_t Char)
	{
		SetString(&Char,1);
		return *this;
	}
	CEasyStringT<T>& operator=(const CEasyStringT<char>& Str)
	{
		SetString(Str.GetBuffer(),Str.GetLength());
		return *this;
	}
	CEasyStringT<T>& operator=(const CEasyStringT<wchar_t>& Str)
	{
		SetString(Str.GetBuffer(),Str.GetLength());
		return *this;
	}
	int Compare(const T* pStr)
	{
		if(m_pBuffer)
			return CompareString(m_pBuffer,pStr);
		else
			return -1;
	}
	int CompareNoCase(const T * pStr)
	{
		if(m_pBuffer)
			return CompareStringNoCase(m_pBuffer,pStr);
		else
			return -1;
	}
	bool operator==(const T* pStr)
	{
		return Compare(pStr)==0;
	}
	bool operator!=(const T* pStr)
	{
		return Compare(pStr)!=0;
	}
	bool operator>(const T* pStr)
	{
		return Compare(pStr)>0;
	}
	bool operator<(const T* pStr)
	{
		return Compare(pStr)<0;
	}
	const CEasyStringT<T>& operator+=(const char * pStr)
	{
		AppendString(pStr,GetStrLen(pStr));
		return *this;
	}
	const CEasyStringT<T>& operator+=(const wchar_t * pStr)
	{
		AppendString(pStr,GetStrLen(pStr));
		return *this;
	}
	const CEasyStringT<T>& operator+=(char Char)
	{
		AppendString(&Char,1);
		return *this;
	}
	const CEasyStringT<T>& operator+=(wchar_t Char)
	{
		AppendString(&Char,1);
		return *this;
	}
	const CEasyStringT<T>& operator+=(const CEasyStringT<T>& Str)
	{
		AppendString(Str.m_pBuffer,Str.m_StringLength);
		return *this;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,const char * pStr2)
	{
		CEasyStringT<T> String;
		String=Str1;
		String+=pStr2;
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,const wchar_t * pStr2)
	{
		CEasyStringT<T> String;
		String=Str1;
		String+=pStr2;
		return String;
	}
	friend CEasyStringT<T> operator+(const  char * pStr1,const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String=pStr1;
		String.AppendString(Str2.m_pBuffer,Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(const  wchar_t * pStr1,const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String=pStr1;
		String.AppendString(Str2.m_pBuffer,Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String.SetString(Str1.m_pBuffer,Str1.m_StringLength);
		String.AppendString(Str2.m_pBuffer,Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,char Char2)
	{
		CEasyStringT<T> String;
		String=Str1;
		String+=Char2;
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,wchar_t Char2)
	{
		CEasyStringT<T> String;
		String=Str1;
		String+=Char2;
		return String;
	}
	friend CEasyStringT<T> operator+(char Char1,const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String=Char1;
		String.AppendString(Str2.m_pBuffer,Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(wchar_t Char1,const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String=Char1;
		String.AppendString(Str2.m_pBuffer,Str2.m_StringLength);
		return String;
	}
	void TrimLeft(T TrimChar)
	{
		T* pTrimedStr=StringTrimLeftT<T>(m_pBuffer,TrimChar);
		SetString(pTrimedStr,GetStrLen(pTrimedStr));
	}
	void TrimRight(T TrimChar)
	{
		T* pTrimedStr=StringTrimRightT<T>(m_pBuffer,TrimChar);
		SetString(pTrimedStr,GetStrLen(pTrimedStr));
	}
	void Trim(T TrimChar)
	{
		T* pTrimedStr=StringTrimT<T>(m_pBuffer,TrimChar);
		SetString(pTrimedStr,GetStrLen(pTrimedStr));
	}
	CEasyStringT<T> SubStr(size_t Start,size_t Number)
	{
		if(Start<0)
			Start=0;
		if(Start>m_StringLength)
			Start=m_StringLength;
		if(Number<0)
			Number=0;
		if(Start+Number>m_StringLength)
			Number=m_StringLength-Start;
		return CEasyStringT<T>(m_pBuffer+Start,Number);
	}
	CEasyStringT<T> Left(size_t Number)
	{
		if(Number>m_StringLength)
			return *this;
		return CEasyStringT<T>(m_pBuffer,Number);
	}
	CEasyStringT<T> Right(size_t Number)
	{
		if(Number>m_StringLength)
			return *this;
		return CEasyStringT<T>(m_pBuffer+m_StringLength-Number,Number);
	}
	void Format(const char * pFormat,...);
	void Format(const wchar_t * pFormat,...);
	void MakeUpper();
	void MakeLower();
	int Find(const char * pDestStr,size_t StartPos=0)
	{
		if(StartPos>m_StringLength)
			StartPos=m_StringLength;
		const char * pResult=strstr(m_pBuffer+StartPos,pDestStr);
		if(pResult==NULL)
			return -1;
		else
			return (int)(pResult-m_pBuffer);
	}
	int Find(const wchar_t * pDestStr,size_t StartPos=0)
	{
		if(StartPos>m_StringLength)
			StartPos=m_StringLength;
		const wchar_t * pResult=wcsstr(m_pBuffer+StartPos,pDestStr);
		if(pResult==NULL)
			return -1;
		else
			return (int)(pResult-m_pBuffer);
	}
	int Find(char DestChar,size_t StartPos=0)
	{
		if(StartPos>m_StringLength)
			StartPos=m_StringLength;
		const char * pResult=strchr(m_pBuffer+StartPos,DestChar);
		if(pResult==NULL)
			return -1;
		else
			return (int)(pResult-m_pBuffer);
	}
	int Find(wchar_t DestChar,size_t StartPos=0)
	{
		if(StartPos>m_StringLength)
			StartPos=m_StringLength;
		const wchar_t * pResult=wcschr(m_pBuffer+StartPos,DestChar);
		if(pResult==NULL)
			return -1;
		else
			return (int)(pResult-m_pBuffer);
	}
	int ReverseFind(char DestChar)
	{
		const char * pResult=strrchr(m_pBuffer,DestChar);
		if(pResult==NULL)
			return -1;
		else
			return (int)(pResult-m_pBuffer);
	}
	int ReverseFind(wchar_t DestChar)
	{
		const wchar_t * pResult=wcsrchr(m_pBuffer,DestChar);
		if(pResult==NULL)
			return -1;
		else
			return (int)(pResult-m_pBuffer);
	}
	void Replace(T OldChar,T NewChar)
	{
		if(OldChar!=NewChar)
		{
			for(size_t i=0;i<m_StringLength;i++)
			{
				if(m_pBuffer[i]==OldChar)
					m_pBuffer[i]=NewChar;
			}
		}
	}
	void Replace(const T * pOldStr,const T * pNewStr)
	{
		if(CompareString(pOldStr,pNewStr)!=0)
		{
			int StartPos=0;
			int ReplaceCount=0;
			int ReplaceSrcLen=(int)GetStrLen(pOldStr);
			int ReplaceDestLen=(int)GetStrLen(pNewStr);
			int * pReplacePos=new int[m_StringLength/ReplaceSrcLen+2];
			int FindPos=Find(pOldStr,StartPos);
			while(FindPos>=0)
			{
				pReplacePos[ReplaceCount]=FindPos;
				ReplaceCount++;
				StartPos=FindPos+ReplaceSrcLen;
				FindPos=Find(pOldStr,StartPos);
			}


			size_t NewStrLen=(int)m_StringLength+ReplaceCount*(ReplaceDestLen-ReplaceSrcLen);
			if(NewStrLen>=m_BufferSize)
				Resize(NewStrLen);

			if(NewStrLen>=m_StringLength)
			{
				int SrcPos=(int)m_StringLength;
				int DestPos=(int)NewStrLen;
				while(ReplaceCount)
				{
					ReplaceCount--;
					int OldSrcPos=SrcPos;
					SrcPos=pReplacePos[ReplaceCount]+ReplaceSrcLen;
					int CopySize=OldSrcPos-SrcPos;
					DestPos-=CopySize;
					if(SrcPos!=DestPos&&CopySize!=0)
						memmove(m_pBuffer+DestPos,m_pBuffer+SrcPos,CopySize*sizeof(T));
					SrcPos-=ReplaceSrcLen;
					DestPos-=ReplaceDestLen;
					if(ReplaceDestLen)
						memcpy(m_pBuffer+DestPos,pNewStr,ReplaceDestLen*sizeof(T));
				}
			}
			else
			{
				pReplacePos[ReplaceCount]=(int)m_StringLength;
				int SrcPos=pReplacePos[0];
				int DestPos=pReplacePos[0];
				for(int i=0;i<ReplaceCount;i++)
				{
					if(ReplaceDestLen)
						memcpy(m_pBuffer+DestPos,pNewStr,ReplaceDestLen*sizeof(T));
					SrcPos+=ReplaceSrcLen;
					DestPos+=ReplaceDestLen;

					int CopySize=pReplacePos[i+1]-SrcPos;
					if(SrcPos!=DestPos&&CopySize!=0)
						memmove(m_pBuffer+DestPos,m_pBuffer+SrcPos,CopySize*sizeof(T));
					SrcPos+=CopySize;
					DestPos+=CopySize;
				}
			}
			m_StringLength=NewStrLen;
			m_pBuffer[m_StringLength]=0;
			SAFE_DELETE_ARRAY(pReplacePos);
		}
	}
	void Delete(size_t StartPos,size_t Len)
	{
		if(StartPos<m_StringLength)
		{
			if(StartPos+Len>m_StringLength)
				Len=m_StringLength-StartPos;
			memmove(m_pBuffer+StartPos,m_pBuffer+StartPos+Len,(m_StringLength-StartPos-Len)*sizeof(T));
			m_StringLength-=Len;
			m_pBuffer[m_StringLength]=0;
		}
	}
	void Remove(T Char)
	{
		int Pos=0;
		while((Pos=Find(Char))>=0)
		{
			Delete(Pos,1);
		}
	}
	void Remove(const T * pStr)
	{
		size_t Len=GetStrLen(pStr);
		if(Len>0)
		{
			int Pos=0;
			while((Pos=Find(pStr))>=0)
			{
				Delete(Pos,Len);
			}
		}
	}
	void Insert(size_t Pos,T Char)
	{
		Insert(Pos,&Char,1);
	}
	void Insert(size_t Pos,const T * pStr,size_t StrLen=0)
	{

		if(Pos<=m_StringLength)
		{
			if(StrLen==0)
				StrLen=GetStrLen(pStr);
			size_t NewStrLen=m_StringLength+StrLen;
			if(NewStrLen>=m_BufferSize)
				Resize(NewStrLen);
			memmove(m_pBuffer+Pos+StrLen,m_pBuffer+Pos,(m_StringLength-Pos)*sizeof(T));
			memcpy(m_pBuffer+Pos,pStr,StrLen*sizeof(T));
			m_StringLength+=StrLen;
			m_pBuffer[m_StringLength]=0;
		}
	}
protected:
	size_t GetStrLen(const char * pStr)
	{
		if(pStr)
			return strlen(pStr);
		else
			return 0;
	}
	size_t GetStrLen(const wchar_t * pStr)
	{
		if(pStr)
			return wcslen(pStr);
		else
			return 0;
	}
};

template<>
inline void CEasyStringT<char>::SetString(const char * pStr,size_t Size)
{
	if(Size>0)
	{
		if(Size+1>m_BufferSize)
			Resize(Size,false);
		if(pStr)
			memcpy(m_pBuffer,pStr,Size);
		m_pBuffer[Size]=0;
		m_StringLength=Size;
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<wchar_t>::SetString(const wchar_t * pStr,size_t Size)
{
	if(Size>0)
	{
		if(Size+1>m_BufferSize)
			Resize(Size,false);
		if(pStr)
			memcpy(m_pBuffer,pStr,sizeof(wchar_t)*Size);
		m_pBuffer[Size]=0;
		m_StringLength=Size;
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<char>::SetString(const wchar_t * pStr,size_t Size)
{
	if(pStr&&Size>0)
	{
		size_t AnsiSize=UnicodeToAnsi(pStr,Size,NULL,0);

		if(AnsiSize>=m_BufferSize)
			Resize(AnsiSize,false);
		if(pStr)
			UnicodeToAnsi(pStr,Size,m_pBuffer,AnsiSize);
		m_pBuffer[AnsiSize]=0;
		m_StringLength=AnsiSize;
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<wchar_t>::SetString(const char * pStr,size_t Size)
{
	if(pStr&&Size>0)
	{
		size_t UnicodeSize=AnsiToUnicode(pStr,Size,NULL,0);
		if(UnicodeSize>=m_BufferSize)
			Resize(UnicodeSize,false);
		if(pStr)
			AnsiToUnicode(pStr,Size,m_pBuffer,UnicodeSize);
		m_pBuffer[UnicodeSize]=0;
		m_StringLength=UnicodeSize;
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<char>::AppendString(const char * pStr,size_t Size)
{
	if(Size>0)
	{
		if(m_StringLength+Size>=m_BufferSize)
			Resize(m_StringLength+Size);
		if(pStr)
			memcpy(m_pBuffer+m_StringLength,pStr,Size);
		m_pBuffer[m_StringLength+Size]=0;
		m_StringLength+=Size;
	}
}

template<>
inline void CEasyStringT<wchar_t>::AppendString(const wchar_t * pStr,size_t Size)
{
	if(Size>0)
	{
		if(m_StringLength+Size>=m_BufferSize)
			Resize(m_StringLength+Size);
		if(pStr)
			memcpy(m_pBuffer+m_StringLength,pStr,sizeof(wchar_t)*Size);
		m_pBuffer[m_StringLength+Size]=0;
		m_StringLength+=Size;
	}
}

template<>
inline void CEasyStringT<char>::AppendString(const wchar_t * pStr,size_t Size)
{
	if(Size>0)
	{
		size_t AnsiSize=UnicodeToAnsi(pStr,Size,NULL,0);
		if(m_StringLength+AnsiSize>=m_BufferSize)
			Resize(m_StringLength+AnsiSize);

		if(pStr)
			UnicodeToAnsi(pStr,Size,m_pBuffer+m_StringLength,AnsiSize);
		m_pBuffer[m_StringLength+AnsiSize]=0;
		m_StringLength+=AnsiSize;
	}
}


template<>
inline void CEasyStringT<wchar_t>::AppendString(const char * pStr,size_t Size)
{
	if(pStr&&Size>0)
	{
		size_t UnicodeSize=AnsiToUnicode(pStr,Size,NULL,0);
		if(m_StringLength+UnicodeSize>=m_BufferSize)
			Resize(m_StringLength+UnicodeSize);

		if(pStr)
			AnsiToUnicode(pStr,Size,m_pBuffer+m_StringLength,UnicodeSize);
		m_pBuffer[m_StringLength+UnicodeSize]=0;
		m_StringLength+=UnicodeSize;
	}
}

template<>
inline void CEasyStringT<char>::Format(const char * pFormat,...)
{
	va_list	vl;
	va_start(vl,pFormat);
	size_t Len=_vscprintf(pFormat,vl);
	Resize(Len,false);
	vsprintf_s(m_pBuffer,m_BufferSize,pFormat,vl);
	m_StringLength=Len;
	va_end( vl);
}

template<>
inline void CEasyStringT<wchar_t>::Format(const wchar_t * pFormat,...)
{
	va_list	vl;
	va_start(vl,pFormat);
	size_t Len=_vscwprintf(pFormat,vl);
	Resize(Len,false);
	vswprintf_s(m_pBuffer,m_BufferSize,pFormat,vl);
	m_StringLength=Len;
	va_end( vl);
}

template<>
inline void CEasyStringT<char>::Format(const wchar_t * pFormat,...)
{
	va_list	vl;
	va_start(vl,pFormat);
	size_t Len=_vscwprintf(pFormat,vl);
	wchar_t * pBuffer=new wchar_t [Len];
	vswprintf_s(pBuffer,Len,pFormat,vl);
	SetString(pBuffer,Len);
	SAFE_DELETE_ARRAY(pBuffer);
	va_end( vl);
}

template<>
inline void CEasyStringT<wchar_t>::Format(const char * pFormat,...)
{
	va_list	vl;
	va_start(vl,pFormat);
	size_t Len=_vscprintf(pFormat,vl);
	char * pBuffer=new char [Len];
	vsprintf_s(pBuffer,Len,pFormat,vl);
	SetString(pBuffer,Len);
	SAFE_DELETE_ARRAY(pBuffer);
	va_end( vl);
}

template<>
inline void CEasyStringT<char>::MakeUpper()
{
	_strupr_s(m_pBuffer,m_BufferSize);
}

template<>
inline void CEasyStringT<wchar_t>::MakeUpper()
{
	_wcsupr_s(m_pBuffer,m_BufferSize);
}

template<>
inline void CEasyStringT<char>::MakeLower()
{
	_strlwr_s(m_pBuffer,m_BufferSize);
}

template<>
inline void CEasyStringT<wchar_t>::MakeLower()
{
	_wcslwr_s(m_pBuffer,m_BufferSize);
}




typedef CEasyStringT<wchar_t> CEasyStringW;

typedef CEasyStringT<char> CEasyStringA;

typedef CEasyStringT<TCHAR> CEasyString;


