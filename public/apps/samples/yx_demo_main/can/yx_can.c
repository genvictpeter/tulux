#include "def.h"
#include "serial_link/yx_list.h"
#include "serial_link/yx_chksum.h"
#include "serial_link/yx_string.h"
#include "serial_link/yx_fsm.h"
#include "serial_link/serial_link.h"
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "yx_can.h"


#define CAN_REC_CMD     0x0030
#define CAN_SEND_CMD	0x0031
#define CAN_SET_BAUD    0x0032

typedef struct{
	INT8U ch;
	INT8U id[4];
	INT8U format;
	INT8U type;
	INT8U len;
	INT8U data[8];
}yx_can_txrx_t;

typedef struct {
	INT8U ch;
	INT8U baud[4];
}yx_can_baud_set_t;

static pCanRxFun p_s_CanRxCallBack = NULL;


static void _yx_can_rx_handle(const INT8U* pbuf, int size)
{
	YX_CAN_RX_T tCanRxMsg;
	yx_sl_item_t * pSlItem = (yx_sl_item_t *)pbuf;
	yx_can_txrx_t * pCanData = (yx_can_txrx_t *)pSlItem->Data;
	INT16U cmd = yx_str_char2short_msb(&pSlItem->Head.cmd_h);
	if (cmd == CAN_REC_CMD) {
		tCanRxMsg.CanId =  yx_str_char2long_lsb(pCanData->id);
		tCanRxMsg.ch 		= pCanData->ch;
		tCanRxMsg.Format 	= pCanData->format;
		tCanRxMsg.Type 		=  pCanData->type;
		tCanRxMsg.Len 		= pCanData->len;
		memcpy(tCanRxMsg.Data,pCanData->data,pCanData->len);
		if (p_s_CanRxCallBack != NULL) {
			p_s_CanRxCallBack(&tCanRxMsg);
		}
	}
}


/*******************************************************************************
** 函数名:     Yx_Can_Init
** 功能描述:   Can 模块初始化
** 输入参数:  
** 返回参数:  无
******************************************************************************/
void Yx_Can_Init(void)
{
	Yx_Serial_Link_DispatcherRegister(CAN_REC_CMD,CAN_REC_CMD,_yx_can_rx_handle);
}

/*******************************************************************************
** 函数名:     Yx_Can_Send
** 功能描述:   Can 数据发送
** 输入参数:  ch：通道号
**  		format：标准帧 or 扩展帧
**  		type：数据帧 or 远程帧
**  		Canid:Can ID
**  		pdata 发送数据指针
**  		len 发送数据长度			(只支持8数据长度发送)
** 返回参数:-1 : 通道号错误 -2 帧格式错误 -3 帧类型错误 -4 长度错误  0：正确
******************************************************************************/
INT32S Yx_Can_Send(CAN_CHANNEL_E ch, CAN_FORMAT_E format,CAN_FRAME_TYPE_E type,INT32U Canid,INT8U* pdata,INT8U len)
{
	yx_can_txrx_t tCanMsg;

	if (ch <= CAN_CHANNEL_NULL || ch >= CAN_CHANNEL_MAX) {
		return -1;
	}
	if (format != CAN_FORMAT_STD && format != CAN_FORMAT_EXT) {
		return -2;
	}
	if (type != CAN_FRAME_TYPE_DATA && type != CAN_FRAME_TYPE_REMOTE) {
		return -3;
	}
	if (len > 8) {
		return -4;
	}
	yx_str_long2char_lsb(tCanMsg.id , Canid);
	tCanMsg.ch 		= ch;
	tCanMsg.format 	= format;
	tCanMsg.type 	= type;
	tCanMsg.len 	= 8;
	memcpy(tCanMsg.data,pdata,len);
	Yx_Serial_Link_Send(CAN_SEND_CMD, &tCanMsg,sizeof(yx_can_txrx_t));
	return 0;
}

/*******************************************************************************
** 函数名:     Yx_Can_SetBaud
** 功能描述:   Can 波特率设置
** 输入参数:  ch：通道号
**  		baud ：波特率     500K  即baud =  500000
** 返回参数:-1 : 通道号错误   0：正确
******************************************************************************/
INT32S Yx_Can_SetBaud(CAN_CHANNEL_E ch,INT32U baud)
{
	yx_can_baud_set_t s_baud;
	if (ch <= CAN_CHANNEL_NULL || ch >= CAN_CHANNEL_MAX) {
		return -1;
	}
	s_baud.ch = ch;
	yx_str_long2char_lsb(s_baud.baud , baud);
	Yx_Serial_Link_Send(CAN_SET_BAUD, &s_baud,sizeof(yx_can_baud_set_t));
	return 0;
}


/*******************************************************************************
** 函数名:     Yx_Can_RegisterRxCallBack
** 功能描述:   Can 接收注册函数
** 输入参数:  pCb ： can 接收数据回调
** 返回参数:无
******************************************************************************/
void Yx_Can_RegisterRxCallBack(pCanRxFun pCb)
{	
	p_s_CanRxCallBack = pCb;
}

