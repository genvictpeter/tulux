/********************************************************************************
**
**  文件名称:  yx_chksum.h
**  版权所有:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  文件描述:  校验码工具算法函数
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   | 修改记录
**===============================================================================
**| 2019-12-10 | 夏普   | 创建本模块
*********************************************************************************/

#ifndef _YX_CHKSUM_H_
#define _YX_CHKSUM_H_

#include "def.h"

/*******************************************************************
** 函数名:     yx_chksum_get8bit
** 功能描述:   单字节累加带进位校验码算法
** 参数:       [in]  ptr:           目标数据地址
**             [in]  len:           目标数据长度
** 返回:       单字节校验码
********************************************************************/
INT8U yx_chksum_get8bit(INT8U *ptr, INT32U len);

/*******************************************************************
** 函数名:     yx_chksum_get8bitb
** 功能描述:   单字节校验码算法,数据取反码后再计算校验和
** 参数:       [in]  ptr:           目标数据地址
**             [in]  len:           目标数据长度
** 返回:       单字节校验码
********************************************************************/
INT8U yx_chksum_get8bitb(INT8U *ptr, INT32U len);

/*******************************************************************
** 函数名:     yx_chksum_get16bit
** 功能描述:   双字节校验码算法
** 参数:       [in]  ptr:           目标数据地址
**             [in]  len:           目标数据长度
** 返回:       双字节校验码
********************************************************************/
INT16U yx_chksum_get16bit(INT8U *ptr, INT32U len);

/*******************************************************************
** 函数名:     yx_chksum_getsum
** 功能描述:   获取数据区的累加校验码
** 参数:       [in]  dptr:          数据区地址
**             [in]  len:           数据区长度
** 返回:       单字节累加校验码
********************************************************************/
INT8U yx_chksum_getsum(INT8U *dptr, INT16U len);

/*******************************************************************
** 函数名:     yx_chksum_getsumb
** 功能描述:   获取数据区的累加校验码的反码
** 参数:       [in]  dptr:          数据区地址
**             [in]  len:           数据区长度
** 返回:       单字节累加校验码反码
********************************************************************/
INT8U yx_chksum_getsumb(INT8U *dptr, INT16U len);

/*******************************************************************
** 函数名:     yx_chksum_getsum_c
** 功能描述:   获取数据区带进位累加校验码
** 参数:       [in]  dptr:          数据区地址
**             [in]  len:           数据区长度
** 返回:       单字节带进位累加校验码
********************************************************************/
INT8U yx_chksum_getsum_c(INT8U *dptr, INT16U len);

/*******************************************************************
** 函数名:     yx_chksum_getxor
** 功能描述:   获取数据区异或校验码
** 参数:       [in]  Ptr:           数据区地址
**             [in]  Len:           数据区长度
** 返回:       单字节带异或校验码
********************************************************************/
INT8U yx_chksum_getxor(INT8U *Ptr, INT32U Len);

#endif


