/********************************************************************************
**
**  文件名称:  yx_list.h
**  版权所有:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  文件描述:  链表工具模块
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   | 修改记录
**===============================================================================
**| 2019-12-11 | 夏普   | 创建本模块
*********************************************************************************/

#ifndef _YX_LIST_H_
#define _YX_LIST_H_
/****************************************************************
头文件引用声明
*****************************************************************/

#include "def.h"

/****************************************************************
宏定义
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
结构、枚举、公用体等结构定义
*****************************************************************/

typedef struct yx_list_t yx_list_t;                  /* Type for lists. */

struct yx_list_t {
     yx_list_t *next;                          /* point to next node. */
     yx_list_t *prev;                          /* point to prev node. */
};




/****************************************************************
源文件定义变量声明

extern 外部变量声明
*****************************************************************/


/****************************************************************
源文件函数声明

extern 外部函数声明
*****************************************************************/


/*******************************************************************
** 函数名:     yx_list_init
** 功能描述:   链表初始化
** 参数:       [in]  ptList:        链表
** 返回:       操作结果
********************************************************************/
int yx_list_init(yx_list_t *ptList);

/*******************************************************************
** 函数名:     yx_list_add_after
** 功能描述:   在链表的最后增加一个节点
** 参数:       [in]  ptList:        链表
**             [in]  ptNew:         新的节点
** 返回:       无
********************************************************************/
int yx_list_add_after(yx_list_t *ptList, yx_list_t *ptNew);

/*******************************************************************
** 函数名:     yx_list_add_before
** 功能描述:   在链表的最前面添加一个节点
** 参数:       [in]  ptList:        链表
**             [in]  ptNew:         新的节点
** 返回:       无
********************************************************************/
int yx_list_add_before(yx_list_t *ptList, yx_list_t *ptNew);

/*******************************************************************
** 函数名:     yx_list_del
** 功能描述:   将一个节点从它所在的链表上删除
** 参数:       [in]  ptEntry:       要删除的节点
** 返回:       无
********************************************************************/
int yx_list_del(yx_list_t *ptEntry);

/*******************************************************************
** 函数名:     yx_list_empty
** 功能描述:   检测链表节点是否为空
** 参数:       [in]  ptListHead:    链表
** 返回:       TRUE or FALSE
********************************************************************/
BOOLEAN yx_list_empty(yx_list_t *ptListHead);

/*******************************************************************
** 函数名:     yx_list_get_count
** 功能描述:   获取链表节点数量
** 参数:       [in]  ptListHead:    链表
** 返回:       节点数量
********************************************************************/
int yx_list_get_count(yx_list_t *ptListHead);


#endif /*_YX_UTIL_H*/



