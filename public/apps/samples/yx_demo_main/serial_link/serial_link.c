/********************************************************************************
**
**  文件名称:  yx_serial_link.c
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
#include "serial_link.h"
#include "def.h"
#include "yx_list.h"
#include "yx_chksum.h"
#include "yx_string.h"
#include "yx_fsm.h"
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "yx_spi_drv.h"

#if 0
#define LINK_LOG_TRACE(psFmz, args...)       do {printf("\n T:");printf(psFmz, ##args);}while(0)
#define LINK_LOG_WARNING(psFmz, args...)     do {printf("\n W:");printf(psFmz, ##args);}while(0)
#define LINK_LOG_ERROR(psFmz, args...)       do {printf("\n E:");printf(psFmz, ##args);}while(0)
#define LINK_LOG_BUF(psFmz, pBuf, Len)       do {  \
		int i;							\
        printf("\n%s:%d\n",psFmz,Len); 		\
        for (i = 0; i < (Len); ++i) {     \
            printf("%02x,", (pBuf)[i]); \
        }                               \
        printf("\n");\
    } while(0)
#define LINK_LOG_INFO(psFmz, args...)		do {printf("\n I:");printf(psFmz, ##args);}while(0)
#else 
#define LINK_LOG_TRACE(psFmz, args...)       
#define LINK_LOG_WARNING(psFmz, args...)     
#define LINK_LOG_ERROR(psFmz, args...)       	do {printf("\n E:");printf(psFmz, ##args);}while(0)
#define LINK_LOG_BUF(psFmz, pBuf, Len)        
#define LINK_LOG_INFO(psFmz, args...)			do {printf("\n I:");printf(psFmz, ##args);}while(0)
#endif

#define YX_SERIAL_LINK_DEVICE_TYPE              0x18


typedef enum {
    SERIAL_STATE_DISCONNCT,                                                     /* 断开连接 */
    SERIAL_STATE_CONNECT,                                                       /* 连接状态 */
} SerialFsm_State_t;

typedef enum {
    CONNECT_FLAG_DISCONNECT = 0,
    CONNECT_FLAG_WAIT,
    CONNECT_FLAG_CONNECT,
} CONNECT_FALG_TYPE_E;

typedef struct {
    Fsm super;
    FsmState Disconnect, Connect;
} serial_fsm_t;

typedef struct {
    yx_list_t   list;
    INT8U       node_attrib     :1;
    INT8U       node_repeat     :3;
    INT8U       node_time       :4;
    int      	node_len;
    INT8U*      databuf;
} serial_item_node_t;


typedef struct {
    yx_list_t       stFreeList;                                                 /* 空闲链表 */
    yx_list_t       stReSendList;                                               /* 重发链表 */
    yx_list_t       stWaitTxList;                                               /* 发送链表 */
    INT32U          LinkReqPeriod;
    INT8U           uConnectFlag;
    INT8U           uMsgFlag;
    int          	rx_len;
    INT8U           rx_buf[YX_SERIAL_LINK_RX_LEN_MAX + sizeof(yx_sl_head_t)];
    INT8U           tx_buf[YX_SERIAL_LINK_TX_LEN_MAX + sizeof(yx_sl_head_t)];
    pSerialEventFun event_cb;
    pSerialRxFun    rx_ana_cb;
} yx_serial_t;



#define MAX_REGISTER                    50      /* 允许最大的注册个数 */
/*
********************************************************************************
* 定义注册块数据结构
********************************************************************************
*/
typedef struct {
    INT16U       b_cmd;      /* 需处理的起始命令 */
    INT16U       e_cmd;      /* 需处理的结束命令 */
    void       (*handler)(const INT8U *, INT32U);
} rcb_t;

/*
********************************************************************************
* 定义模块局部变量
********************************************************************************
*/
static INT32U       s_registered = 0;
static rcb_t        s_rcb[MAX_REGISTER];




static serial_item_node_t   sgSerialItemNode[YX_SERIAL_ITEM_NODE_USED_MAX];
static yx_serial_t          sgYxSeiralDlObj;
static serial_fsm_t         sgSerialFsm;



static int sgYxMsgQueue;
#define    LINK_QUEUE_MSG_KEY         0x3030


#define  LINK_RX_FLAG    	0x01
#define  LINK_TX_FLAG    	0x02

#define _SERIAL_LINK_CONNECT_TIME           1000
#define _SERIAL_LINK_CONNECT_ACK_OUT_TIME   500
#define _SERIAL_LINK_CNNECT_TIME_OUT        6000000
#define _SERIAL_LINK_BEAT_TIME              30000
#define _SERIAL_LINK_BEAT_ACK_OUT_TIME      5000
#define _SERIAL_LINK_REQ_WAIT_TIME          100 

static void _yx_serial_link_fsmctor(serial_fsm_t *me);
static st_fm_msg_t* _yx_serial_link_disconnect_hndlr(st_fm_msg_t*);
static st_fm_msg_t* _yx_serial_link_connect_hndlr(st_fm_msg_t*);
static int _yx_serial_send_msg(INT32U MsgId,void* args);


static void _yx_serial_link_lock(void)
{
}

static void _yx_serial_link_unlock(void)
{
}

static inline void _yx_serial_repeat_timer_ctl(BOOLEAN bOn,INT32U time_ms)
{
	
}

