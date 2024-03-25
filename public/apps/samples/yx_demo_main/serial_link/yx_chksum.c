/********************************************************************************
**
**  �ļ�����:  yx_chksum.c
**  ��Ȩ����:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  �ļ�����:  У���빤���㷨����
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   | �޸ļ�¼
**===============================================================================
**| 2019-12-10 | ����   | ������ģ��
*********************************************************************************/

#include "yx_chksum.h"

/*******************************************************************
** ������:     yx_chksum_get8bit
** ��������:   ���ֽ��ۼӴ���λУ�����㷨
** ����:       [in]  ptr:           Ŀ�����ݵ�ַ
**             [in]  len:           Ŀ�����ݳ���
** ����:       ���ֽ�У����
********************************************************************/
INT8U yx_chksum_get8bit(INT8U *ptr, INT32U len)
{
    INT32U i;
    YX_HWORD_U chksum;
    
    chksum.hword = 0;
    
    for (i = 1; i <= len; i++) {
        chksum.hword += *ptr++;
        if (chksum.bytes.high > 0) {
            chksum.bytes.low += chksum.bytes.high;
            chksum.bytes.high = 0;
        }
        if ((i % 4096) == 0) {
            /* ���ļ��ϴ󣬴˴��� */
            //PORT_ClearWatchdogHS();
        }
    }
    chksum.bytes.low += 1;
    
    return chksum.bytes.low;
}

/*******************************************************************
** ������:     yx_chksum_get8bitb
** ��������:   ���ֽ�У�����㷨,����ȡ������ټ���У���
** ����:       [in]  ptr:           Ŀ�����ݵ�ַ
**             [in]  len:           Ŀ�����ݳ���
** ����:       ���ֽ�У����
********************************************************************/
INT8U yx_chksum_get8bitb(INT8U *ptr, INT32U len)
{
    INT32U i;
    YX_HWORD_U chksum;
    
    chksum.hword = 0;
    for (i = 1; i <= len; i++) {
        chksum.hword += (INT8U)(~(*ptr++));
        if (chksum.bytes.high > 0) {
            chksum.bytes.low += chksum.bytes.high;
            chksum.bytes.high = 0;
        }
        
        if ((i % 4096) == 0) {
            /* ���ļ��ϴ󣬴˴��� */
            //PORT_ClearWatchdogHS();
        }
    }
    chksum.bytes.low += 1;
    return chksum.bytes.low;
}

/*******************************************************************
** ������:     yx_chksum_get16bit
** ��������:   ˫�ֽ�У�����㷨
** ����:       [in]  ptr:           Ŀ�����ݵ�ַ
**             [in]  len:           Ŀ�����ݳ���
** ����:       ˫�ֽ�У����
********************************************************************/
INT16U yx_chksum_get16bit(INT8U *ptr, INT32U len)
{
    INT8U  temp;
    INT16U chksum;
    
    temp    = yx_chksum_get8bit(ptr, len);
    chksum  = (INT16U)temp << 8;
    temp    = yx_chksum_get8bitb(ptr, len);
    chksum |= temp;
    return chksum;
}

/*******************************************************************
** ������:     yx_chksum_getsum
** ��������:   ��ȡ���������ۼ�У����
** ����:       [in]  dptr:          ��������ַ
**             [in]  len:           ����������
** ����:       ���ֽ��ۼ�У����
********************************************************************/
INT8U yx_chksum_getsum(INT8U *dptr, INT16U len)
{
    INT8U result;
    
    result = 0;
    for (; len > 0; len--) {
        result += *dptr++;
    }
    return result;
}

/*******************************************************************
** ������:     yx_chksum_getsumb
** ��������:   ��ȡ���������ۼ�У����ķ���
** ����:       [in]  dptr:          ��������ַ
**             [in]  len:           ����������
** ����:       ���ֽ��ۼ�У���뷴��
********************************************************************/
INT8U yx_chksum_getsumb(INT8U *dptr, INT16U len)
{
    return (yx_chksum_getsum(dptr, len));
}

/*******************************************************************
** ������:     yx_chksum_getsum_c
** ��������:   ��ȡ����������λ�ۼ�У����
** ����:       [in]  dptr:          ��������ַ
**             [in]  len:           ����������
** ����:       ���ֽڴ���λ�ۼ�У����
********************************************************************/
INT8U yx_chksum_getsum_c(INT8U *dptr, INT16U len)
{
    YX_HWORD_U result;
    
    result.hword = 0;
    for (; len > 0; len--) {
        result.hword += *dptr++;
        
        if (result.bytes.high > 0) {
            result.bytes.low += result.bytes.high;
            result.bytes.high = 0;
        }
    }
    return (result.bytes.low);
}

/*******************************************************************
** ������:     yx_chksum_getxor
** ��������:   ��ȡ���������У����
** ����:       [in]  Ptr:           ��������ַ
**             [in]  Len:           ����������
** ����:       ���ֽڴ����У����
********************************************************************/
INT8U yx_chksum_getxor(INT8U *Ptr, INT32U Len)
{
    INT8U Sum;
   
    Sum = 0;
    while (Len--) {
        Sum ^= *Ptr++;
    }
   
    return Sum;
}






