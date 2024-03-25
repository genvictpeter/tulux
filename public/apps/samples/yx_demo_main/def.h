#ifndef DEF_H
#define DEF_H

#include <stdio.h>



#define YX_MIN(x, y) ((x) > (y) ? (y) : (x))
#define YX_MAX(x, y) ((x) > (y) ? (x) : (y))


typedef unsigned char       BOOLEAN;                                            /* 布尔型 */
typedef unsigned char       INT8U;                                              /* 无符号字符型 */
typedef signed   char       INT8S;                                              /* 有符号字符型 */
typedef unsigned short      INT16U;                                             /* 无符号16bit短整型 */
typedef signed   short      INT16S;                                             /* 有符号16bit短整型 */
typedef unsigned int        INT32U;                                             /* 无符号32bit整型(同unsigned long) */
typedef signed   int        INT32S;                                             /* 有符号32bit整形(同signed long) */
typedef unsigned long long  INT64U;                                             /* 无符号64bit长长整型 */
typedef signed   long long  INT64S;                                             /* 有符号64bit长长整型 */
typedef float               FP32;                                               /* 浮点型 */
typedef double              FP64;                                               /* 双精度浮点型 */
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

/* 正确,错误,成功,失败宏的统一定义 */
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
