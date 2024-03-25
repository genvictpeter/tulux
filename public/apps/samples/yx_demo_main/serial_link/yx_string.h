/********************************************************************************
**
**  �ļ�����:  yx_string.h
**  ��Ȩ����:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  �ļ�����:  �ַ�ת��/���ҹ���ģ��
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   | �޸ļ�¼
**===============================================================================
**| 2019-12-11 | ����   | ������ģ��
*********************************************************************************/

#ifndef _YX_STRING_H_
#define _YX_STRING_H_ 

#include "def.h"

#define STR_EQUAL                           0
#define STR_GREAT                           1
#define STR_LESS                            2


#define CASESENSITIVE                       1


/*******************************************************************
** ������:     yx_mem_cmp
** ��������:   �������ݻ���Ƚ����ݴ�С
** ����:       [in]  ptr1:          ����1
**             [in]  ptr2:          ����2
**             [in]  len:           ����
** ����:       �ȽϽ�� STR_EQUAL��� STR_GREATǰ�߽ϴ� STR_LESSǰ�߽�С
********************************************************************/
INT8U yx_mem_cmp(INT8U *ptr1, INT8U *ptr2, INT16U len);

/*******************************************************************
** ������:     yx_mem_findbyte
** ��������:   ��һ�����ݻ����в���һ���ַ�
** ����:       [in]  ptr:           ���ݻ���ָ��
**             [in]  len:           ���ݻ��泤��
**             [in]  cb:            ��Ҫ���ҵ��ַ�
** ����:       TRUE or FALSE
********************************************************************/
BOOLEAN yx_mem_findbyte(INT8U *ptr, INT16U len, INT8U cb);

/*******************************************************************
** ������:     yx_mem_findbyte
** ��������:   ��һ�����ݻ����в���һ���ַ���
** ����:       [in]  bptr:          ���ݻ���ָ��
**             [in]  blen:          ���ݻ��泤��
**             [in]  sptr:          ��Ҫ���ҵ��ַ���
**             [in]  slen:          �ַ�������
** ����:       TRUE or FALSE
********************************************************************/
BOOLEAN yx_str_search(INT8U *bptr, INT16U blen, INT8U *sptr, INT16U slen);

/*******************************************************************
** ������:     yx_str_searchkeyword_case
** ��������:   �����ַ������������ִ�Сд����s1�������Ƿ������s2
** ����:       [in]  matchcase:     TRUE-���ִ�Сд��FALSE-�����ִ�Сд
**             [in]  s1��           ԭ�ַ���
**             [in]  len1��         s1����
**             [in]  s2��           Ҫ���ҵ��ַ���
**             [in]  len2��         s2����
** ����:       �ɹ�����TRUE��ʧ�ܷ���FALSE
********************************************************************/
BOOLEAN yx_str_searchkeyword_case(BOOLEAN matchcase, INT8U *s1, INT16U len1, INT8U *s2, INT16U len2);

/*******************************************************************
** ������:     yx_str_searchkeywordfromhead
** ��������:   ���Ŀ�����ݻ�������ͷ�Ƿ����ַ���ƥ��
** ����:       [in]  ptr:           ��������С
**             [in]  maxlen:        ����������
**             [in]  sptr:          �ַ���ָ��
** ����:       TRUE or FALSE
********************************************************************/
BOOLEAN yx_str_searchkeywordfromhead(INT8U *ptr, INT16U maxlen, char *sptr);

/*******************************************************************
** ������:     yx_str_searchdigitalstring
** ��������:   ���ַ����в���10�������֣��Ե�numflag��flagchar����Ϊ������־
** ����:       [in]  ptr:           ��������С
**             [in]  maxlen:        ����������
**             [in]  flagchar:      ��־�ַ�
**             [in]  numflag:       ��־�ַ����ִ���
** ����:       �ҵ������֡�δ�ҵ��򷵻�0xFFFF
********************************************************************/
INT16U yx_str_searchdigitalstring(INT8U *ptr, INT16U maxlen, INT8U flagchar, INT8U numflag);

/*******************************************************************
** ������:     yx_str_searchdigitalstring
** ��������:   ���ַ����в���16�������֣��Ե�numflag��flagchar����Ϊ������־
** ����:       [in]  ptr:           ��������С
**             [in]  maxlen:        ����������
**             [in]  flagchar:      ��־�ַ�
**             [in]  numflag:       ��־�ַ����ִ���
** ����:       �ҵ������֡�δ�ҵ��򷵻�0xFFFF
********************************************************************/
INT16U yx_str_searchhexstring(INT8U *ptr, INT16U maxlen, INT8U flagchar, INT8U numflag);

