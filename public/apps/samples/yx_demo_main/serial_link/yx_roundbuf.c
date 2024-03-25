/********************************************************************************
**
**  文件名称:  yx_roundbuf.c
**  版权所有:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  文件描述:  环形缓冲区工具模块
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   | 修改记录
**===============================================================================
**| 2019-12-11 | 夏普   | 创建本模块
*********************************************************************************/


#include <string.h>
#include "yx_roundbuf.h"

#ifndef MIN
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

static inline BOOLEAN is_power_of_2(INT32U n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}

static inline INT32U roundup_power_of_2(INT32U a)
{
	INT32U position = 0;
	int i;
	if (a == 0)
		return 0;

	for ( i = a; i != 0; i >>= 1) {
		position++;
	}

	return (INT32U)(1 << (position-1));
}


/*******************************************************************
** 函数名:     yx_roundbuf_init
** 功能描述:   环形缓冲区初始化
** 参数:       [in]  rb:            环形缓冲区
**             [in]  pool:          数据缓存指针
**             [in]  size:          缓冲区大小
** 返回:       无
********************************************************************/
void yx_roundbuf_init(yx_roundbuf_t *rb, INT8U *pool, INT16U size)
{
    /* initialize read and write index */
	rb->read_index 	= 0;
	rb->write_index = 0;
    /* set buffer pool and size */
    rb->buffer_ptr = pool;
	rb->buffer_size = size;
	if (!is_power_of_2(size)) {
		rb->buffer_size = roundup_power_of_2(size);
	} 
}

/*******************************************************************
** 函数名:     yx_roundbuf_put
** 功能描述:   环形缓冲写入数据
** 参数:       [in]  rb:            环形缓冲区
**             [in]  ptr:           数据指针
**             [in]  length:        数据长度
** 返回:       实际写入长度
********************************************************************/
INT32S yx_roundbuf_put(yx_roundbuf_t *rb, const INT8U *ptr, INT16U length)
{
	INT16U l; 	
	length = MIN(length, rb->buffer_size - rb->write_index + rb->read_index);

	if (length == 0) {
		return 0;
	}
	/* first put the data starting from fifo->in to buffer end */	  
	l = MIN(length, rb->buffer_size - (rb->write_index & (rb->buffer_size - 1)));	  
	memcpy(rb->buffer_ptr + (rb->write_index & (rb->buffer_size - 1)), ptr, l);	 
	/* then put the rest (if any) at the beginning of the buffer */ 
	if (l < length) {
		memcpy(rb->buffer_ptr, ptr + l, length - l);	   
	}
	
	rb->write_index += length;	 

    return length;

}

/*******************************************************************
** 函数名:     yx_roundbuf_put_force
** 功能描述:   环形缓冲写入数据 (若已满则覆盖最旧的数据)
** 参数:       [in]  rb:            环形缓冲区
**             [in]  ptr:           数据指针
**             [in]  length:        数据长度
** 返回:       实际写入长度
** 注意:       调用该接口需要加中断保护
********************************************************************/
INT32S yx_roundbuf_put_force(yx_roundbuf_t *rb, const INT8U *ptr, INT16U length)
{
}


/*******************************************************************
** 函数名:     yx_roundbuf_get
** 功能描述:   从环形缓冲区读取数据
** 参数:       [in]  rb:            环形缓冲区
**             [out] ptr:           数据指针
**             [in]  length:        数据长度
** 返回:       实际读取数据长度
********************************************************************/
INT32S yx_roundbuf_get(yx_roundbuf_t *rb, INT8U *ptr, INT16U length)
{
	INT16U l; 	
	length = MIN(length, rb->write_index - rb->read_index);	  
	if (length == 0) {
		return 0;
	}
	l = MIN(length, rb->buffer_size - (rb->read_index & (rb->buffer_size - 1)));	   
	memcpy(ptr, rb->buffer_ptr + (rb->read_index & (rb->buffer_size - 1)), l);	 
	if (l < length ) {
		memcpy(ptr + l, rb->buffer_ptr, length - l);	   
	}
	rb->read_index += length;	  
	return length; 	

}

/*******************************************************************
** 函数名:     yx_roundbuf_putchar
** 功能描述:   向环形缓冲区写入一个字节数据
** 参数:       [in]  rb:            环形缓冲区
**             [in]  ch:            写入的数据
** 返回:       实际写入长度
********************************************************************/
INT32S yx_roundbuf_putchar(yx_roundbuf_t *rb, const INT8U ch)
{
    /* whether has enough space */
    if ((rb->buffer_size -rb->write_index + rb->read_index) == 0) {
        return 0;
    }
	rb->buffer_ptr[rb->write_index & (rb->buffer_size-1)] = ch;
    rb->write_index++;
	return 1;
}

/*******************************************************************
** 函数名:     yx_roundbuf_putchar_force
** 功能描述:   向环形缓冲区写入一个字节数据 (若已满则覆盖最旧的数据)
** 参数:       [in]  rb:            环形缓冲区
**             [in]  ch:            写入的数据
** 返回:       实际写入长度
** 注意:       调用该接口需要加中断保护
********************************************************************/
INT32S yx_roundbuf_putchar_force(yx_roundbuf_t *rb, const INT8U ch)
{
}

/*******************************************************************
** 函数名:     yx_roundbuf_getchar
** 功能描述:   从环形缓冲区读取一个字节数据
** 参数:       [in]  rb:            环形缓冲区
**             [out] ch:            数据指针
** 返回:       实际读出的数据长度
********************************************************************/
INT32S yx_roundbuf_getchar(yx_roundbuf_t *rb, INT8U *ch)
{
    /* ringbuffer is empty */
    if (rb->write_index - rb->read_index == 0) {
        return 0;
    }
    /* put character */
	*ch = rb->buffer_ptr[rb->read_index & (rb->buffer_size-1)];
    rb->read_index++;
    return 1;
}


/*******************************************************************
** 函数名:     yx_roundbuf_get_size
** 功能描述:   获取一个环形缓冲区总容量
** 参数:       [in]  rb:            环形缓冲区
** 返回:       容量
********************************************************************/
INT16U yx_roundbuf_get_size(yx_roundbuf_t *rb)
{
    return rb->buffer_size;
}

/*******************************************************************
** 函数名:     yx_roundbuf_data_len
** 功能描述:   获取一个环形缓冲区已有数据容量
** 参数:       [in]  rb:            环形缓冲区
** 返回:       已有数据容量
********************************************************************/
INT16U yx_roundbuf_data_len(yx_roundbuf_t *rb)
{
	return (INT16U)(rb->write_index - rb->read_index);
}

/*******************************************************************
** 函数名:     yx_roundbuf_space_len
** 功能描述:   获取一个环形缓冲区剩余容量
** 参数:       [in]  rb:            环形缓冲区
** 返回:       剩余容量
********************************************************************/
INT16U yx_roundbuf_space_len(yx_roundbuf_t *rb)
{
    return (INT16U)(rb->buffer_size -rb->write_index + rb->read_index);
}

