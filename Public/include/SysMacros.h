#ifndef __ZEPHYR_SYS_MACRO_H__
#define __ZEPHYR_SYS_MACRO_H__

/*-------------------------------------------------------------
 Copyright (C)| 
 File: SysMacros.h
 Author: Zephyr Shannon
 Date: 2010-3-2 10:31:50
 Version: 
 Description:  所有平台相关的宏定义都在这里
 Others: 
 Function List: 
 Histroy: 
 -------------------------------------------------------------*/


#ifdef _MSC_VER
#ifndef WIN32
#error you must specify WIN32 specification
#endif
#endif

#if !defined (_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // pretend it's at least Windows XP or Win2003
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif


#define INIT_INT32_VALUE (-2147483648)
#define INIT_INT16_VALUE (-32768)

#define SUCCESS 0

#define FAIL -1

#define NULL 0

#define OUT_OF_MEM -2

#define INPUT_PARA_ERROR -3

#define OUT_OF_RANGE -4




#define ALREADY_EXIST -5



#define DOES_NOT_EXIST -6

#define NOT_INIT_AS_LIST -7

#define MSG_FORMAT_ERROR -8

#define CONN_FAIL       -9



#define UNSEND_MSG_EXIST -10




#define NULL_POINTER -12


#define NOT_BELONG_TO_THIS_CAPSULA - 13 

#define VIRTUAL_METHOD_NOT_IMPLEMENTED -14

#define CAN_NOT_UPPER_CAST_TO_DERIVED_CLASS -15

#define ITEM_ALREADY_IN_THE_TREE -16
#define KEY_ALREADY_EXIST -17

#define WRONG_TYPE -18

#define CAN_NOT_HANDLE_THIS_MSG -19

#define SERVICE_NOT_REGISTERED -20

#define NOT_INITIALIZED -21

#define MSG_LENGTH_INCORRECT -22

#define ERROR_STATE   -23

#define TIMER_ALREADY_TRIGGERED -24

#define TIMER_ALREADY_EXPIRED - 25

#define TASK_IS_RUNING (-26)

#define TASK_ALREADY_INITED (-27)

#define TASK_NOT_FOUND (-28)

#define ITEM_NOT_FOUND_IN_LIST (-29)

#define CONNECTION_NOT_ESTABLISHED (-30)

#define MSG_QUEUE_BLOCKED (-31)

#define MEM_NOT_ALIGNED (-32)

#define ALREADY_IN_TREE (-33)

#define NR_OF_NET_WORKER (1)

#define ABS(n)   (n>0 ? n : -n)


#ifndef INFINITE
#define INFINITE	0xFFFFFFFF
#endif

#define MIN_SYSTEM_SLEEP_TIME 15

#define MAX_TASK_NR_IN_MGR 32
#define TASK_RUN_COUNT 128
#define TASK_ALREADY_RUNNING (-27)


#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


#define MAKE_SERVER_ID(ID)			((ID<<10)&0x7C00)
#define MAKE_INTERFACE_ID(ID)		((ID<<7)&0x380)

const unsigned int REPLY_MSG			= 0x10000000;

#define CONNECTION_DATA_INCORRECT (-28)

#define CONNECTION_IN_WRONG_STATE (-29)

#define TIME_OUT (-30)

#define CAN_NOT_OPEN_FILE (-31)

#define MAX_IOCP_PACKET_DATA_LENGTH 1400

#define SELECT_TIME_OUT (1)

#define DEFAULT_THREAD_TERMINATE_TIME 5000



#define NR_OF_QUEUE_IN_TASK (3)


#define NET_CONNECTION_ESTABLISHED  (0x01)
#define NET_CONNECTION_READABLE     (0x02)
#define NET_CONNECTION_WRITABLE     (0x03)
#define NET_CONNECTION_BROKEN       (0x04)


//
// Formats used in printf function, for eg:
// printf("The int64 number is: "FMT_I64, llNumber);
// printf("The uint64 number is: "FMT_U64, qwNumber);
// printf("The uint64 number is: "FMT_64X, qwNumber);
//

//time1 >= time2
#define GET_TIME_2_TIME(result,time1,time2) \
{ \
    if ((TUInt32)time1 >= (TUInt32)time2) \
    { \
        (result) = time1 - time2; \
    } \
    else \
    { \
        ((TUInt32)result) = ((TUInt32)0xFFFFFFFF) - ((TUInt32)time2); \
        (TUInt32)result += (TUInt32)time1; \
    } \
} \

#define NEW(POINTER,TYPE,MEM_SIZE) \
try \
{ \
    POINTER = new TYPE[MEM_SIZE]; \
} \
catch(...) \
{ \
} \

#define DELETEP(POINTER) \
{ \
    delete POINTER; \
    POINTER = NULL; \
}

#define DELETEARRAY(ARRAYPOINTER) \
{ \
    delete [] POINTER; \
    POINTER = NULL; \
} \

#ifdef WIN32
    #define FMT_I64 "%I64"
    #define FMT_U64 "%I64u"
    #define FMT_64X "%I64x"
#else
    #define FMT_I64 "%ll"
    #define FMT_U64 "%llu"
    #define FMT_64X "%llx"
#endif


#define DECLARE_CLASS_LIST(LISTCLASS) \
protected:\
struct T##LISTCLASS##LIST \
{ \
    LISTCLASS* m_pPrev; \
    LISTCLASS* m_pNext; \
    T##LISTCLASS##LIST() \
    { \
        m_pPrev = NULL; \
        m_pNext = NULL; \
    } \
};\
T##LISTCLASS##LIST m_##LISTCLASS##List; \
public:\
LISTCLASS *GetNext()\
{\
return m_##LISTCLASS##List.m_pNext;\
}\
LISTCLASS *GetPrev()\
{\
    return m_##LISTCLASS##List.m_pPrev;\
}\
void Detach() \
{ \
    if(m_##LISTCLASS##List.m_pPrev)\
    {\
        m_##LISTCLASS##List.m_pPrev->m_##LISTCLASS##List.m_pNext = m_##LISTCLASS##List.m_pNext;\
    }\
    if(m_##LISTCLASS##List.m_pNext)\
    {\
        m_##LISTCLASS##List.m_pNext->m_##LISTCLASS##List.m_pPrev = m_##LISTCLASS##List.m_pPrev;\
    }\
    m_##LISTCLASS##List.m_pNext = NULL;\
    m_##LISTCLASS##List.m_pPrev = NULL;\
}\
void AttachList(LISTCLASS* pClass)\
{\
if (pClass)\
{\
    pClass->m_##LISTCLASS##List.m_pPrev = this;\
}\
    m_##LISTCLASS##List.m_pNext = pClass;\
}\
void InsertNode(LISTCLASS* pClass)\
{\
    if (pClass)\
{\
    pClass->m_##LISTCLASS##List.m_pPrev = this;\
    pClass->m_##LISTCLASS##List.m_pNext = m_##LISTCLASS##List.m_pNext;\
}\
    m_##LISTCLASS##List.m_pNext = pClass;\
}\

#endif 