static inline void _yx_serial_connect_timer_ctl(BOOLEAN bOn,INT32U time_ms)
{
	
}

static inline int _yx_serial_send_tx_req_msg(void)
{
    if (!(sgYxSeiralDlObj.uMsgFlag & LINK_TX_FLAG)) {
        sgYxSeiralDlObj.uMsgFlag |= LINK_TX_FLAG;
		_yx_serial_send_msg(MSG_SERIAL_TX_DATA,NULL);
        //Yx_Ps_SendPsMsg2Ps(MSG_ID_S_S_SERIAL_TX_DATA, NULL, 0);                 /* 向上层发送消息 发送数据 */
    }
    return SUCC;
}

static void  _yx_serial_link_rx_ind_cb(void* parg)
{
    if (!(sgYxSeiralDlObj.uMsgFlag & LINK_RX_FLAG)) {
        sgYxSeiralDlObj.uMsgFlag |= LINK_RX_FLAG;
		_yx_serial_send_msg(MSG_SERIAL_RX_RCV,NULL);
    }
    return ;
}
/*****************************硬件适配*****************************************/
static int _yx_serial_link_hw_init(void)
{
	yx_spi_drv_init();
    yx_spi_drv_register_rx_notice(_yx_serial_link_rx_ind_cb);
    return SUCC;
}

static inline int _yx_serial_link_hw_senddata(INT8U* pdata, int len)
{
    int ret;
	ret = yx_spi_drv_send(pdata,len);
	if (ret != len) {
		printf("hw send data err!!\n");
	}
    return  ret;
}

static inline int _yx_serial_link_hw_readdata(INT8U* pdata, int maxlen)
{
	return yx_spi_drv_read(pdata,maxlen);
}

static  int _yx_serial_link_hw_reset(void)
{
	return SUCC;
}


/******************************************************************************/


static inline void _yx_serial_link_event_cb(YX_SL_EVENT_T eEvent, void* parg)
{
    LINK_LOG_TRACE("<Serial Link>:Event: event =%d!!\n", eEvent);
    if (sgYxSeiralDlObj.event_cb != NULL) {
        sgYxSeiralDlObj.event_cb(eEvent, parg);
    }
}

static inline void _yx_serial_link_rx_ana_cb(INT8U *pdata, int len)
{
	INT8U i;
	yx_sl_item_t * pSlItem = (yx_sl_item_t *)pdata;
	INT16U cmd = yx_str_char2short_msb(&pSlItem->Head.cmd_h);

	for (i = 0; i < s_registered; i++) {										/* 根据类型查找对应的处理接口 */
		if (cmd >= s_rcb[i].b_cmd && cmd <= s_rcb[i].e_cmd) {	
			if (s_rcb[i].handler != NULL) {
				s_rcb[i].handler(pdata, len);
			}
			break;
		}
	}
	
    if (sgYxSeiralDlObj.rx_ana_cb != NULL) {
        sgYxSeiralDlObj.rx_ana_cb(pdata, len);
    }
}

static void _yx_serial_list_reset(void)
{
    INT8U i;
    serial_item_node_t* node;
    yx_list_t *tlist;
    
    tlist = sgYxSeiralDlObj.stWaitTxList.next;
    while (tlist != &(sgYxSeiralDlObj.stWaitTxList)) {
        node = YX_LIST_ENTRY(tlist, serial_item_node_t, list);
        tlist = tlist->next;
        yx_free(node->databuf);                                                 /* 释放内存 */
    }

    tlist = sgYxSeiralDlObj.stReSendList.next;

    while (tlist != &(sgYxSeiralDlObj.stReSendList)) {
        node = YX_LIST_ENTRY(tlist, serial_item_node_t, list);
        tlist = tlist->next;
        yx_free(node->databuf);                                                 /* 释放内存 */
    }

    yx_list_init(&sgYxSeiralDlObj.stFreeList);
    yx_list_init(&sgYxSeiralDlObj.stWaitTxList);
    yx_list_init(&sgYxSeiralDlObj.stReSendList);

    /* 初始化freelist */
    for (i = 0; i < YX_SERIAL_ITEM_NODE_USED_MAX; i++) {
        yx_list_add_before(&sgYxSeiralDlObj.stFreeList, &sgSerialItemNode[i].list);/* 插入到尾部 */
    }
}

static int _yx_serial_send_waittxlistnode(void)
{
    int sta = FAIL;
    serial_item_node_t* node;
    yx_list_t *tlist;
    
    if (sgYxSeiralDlObj.uConnectFlag != CONNECT_FLAG_CONNECT) {
        return FAIL;
    }
    
    tlist = sgYxSeiralDlObj.stWaitTxList.next;
    _yx_serial_link_lock();
    while (tlist != &(sgYxSeiralDlObj.stWaitTxList)) {
        node = YX_LIST_ENTRY(tlist, serial_item_node_t, list);
        tlist = tlist->next;
        if (_yx_serial_link_hw_senddata(node->databuf, node->node_len) > 0) {   /* 成功 */
            yx_list_del(&node->list);                                           /* 先从waittx list 中删除 */

            if (node->node_attrib == 1) {
                yx_list_add_before(&sgYxSeiralDlObj.stReSendList, &node->list); /* 添加到resendlist */
            } else {
                yx_free(node->databuf);                                         /* 释放内存 */
                yx_list_add_before(&sgYxSeiralDlObj.stFreeList, &node->list);   /* 添加到freelist */
            }

            sta = SUCC;
        } else {
            sta = FAIL;
            break;
        }
    }
    _yx_serial_link_unlock();

    if (!yx_list_empty(&sgYxSeiralDlObj.stReSendList)) {
        _yx_serial_repeat_timer_ctl(TRUE,1000);	/* 启动重发定时器 */
    }

    if (!yx_list_empty(&sgYxSeiralDlObj.stWaitTxList)) {
        _yx_serial_send_tx_req_msg();
    }
    return sta;
}


