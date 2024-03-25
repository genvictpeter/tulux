/********************************************************************************
**
**  文件名称:  yx_fsm.h
**  版权所有:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  文件描述:  状态机工具模块
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   | 修改记录
**===============================================================================
**| 2019-12-11 | 夏普   | 创建本模块
*********************************************************************************/

#ifndef _YX_FSM_H_
#define _YX_FSM_H_

#include "def.h"


typedef struct {
    INT32U MsgID;
    void  *PtrMsgSt;
} st_fm_msg_t;

typedef st_fm_msg_t* (*EvtHndlr)(st_fm_msg_t*);


/*------------------------------- MACROS & CONSTANTS--------------------------*/

#define MSG_HSM_START        0xFFE1
#define MSG_STATE_ENTRY      0xFFE2
#define MSG_STATE_EXIT       0xFFE3

/*------------------------ FUNCTION PROTOTYPE(S) -----------------------------*/


#define StateOnEvent(me_, msg_) (*(me_)->hndlr)(msg_)


struct FsmState {
    EvtHndlr hndlr;                                                             /* state's handler function */
    char const *name;
};
typedef struct FsmState FsmState;

void FsmStateCtor(FsmState *me, char const *name, EvtHndlr hndlr);

typedef struct Fsm Fsm;
struct Fsm {                                                                    /* State Machine base class */
    char const *name;                                                           /* pointer to static name */
    FsmState *curr;                                                             /* current state */
    FsmState *next;                                                             /* next state (non 0 if transition taken) */
    st_fm_msg_t entryMsg;
    st_fm_msg_t exitMsg;
};

/*******************************************************************
** 函数名:     FsmCtor
** 功能描述:   状态机实体初始化
** 参数:       [in]  me:            状态机实体指针
**             [in]  name:          状态机名称
**             [in]  initState:     状态机初始状态
** 返回:       无
********************************************************************/
void FsmCtor(Fsm *me, char const *name, FsmState* initState);

/*******************************************************************
** 函数名:     FsmStateCtor
** 功能描述:   注册各个阶段状态处理函数
** 参数:       [in]  me:            状态机实体指针
**             [in]  name:          子状态名称
**             [in]  hndlr:         回调函数
** 返回:       无
********************************************************************/
void FsmStateCtor(FsmState *me, char const *name, EvtHndlr hndlr);

/*******************************************************************
** 函数名:     FsmOnStart
** 功能描述:   状态机进入初始状态
** 参数:       [in]  me:            状态机实体指针
** 返回:       无
********************************************************************/
void FsmOnStart(Fsm *me);
/*******************************************************************
** 函数名:     FsmOnEvent
** 功能描述:   当前状态下处理消息
** 参数:       [in]  me:            状态机实体指针
**             [in]  msg:           消息指针
** 返回:       无
********************************************************************/
BOOLEAN FsmOnEvent(Fsm *me, st_fm_msg_t* msg);

/*******************************************************************
** 函数名:     FsmExit_
** 功能描述:   离开当前状态时执行回调函数
** 参数:       无
** 返回:       无
********************************************************************/
void FsmExit_(Fsm *me) ;

/* 获取当前状态 */
#define FSM_STATE_CURR(me_)  (((Fsm *)me_)->curr)


/* 切换状态 */
#define FSM_STATE_TRAN(me_, target_)   if (1) { \
                                           FsmExit_((Fsm *)(me_));\
                                           ((Fsm *)(me_))->next = (target_);\
                                       } else ((void)0)

#define START_EVT (-1)
#define ENTRY_EVT (-2)
#define EXIT_EVT  (-3)



#endif    /* _FSM_H_ */