/*******************************************************************
** ������:     yx_str_searchstring
** ��������:   �ڻ������ж�����һ��flagchar�͵�numflag��֮������ݣ����ȱ���С��limitlen
** ����:       [in]  dptr:          ����������ָ��
**             [in]  limitlen:      ����������
**             [in]  sptr:          ���ҵĻ�����ָ��
**             [in]  maxlen:        ���ҵĻ���������
**             [in]  flagchar:      ���ҵ��ַ�
**             [in]  numflag:       ���ַ����ֵ�numflag��λ��Ϊ������־
** ����:       ʵ�ʳ��ȡ�
** ע��:      ������ȴ���limitlen���Ի�������ݣ������س���Ϊ0
********************************************************************/
INT16U yx_str_searchstring(INT8U *dptr, INT16U limitlen, INT8U *sptr, INT16U maxlen, INT8U flagchar, INT8U numflag);

/*******************************************************************
** ������:     yx_str_searchdigitalstring
** ��������:   ���ַ����в��ҵ�numflag��flagchar���ֵ�λ��
** ����:       [in]  sptr:          ��������С
**             [in]  maxlen:        ����������
**             [in]  flagchar:      ��־�ַ�
**             [in]  numflag:       ��־�ַ����ִ���
** ����:       �ҵ��ĵ�ַָ�룬δ�ҵ�����NULL
********************************************************************/
INT8U* yx_str_findcharpos(INT8U *sptr, char findchar, INT8U numchar, INT16U maxlen);

/*******************************************************************
** ������:     yx_str_hex2char
** ��������:   ��16���ڵ�hexתΪASCII�ַ�
** ����:       [in]  sbyte:         ��Ҫת����hex
** ����:       ASCII�ַ�
********************************************************************/
INT8U yx_str_hex2char(INT8U sbyte);

/*******************************************************************
** ������:     yx_str_char2hex
** ��������:   ��16���ڵ�ASCII�ַ�תΪhex
** ����:       [in]  schar:         ��Ҫת�����ַ�
** ����:       hex
********************************************************************/
INT8U yx_str_char2hex(INT8U schar);

/*******************************************************************
** ������:     yx_str_hex2ascii
** ��������:   ��һ��hexתΪ�ɼ�ASCII���ַ�
** ����:       [out] dptr:          ���ָ��
**             [in]  sptr:          Ҫת�������ݻ���
**             [in]  len:           Ҫת�������ݳ���
** ����:       ת����ĳ���
********************************************************************/
INT16U yx_str_hex2ascii(INT8U *dptr, INT8U *sptr, INT16U len);

/*******************************************************************
** ������:     yx_str_ascii2hex
** ��������:   ��һ��ASCII��תΪhex
** ����:       [out] dptr:          ���ָ��
**             [in]  sptr:          Ҫת�������ݻ���
**             [in]  len:           Ҫת�������ݳ���
** ����:       ת����ĳ���
********************************************************************/
INT16U yx_str_ascii2hex(INT8U *dptr, INT8U *sptr, INT16U len);

/*******************************************************************
** ������:     yx_str_dec2ascii
** ��������:   ��һ������������תΪASCII��
** ����:       [out] dptr:          ���ָ��
**             [in]  data:          Ҫת���ĳ���������
**             [in]  len:           �������С���ݳ��ȣ������ʵ��ת��֮��ĳ��ȴ������ǰ�油'0'
** ����:       ʵ���������
** SAMPLE:    7899 -----> '7','8','9','9'
********************************************************************/
INT8U yx_str_dec2ascii(INT8U *dptr, INT32U data, INT8U reflen);

/*******************************************************************
** ������:     yx_str_ascii2dec
** ��������:   ��һ��ASCII��ת��Ϊһ������������
** ����:       [in]  sptr:          Ҫת��������ָ��
**             [in]  len:           Ҫת�������ݳ���
** ����:       ����������
** SAMPLE:    '7','8','9','9' -----> 7899
********************************************************************/
INT32U yx_str_ascii2dec(INT8U *sptr, INT8U len);