static int _yx_serial_del_repeatlistbycmd(int command)
{
    int sta = FAIL;
    yx_sl_item_t* pItem;
    serial_item_node_t* node;
    yx_list_t *tlist;
    int cmd;
    
    tlist = sgYxSeiralDlObj.stReSendList.next;
     
    while (tlist != &(sgYxSeiralDlObj.stReSendList)) {
        node = YX_LIST_ENTRY(tlist, serial_item_node_t, list);
        tlist = tlist->next;
        pItem = (yx_sl_item_t*)node->databuf;
        cmd = yx_str_char2short_msb(&pItem->Head.cmd_h);
        
        if (cmd == command) {
            yx_list_del(&node->list);                                           /* 先从waittx list 中删除 */
            yx_free(node->databuf);                                             /* 释放内存 */
            yx_list_add_before(&sgYxSeiralDlObj.stFreeList, &node->list);       /* 添加到Freelist */
            break;
        }
    }

    if (yx_list_empty(&sgYxSeiralDlObj.stReSendList)) {
        _yx_serial_repeat_timer_ctl(FALSE,0);
    }

    return sta;
}

static int _yx_serial_send_repeatlistnode(void)
{   
    int sta = FAIL;
    serial_item_node_t* node;
    yx_list_t *tlist;

    if (sgYxSeiralDlObj.uConnectFlag == CONNECT_FLAG_DISCONNECT) {
        return FAIL;
    }
    
    tlist = sgYxSeiralDlObj.stReSendList.next;
    _yx_serial_link_lock();
    while (tlist != &(sgYxSeiralDlObj.stReSendList)) {
        node = YX_LIST_ENTRY(tlist, serial_item_node_t, list);
        tlist = tlist->next;

        if (node->node_time != 0) {
            node->node_time--;
        }

        if (node->node_time == 0) {
            node->node_time = 3;

            if (_yx_serial_link_hw_senddata(node->databuf, node->node_len) > 0) {/* 成功 */
                sta = SUCC;
            }

            if (node->node_repeat != 0) {
                node->node_repeat--;
            }

            if (node->node_repeat == 0) {
                yx_list_del(&node->list);                                       /* 先从waittx list 中删除 */
                yx_list_add_before(&sgYxSeiralDlObj.stFreeList, &node->list);   /* 添加到Freelist */
            }
        }
    }
    _yx_serial_link_unlock();
    if (yx_list_empty(&sgYxSeiralDlObj.stReSendList)) {
        _yx_serial_repeat_timer_ctl(FALSE,0);
    }

    return sta;
}


static int _yx_serial_link_addlinkreq(void)
{
    int sta = FAIL;
    yx_sl_item_t* pItemCmd;
    
    _yx_serial_link_lock();
    pItemCmd = (yx_sl_item_t *)sgYxSeiralDlObj.tx_buf;
    pItemCmd->Head.head[0] = YX_SERIAL_LINK_HEAD_UP_1;
    pItemCmd->Head.head[1] = YX_SERIAL_LINK_HEAD_UP_2;
    pItemCmd->Head.dev_type = YX_SERIAL_LINK_DEVICE_TYPE;

    if (sgYxSeiralDlObj.uConnectFlag == CONNECT_FLAG_CONNECT) {                 /* 已连接 发送维护命令 */
        pItemCmd->Head.cmd_h = (YX_SERIAL_LINK_BEAT_CMD >> 8) & 0xFF;
        pItemCmd->Head.cmd_l = YX_SERIAL_LINK_BEAT_CMD & 0xFF;
        pItemCmd->Data[0]    = _SERIAL_LINK_BEAT_TIME / 1000;
    } else {
        pItemCmd->Head.cmd_h = (YX_SERIAL_LINK_REQ_CMD >> 8) & 0xFF;
        pItemCmd->Head.cmd_l = YX_SERIAL_LINK_REQ_CMD & 0xFF;
        pItemCmd->Data[0]    = _SERIAL_LINK_CONNECT_TIME / 1000;
    }
    
    pItemCmd->Head.len_h = 0x00;
    pItemCmd->Head.len_l = 0x01;
    pItemCmd->Head.crc = yx_chksum_getxor(&pItemCmd->Head.cmd_h, 5);
    
    
    if (_yx_serial_link_hw_senddata(sgYxSeiralDlObj.tx_buf, 1 + sizeof(yx_sl_head_t)) != 0) {
        sta = SUCC;
    }
    _yx_serial_link_unlock();

    return sta;
}

