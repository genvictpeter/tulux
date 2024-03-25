#ifndef DEF_H
#define DEF_H

#include <stdio.h>



#define YX_MIN(x, y) ((x) > (y) ? (y) : (x))
#define YX_MAX(x, y) ((x) > (y) ? (x) : (y))


typedef unsigned char       BOOLEAN;                                            /* ������ */
typedef unsigned char       INT8U;                                              /* �޷����ַ��� */
typedef signed   char       INT8S;                                              /* �з����ַ��� */
typedef unsigned short      INT16U;                                             /* �޷���16bit������ */
typedef signed   short      INT16S;                                             /* �з���16bit������ */
typedef unsigned int        INT32U;                                             /* �޷���32bit����(ͬunsigned long) */
typedef signed   int        INT32S;                                             /* �з���32bit����(ͬsigned long) */
typedef unsigned long long  INT64U;                                             /* �޷���64bit�������� */
typedef signed   long long  INT64S;                                             /* �з���64bit�������� */
typedef float               FP32;                                               /* ������ */
typedef double              FP64;                                               /* ˫���ȸ����� */
#ifndef CHAR
typedef char                CHAR;
#endif
#ifndef wchar
typedef INT16U              wchar;
#endif
#ifndef WCHAR
typedef INT16U              WCHAR;
#endif

typedef struct {
    INT32U msgid;
    void  *PtrMsgSt;
} st_yx_msg_t;

/* ��ȷ,����,�ɹ�,ʧ�ܺ��ͳһ���� */
#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif


#ifndef SUCC
#define SUCC    0
#endif

#ifndef FAIL
#define FAIL    -1
#endif


#ifndef   NULL
#define   NULL  ((void*)0)
#endif

typedef union {
    INT16U     hword;
#ifdef __BIG_ENDIAN
    struct {
        INT8U  high;
        INT8U  low;
    } bytes;
#else
    struct {
        INT8U  low;
        INT8U  high;
    } bytes;
#endif
} YX_HWORD_U;

/*
********************************************************************************
*                  DEFINE LONG UNION
********************************************************************************
*/
typedef union {
    INT32U ulong;
#ifdef  __BIG_ENDIAN
    struct {
        INT8U  byte1;
        INT8U  byte2;
        INT8U  byte3;
        INT8U  byte4;
    } bytes;
#else
    struct {
        INT8U  byte4;
        INT8U  byte3;
        INT8U  byte2;
        INT8U  byte1;
    } bytes;
#endif
} YX_LWORD_U;
#define YX_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

//#define MY_LOG

#ifdef MY_LOG
#define LOG_DEBUG(msg, args...)
#define LOG_INFO(msg, args...)
#define LOG_WARN(msg, args...)
#define LOG_ERROR(msg, args...)
#define LOG_FATAL(msg, args...)
#else
#define LOG_DEBUG(msg, args...)   printf("\nD:%s:",msg, ##args)
#define LOG_INFO(msg, args...)    printf("\nINFO:%s:",msg, ##args)
#define LOG_WARN(msg, args...)    printf("\nWARN:%s:",msg, ##args)
#define LOG_ERROR(msg, args...)   printf("\nERROR:%s:",msg, ##args)
#define LOG_FATAL(msg, args...)   printf("\nFATAL:%s:",msg, ##args)
#endif



#define MALLOC      malloc
#define FREE        free

#define yx_malloc  malloc
#define yx_free    free

#define BUFFER_PRINT(buf, len)    do {  \
        int i;                          \
        for (i = 0; i < (len); ++i) {     \
            LOG_DEBUG("%02x ", (buf)[i]); \
            if (i == (len - 1))         \
                LOG_DEBUG("\n");        \
        }                               \
    } while(0)

#endif // DEF_H
