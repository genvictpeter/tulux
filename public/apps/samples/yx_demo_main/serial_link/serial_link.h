/********************************************************************************
**
**  文件名称:  yx_serial_link.h
**  版权所有:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  文件描述:  串行通讯管理模块
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   | 修改记录
**===============================================================================
**| 2019-12-11 | 夏普   | 创建本模块
*********************************************************************************/

#ifndef _YX_SERIAL_LINK_H_
#define _YX_SERIAL_LINK_H_
#include "def.h"


#define YX_SERIAL_ITEM_NODE_USED_MAX            40

#define YX_SERIAL_LINK_RX_LEN_MAX               1024
#define YX_SERIAL_LINK_TX_LEN_MAX               1024

#define YX_SERIAL_LINK_HEAD_UP_1                0x55
#define YX_SERIAL_LINK_HEAD_UP_2                0x7A//0xEA

#define YX_SERIAL_LINK_HEAD_DOWN_1              0xAA
#define YX_SERIAL_LINK_HEAD_DOWN_2              0x75//0xE5


#define YX_SERIAL_LINK_REQ_CMD                  0x001
#define YX_SERIAL_LINK_BEAT_CMD                 0x002



#pragma pack(1)
typedef struct {
    INT8U  head[2];
    INT8U  crc;
    INT8U  dev_type;
    INT8U  cmd_h;
    INT8U  cmd_l;
    INT8U  len_h;
    INT8U  len_l;
} yx_sl_head_t;

typedef struct {
    yx_sl_head_t      Head;
    INT8U             Data[1];
} yx_sl_item_t;
#pragma pack()


typedef enum {
    YX_SL_EVENT_NONE,
    YX_SL_EVENT_CONNECT,
    YX_SL_EVENT_DISCONNECT,
    YX_SL_EVENT_RX_ERROR,                                                       /* 接受错误 */
} YX_SL_EVENT_T;



typedef enum {
    MSG_SERIAL_NONE,
    MSG_SERIAL_TX_DATA,
    MSG_SERIAL_RX_RCV,
    MSG_SERIAL_TIME_TX_WAIT_TIMEOUT,
    MSG_SERIAL_TIME_LINK_REQ,
    MSG_SERIAL_TIME_LINK_ACK_TIMEROUT,
    MSG_SERIAL_TIME_CONNECT_TIMEOUT,
    MSG_SERIAL_TIME_RXANA_TIMEOUT,
    MSG_SERIAL_TIME_REPEAT_TIMEOUT,
} YX_SERIAL_MSG_E;



typedef void (*pSerialEventFun)(INT8U evnet, void* parg);
typedef void (*pSerialRxFun)(INT8U* pbuf, int size);


/*******************************************************************************
** 函数名:     Yx_Serial_Link_Handle
** 功能描述:   链路层消息处理函数
** 输入参数:  eMsg 消息类型
** 返回参数:  无
******************************************************************************/
void Yx_Serial_Link_Handle(YX_SERIAL_MSG_E eMsg);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_TimerAnalyse
** 功能描述:   链路层定时器消息处理函数
** 输入参数:  timer_id 定时器ID
** 返回参数:  无
******************************************************************************/
BOOLEAN Yx_Serial_Link_TimerAnalyse(int timer_id);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Tx_Handle
** 功能描述:   链路层TX 链表发送消息处理函数
** 输入参数:  
** 返回参数:  无
******************************************************************************/
void Yx_Serial_Link_Tx_Handle(void);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Analyse
** 功能描述:   链路层RX 接收 数据解析处理
** 输入参数:  
** 返回参数:  无
******************************************************************************/
void Yx_Serial_Link_Analyse(void);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Init
** 功能描述:   链路层模块初始化
** 输入参数:  
** 返回参数:  无
******************************************************************************/
void Yx_Serial_Link_Init(void);
/*******************************************************************
** 函数名:     rm_comm_dispatcher_register
** 函数描述:   注册数据帧处理接口
** 参数:       [in]  b_cmd:         	接口可处理的起始数据帧类型
**           [in]  e_cmd:         接口可处理的结束数据帧类型
**           [in]  handler:       数据帧处理接口, 此接口有2个形参
**                                  第2形参: 用户数据指针
**                                  第3形参: 用户数据长度
** 返回:       TRUE:  注册成功
**           FALSE: 注册失败, 如与之前的注册接口在数据帧类型范围上出现重叠
********************************************************************/
BOOLEAN Yx_Serial_Link_DispatcherRegister(INT32U b_cmd, INT32U e_cmd, void (*handler)(const INT8U *, INT32U));


/*******************************************************************************
** 函数名:     Yx_Serial_Link_Init
** 功能描述:   链路层事件上报注册函数  事件类型YX_SL_EVENT_T
** 输入参数:  
** 返回参数:  无
******************************************************************************/
int Yx_Serial_Link_RegisterEventCb(pSerialEventFun event_cb);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Init
** 功能描述:   链路层接收处理函数注册
** 输入参数:  
** 返回参数:  无
******************************************************************************/
int Yx_Serial_Link_RegisterRxAnaCb(pSerialRxFun rx_cb);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Send
** 功能描述:   链路层发送数据接口 挂在发送链表发送
** 输入参数:  cmd： 命令
             pdata: 数据
             len  : 数据长度
** 返回参数:  FAIL    SUCC
******************************************************************************/
int Yx_Serial_Link_Send2List(BOOLEAN need_ack, int cmd, INT8U* pdata, int len);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Send
** 功能描述:   链路层发送数据接口
** 输入参数:  cmd： 命令
             pdata: 数据
             len  : 数据长度
** 返回参数:  FAIL    SUCC
******************************************************************************/
int Yx_Serial_Link_Send(int cmd, INT8U* pdata, int len);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Send
** 功能描述:   链路层发送ACK数据接口
** 输入参数:  cmd： 命令
             pdata: 数据
             len  : 数据长度
** 返回参数:  FAIL    SUCC
******************************************************************************/
int Yx_Serial_Link_Send(int cmd, INT8U* pdata, int len);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Reset
** 功能描述:   复位链路层
** 输入参数:  
** 返回参数:  FAIL    SUCC
******************************************************************************/
int Yx_Serial_Link_Reset(void);


/*******************************************************************************
** 函数名:     Yx_Serial_Link_Start
** 功能描述:   启动连接
** 输入参数:  
** 返回参数:  FAIL    SUCC
******************************************************************************/
int Yx_Serial_Link_Start(void);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Sleep
** 功能描述:   休眠
** 输入参数:  
** 返回参数:  FAIL    SUCC
******************************************************************************/
void Yx_Serial_Link_Sleep(void);

/*******************************************************************************
** 函数名:     Yx_Serial_Link_WakeUp
** 功能描述:   唤醒
** 输入参数:  
** 返回参数:  FAIL    SUCC
******************************************************************************/
void Yx_Serial_Link_WakeUp(void);

#endif