static int _yx_serial_link_addlinkack(INT16U cmd)
{
    int sta = FAIL;
    yx_sl_item_t* pItemCmd;
    
    _yx_serial_link_lock();
    pItemCmd = (yx_sl_item_t *)sgYxSeiralDlObj.tx_buf;
    pItemCmd->Head.head[0] = YX_SERIAL_LINK_HEAD_DOWN_1;
    pItemCmd->Head.head[1] = YX_SERIAL_LINK_HEAD_DOWN_2;
    pItemCmd->Head.dev_type = YX_SERIAL_LINK_DEVICE_TYPE;

    pItemCmd->Head.cmd_h = (cmd >> 8) & 0xFF;
    pItemCmd->Head.cmd_l = cmd & 0xFF;
    pItemCmd->Data[0]    = 0x01;
    
    pItemCmd->Head.len_h = 0x00;
    pItemCmd->Head.len_l = 0x01;
    pItemCmd->Head.crc = yx_chksum_getxor(&pItemCmd->Head.cmd_h, 5);
    
    if (_yx_serial_link_hw_senddata(sgYxSeiralDlObj.tx_buf, 1 + sizeof(yx_sl_head_t)) != 0) {
        sta = SUCC;
    }
    _yx_serial_link_unlock();

    return sta;
}



static inline int _yx_serial_link_findframehead(INT8U *pdata, int datalen)
{
    int offset = 0;
    
    for (offset = 0; offset < datalen - 1; offset++) {
        if (pdata[offset] == YX_SERIAL_LINK_HEAD_UP_1 && pdata[offset + 1] == YX_SERIAL_LINK_HEAD_UP_2) {
            return offset;
        }
    }

    if (offset == datalen - 1 && pdata[datalen - 1] == YX_SERIAL_LINK_HEAD_UP_1) {
        return datalen - 1;
    }

    return datalen;
}

static int _yx_serial_link_rxrec(INT8U* pbuf, int datalen)
{
    int cmd;
    INT8U* pTemp = pbuf;
    int offset = 0;
    int size = 0;
    yx_sl_head_t* pDlHead;
    
    if (datalen < sizeof(yx_sl_head_t)) {
        return 0;
    }
    
_ANALYSE:
    pTemp = pbuf + offset;
    
    offset += _yx_serial_link_findframehead(pTemp, datalen - offset);

    if ((datalen - offset) < sizeof(yx_sl_head_t)) {
        return offset;
    } else {
        pDlHead = (yx_sl_head_t*)(pbuf + offset);
        size = ((pDlHead->len_h << 8) + pDlHead->len_l);

        if ((datalen - offset) >= (size + sizeof(yx_sl_head_t))) {
            if ((pDlHead->crc == yx_chksum_getxor(&pDlHead->cmd_h, size + 4))) {
                offset += (size + sizeof(yx_sl_head_t));
                cmd = yx_str_char2short_msb(&pDlHead->cmd_h);
                _yx_serial_del_repeatlistbycmd(cmd);
				LINK_LOG_TRACE("<serial Link>: rxcmd  0x%x",cmd);
                if (cmd == YX_SERIAL_LINK_REQ_CMD ) {
					sgYxSeiralDlObj.LinkReqPeriod = ((yx_sl_item_t*)pDlHead)->Data[0];
					sgYxSeiralDlObj.LinkReqPeriod *=1000;
					_yx_serial_link_addlinkack(cmd);
                    if (sgYxSeiralDlObj.uConnectFlag != CONNECT_FLAG_CONNECT) {
                        sgYxSeiralDlObj.uConnectFlag = CONNECT_FLAG_CONNECT;
                    } else {
						_yx_serial_connect_timer_ctl(TRUE,5000);
					}
                    
                } else if (cmd == YX_SERIAL_LINK_BEAT_CMD) {
	                _yx_serial_link_addlinkack(cmd);
					_yx_serial_connect_timer_ctl(TRUE,5000);

                } else {
                    _yx_serial_link_rx_ana_cb((INT8U*)pDlHead, size + sizeof(yx_sl_head_t));
                }

                if (offset < datalen) {
                    goto _ANALYSE;
                }
            } else {
                LINK_LOG_ERROR("<Serial Link>:Crc Err!!");
                offset += 2;
                goto _ANALYSE;

            }
        } else if (datalen > YX_SERIAL_LINK_RX_LEN_MAX) {
            LINK_LOG_ERROR("<Serial Link>:Rx Len Over!!");
            offset += 2;
            goto _ANALYSE;
        } else {
            return offset;
        }
    }
    
    return offset;
}


static void _yx_serial_link_rx_analyse(void)
{
    int rx_len = 0;
    int rx_offset = 0;
    
    while ((rx_len =_yx_serial_link_hw_readdata(&sgYxSeiralDlObj.rx_buf[sgYxSeiralDlObj.rx_len],
                YX_SERIAL_LINK_RX_LEN_MAX - sgYxSeiralDlObj.rx_len)) != 0) {
        //YX_OS_TimerStop(sgYxSeiralDlObj.LinkRxAnaOutTimer);
        LINK_LOG_BUF("<Serial Link>: Rx:", &sgYxSeiralDlObj.rx_buf[sgYxSeiralDlObj.rx_len], rx_len);
        sgYxSeiralDlObj.rx_len += rx_len;
        rx_offset = _yx_serial_link_rxrec(sgYxSeiralDlObj.rx_buf, sgYxSeiralDlObj.rx_len);
        sgYxSeiralDlObj.rx_len -= rx_offset;
        if (sgYxSeiralDlObj.rx_len != 0) {
            //YX_OS_TimerStartup(sgYxSeiralDlObj.LinkRxAnaOutTimer, 500);       /* 处理断帧时  是否启动接收超时定时器 */
            memmove(sgYxSeiralDlObj.rx_buf, &sgYxSeiralDlObj.rx_buf[rx_offset], sgYxSeiralDlObj.rx_len);
        }
    }
}

