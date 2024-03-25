/********************************************************************************
**
**  �ļ�����:  yx_list.h
**  ��Ȩ����:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  �ļ�����:  ������ģ��
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   | �޸ļ�¼
**===============================================================================
**| 2019-12-11 | ����   | ������ģ��
*********************************************************************************/

#ifndef _YX_LIST_H_
#define _YX_LIST_H_
/****************************************************************
ͷ�ļ���������
*****************************************************************/

#include "def.h"

/****************************************************************
�궨��
*****************************************************************/
#define YX_LIST_HEAD_INIT(name) { &(name), &(name) }

#define YX_OFFSETOF(type, member)   ((unsigned int) &((type *)0)->member)

/* 
* YX_CONTAINER_OF - cast a member of a structure out to the containing structure
* @ptr:     the pointer to the member.
* @type:     the type of the container struct this is embedded in.
* @member:     the name of the member within the struct.
*
*/
#define YX_CONTAINER_OF(ptr, type, member) \
            ((type *)((char *)(ptr) - YX_OFFSETOF(type, member)))

/* 
 * YX_LIST_ENTRY - get the struct for this entry
 * @ptr:    the &struct list_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 */
#define YX_LIST_ENTRY(ptr, type, member) YX_CONTAINER_OF(ptr, type, member)

/* 
 * YX_LIST_FIRST_ENTRY - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define YX_LIST_FIRST_ENTRY(ptListHead, type, member)   YX_LIST_ENTRY((ptListHead)->next, type, member)


/* 
 * YX_LIST_FIRST_ENTRY - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define YX_LIST_LAST_ENTRY(ptListHead, type, member)    YX_LIST_ENTRY((ptListHead)->prev, type, member)

/* 
 * YX_LIST_FOR_EACH -   iterate over a list
 * @ptListNodePos:  the &struct list_head to use as a loop cursor.
 * @ptListHead:     the head for your list.
 */
#define YX_LIST_FOR_EACH(ptListHead, ptListNodePos) \
        for (ptListNodePos = (ptListHead)->next; ptListNodePos != (ptListHead); ptListNodePos = ptListNodePos->next)


/* 
 * YX_LIST_FOR_EACH_PREV    -   iterate over a list backwards
 * @ptListNodePos:  the &struct list_head to use as a loop cursor.
 * @ptListHead:     the head for your list.
 */
#define YX_LIST_FOR_EACH_PREV(ptListHead, ptListNodePos) \
        for (ptListNodePos = (ptListHead)->prev; ptListNodePos != (ptListHead); ptListNodePos = ptListNodePos->prev)


/* 
 * YX_LIST_FOR_EACH_SAVE - iterate over a list safe against removal of list entry
 * @ptListNodePos:  the &struct list_head to use as a loop cursor.
 * @ptListNodeTemp: another &struct list_head to use as temporary storage
 * @ptListHead:     the head for your list.
 */
#define YX_LIST_FOR_EACH_SAVE(ptListHead,ptListNodePos,ptListNodeTemp) \
            for (ptListNodePos = (ptListHead)->next, ptListNodeTemp = ptListNodePos->next;  \
                ptListNodePos != (ptListHead); \
                ptListNodePos = ptListNodeTemp, ptListNodeTemp = ptListNodePos->next)

/* 
 * YX_LIST_FOR_EACH_PREV_SAVE - iterate over a list backwards safe against removal of list entry
 * @ptListNodePos:  the &struct list_head to use as a loop cursor.
 * @ptListNodeTemp: another &struct list_head to use as temporary storage
 * @ptListHead:     the head for your list.
 */
#define YX_LIST_FOR_EACH_PREV_SAVE(ptListHead, ptListNodePos, ptListNodeTemp) \
            for (ptListNodePos = (ptListHead)->prev, ptListNodeTemp = ptListNodePos->prev; \
                ptListNodePos != (ptListHead); \
                ptListNodePos = ptListNodeTemp, ptListNodeTemp = ptListNodePos->prev)

/****************************************************************
�ṹ��ö�١�������Ƚṹ����
*****************************************************************/

typedef struct yx_list_t yx_list_t;                  /* Type for lists. */

struct yx_list_t {
     yx_list_t *next;                          /* point to next node. */
     yx_list_t *prev;                          /* point to prev node. */
};




/****************************************************************
Դ�ļ������������

extern �ⲿ��������
*****************************************************************/


/****************************************************************
Դ�ļ���������

extern �ⲿ��������
*****************************************************************/


/*******************************************************************
** ������:     yx_list_init
** ��������:   �����ʼ��
** ����:       [in]  ptList:        ����
** ����:       �������
********************************************************************/
int yx_list_init(yx_list_t *ptList);

/*******************************************************************
** ������:     yx_list_add_after
** ��������:   ��������������һ���ڵ�
** ����:       [in]  ptList:        ����
**             [in]  ptNew:         �µĽڵ�
** ����:       ��
********************************************************************/
int yx_list_add_after(yx_list_t *ptList, yx_list_t *ptNew);

/*******************************************************************
** ������:     yx_list_add_before
** ��������:   ���������ǰ�����һ���ڵ�
** ����:       [in]  ptList:        ����
**             [in]  ptNew:         �µĽڵ�
** ����:       ��
********************************************************************/
int yx_list_add_before(yx_list_t *ptList, yx_list_t *ptNew);

/*******************************************************************
** ������:     yx_list_del
** ��������:   ��һ���ڵ�������ڵ�������ɾ��
** ����:       [in]  ptEntry:       Ҫɾ���Ľڵ�
** ����:       ��
********************************************************************/
int yx_list_del(yx_list_t *ptEntry);

/*******************************************************************
** ������:     yx_list_empty
** ��������:   �������ڵ��Ƿ�Ϊ��
** ����:       [in]  ptListHead:    ����
** ����:       TRUE or FALSE
********************************************************************/
BOOLEAN yx_list_empty(yx_list_t *ptListHead);

/*******************************************************************
** ������:     yx_list_get_count
** ��������:   ��ȡ����ڵ�����
** ����:       [in]  ptListHead:    ����
** ����:       �ڵ�����
********************************************************************/
int yx_list_get_count(yx_list_t *ptListHead);


#endif /*_YX_UTIL_H*/



