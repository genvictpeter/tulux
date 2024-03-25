/********************************************************************************
**
**  �ļ�����:  yx_string.c
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

#include "yx_string.h"

/*******************************************************************
** ������:     _yx_char_isupper
** ��������:   ��Сд��ĸASCIIת��Ϊ��д
** ����:       [in]  ch:            Ҫת�����ַ�
** ����:       ת��֮����ַ�
********************************************************************/
static INT8U _yx_char_isupper(INT8U ch)
{
    if (ch >= 'a' && ch <= 'z') {
        ch = 'A' + (ch - 'a');
    }
    return ch;
}

/*******************************************************************
** ������:     _yx_char_cmp
** ��������:   �����ַ��Ƚ�
** ����:       [in]  matchcase:     ��Ϊ1����Դ�Сд��Ϊ1��ԭʼ���ݱȽ�
**             [in]  ch1:           �ַ�1
**             [in]  ch2:           �ַ�2
** ����:       �ȽϽ�� STR_EQUAL��� STR_GREATǰ�߽ϴ� STR_LESSǰ�߽�С
********************************************************************/
static INT8U _yx_char_cmp(BOOLEAN matchcase, INT8U ch1, INT8U ch2)
{
    if (matchcase != CASESENSITIVE) {
        ch1 = _yx_char_isupper(ch1);
        ch2 = _yx_char_isupper(ch2);
    }
    if (ch1 > ch2) {
        return STR_GREAT;
    } else if (ch1 < ch2) {
        return STR_LESS;
    } else {
        return STR_EQUAL;
    }
}

/*******************************************************************
** ������:     yx_mem_cmp
** ��������:   �������ݻ���Ƚ����ݴ�С
** ����:       [in]  ptr1:          ����1
**             [in]  ptr2:          ����2
**             [in]  len:           ����
** ����:       �ȽϽ�� STR_EQUAL��� STR_GREATǰ�߽ϴ� STR_LESSǰ�߽�С
********************************************************************/
INT8U yx_mem_cmp(INT8U *ptr1, INT8U *ptr2, INT16U len)
{
    INT8U result;
    
    for (;;) {
        if (len == 0) {
            break;
        }
        result = _yx_char_cmp(CASESENSITIVE, *ptr1++, *ptr2++);
        if (result != STR_EQUAL) {
            return result;
        } else {
            len--;
        }
    }
    return STR_EQUAL;
}

/*******************************************************************
** ������:     yx_mem_findbyte
** ��������:   ��һ�����ݻ����в���һ���ַ�
** ����:       [in]  ptr:           ���ݻ���ָ��
**             [in]  len:           ���ݻ��泤��
**             [in]  cb:            ��Ҫ���ҵ��ַ�
** ����:       TRUE or FALSE
********************************************************************/
BOOLEAN yx_mem_findbyte(INT8U *ptr, INT16U len, INT8U cb)
{
    if (len == 0) {
        return FALSE;
    }
    for (; len > 0; len--) {
        if (*ptr++ != cb) {
            return FALSE;
        }
    }
    return TRUE;
}

/*******************************************************************
** ������:     yx_mem_findbyte
** ��������:   ��һ�����ݻ����в���һ���ַ���
** ����:       [in]  bptr:          ���ݻ���ָ��
**             [in]  blen:          ���ݻ��泤��
**             [in]  sptr:          ��Ҫ���ҵ��ַ���
**             [in]  slen:          �ַ�������
** ����:       TRUE or FALSE
********************************************************************/
BOOLEAN yx_str_search(INT8U *bptr, INT16U blen, INT8U *sptr, INT16U slen)
{
    INT8U *tempptr;
    INT16U templen;
    
    if ((slen == 0) || (slen > blen)) {
        return FALSE;
    }
    tempptr = sptr;
    templen = slen;
    while(blen > 0) {
        if (*bptr == *tempptr) {
            bptr++;
            tempptr++;
            blen--;
            if (--templen == 0) {
                return TRUE;
            }
        } else {
            if (tempptr == sptr) {
                bptr++;
                blen--;
            }
            tempptr = sptr;
            templen = slen;
            continue;
        }
    }
    return FALSE;
}

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
BOOLEAN yx_str_searchkeyword_case(BOOLEAN matchcase, INT8U *s1, INT16U len1, INT8U *s2, INT16U len2)
{
    INT16U cmplen1, cmplen2;
    INT8U ch1, ch2;
    INT8U *xptr, *cptr;

    if (len2 == 0 || len1 == 0 || len2 > len1) {
        return FALSE;
    }
        
    xptr = s1;
    cptr = s2;
    cmplen1 = len1;
    cmplen2 = len2;
    while (len1 > 0) {
        ch1 = *xptr++;
        ch2 = *cptr++;
        if (matchcase == FALSE) {                                              /* �����ִ�Сд */
            ch1 = _yx_char_isupper(ch1);
            ch2 = _yx_char_isupper(ch2);
        }
        if (ch1 == ch2) {
            if (--cmplen2 == 0) {
                return TRUE;
            }
            if (--cmplen1 == 0) {
                return FALSE;
            }
        } else {
            cptr = s2;
            cmplen2 = len2;
            
            s1++;
            len1--;
            xptr = s1;
            cmplen1 = len1;
        }
    }
    return FALSE;
}

