/*************************************************
  Copyright (C), 1988-1999
  File name:      slkReader.h
  Author:         ½־��
  Version:        V1.2
  Date:           12-22-2009
  Description:    ��ȡ.slk��
  Others:         none
  Function List:  
  History:
*************************************************/

#ifndef _SLK_SLKREADER_H__
#define _SLK_SLKREADER_H__
//#include "types.h"
#include <stdio.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////
#define SLK_EOF	0xFFFFFFFF
#define SLK_ERR 0xFFFFFFFF

#define MAX_SLK_NUM_STRING_LEN	64
#define MAX_SLK_COL_NAME_LEN	128
#define MAX_SLK_VALUE_STRING	2048



//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
typedef signed char			sInt8;
typedef unsigned char		uInt8;
typedef signed short		    sInt16;
typedef unsigned short		uInt16;
typedef signed int			sInt32;
typedef unsigned int		    uInt32;
#ifdef WIN32
typedef __int64				sInt64;
typedef unsigned __int64	uInt64;
#else
typedef long long				sInt64;
typedef long long 	        uInt64;
#endif

typedef float				flt32;		// 32-bit IEEE floating point.
typedef double				flt64;		// 64-bit IEEE double.


class IFileReader
{
public:
	virtual					~IFileReader(){};

	virtual bool			OpenFile( const char* pFileName ) = 0;
	virtual void			Close() = 0;

	virtual size_t			Read( void* pBuffer, uInt32 pSizeToRead ) = 0;
	virtual bool			Seek( uInt32 pFinalPosition, sInt32 pRelativeMovement = SEEK_SET ) = 0;

	virtual uInt32			GetSize() const = 0;

};

//////////////////////////////////////////////////////////////////////////
extern	bool CreateFileReader( IFileReader** pReader );

//////////////////////////////////////////////////////////////////////////
struct TSLKValue
{
	friend class CFileReadSLK;
	friend struct TSLKRow;
public:
	TSLKValue();
	~TSLKValue();

	sInt32		ToInt32() const;
	flt64		ToFlt64() const;
	const char*	ToString() const;
protected:
	sInt32	intValue;
	double	floatValue;
	// NOTE:����ַ���ָ�����������䣬�����ͷš�
	char*	stringValue;
};


//////////////////////////////////////////////////////////////////////////
struct TSLKColumn
{
	char name[MAX_SLK_COL_NAME_LEN];
};

//////////////////////////////////////////////////////////////////////////
struct TSLKRow
{
	friend class CFileReadSLK;
public:
	TSLKRow();
	~TSLKRow();

	TSLKValue* GetValue( uInt32 pColumn ) const;
	bool SetValue( uInt32 pColumn, TSLKValue* pValue );
private:
	TSLKValue*	value;
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class CFileReadSLK
{
public:
						CFileReadSLK();
						~CFileReadSLK();

	// Slow. you must call at initialize time.
	// ͬһʱ��һ�� CFileReadSLK ֻ�ܶ�Ӧһ���ļ���
	// ��Ϊÿ����һ�� LoadFromFile ��Ҫ��һ���ļ������ԱȽ�����
	bool				LoadFromFile( const char* pFileName );
	// �ر� Slk �ļ���������� buffer.
	void				Close();

	// �����к��еĸ�����
	uInt32				GetRowCount() const{ return m_row; };
	uInt32				GetColCount() const{ return m_col; };

	// access straight. û�б߽��顣.
	const TSLKValue*	GetValue( uInt32 pRow, uInt32 pCol ) const;
	TSLKValue*			GetValue( uInt32 pRow, uInt32 pCol );

	// ����ȡֵ���������ֵΪ SLK_EOF ���ļ�������.
	uInt32				GetFirstRow();
	uInt32				GetNextRow();
	uInt32				GetLastRow();

	//const TSLKValue* GetData( const char* pColName );
	// NOTE:���ص�ֵ�����ں� CFileReadSLK ��ͬ��
	TSLKValue*			GetData( const char* pColName );
	const TSLKValue*	GetData( uInt32 pColumn ) const;
	TSLKValue*			GetData( uInt32 pColumn );

	int GetDataInteger(const char* pColName,int DefualtValue);
	double GetDataDouble(const char* pColName,double DefualtValue);
	const char * GetDataString(const char* pColName,const char * DefualtValue);

	const char * GetColName(uInt32 pColumn);

private:
	char*	FetchKnownKeyValue( char* pData, bool& pIsEnd );
	char*	EvalFKeyValue( char* pData, bool& pIsEnd );
	char*	EvalCKeyValue( char* pData, bool& pIsEnd );
	char*	EvalUnknowKey( char* pData, bool& pIsEnd );
	void	EvalKeyLine( char* pData );
	bool	GetInfoFromMemory( void* pMemory, uInt32 pSize );
	uInt32	GetColByName( const char* pName );


private:
	TSLKColumn*	m_column;
	TSLKRow*	m_data;
	uInt32		m_curPos;

	uInt32		m_row;
	uInt32		m_col;

	uInt32		m_curRow;
	uInt32		m_curCol;

	struct _TSLKValue
	{
		uInt32		row;
		uInt32		col;
		TSLKValue	value;
	};

	std::vector< _TSLKValue* >	_TSLKValueArray;
};




#endif	// _SLK_SLKREADER_H__


