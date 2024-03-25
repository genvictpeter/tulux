/********************************************************************************
**
**  文件名称:  yx_fsm.c
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

#include "yx_fsm.h"

/* Fsm ctor */
/*******************************************************************
** 函数名:     FsmCtor
** 功能描述:   状态机实体初始化
** 参数:       [in]  me:            状态机实体指针
**             [in]  name:          状态机名称
**             [in]  initState:     状态机初始状态
** 返回:       无
********************************************************************/
void FsmCtor(Fsm *me, char const *name, FsmState* initState) 
{
    me->name = name;
    me->curr = initState;
    me->next = NULL;
    me->entryMsg.MsgID = MSG_STATE_ENTRY;
    me->exitMsg.MsgID = MSG_STATE_EXIT;
}



/*******************************************************************
** 函数名:     FsmStateCtor
** 功能描述:   注册各个阶段状态处理函数
** 参数:       [in]  me:            状态机实体指针
**             [in]  name:          子状态名称
**             [in]  hndlr:         回调函数
** 返回:       无
********************************************************************/
void FsmStateCtor(FsmState *me, char const *name, EvtHndlr hndlr)
{   
    me->name = name;
    me->hndlr = hndlr;
}


/*******************************************************************
** 函数名:     FsmOnStart
** 功能描述:   状态机进入初始状态
** 参数:       [in]  me:            状态机实体指针
** 返回:       无
********************************************************************/
void FsmOnStart(Fsm *me) 
{
    StateOnEvent(me->curr, &(me->entryMsg));
}

/*******************************************************************
** 函数名:     FsmOnEvent
** 功能描述:   当前状态下处理消息
** 参数:       [in]  me:            状态机实体指针
**             [in]  msg:           消息指针
** 返回:       无
********************************************************************/
BOOLEAN FsmOnEvent(Fsm *me, st_fm_msg_t* msg) 
{
    if (StateOnEvent(me->curr, msg) == 0) {
        if (me->next) {                                                         /* state transition taken? */
            StateOnEvent(me->next, &(me->entryMsg));
            me->curr = me->next;
            me->next = 0;
        }
        return TRUE;
    }
    return FALSE;
}



/*******************************************************************
** 函数名:     FsmExit_
** 功能描述:   离开当前状态时执行回调函数
** 参数:       无
** 返回:       无
********************************************************************/
void FsmExit_(Fsm *me) 
{
    StateOnEvent(me->curr, &(me->exitMsg));
}