/*******************************************************************
** ������:     yx_str_searchkeywordfromhead
** ��������:   ���Ŀ�����ݻ�������ͷ�Ƿ����ַ���ƥ��
** ����:       [in]  ptr:           ��������С
**             [in]  maxlen:        ����������
**             [in]  sptr:          �ַ���ָ��
** ����:       TRUE or FALSE
********************************************************************/
BOOLEAN yx_str_searchkeywordfromhead(INT8U *ptr, INT16U maxlen, char *sptr)
{
    if (*sptr == 0) {
        return FALSE;
    }
    while(maxlen > 0) {
        if (*ptr++ == *sptr++) {
            if (*sptr == 0) {
                return TRUE;
            }
        } else {
            return FALSE;
        }
        maxlen--;
    }
    return FALSE;
}

//#pragma O0
/*******************************************************************
** ������:     yx_str_searchdigitalstring
** ��������:   ���ַ����в���10�������֣��Ե�numflag��flagchar����Ϊ������־
** ����:       [in]  ptr:           ��������С
**             [in]  maxlen:        ����������
**             [in]  flagchar:      ��־�ַ�
**             [in]  numflag:       ��־�ַ����ִ���
** ����:       �ҵ������֡�δ�ҵ��򷵻�0xFFFF
********************************************************************/
INT16U yx_str_searchdigitalstring(INT8U *ptr, INT16U maxlen, INT8U flagchar, INT8U numflag)
{
    BOOLEAN find;
    INT16U  result;
    INT8U *bptr = NULL;
    INT8U *eptr = NULL;
    
    find = FALSE;
    for (;;) {
        if (maxlen == 0) {
            find = FALSE;
            break;
        }
        if (*ptr == flagchar) {
            numflag--;
            if (numflag == 0) {
                eptr = ptr;
                break;
            }
        }
        if (*ptr >= '0' && *ptr <= '9') {
            if (!find) {
                find = TRUE;
                bptr = ptr;
            }
        } else {
            find = FALSE;
        }
        ptr++;
        maxlen--;
    }
    
    if (find) {
        result = 0;
        for(;;) {
            result = result * 10 + yx_str_char2hex(*bptr++);
            if (bptr == eptr) {
                break;
            }
        }
    } else {
        result = 0xffff;
    }
    return result;
}

/*******************************************************************
** ������:     yx_str_searchdigitalstring
** ��������:   ���ַ����в���16�������֣��Ե�numflag��flagchar����Ϊ������־
** ����:       [in]  ptr:           ��������С
**             [in]  maxlen:        ����������
**             [in]  flagchar:      ��־�ַ�
**             [in]  numflag:       ��־�ַ����ִ���
** ����:       �ҵ������֡�δ�ҵ��򷵻�0xFFFF
********************************************************************/
INT16U yx_str_searchhexstring(INT8U *ptr, INT16U maxlen, INT8U flagchar, INT8U numflag)
{
    BOOLEAN find;
    INT16U  result;
    INT8U *bptr = NULL;
    INT8U *eptr = NULL;
    
    find = FALSE;
    for (;;) {
        if (maxlen == 0) {
            find = FALSE;
            break;
        }
        if (*ptr == flagchar) {
            numflag--;
            if (numflag == 0) {
                eptr = ptr;
                break;
            }
        }
        if ((*ptr >= '0' && *ptr <= '9') || (*ptr >= 'a' && *ptr <= 'f') || (*ptr >= 'A' && *ptr <= 'F')) {
            if (!find) {
                find = TRUE;
                bptr = ptr;
            }
        } else {
            find = FALSE;
        }
        ptr++;
        maxlen--;
    }
    
    if (find) {
        result = 0;
        for(;;) {
            result = result * 16 + yx_str_char2hex(*bptr++);
            if (bptr == eptr) {
                break;
            }
        }
    } else {
        result = 0xffff;
    }
    return result;
}

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
INT16U yx_str_searchstring(INT8U *dptr, INT16U limitlen, INT8U *sptr, INT16U maxlen, INT8U flagchar, INT8U numflag)
{
    BOOLEAN find;
    INT16U  len;
    INT8U *bptr = NULL;
    INT8U *eptr = NULL;
    
    find = FALSE;
    for (;;) {
        if (maxlen == 0) {
            find = FALSE;
            break;
        }
        if (*sptr == flagchar) {
            if (!find) {
                find = TRUE;
                bptr = sptr;
            } else {
                numflag--;
                if (numflag == 0) {
                    eptr = sptr;
                    break;
                } else {
                    find = FALSE;
                }
            }
        }
        sptr++;
        maxlen--;
    }
    
    if (find) {
        len = 0;
        bptr++;
        while (bptr != eptr) {
            if (len >= limitlen) {
                len = 0;
                break;
            }
            *dptr++ = *bptr++;
            len++;
        }
        return len;
    } else {
        return 0;
    }
}

