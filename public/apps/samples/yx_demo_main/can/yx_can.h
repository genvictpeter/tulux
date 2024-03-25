

#ifndef _YX_CAN_H_
#define _YX_CAN_H_
#include "def.h"


typedef enum {
	CAN_CHANNEL_NULL	= 0,
	CAN_CHANNEL_1,
	CAN_CHANNEL_2,
	CAN_CHANNEL_3,
	CAN_CHANNEL_MAX,
}CAN_CHANNEL_E;

typedef enum {
	CAN_FORMAT_STD	= 0,
	CAN_FORMAT_EXT,
}CAN_FORMAT_E;

typedef enum 
{
    CAN_FRAME_TYPE_DATA 	= 0,   /*!< Data frame type attribute. */
    CAN_FRAME_TYPE_REMOTE , 		/*!< Remote frame type attribute. */
} CAN_FRAME_TYPE_E;


typedef struct{
	CAN_CHANNEL_E 		ch;
	INT32U 				CanId;
	CAN_FORMAT_E 		Format;
	CAN_FRAME_TYPE_E 	Type;
	INT8U 				Len;
	INT8U 				Data[8];
}YX_CAN_RX_T;

typedef void(*pCanRxFun)(YX_CAN_RX_T* pData);

/*******************************************************************************
** 函数名:     Yx_Can_Init
** 功能描述:   Can 模块初始化
** 输入参数:  
** 返回参数:  无
******************************************************************************/
void Yx_Can_Init(void);

/*******************************************************************************
** 函数名:     Yx_Can_Send
** 功能描述:   Can 数据发送
** 输入参数:  ch：通道号
**  		format：标准帧 or 扩展帧
**  		type：数据帧 or 远程帧
** 			Canid : can id
**  		pdata 发送数据指针
**  		len 发送数据长度			(只支持8数据长度发送)
** 返回参数:-1 : 通道号错误 -2 帧格式错误 -3 帧类型错误 -4 长度错误  0：正确
******************************************************************************/
INT32S Yx_Can_Send(CAN_CHANNEL_E ch, CAN_FORMAT_E format,CAN_FRAME_TYPE_E type,INT32U Canid,INT8U* pdata,INT8U len);

/*******************************************************************************
** 函数名:     Yx_Can_SetBaud
** 功能描述:   Can 波特率设置
** 输入参数:  ch：通道号
**  		baud ：波特率     500K  即baud =  500000
** 返回参数:-1 : 通道号错误   0：正确
******************************************************************************/
INT32S Yx_Can_SetBaud(CAN_CHANNEL_E ch,INT32U baud);

/*******************************************************************************
** 函数名:     Yx_Can_RegisterRxCallBack
** 功能描述:   Can 接收注册函数
** 输入参数:  pCb ： can 接收数据回调
** 返回参数:无
******************************************************************************/
void Yx_Can_RegisterRxCallBack(pCanRxFun pCb);


#endif