static void _yx_serial_link_rx_reset(void)
{
    if (sgYxSeiralDlObj.rx_len != 0) {
        sgYxSeiralDlObj.rx_len = 0;
    }
}


static void _yx_serial_link_fsmctor(serial_fsm_t *me)
{
    FsmCtor((Fsm *)me, "commdl", &me->Disconnect);
    FsmStateCtor(&me->Disconnect, "dis", (EvtHndlr)_yx_serial_link_disconnect_hndlr);
    FsmStateCtor(&me->Connect, "con", (EvtHndlr)_yx_serial_link_connect_hndlr);
}



static void _yx_serial_link_statetrans(SerialFsm_State_t TargetState)
{
    switch(TargetState)
    {
    case SERIAL_STATE_DISCONNCT:
        FSM_STATE_TRAN((Fsm*)(&sgSerialFsm), &(sgSerialFsm.Disconnect));
        break;
    case SERIAL_STATE_CONNECT:
        FSM_STATE_TRAN((Fsm*)(&sgSerialFsm), &(sgSerialFsm.Connect));
        break;
    default:
        break;
    }
}

/*断开连接状态*/
static st_fm_msg_t* _yx_serial_link_disconnect_hndlr(st_fm_msg_t* pMsg)
{
    switch(pMsg->MsgID)
    {
    case MSG_STATE_ENTRY:
        /* clear resend list */
        /* clear ack list */
        _yx_serial_connect_timer_ctl(FALSE,0);
		_yx_serial_repeat_timer_ctl(FALSE,0);
        return 0;
    case MSG_STATE_EXIT:
        sgYxSeiralDlObj.uConnectFlag = CONNECT_FLAG_CONNECT;
        _yx_serial_link_event_cb(YX_SL_EVENT_CONNECT, NULL);                    /* 通知上层链路连接 */
        return 0;
    case MSG_SERIAL_RX_RCV:
        /* 接收 */
        _yx_serial_link_rx_analyse();
        
        if (sgYxSeiralDlObj.uConnectFlag == CONNECT_FLAG_CONNECT) {
            _yx_serial_link_statetrans(SERIAL_STATE_CONNECT);
        }
        return 0;
    case MSG_SERIAL_TIME_LINK_REQ:
//        if (sgYxSeiralDlObj.uConnectFlag != CONNECT_FLAG_DISCONNECT) {          /* 已断开连接  不再发送请求 需要应用层启动连接 */
//            if (_yx_serial_link_addlinkreq() == SUCC) {
//            } else {
//            }
//        }
        return 0;
    case MSG_SERIAL_TIME_TX_WAIT_TIMEOUT:
//        if (_yx_serial_link_addlinkreq() == SUCC) {
//        } else {
//        }
        return 0;
    case MSG_SERIAL_TIME_CONNECT_TIMEOUT:                                       /* 60s 超时 */
        sgYxSeiralDlObj.uConnectFlag = CONNECT_FLAG_DISCONNECT;
        _yx_serial_link_event_cb(YX_SL_EVENT_DISCONNECT, NULL);
        return 0;
    case MSG_SERIAL_TIME_RXANA_TIMEOUT:
        _yx_serial_link_rx_reset();
        return 0;
    case MSG_SERIAL_TX_DATA:
        return 0;
    default:
        return 0;
    }
}


static st_fm_msg_t* _yx_serial_link_connect_hndlr(st_fm_msg_t* pMsg)
{
    switch(pMsg->MsgID)
    {
    case MSG_STATE_ENTRY:
        /* 发送心跳维护数据 */
        sgYxSeiralDlObj.LinkReqPeriod = _SERIAL_LINK_BEAT_TIME;                 /* 30s 发送一次 */
		_yx_serial_connect_timer_ctl(TRUE,50000);
        if (!yx_list_empty(&sgYxSeiralDlObj.stWaitTxList)) {
            _yx_serial_send_tx_req_msg();
        }

        if (!yx_list_empty(&sgYxSeiralDlObj.stReSendList)) {
			_yx_serial_repeat_timer_ctl(TRUE,1000);
        }
        return 0;
    case MSG_STATE_EXIT:
		_yx_serial_connect_timer_ctl(FALSE,0);
		_yx_serial_repeat_timer_ctl(FALSE,0);
        //_yx_serial_link_hw_reset();                                           /* 硬件复位 */
        _yx_serial_link_event_cb(YX_SL_EVENT_DISCONNECT, NULL);                 /* 通知上层断开连接 */
        sgYxSeiralDlObj.uConnectFlag = CONNECT_FLAG_WAIT;
        return 0;
    case MSG_SERIAL_RX_RCV:
        _yx_serial_link_rx_analyse();
        return 0;
    case MSG_SERIAL_TIME_LINK_ACK_TIMEROUT:
        _yx_serial_link_statetrans(SERIAL_STATE_DISCONNCT);
        return 0;
    case MSG_SERIAL_TIME_LINK_REQ:
//        if (_yx_serial_link_addlinkreq() == SUCC) {
//        } else {
//        }
        return 0;
    case MSG_SERIAL_TIME_TX_WAIT_TIMEOUT:
//        if (_yx_serial_link_addlinkreq() == SUCC) {
//        } else {
//        }
        return 0;
    case MSG_SERIAL_TIME_RXANA_TIMEOUT:
        _yx_serial_link_rx_reset();
        return 0;
    case MSG_SERIAL_TIME_REPEAT_TIMEOUT:
        _yx_serial_send_repeatlistnode();
        return 0;
    case MSG_SERIAL_TX_DATA:
        _yx_serial_send_waittxlistnode();
        return 0;
    default:
        break;
    } 
    return pMsg;
}