/*******************************************************************
** ������:     yx_str_searchdigitalstring
** ��������:   ���ַ����в��ҵ�numflag��flagchar���ֵ�λ��
** ����:       [in]  sptr:          ��������С
**             [in]  maxlen:        ����������
**             [in]  flagchar:      ��־�ַ�
**             [in]  numflag:       ��־�ַ����ִ���
** ����:       �ҵ��ĵ�ַָ�룬δ�ҵ�����NULL
********************************************************************/
INT8U* yx_str_findcharpos(INT8U *sptr, char findchar, INT8U numchar, INT16U maxlen)
{
    if (maxlen == 0) {
        return NULL;
    }
    for (;;) {
        if (*sptr == findchar) {
            if (numchar == 0) {
                return sptr;
            } else {
                numchar--;
            }
        }

        sptr++;
        maxlen--;
        if (maxlen == 0 || *sptr == 0) {
            break;
        }
    }
    return NULL;
}



/*******************************************************************
** ������:     yx_str_hex2char
** ��������:   ��16���ڵ�hexתΪASCII�ַ�
** ����:       [in]  sbyte:         ��Ҫת����hex
** ����:       ASCII�ַ�
********************************************************************/
INT8U yx_str_hex2char(INT8U sbyte)
{
    sbyte &= 0x0F;
    if (sbyte < 0x0A) {
        return (sbyte + '0');
    } else {
        return (sbyte - 0x0A + 'A');
    }
}

/*******************************************************************
** ������:     yx_str_char2hex
** ��������:   ��16���ڵ�ASCII�ַ�תΪhex
** ����:       [in]  schar:         ��Ҫת�����ַ�
** ����:       hex
********************************************************************/
INT8U yx_str_char2hex(INT8U schar)
{
    if (schar >= '0' && schar <= '9') {
        return (schar - '0');
    } else if (schar >= 'A' && schar <= 'F') {
        return (schar - 'A' + 0x0A);
    } else if (schar >= 'a' && schar <= 'f') {
        return (schar - 'a' + 0x0A);
    } else {
        return 0;
    }
}

/*******************************************************************
** ������:     yx_str_hex2ascii
** ��������:   ��һ��hexתΪ�ɼ�ASCII���ַ�
** ����:       [out] dptr:          ���ָ��
**             [in]  sptr:          Ҫת�������ݻ���
**             [in]  len:           Ҫת�������ݳ���
** ����:       ת����ĳ���
********************************************************************/
INT16U yx_str_hex2ascii(INT8U *dptr, INT8U *sptr, INT16U len)
{
    INT16U i;
    INT8U  stemp;
    
    for (i = 1; i <= len; i++) {
        stemp = *sptr++;
        *dptr++ = yx_str_hex2char(stemp >> 4);
        *dptr++ = yx_str_hex2char(stemp & 0x0F);
    }
    return (2 * len);
}

/*******************************************************************
** ������:     yx_str_ascii2hex
** ��������:   ��һ��ASCII��תΪhex
** ����:       [out] dptr:          ���ָ��
**             [in]  sptr:          Ҫת�������ݻ���
**             [in]  len:           Ҫת�������ݳ���
** ����:       ת����ĳ���
********************************************************************/
INT16U yx_str_ascii2hex(INT8U *dptr, INT8U *sptr, INT16U len)
{
    INT16U i;
    INT8U  dtemp, stemp;
    
    if (len % 2) {
        return 0;
    }
    len /= 2;
    for (i = 1; i <= len; i++) {
        stemp   = yx_str_char2hex(*sptr++);
        dtemp   = stemp << 4;
        *dptr++ = yx_str_char2hex(*sptr++) | dtemp;
    }
    return len;
}

