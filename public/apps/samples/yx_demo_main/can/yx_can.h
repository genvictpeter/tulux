

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
** ������:     Yx_Can_Init
** ��������:   Can ģ���ʼ��
** �������:  
** ���ز���:  ��
******************************************************************************/
void Yx_Can_Init(void);

/*******************************************************************************
** ������:     Yx_Can_Send
** ��������:   Can ���ݷ���
** �������:  ch��ͨ����
**  		format����׼֡ or ��չ֡
**  		type������֡ or Զ��֡
** 			Canid : can id
**  		pdata ��������ָ��
**  		len �������ݳ���			(ֻ֧��8���ݳ��ȷ���)
** ���ز���:-1 : ͨ���Ŵ��� -2 ֡��ʽ���� -3 ֡���ʹ��� -4 ���ȴ���  0����ȷ
******************************************************************************/
INT32S Yx_Can_Send(CAN_CHANNEL_E ch, CAN_FORMAT_E format,CAN_FRAME_TYPE_E type,INT32U Canid,INT8U* pdata,INT8U len);

/*******************************************************************************
** ������:     Yx_Can_SetBaud
** ��������:   Can ����������
** �������:  ch��ͨ����
**  		baud ��������     500K  ��baud =  500000
** ���ز���:-1 : ͨ���Ŵ���   0����ȷ
******************************************************************************/
INT32S Yx_Can_SetBaud(CAN_CHANNEL_E ch,INT32U baud);

/*******************************************************************************
** ������:     Yx_Can_RegisterRxCallBack
** ��������:   Can ����ע�ắ��
** �������:  pCb �� can �������ݻص�
** ���ز���:��
******************************************************************************/
void Yx_Can_RegisterRxCallBack(pCanRxFun pCb);


#endif
