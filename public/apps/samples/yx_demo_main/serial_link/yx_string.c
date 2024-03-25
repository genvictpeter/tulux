/********************************************************************************
**
**  文件名称:  yx_string.c
**  版权所有:  CopyRight @ Xiamen Yaxon NetWork CO.LTD. 2019
**  文件描述:  字符转换/查找工具模块
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   | 修改记录
**===============================================================================
**| 2019-12-11 | 夏普   | 创建本模块
*********************************************************************************/

#include "yx_string.h"

/*******************************************************************
** 函数名:     _yx_char_isupper
** 功能描述:   将小写字母ASCII转化为大写
** 参数:       [in]  ch:            要转化的字符
** 返回:       转化之后的字符
********************************************************************/
static INT8U _yx_char_isupper(INT8U ch)
{
    if (ch >= 'a' && ch <= 'z') {
        ch = 'A' + (ch - 'a');
    }
    return ch;
}

/*******************************************************************
** 函数名:     _yx_char_cmp
** 功能描述:   两个字符比较
** 参数:       [in]  matchcase:     不为1则忽略大小写，为1则按原始数据比较
**             [in]  ch1:           字符1
**             [in]  ch2:           字符2
** 返回:       比较结果 STR_EQUAL相等 STR_GREAT前者较大 STR_LESS前者较小
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
** 函数名:     yx_mem_cmp
** 功能描述:   两串数据缓存比较内容大小
** 参数:       [in]  ptr1:          缓存1
**             [in]  ptr2:          缓存2
**             [in]  len:           长度
** 返回:       比较结果 STR_EQUAL相等 STR_GREAT前者较大 STR_LESS前者较小
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
** 函数名:     yx_mem_findbyte
** 功能描述:   在一块数据缓存中查找一个字符
** 参数:       [in]  ptr:           数据缓存指针
**             [in]  len:           数据缓存长度
**             [in]  cb:            索要查找的字符
** 返回:       TRUE or FALSE
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
** 函数名:     yx_mem_findbyte
** 功能描述:   在一块数据缓存中查找一个字符串
** 参数:       [in]  bptr:          数据缓存指针
**             [in]  blen:          数据缓存长度
**             [in]  sptr:          索要查找的字符串
**             [in]  slen:          字符串长度
** 返回:       TRUE or FALSE
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
** 函数名:     yx_str_searchkeyword_case
** 函数描述:   搜索字符串，可以区分大小写，在s1中搜索是否包含有s2
** 参数:       [in]  matchcase:     TRUE-区分大小写，FALSE-不区分大小写
**             [in]  s1：           原字符串
**             [in]  len1：         s1长度
**             [in]  s2：           要查找的字符串
**             [in]  len2：         s2长度
** 返回:       成功返回TRUE，失败返回FALSE
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
        if (matchcase == FALSE) {                                              /* 不区分大小写 */
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
** 函数名:     yx_str_searchkeywordfromhead
** 功能描述:   检测目标数据缓冲区开头是否与字符串匹配
** 参数:       [in]  ptr:           缓冲区大小
**             [in]  maxlen:        缓冲区长度
**             [in]  sptr:          字符串指针
** 返回:       TRUE or FALSE
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
** 函数名:     yx_str_searchdigitalstring
** 功能描述:   在字符串中查找10进制数字，以第numflag个flagchar出现为结束标志
** 参数:       [in]  ptr:           缓冲区大小
**             [in]  maxlen:        缓冲区长度
**             [in]  flagchar:      标志字符
**             [in]  numflag:       标志字符出现次数
** 返回:       找到的数字。未找到则返回0xFFFF
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
** 函数名:     yx_str_searchdigitalstring
** 功能描述:   在字符串中查找16进制数字，以第numflag个flagchar出现为结束标志
** 参数:       [in]  ptr:           缓冲区大小
**             [in]  maxlen:        缓冲区长度
**             [in]  flagchar:      标志字符
**             [in]  numflag:       标志字符出现次数
** 返回:       找到的数字。未找到则返回0xFFFF
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
** 函数名:     yx_str_searchstring
** 功能描述:   在缓冲区中读出第一个flagchar和第numflag个之间的数据，长度必须小于limitlen
** 参数:       [in]  dptr:          读出缓存区指针
**             [in]  limitlen:      最大读出长度
**             [in]  sptr:          查找的缓存区指针
**             [in]  maxlen:        查找的缓存区长度
**             [in]  flagchar:      查找的字符
**             [in]  numflag:       以字符出现第numflag的位置为结束标志
** 返回:       实际长度。
** 注意:      如果长度大于limitlen，仍会读出数据，但返回长度为0
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
** 函数名:     yx_str_searchdigitalstring
** 功能描述:   在字符串中查找第numflag个flagchar出现的位置
** 参数:       [in]  sptr:          缓冲区大小
**             [in]  maxlen:        缓冲区长度
**             [in]  flagchar:      标志字符
**             [in]  numflag:       标志字符出现次数
** 返回:       找到的地址指针，未找到返回NULL
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
** 函数名:     yx_str_hex2char
** 功能描述:   将16以内的hex转为ASCII字符
** 参数:       [in]  sbyte:         所要转化的hex
** 返回:       ASCII字符
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
** 函数名:     yx_str_char2hex
** 功能描述:   将16以内的ASCII字符转为hex
** 参数:       [in]  schar:         所要转化的字符
** 返回:       hex
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
** 函数名:     yx_str_hex2ascii
** 功能描述:   将一串hex转为可见ASCII码字符
** 参数:       [out] dptr:          输出指针
**             [in]  sptr:          要转化的数据缓存
**             [in]  len:           要转化的数据长度
** 返回:       转化后的长度
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
** 函数名:     yx_str_ascii2hex
** 功能描述:   将一串ASCII码转为hex
** 参数:       [out] dptr:          输出指针
**             [in]  sptr:          要转化的数据缓存
**             [in]  len:           要转化的数据长度
** 返回:       转化后的长度
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
** 函数名:     yx_str_dec2ascii
** 功能描述:   将一个长整形数据转为ASCII码
** 参数:       [out] dptr:          输出指针
**             [in]  data:          要转化的长整形数据
**             [in]  len:           输出的最小数据长度，如果比实际转化之后的长度大，则会在前面补'0'
** 返回:       实际输出长度
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
** 函数名:     yx_str_ascii2dec
** 功能描述:   将一串ASCII码转化为一个长整形数据
** 参数:       [in]  sptr:          要转化的数据指针
**             [in]  len:           要转化的数据长度
** 返回:       长整形数据
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
** 函数名:     yx_str_bcd_to_hex
** 功能描述:   将bcd转为hex
** 参数:       [in]  mask:          掩码
**             [in]  bcd:           要转化的数据
** 返回:       转化之后的数据
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
** 函数名:     yx_str_hex_to_bcd
** 功能描述:   将hex转为bcd
** 参数:       [in]  hex:           要转化的数据
** 返回:       转化之后的数据
********************************************************************/
INT8U yx_str_hex_to_bcd(INT8U hex)
{
    INT8U val;
    
    val = ((hex / 10) << 4) + (hex % 10);
    return val;
}