/*******************************************************************
** ������:     yx_str_dec2ascii
** ��������:   ��һ������������תΪASCII��
** ����:       [out] dptr:          ���ָ��
**             [in]  data:          Ҫת���ĳ���������
**             [in]  len:           �������С���ݳ��ȣ������ʵ��ת��֮��ĳ��ȴ������ǰ�油'0'
** ����:       ʵ���������
** SAMPLE:    7899 -----> '7','8','9','9'
********************************************************************/
INT8U yx_str_dec2ascii(INT8U *dptr, INT32U data, INT8U reflen)
{
    INT8U i, len, temp;
    INT8U *tempptr;
    
    len     = 0;
    tempptr = dptr;
    for (;;) {
        *dptr++ = yx_str_hex2char(data % 10);
        len++;
        if (data < 10) {
            break;
        } else {
            data /= 10;
        }
    }
    if (len != 0) {
        dptr = tempptr;
        for (i = 0; i < (len / 2); i++) {
            temp = *(dptr + i);
            *(dptr + i) = *(dptr + (len - 1 - i));
            *(dptr + (len - 1 - i)) = temp;
        }
        if (reflen > len) {
            dptr = tempptr + (len - 1);
            for (i = 1; i <= reflen; i++) {
                if (i <= len) {
                    *(dptr + (reflen - len)) = *dptr;
                } else {
                    *(dptr + (reflen - len)) = '0';
                }
                dptr--;
            }
            len = reflen;
        }
    }
    return len;
}

/*******************************************************************
** ������:     yx_str_ascii2dec
** ��������:   ��һ��ASCII��ת��Ϊһ������������
** ����:       [in]  sptr:          Ҫת��������ָ��
**             [in]  len:           Ҫת�������ݳ���
** ����:       ����������
** SAMPLE:    '7','8','9','9' -----> 7899
********************************************************************/
INT32U yx_str_ascii2dec(INT8U *sptr, INT8U len)
{
    INT32U result;
    
    result = 0;
    for (; len > 0; len--) {
        result = result * 10 + yx_str_char2hex(*sptr++);
    }
    return result;
}

/*******************************************************************
** ������:     yx_str_bcd_to_hex
** ��������:   ��bcdתΪhex
** ����:       [in]  mask:          ����
**             [in]  bcd:           Ҫת��������
** ����:       ת��֮�������
********************************************************************/
INT8U yx_str_bcd_to_hex(INT8U mask, INT8U bcd)
{
    INT8U temp1, temp2, val;
    
    temp1 = bcd & mask;
    temp2 = bcd & 0x0f;
    temp1 >>= 4;
    val = temp1 * 10 + temp2;
    return val;
}

/*******************************************************************
** ������:     yx_str_hex_to_bcd
** ��������:   ��hexתΪbcd
** ����:       [in]  hex:           Ҫת��������
** ����:       ת��֮�������
********************************************************************/
INT8U yx_str_hex_to_bcd(INT8U hex)
{
    INT8U val;
    
    val = ((hex / 10) << 4) + (hex % 10);
    return val;
}

/*******************************************************************
** ������:     yx_str_long2char_msb
** ��������:   ���������������ģʽת��Ϊ4���ֽڣ�����λ�ֽ��ڵ͵�ַ
** ����:       [in]  dest:         ���ָ��
**             [in]  src:          ��ת��������
** ����:       ��
********************************************************************/
void yx_str_long2char_msb(INT8U *dest, INT32U src)
{
    dest[0] = (INT8U)(src >> 24);
    dest[1] = (INT8U)(src >> 16);
    dest[2] = (INT8U)(src >> 8);
    dest[3] = (INT8U)(src);
}

/*******************************************************************
** ������:     yx_str_long2char_lsb
** ��������:   ������������С��ģʽת��Ϊ4���ֽڣ�����λ�ֽ��ڸߵ�ַ
** ����:       [in]  dest:          ���ָ��
**             [in]  src:           ��ת��������
** ����:       ��
********************************************************************/
void yx_str_long2char_lsb(INT8U *dest, INT32U src)
{
    dest[0] = (INT8U)(src);
    dest[1] = (INT8U)(src >> 8);
    dest[2] = (INT8U)(src >> 16);
    dest[3] = (INT8U)(src >> 24);
}