/*******************************************************************
** ������:     yx_str_bcd_to_hex
** ��������:   ��bcdתΪhex
** ����:       [in]  mask:          ����
**             [in]  bcd:           Ҫת��������
** ����:       ת��֮�������
********************************************************************/
INT8U yx_str_bcd_to_hex(INT8U mask, INT8U bcd);

/*******************************************************************
** ������:     yx_str_hex_to_bcd
** ��������:   ��hexתΪbcd
** ����:       [in]  hex:           Ҫת��������
** ����:       ת��֮�������
********************************************************************/
INT8U yx_str_hex_to_bcd(INT8U hex);

/*******************************************************************
** ������:     yx_str_long2char_msb
** ��������:   ���������������ģʽת��Ϊ4���ֽڣ�����λ�ֽ��ڵ͵�ַ
** ����:       [in]  dest:         ���ָ��
**             [in]  src:          ��ת��������
** ����:       ��
********************************************************************/
void yx_str_long2char_msb(INT8U *dest, INT32U src);

/*******************************************************************
** ������:     yx_str_long2char_lsb
** ��������:   ������������С��ģʽת��Ϊ4���ֽڣ�����λ�ֽ��ڸߵ�ַ
** ����:       [in]  dest:         ���ָ��
**             [in]  src:          ��ת��������
** ����:       ��
********************************************************************/
void yx_str_long2char_lsb(INT8U *dest, INT32U src);

/*******************************************************************
** ������:     yx_str_char2long_msb
** ��������:   ��4���ֽڰ����ģʽת��Ϊ������, ����λ�ֽ��ڵ͵�ַ
** ����:       [in]  src:           ��ת��������ָ��
** ����:       ����������
********************************************************************/
INT32U yx_str_char2long_msb(INT8U *src);

/*******************************************************************
** ������:     yx_str_char2long_lsb
** ��������:   ��4���ֽڰ����ģʽת��Ϊ������, ����λ�ֽ��ڵ͵�ַ
** ����:       [in]  src:           ��ת��������ָ��
** ����:       ����������
********************************************************************/
INT32U yx_str_char2long_lsb(INT8U *src);

/*******************************************************************
** ������:     yx_str_short2char_msb
** ��������:   ���������������ģʽת��Ϊ2���ֽڣ�����λ�ֽ��ڵ͵�ַ
** ����:       [in]  dest:         ���ָ��
**             [in]  src:          ��ת��������
** ����:       ��
********************************************************************/
void yx_str_short2char_msb(INT8U *dest, INT16U src);

/*******************************************************************
** ������:     yx_str_short2char_lsb
** ��������:   ������������С��ģʽת��Ϊ2���ֽڣ�����λ�ֽ��ڸߵ�ַ
** ����:       [in]  dest:         ���ָ��
**             [in]  src:          ��ת��������
** ����:       ��
********************************************************************/
void yx_str_short2char_lsb(INT8U *dest, INT16U src);

/*******************************************************************
** ������:     yx_str_char2short_msb
** ��������:   ��2���ֽڰ����ģʽת��Ϊ������, ����λ�ֽ��ڵ͵�ַ
** ����:       [in]  src:           ��ת��������ָ��
** ����:       ����������
********************************************************************/
INT16U yx_str_char2short_msb(INT8U *src);

/*******************************************************************
** ������:     yx_str_char2short_lsb
** ��������:   ��2���ֽڰ�С��ģʽת��Ϊ������, ����λ�ֽ��ڸߵ�ַ
** ����:       [in]  src:           ��ת��������ָ��
** ����:       ����������
********************************************************************/
INT16U yx_str_char2short_lsb(INT8U *src);

/*******************************************************************
** ������:     bal_DW_BigToLit
** ��������:   ����˴洢��4�ֽ�����ת��С��ģʽ�洢
** ����:       [in]  sptr:          Դ������
**             [in]  dptr:          Ŀ�껺����
** ����:       ˫��ֵ
********************************************************************/
INT8U yx_str_word_big2lit(INT8U *sptr, INT8U *dptr);

/*******************************************************************
** ������:     bal_HW_BigToLit
** ��������:   ����˴洢��2�ֽ�����ת��ΪС��ģʽ�洢
** ����:       [in]  sptr:          Դ������
**             [in]  dptr:          Ŀ�껺����
** ����:       ˫��ֵ
********************************************************************/
INT8U yx_str_short_big2lit(INT8U *sptr, INT8U *dptr);

#endif

