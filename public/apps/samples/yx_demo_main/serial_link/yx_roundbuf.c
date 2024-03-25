/********************************************************************************
**
**  �ļ�����:  yx_roundbuf.c
**  ��Ȩ����:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  �ļ�����:  ���λ���������ģ��
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   | �޸ļ�¼
**===============================================================================
**| 2019-12-11 | ����   | ������ģ��
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
** ������:     yx_roundbuf_init
** ��������:   ���λ�������ʼ��
** ����:       [in]  rb:            ���λ�����
**             [in]  pool:          ���ݻ���ָ��
**             [in]  size:          ��������С
** ����:       ��
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
** ������:     yx_roundbuf_put
** ��������:   ���λ���д������
** ����:       [in]  rb:            ���λ�����
**             [in]  ptr:           ����ָ��
**             [in]  length:        ���ݳ���
** ����:       ʵ��д�볤��
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
** ������:     yx_roundbuf_put_force
** ��������:   ���λ���д������ (�������򸲸���ɵ�����)
** ����:       [in]  rb:            ���λ�����
**             [in]  ptr:           ����ָ��
**             [in]  length:        ���ݳ���
** ����:       ʵ��д�볤��
** ע��:       ���øýӿ���Ҫ���жϱ���
********************************************************************/
INT32S yx_roundbuf_put_force(yx_roundbuf_t *rb, const INT8U *ptr, INT16U length)
{
}


/*******************************************************************
** ������:     yx_roundbuf_get
** ��������:   �ӻ��λ�������ȡ����
** ����:       [in]  rb:            ���λ�����
**             [out] ptr:           ����ָ��
**             [in]  length:        ���ݳ���
** ����:       ʵ�ʶ�ȡ���ݳ���
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
** ������:     yx_roundbuf_putchar
** ��������:   ���λ�����д��һ���ֽ�����
** ����:       [in]  rb:            ���λ�����
**             [in]  ch:            д�������
** ����:       ʵ��д�볤��
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
** ������:     yx_roundbuf_putchar_force
** ��������:   ���λ�����д��һ���ֽ����� (�������򸲸���ɵ�����)
** ����:       [in]  rb:            ���λ�����
**             [in]  ch:            д�������
** ����:       ʵ��д�볤��
** ע��:       ���øýӿ���Ҫ���жϱ���
********************************************************************/
INT32S yx_roundbuf_putchar_force(yx_roundbuf_t *rb, const INT8U ch)
{
}

/*******************************************************************
** ������:     yx_roundbuf_getchar
** ��������:   �ӻ��λ�������ȡһ���ֽ�����
** ����:       [in]  rb:            ���λ�����
**             [out] ch:            ����ָ��
** ����:       ʵ�ʶ��������ݳ���
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
** ������:     yx_roundbuf_get_size
** ��������:   ��ȡһ�����λ�����������
** ����:       [in]  rb:            ���λ�����
** ����:       ����
********************************************************************/
INT16U yx_roundbuf_get_size(yx_roundbuf_t *rb)
{
    return rb->buffer_size;
}

/*******************************************************************
** ������:     yx_roundbuf_data_len
** ��������:   ��ȡһ�����λ�����������������
** ����:       [in]  rb:            ���λ�����
** ����:       ������������
********************************************************************/
INT16U yx_roundbuf_data_len(yx_roundbuf_t *rb)
{
	return (INT16U)(rb->write_index - rb->read_index);
}

/*******************************************************************
** ������:     yx_roundbuf_space_len
** ��������:   ��ȡһ�����λ�����ʣ������
** ����:       [in]  rb:            ���λ�����
** ����:       ʣ������
********************************************************************/
INT16U yx_roundbuf_space_len(yx_roundbuf_t *rb)
{
    return (INT16U)(rb->buffer_size -rb->write_index + rb->read_index);
}