/*******************************************************************
** 函数名:     yx_str_long2char_msb
** 功能描述:   将长整型数按大端模式转换为4个字节，即高位字节在低地址
** 参数:       [in]  dest:         输出指针
**             [in]  src:          需转换的数据
** 返回:       无
********************************************************************/
void yx_str_long2char_msb(INT8U *dest, INT32U src)
{
    dest[0] = (INT8U)(src >> 24);
    dest[1] = (INT8U)(src >> 16);
    dest[2] = (INT8U)(src >> 8);
    dest[3] = (INT8U)(src);
}

/*******************************************************************
** 函数名:     yx_str_long2char_lsb
** 功能描述:   将长整型数按小端模式转换为4个字节，即高位字节在高地址
** 参数:       [in]  dest:          输出指针
**             [in]  src:           需转换的数据
** 返回:       无
********************************************************************/
void yx_str_long2char_lsb(INT8U *dest, INT32U src)
{
    dest[0] = (INT8U)(src);
    dest[1] = (INT8U)(src >> 8);
    dest[2] = (INT8U)(src >> 16);
    dest[3] = (INT8U)(src >> 24);
}

/*******************************************************************
** 函数名:     yx_str_char2long_msb
** 功能描述:   将4个字节按大端模式转换为长整型, 即高位字节在低地址
** 参数:       [in]  src:           需转换的数据指针
** 返回:       长整形数据
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
** 函数名:     yx_str_char2long_lsb
** 功能描述:   将4个字节按大端模式转换为长整型, 即高位字节在低地址
** 参数:       [in]  src:           需转换的数据指针
** 返回:       长整形数据
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
** 函数名:     yx_str_short2char_msb
** 功能描述:   将短整型数按大端模式转换为2个字节，即高位字节在低地址
** 参数:       [in]  dest:         输出指针
**             [in]  src:          需转换的数据
** 返回:       无
********************************************************************/
void yx_str_short2char_msb(INT8U *dest, INT16U src)
{
    dest[0] = (INT8U)(src >> 8);
    dest[1] = (INT8U)(src);
}

/*******************************************************************
** 函数名:     yx_str_short2char_lsb
** 功能描述:   将短整型数按小端模式转换为2个字节，即高位字节在高地址
** 参数:       [in]  dest:         输出指针
**             [in]  src:          需转换的数据
** 返回:       无
********************************************************************/
void yx_str_short2char_lsb(INT8U *dest, INT16U src)
{
    dest[0] = (INT8U)(src);
    dest[1] = (INT8U)(src >> 8);
}

/*******************************************************************
** 函数名:     yx_str_char2short_msb
** 功能描述:   将2个字节按大端模式转换为短整型, 即高位字节在低地址
** 参数:       [in]  src:           需转换的数据指针
** 返回:       短整型数据
********************************************************************/
INT16U yx_str_char2short_msb(INT8U *src)
{
    INT16U ret = 0;
    
    ret = (INT16U)((INT16U) (src[0]) << 8);
    ret |= (INT16U)(src[1]);
    return ret;
}

/*******************************************************************
** 函数名:     yx_str_char2short_lsb
** 功能描述:   将2个字节按小端模式转换为短整型, 即高位字节在高地址
** 参数:       [in]  src:           需转换的数据指针
** 返回:       短整型数据
********************************************************************/
INT16U yx_str_char2short_lsb(INT8U *src)
{
    INT16U ret = 0;
    
    ret = (INT16U)((INT16U)(src[1]) << 8);
    ret |= (INT16U)(src[0]);
    return ret;
}

/*******************************************************************
** 函数名:     bal_DW_BigToLit
** 函数描述:   将大端存储的4字节数据转换小端模式存储
** 参数:       [in]  sptr:          源缓冲区
**             [in]  dptr:          目标缓冲区
** 返回:       双字值
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
** 函数名:     bal_HW_BigToLit
** 函数描述:   将大端存储的2字节数据转换为小端模式存储
** 参数:       [in]  sptr:          源缓冲区
**             [in]  dptr:          目标缓冲区
** 返回:       双字值
********************************************************************/
INT8U yx_str_short_big2lit(INT8U *sptr, INT8U *dptr)
{
    dptr[0] = sptr[1];
    dptr[1] = sptr[0];
    return 2;
}


