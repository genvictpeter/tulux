/********************************************************************************
**
**  �ļ�����:  yx_fsm.h
**  ��Ȩ����:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  �ļ�����:  ״̬������ģ��
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   | �޸ļ�¼
**===============================================================================
**| 2019-12-11 | ����   | ������ģ��
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
** ������:     FsmCtor
** ��������:   ״̬��ʵ���ʼ��
** ����:       [in]  me:            ״̬��ʵ��ָ��
**             [in]  name:          ״̬������
**             [in]  initState:     ״̬����ʼ״̬
** ����:       ��
********************************************************************/
void FsmCtor(Fsm *me, char const *name, FsmState* initState);

/*******************************************************************
** ������:     FsmStateCtor
** ��������:   ע������׶�״̬������
** ����:       [in]  me:            ״̬��ʵ��ָ��
**             [in]  name:          ��״̬����
**             [in]  hndlr:         �ص�����
** ����:       ��
********************************************************************/
void FsmStateCtor(FsmState *me, char const *name, EvtHndlr hndlr);

/*******************************************************************
** ������:     FsmOnStart
** ��������:   ״̬�������ʼ״̬
** ����:       [in]  me:            ״̬��ʵ��ָ��
** ����:       ��
********************************************************************/
void FsmOnStart(Fsm *me);
/*******************************************************************
** ������:     FsmOnEvent
** ��������:   ��ǰ״̬�´�����Ϣ
** ����:       [in]  me:            ״̬��ʵ��ָ��
**             [in]  msg:           ��Ϣָ��
** ����:       ��
********************************************************************/
BOOLEAN FsmOnEvent(Fsm *me, st_fm_msg_t* msg);

/*******************************************************************
** ������:     FsmExit_
** ��������:   �뿪��ǰ״̬ʱִ�лص�����
** ����:       ��
** ����:       ��
********************************************************************/
void FsmExit_(Fsm *me) ;

/* ��ȡ��ǰ״̬ */
#define FSM_STATE_CURR(me_)  (((Fsm *)me_)->curr)


/* �л�״̬ */
#define FSM_STATE_TRAN(me_, target_)   if (1) { \
                                           FsmExit_((Fsm *)(me_));\
                                           ((Fsm *)(me_))->next = (target_);\
                                       } else ((void)0)

#define START_EVT (-1)
#define ENTRY_EVT (-2)
#define EXIT_EVT  (-3)



#endif    /* _FSM_H_ */