/*******************************************************************************
** 函数名:     Yx_Serial_Link_Handle
** 功能描述:   链路层消息处理函数
** 输入参数:  eMsg 消息类型
** 返回参数:  无
******************************************************************************/
void Yx_Serial_Link_Handle(YX_SERIAL_MSG_E eMsg)
{
    st_fm_msg_t Msg;
    
    Msg.MsgID = eMsg;
    Msg.PtrMsgSt = NULL;
    FsmOnEvent((Fsm*)&sgSerialFsm, &Msg);
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_TimerAnalyse
** 功能描述:   链路层定时器消息处理函数
** 输入参数:  timer_id 定时器ID
** 返回参数:  无
******************************************************************************/
BOOLEAN Yx_Serial_Link_TimerAnalyse(int timer_id)
{   
    YX_SERIAL_MSG_E eMsg = MSG_SERIAL_NONE;

    if (eMsg != MSG_SERIAL_NONE) {
        Yx_Serial_Link_Handle(eMsg);
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Tx_Handle
** 功能描述:   链路层TX 链表发送消息处理函数
** 输入参数:  
** 返回参数:  无
******************************************************************************/
void Yx_Serial_Link_Tx_Handle(void)
{
    sgYxSeiralDlObj.uMsgFlag &= ~LINK_TX_FLAG;
    Yx_Serial_Link_Handle(MSG_SERIAL_TX_DATA);
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Analyse
** 功能描述:   链路层RX 接收 数据解析处理
** 输入参数:  
** 返回参数:  无
******************************************************************************/
void Yx_Serial_Link_Analyse(void)
{
    sgYxSeiralDlObj.uMsgFlag &= ~LINK_RX_FLAG;
    Yx_Serial_Link_Handle(MSG_SERIAL_RX_RCV);
}

typedef struct 
{
	long type;
	st_yx_msg_t myMsg;
}YxSerialMsg_T;

static int _yx_serial_send_msg(INT32U MsgId,void* args)
{
	int ret;
	YxSerialMsg_T sgMsg;
	sgMsg.type = 0x01;
	sgMsg.myMsg.msgid = MsgId;
	sgMsg.myMsg.PtrMsgSt = args;

    ret = msgsnd(sgYxMsgQueue, (struct msg_buf*)&sgMsg, sizeof(st_yx_msg_t), 0);
    if (ret == -1) {
        printf("msg_queue_send error\n");
        return -1;
    }
}







INT32S Yx_Serial_Link_Process(void* args)
{
    int ret;
    st_yx_msg_t* sgpMsg;
	YxSerialMsg_T sgMsg;
	//sgMsg.type = 0x01;
    ret = msgrcv(sgYxMsgQueue, (struct msg_buf*)&sgMsg, sizeof(st_yx_msg_t), 1, 0);
	if (ret == -1) {
		printf("serial rx msg error\n");
	} else {
		sgpMsg = &sgMsg.myMsg;
		switch (sgpMsg->msgid)
		{
		case MSG_SERIAL_TX_DATA:
			Yx_Serial_Link_Tx_Handle();
			break;
		case MSG_SERIAL_RX_RCV:
			Yx_Serial_Link_Analyse();
			break;
		default:
			break;
		}

		if (sgpMsg->PtrMsgSt != NULL) {
			yx_free(sgpMsg->PtrMsgSt);
		}
	}
	return ret;
}



/*******************************************************************
** 函数名:     rm_comm_dispatcher_register
** 函数描述:   注册数据帧处理接口
** 参数:       [in]  btype:         	接口可处理的起始数据帧类型
**           [in]  etype:         接口可处理的结束数据帧类型
**           [in]  handler:       数据帧处理接口, 此接口有3个形参
**                                  第1形参: 数据帧类型
**                                  第2形参: 用户数据指针
**                                  第3形参: 用户数据长度
**                                  返回值:  true--数据帧得到处理, false--数据帧未得到处理
** 返回:       true:  注册成功
**             false: 注册失败, 如与之前的注册接口在数据帧类型范围上出现重叠
********************************************************************/
BOOLEAN Yx_Serial_Link_DispatcherRegister(INT32U b_cmd, INT32U e_cmd, void (*handler)(const INT8U *, INT32U))
{
    s_rcb[s_registered].b_cmd   = b_cmd;
    s_rcb[s_registered].e_cmd   = e_cmd;
    s_rcb[s_registered].handler = handler;
    s_registered++;
    return TRUE;
}


/*******************************************************************************
** 函数名:     _yx_serial_link_thread
** 功能描述:   链路层模块接收线程
** 输入参数:  
** 返回参数:  无
******************************************************************************/
void _yx_serial_link_thread(void* arg)
{	
	pthread_detach(pthread_self());
	
	while (1) {
		Yx_Serial_Link_Process(NULL);
	}
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Init
** 功能描述:   链路层模块初始化
** 输入参数:  
** 返回参数:  无
******************************************************************************/
void Yx_Serial_Link_Init(void)
{
    INT8U i;
	int ret;
    pthread_t pid;
	sgYxMsgQueue = msgget((key_t)(LINK_QUEUE_MSG_KEY + i), IPC_CREAT);		//创建消息队列
    if (sgYxMsgQueue == -1) {
        LINK_LOG_ERROR("<serial msg get error!!>\n");
    }
    /* 初始化状态机 */
    yx_list_init(&sgYxSeiralDlObj.stFreeList);
    yx_list_init(&sgYxSeiralDlObj.stWaitTxList);
    yx_list_init(&sgYxSeiralDlObj.stReSendList);

    /* 初始化freelist */
    for (i = 0; i < YX_SERIAL_ITEM_NODE_USED_MAX; i++) {
        yx_list_add_before(&sgYxSeiralDlObj.stFreeList, &sgSerialItemNode[i].list); /* 插入到尾部 */
    }
    _yx_serial_link_hw_init();
    _yx_serial_link_fsmctor(&sgSerialFsm);
    sgYxSeiralDlObj.uConnectFlag  = CONNECT_FLAG_DISCONNECT;
	
    ret = pthread_create(&pid, NULL, _yx_serial_link_thread, NULL);
	if (ret < 0){
		printf("create serial link thread error %d\n!!",ret);
	}
	
	Yx_Serial_Link_Send(0x01, NULL,0);				/* 上电指示*/
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Init
** 功能描述:   链路层事件上报注册函数  事件类型YX_SL_EVENT_T
** 输入参数:  
** 返回参数:  无
******************************************************************************/
int Yx_Serial_Link_RegisterEventCb(pSerialEventFun event_cb)
{
    sgYxSeiralDlObj.event_cb = event_cb;
    return SUCC;
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Init
** 功能描述:   链路层接收处理函数注册
** 输入参数:  
** 返回参数:  无
******************************************************************************/
int Yx_Serial_Link_RegisterRxAnaCb(pSerialRxFun rx_cb)
{
    sgYxSeiralDlObj.rx_ana_cb = rx_cb;
    return SUCC;
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Send
** 功能描述:   链路层发送数据接口 挂在发送链表发送
** 输入参数:  cmd： 命令
             pdata: 数据
             len  : 数据长度
** 返回参数:  FAIL    SUCC
******************************************************************************/
int Yx_Serial_Link_Send2List(BOOLEAN need_ack, int cmd, INT8U* pdata, int len)
{
    int sta = FAIL;
    serial_item_node_t* node;
    yx_sl_item_t * pItemCmd;

    /* 链路未连接 */
    if (sgYxSeiralDlObj.uConnectFlag != CONNECT_FLAG_CONNECT) {
        return FAIL;
    }
    if (yx_list_empty(&sgYxSeiralDlObj.stFreeList)) {
        LINK_LOG_ERROR("<Serial Link>: FreeList Empty!!");
        return sta;
    }

    _yx_serial_link_lock();
    pItemCmd = (yx_sl_item_t*)yx_malloc(len + sizeof(yx_sl_head_t));
    if (pItemCmd != NULL) {
        node = YX_LIST_FIRST_ENTRY(&sgYxSeiralDlObj.stFreeList, serial_item_node_t, list);
        yx_list_del(&node->list);
        pItemCmd->Head.head[0]  =  YX_SERIAL_LINK_HEAD_DOWN_1;
        pItemCmd->Head.head[1]  =  YX_SERIAL_LINK_HEAD_DOWN_2;
        pItemCmd->Head.dev_type =  YX_SERIAL_LINK_DEVICE_TYPE;
        pItemCmd->Head.cmd_h    =  ((cmd >> 8) & 0xFF);
        pItemCmd->Head.cmd_l    =  (cmd & 0xFF);
        pItemCmd->Head.len_h    =  ((len >> 8) & 0xFF);
        pItemCmd->Head.len_l    =  (len & 0xFF);

        if (len != 0 && len <= YX_SERIAL_LINK_TX_LEN_MAX && pdata != NULL) {
            memcpy(pItemCmd->Data, pdata, len);
        }
        
        pItemCmd->Head.crc =  yx_chksum_getxor(&pItemCmd->Head.cmd_h, len + 4);

        node->node_attrib = need_ack;
        node->node_repeat = 3;
        node->node_time   = 3;
        node->node_len    = len + sizeof(yx_sl_head_t);
        node->databuf     = (INT8U*)pItemCmd;
        yx_list_add_before(&sgYxSeiralDlObj.stWaitTxList, &node->list);
        _yx_serial_send_tx_req_msg();
        sta = SUCC;
    } else {
        LINK_LOG_ERROR("<Serial Link>: Ram Not Enough!!");
    }
    
    _yx_serial_link_unlock();
        
    return sta;
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Send
** 功能描述:   链路层发送数据接口
** 输入参数:  cmd： 命令
             pdata: 数据
             len  : 数据长度
** 返回参数:  FAIL    SUCC
******************************************************************************/
int Yx_Serial_Link_Send(int cmd, INT8U* pdata, int len)
{
    int sta = FAIL;
    serial_item_node_t* node;
    yx_sl_item_t * pItemCmd;
    
    /* 链路未连接 */
    _yx_serial_link_lock();
    pItemCmd = (yx_sl_item_t*)sgYxSeiralDlObj.tx_buf;
    pItemCmd->Head.head[0]  =  YX_SERIAL_LINK_HEAD_DOWN_1;
    pItemCmd->Head.head[1]  =  YX_SERIAL_LINK_HEAD_DOWN_2;
    pItemCmd->Head.dev_type =  YX_SERIAL_LINK_DEVICE_TYPE;
    pItemCmd->Head.cmd_h    =  ((cmd >> 8) & 0xFF);
    pItemCmd->Head.cmd_l    =  (cmd & 0xFF);
    pItemCmd->Head.len_h    =  ((len >> 8) & 0xFF);
    pItemCmd->Head.len_l    =  (len & 0xFF);

    if (len != 0 && len <= YX_SERIAL_LINK_TX_LEN_MAX && pdata != NULL) {
        memcpy(pItemCmd->Data, pdata, len);
    }
    
    pItemCmd->Head.crc = yx_chksum_getxor(&pItemCmd->Head.cmd_h, len + 4);

	
	if (1) {//(sgYxSeiralDlObj.uConnectFlag == CONNECT_FLAG_CONNECT || cmd == 0x21) {
		if (_yx_serial_link_hw_senddata(sgYxSeiralDlObj.tx_buf, len + sizeof(yx_sl_head_t)) != 0) {
			sta = SUCC;
		}
	} else {
		sta = FAIL;
	}
    
    if (sta != SUCC) {                                                          /* 失败之后  挂在链表重新发送 */
        if (yx_list_empty(&sgYxSeiralDlObj.stFreeList)) {
            LINK_LOG_ERROR("<Serial Link>: FreeList Empty!!");
        } else  {
            pItemCmd = (yx_sl_item_t*)yx_malloc(len + sizeof(yx_sl_head_t));
            if (pItemCmd != NULL) {
                node = YX_LIST_FIRST_ENTRY(&sgYxSeiralDlObj.stFreeList, serial_item_node_t, list);
                yx_list_del(&node->list);
                memcpy(pItemCmd,sgYxSeiralDlObj.tx_buf, len + sizeof(yx_sl_head_t));
                node->node_attrib = 0;
                node->node_repeat = 1;
                node->node_time   = 1;
                node->node_len    = len + sizeof(yx_sl_head_t);
                node->databuf     = (INT8U*)pItemCmd;
                yx_list_add_before(&sgYxSeiralDlObj.stWaitTxList, &node->list);
                _yx_serial_send_tx_req_msg();
                sta = SUCC;
            } else {
                LINK_LOG_ERROR("<Serial Link>: Ram Not Enough!!");
            }
        }
    }
    
    _yx_serial_link_unlock();
        
    return sta;
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Reset
** 功能描述:   复位链路层
** 输入参数:  
** 返回参数:  FAIL    SUCC
******************************************************************************/
int Yx_Serial_Link_Reset(void)
{
    LINK_LOG_TRACE("<Serial Link>: Reset!!");
    
    _yx_serial_list_reset();
    sgYxSeiralDlObj.uConnectFlag = CONNECT_FLAG_DISCONNECT;
    /* 初始化状态机 */
    _yx_serial_link_hw_reset();
    _yx_serial_link_fsmctor(&sgSerialFsm);
    FsmOnStart((Fsm*)&sgSerialFsm);

    return SUCC;
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Start
** 功能描述:   启动连接
** 输入参数:  
** 返回参数:  FAIL    SUCC
******************************************************************************/
int Yx_Serial_Link_Start(void)
{
    if (FSM_STATE_CURR((Fsm*)&sgSerialFsm) != &sgSerialFsm.Disconnect) {
        return FAIL;
    }
    _yx_serial_repeat_timer_ctl(FALSE,0);
	_yx_serial_connect_timer_ctl(FALSE,0);
    LINK_LOG_TRACE("<Serial Link>: Start!!");
    sgYxSeiralDlObj.uConnectFlag = CONNECT_FLAG_DISCONNECT;
    FsmOnStart((Fsm*)&sgSerialFsm);
    return SUCC;
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_Sleep
** 功能描述:   休眠
** 输入参数:  
** 返回参数:  FAIL    SUCC
******************************************************************************/
void Yx_Serial_Link_Sleep(void)
{
    LINK_LOG_TRACE("<Serial Link>: Sleep!! %d");
    _yx_serial_repeat_timer_ctl(FALSE,0);
	_yx_serial_connect_timer_ctl(FALSE,0);
    sgYxSeiralDlObj.uConnectFlag = CONNECT_FLAG_DISCONNECT;
    _yx_serial_list_reset();
    /* 初始化状态机 */
    _yx_serial_link_fsmctor(&sgSerialFsm);
}

/*******************************************************************************
** 函数名:     Yx_Serial_Link_WakeUp
** 功能描述:   唤醒
** 输入参数:  
** 返回参数:  FAIL    SUCC
******************************************************************************/
void Yx_Serial_Link_WakeUp(void)
{
    Yx_Serial_Link_Start();
}


