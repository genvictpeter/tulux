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
** ������:     Yx_Can_Init
** ��������:   Can ģ���ʼ��
** �������:  
** ���ز���:  ��
******************************************************************************/
void Yx_Can_Init(void)
{
	Yx_Serial_Link_DispatcherRegister(CAN_REC_CMD,CAN_REC_CMD,_yx_can_rx_handle);
}

/*******************************************************************************
** ������:     Yx_Can_Send
** ��������:   Can ���ݷ���
** �������:  ch��ͨ����
**  		format����׼֡ or ��չ֡
**  		type������֡ or Զ��֡
**  		Canid:Can ID
**  		pdata ��������ָ��
**  		len �������ݳ���			(ֻ֧��8���ݳ��ȷ���)
** ���ز���:-1 : ͨ���Ŵ��� -2 ֡��ʽ���� -3 ֡���ʹ��� -4 ���ȴ���  0����ȷ
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
** ������:     Yx_Can_SetBaud
** ��������:   Can ����������
** �������:  ch��ͨ����
**  		baud ��������     500K  ��baud =  500000
** ���ز���:-1 : ͨ���Ŵ���   0����ȷ
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
** ������:     Yx_Can_RegisterRxCallBack
** ��������:   Can ����ע�ắ��
** �������:  pCb �� can �������ݻص�
** ���ز���:��
******************************************************************************/
void Yx_Can_RegisterRxCallBack(pCanRxFun pCb)
{	
	p_s_CanRxCallBack = pCb;
}

