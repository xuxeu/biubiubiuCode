/* string.h - string library header file */



#ifndef __INCstringh
#define __INCstringh
#include "floatio.h"
#include "stddef.h"
#include "limits.h"
#include "stdarg.h"

#ifdef __cplusplus
extern "C" {
#endif


/* 字长拷贝长度  */
#if LONG_MAX == 2147483647L
#define LBLOCKSIZE 4
#else
#if LONG_MAX == 9223372036854775807L
#define LBLOCKSIZE 8
#else
#error long int is not a 32bit or 64bit type.
#endif
#endif

/* 如果X没按字长对齐，则返回非0.  */
#define ALIGNED(X) \
  (((unsigned long)(X) & (LBLOCKSIZE - 1)) == 0)
  
/* 如果X没按字长对齐，则返回非0  */
#define UNALIGNED(X) ((unsigned long)(X) & (LBLOCKSIZE - 1))

/* 如果X或Y没按字对齐，则返回非0  */
#define UNALIGNED_2(X, Y) \
  (((unsigned long)(X) & (LBLOCKSIZE - 1)) | ((unsigned long)(Y) & (LBLOCKSIZE - 1)))

/* 进行按字长查找的阈值  */
#define TOO_SMALL(LEN)  ((LEN) < LBLOCKSIZE)

/* 如果X包含NULL字符，则返回非0. */
#if LONG_MAX == 2147483647L
#define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)
#else
#if LONG_MAX == 9223372036854775807L
#define DETECTNULL(X) (((X) - 0x0101010101010101) & ~(X) & 0x8080808080808080)
#else
#error long int is not a 32bit or 64bit type.
#endif
#endif

#ifndef DETECTNULL
#error long int is not a 32bit or 64bit byte
#endif

/* 字长拷贝长度  */
#define LITTLEBLOCKSIZE LBLOCKSIZE

char *strcat(char *s1, const char *s2);
char *strcpy(char *dst0, const char *src0);
size_t strlen(const char *str);

/*
 * @brief: 
 *    从源区域拷贝指定长度字节到目的区域。
 * @param[in]: b1: 拷贝源起始地址
 * @param[in]: length: 拷贝长度
 * @param[out]: b2: 拷贝目的起始地址
 * @return: 
 *    无
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @implements: DR.1.1.11
 */
void bcopy(const char *b1, char *b2, size_t length);

/**
 * @req
 * @brief: 
 *    从源区域拷贝指定长度字节到目的区域(带重叠的源区域和目的区域拷贝)。
 * @param[in]: src_void: 拷贝源起始地址
 * @param[in]: length: 拷贝长度
 * @param[out]: dst_void: 拷贝目的起始地址
 * @return: 
 *    拷贝目的起始地址。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查拷贝源和目的起始地址、拷贝长度的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.3
 */
void *memmove(void *dst_void, const void *src_void, size_t length);

/**
 * @req
 * @brief: 
 *    按指定长度将指定字符数组的元素填充为指定字符。
 * @param[in]: c: 填充字符
 * @param[in]: n: 填充长度
 * @param[out]: m: 填充目的起始地址
 * @return: 
 *    填充目的起始地址。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查填充目的起始地址、填充字符和填充长度的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.4
 */
void *memset(void *m, int c, size_t n);
/**
 * @req
 * @brief: 
 *    从源区域拷贝指定长度字节到目的区域。
 * @param[in]: src0: 拷贝源起始地址
 * @param[in]: len0: 拷贝长度
 * @param[out]: dst0: 拷贝目的起始地址
 * @return: 
 *    拷贝目的起始地址。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查拷贝源和目的起始地址、拷贝长度的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.2
 */
void *memcpy(void *dst0, const void *src0, size_t len0);

/**
 * @req
 * @brief: 
 *    依次比较指定的两个字符串的每个字符。
 * @param[in]: s1: 第一个字符串起始地址
 * @param[in]: s2: 第二个字符串起始地址
 * @return: 
 *    0: 两个字符串长度及每个字符都相等。
 *    负值: s1指定的字符串中字符的ASCII码小于s2指定的字符串中对应字符的ASCII码。
 *    正值: s1指定的字符串中字符的ASCII码大于s2指定的字符串中对应字符的ASCII码。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查第一个字符串和第二个字符串起始地址的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.7
 */
int strcmp(const char *s1, const char *s2);

/**
 * @req
 * @brief:
 *    按指定长度依次比较指定的两个字符串的字符。	
 * @param[in]: s1: 第一个字符串起始地址
 * @param[in]: s2: 第二个字符串起始地址
 * @param[in]: n: 比较长度 
 * @return: 
 *    0: 两个字符串前n个长度的每个字符都相等。
 *    负值: s1前n个长度的字符串中字符的ASCII码小于s2前n个长度的字符串中对应
 *          字符的ASCII码。
 *    正值: s1前n个长度的字符串中字符的ASCII码大于s2前n个长度的字符串中对应
 *          字符的ASCII码。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes: 
 *    函数中不检查第一个字符串和第二个字符串起始地址的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.10 
 */
int strncmp(const char *s1, const char *s2, size_t n);

/*
 * @brief: 
 *    按指定个数在指定的字符数组的中查找第一个等于指定字符的元素。
 * @param[in]: src_void: 查找源起始地址
 * @param[in]: c: 查找字符
 * @param[in]: length: 查找长度
 * @return: 
 *    第一个等于指定字符的元素地址。<br>
 *    NULL: 没有找到指定字符。<br>
 * @qualification method: 分析/测试
 * @derived requirement: 否
 * @function location: Internal
 * @implements: DR.1.20
 */
void *memchr(const void *src_void, int c, size_t length);


/**
 * @req
 * @brief: 
 *    从指定的字符串中查找第一个等于指定字符的字符。
 * @param[in]: s1: 查找源字符串起始地址
 * @param[in]: i: 查找字符
 * @return: 
 *    第一个等于指定字符的字符的地址。
 *    NULL: 在字符串中没有找到指定字符。
 * @qualification method: 分析
 * @derived requirement: 否
 * @function location: API
 * @notes:
 *    函数中不检查源字符串起始地址和查找字符的有效性。<br>
 *    字长的用语参考RR.1注释。<br>
 * @implements: DR.1.6 
 */
char *strchr(const char *s1, int i);

#ifdef __cplusplus
}
#endif

#endif /* __INCstringh */