/*******************************************************************
** ������:     yx_str_char2long_msb
** ��������:   ��4���ֽڰ����ģʽת��Ϊ������, ����λ�ֽ��ڵ͵�ַ
** ����:       [in]  src:           ��ת��������ָ��
** ����:       ����������
********************************************************************/
INT32U yx_str_char2long_msb(INT8U *src)
{
    INT32U ret = 0;

    ret = (INT32U)(src[0]) << 24;
    ret |= (INT32U)(src[1]) << 16;
    ret |= (INT32U)(src[2]) << 8;
    ret |= (INT32U)(src[3]);

    return ret;
}

/*******************************************************************
** ������:     yx_str_char2long_lsb
** ��������:   ��4���ֽڰ����ģʽת��Ϊ������, ����λ�ֽ��ڵ͵�ַ
** ����:       [in]  src:           ��ת��������ָ��
** ����:       ����������
********************************************************************/
INT32U yx_str_char2long_lsb(INT8U *src)
{
    INT32U ret = 0;

    ret = (INT32U)(src[3]) << 24;
    ret |= (INT32U)(src[2]) << 16;
    ret |= (INT32U)(src[1]) << 8;
    ret |= (INT32U)(src[0]);

    return ret;
}

/*******************************************************************
** ������:     yx_str_short2char_msb
** ��������:   ���������������ģʽת��Ϊ2���ֽڣ�����λ�ֽ��ڵ͵�ַ
** ����:       [in]  dest:         ���ָ��
**             [in]  src:          ��ת��������
** ����:       ��
********************************************************************/
void yx_str_short2char_msb(INT8U *dest, INT16U src)
{
    dest[0] = (INT8U)(src >> 8);
    dest[1] = (INT8U)(src);
}

/*******************************************************************
** ������:     yx_str_short2char_lsb
** ��������:   ������������С��ģʽת��Ϊ2���ֽڣ�����λ�ֽ��ڸߵ�ַ
** ����:       [in]  dest:         ���ָ��
**             [in]  src:          ��ת��������
** ����:       ��
********************************************************************/
void yx_str_short2char_lsb(INT8U *dest, INT16U src)
{
    dest[0] = (INT8U)(src);
    dest[1] = (INT8U)(src >> 8);
}

/*******************************************************************
** ������:     yx_str_char2short_msb
** ��������:   ��2���ֽڰ����ģʽת��Ϊ������, ����λ�ֽ��ڵ͵�ַ
** ����:       [in]  src:           ��ת��������ָ��
** ����:       ����������
********************************************************************/
INT16U yx_str_char2short_msb(INT8U *src)
{
    INT16U ret = 0;
    
    ret = (INT16U)((INT16U) (src[0]) << 8);
    ret |= (INT16U)(src[1]);
    return ret;
}

/*******************************************************************
** ������:     yx_str_char2short_lsb
** ��������:   ��2���ֽڰ�С��ģʽת��Ϊ������, ����λ�ֽ��ڸߵ�ַ
** ����:       [in]  src:           ��ת��������ָ��
** ����:       ����������
********************************************************************/
INT16U yx_str_char2short_lsb(INT8U *src)
{
    INT16U ret = 0;
    
    ret = (INT16U)((INT16U)(src[1]) << 8);
    ret |= (INT16U)(src[0]);
    return ret;
}

/*******************************************************************
** ������:     bal_DW_BigToLit
** ��������:   ����˴洢��4�ֽ�����ת��С��ģʽ�洢
** ����:       [in]  sptr:          Դ������
**             [in]  dptr:          Ŀ�껺����
** ����:       ˫��ֵ
********************************************************************/
INT8U yx_str_word_big2lit(INT8U *sptr, INT8U *dptr)
{
    dptr[0] = sptr[3];
    dptr[1] = sptr[2];
    dptr[2] = sptr[1];
    dptr[3] = sptr[0];
    return 4;
}

/*******************************************************************
** ������:     bal_HW_BigToLit
** ��������:   ����˴洢��2�ֽ�����ת��ΪС��ģʽ�洢
** ����:       [in]  sptr:          Դ������
**             [in]  dptr:          Ŀ�껺����
** ����:       ˫��ֵ
********************************************************************/
INT8U yx_str_short_big2lit(INT8U *sptr, INT8U *dptr)
{
    dptr[0] = sptr[1];
    dptr[1] = sptr[0];
    return 2;
}


