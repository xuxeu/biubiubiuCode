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


/* �ֳ���������  */
#if LONG_MAX == 2147483647L
#define LBLOCKSIZE 4
#else
#if LONG_MAX == 9223372036854775807L
#define LBLOCKSIZE 8
#else
#error long int is not a 32bit or 64bit type.
#endif
#endif

/* ���Xû���ֳ����룬�򷵻ط�0.  */
#define ALIGNED(X) \
  (((unsigned long)(X) & (LBLOCKSIZE - 1)) == 0)
  
/* ���Xû���ֳ����룬�򷵻ط�0  */
#define UNALIGNED(X) ((unsigned long)(X) & (LBLOCKSIZE - 1))

/* ���X��Yû���ֶ��룬�򷵻ط�0  */
#define UNALIGNED_2(X, Y) \
  (((unsigned long)(X) & (LBLOCKSIZE - 1)) | ((unsigned long)(Y) & (LBLOCKSIZE - 1)))

/* ���а��ֳ����ҵ���ֵ  */
#define TOO_SMALL(LEN)  ((LEN) < LBLOCKSIZE)

/* ���X����NULL�ַ����򷵻ط�0. */
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

/* �ֳ���������  */
#define LITTLEBLOCKSIZE LBLOCKSIZE

char *strcat(char *s1, const char *s2);
char *strcpy(char *dst0, const char *src0);
size_t strlen(const char *str);

/*
 * @brief: 
 *    ��Դ���򿽱�ָ�������ֽڵ�Ŀ������
 * @param[in]: b1: ����Դ��ʼ��ַ
 * @param[in]: length: ��������
 * @param[out]: b2: ����Ŀ����ʼ��ַ
 * @return: 
 *    ��
 * @qualification method: ����/����
 * @derived requirement: ��
 * @function location: Internal
 * @implements: DR.1.1.11
 */
void bcopy(const char *b1, char *b2, size_t length);

/**
 * @req
 * @brief: 
 *    ��Դ���򿽱�ָ�������ֽڵ�Ŀ������(���ص���Դ�����Ŀ�����򿽱�)��
 * @param[in]: src_void: ����Դ��ʼ��ַ
 * @param[in]: length: ��������
 * @param[out]: dst_void: ����Ŀ����ʼ��ַ
 * @return: 
 *    ����Ŀ����ʼ��ַ��
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes: 
 *    �����в���鿽��Դ��Ŀ����ʼ��ַ���������ȵ���Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.3
 */
void *memmove(void *dst_void, const void *src_void, size_t length);

/**
 * @req
 * @brief: 
 *    ��ָ�����Ƚ�ָ���ַ������Ԫ�����Ϊָ���ַ���
 * @param[in]: c: ����ַ�
 * @param[in]: n: ��䳤��
 * @param[out]: m: ���Ŀ����ʼ��ַ
 * @return: 
 *    ���Ŀ����ʼ��ַ��
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes: 
 *    �����в�������Ŀ����ʼ��ַ������ַ�����䳤�ȵ���Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.4
 */
void *memset(void *m, int c, size_t n);
/**
 * @req
 * @brief: 
 *    ��Դ���򿽱�ָ�������ֽڵ�Ŀ������
 * @param[in]: src0: ����Դ��ʼ��ַ
 * @param[in]: len0: ��������
 * @param[out]: dst0: ����Ŀ����ʼ��ַ
 * @return: 
 *    ����Ŀ����ʼ��ַ��
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes: 
 *    �����в���鿽��Դ��Ŀ����ʼ��ַ���������ȵ���Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.2
 */
void *memcpy(void *dst0, const void *src0, size_t len0);

/**
 * @req
 * @brief: 
 *    ���αȽ�ָ���������ַ�����ÿ���ַ���
 * @param[in]: s1: ��һ���ַ�����ʼ��ַ
 * @param[in]: s2: �ڶ����ַ�����ʼ��ַ
 * @return: 
 *    0: �����ַ������ȼ�ÿ���ַ�����ȡ�
 *    ��ֵ: s1ָ�����ַ������ַ���ASCII��С��s2ָ�����ַ����ж�Ӧ�ַ���ASCII�롣
 *    ��ֵ: s1ָ�����ַ������ַ���ASCII�����s2ָ�����ַ����ж�Ӧ�ַ���ASCII�롣
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes: 
 *    �����в�����һ���ַ����͵ڶ����ַ�����ʼ��ַ����Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.7
 */
int strcmp(const char *s1, const char *s2);

/**
 * @req
 * @brief:
 *    ��ָ���������αȽ�ָ���������ַ������ַ���	
 * @param[in]: s1: ��һ���ַ�����ʼ��ַ
 * @param[in]: s2: �ڶ����ַ�����ʼ��ַ
 * @param[in]: n: �Ƚϳ��� 
 * @return: 
 *    0: �����ַ���ǰn�����ȵ�ÿ���ַ�����ȡ�
 *    ��ֵ: s1ǰn�����ȵ��ַ������ַ���ASCII��С��s2ǰn�����ȵ��ַ����ж�Ӧ
 *          �ַ���ASCII�롣
 *    ��ֵ: s1ǰn�����ȵ��ַ������ַ���ASCII�����s2ǰn�����ȵ��ַ����ж�Ӧ
 *          �ַ���ASCII�롣
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes: 
 *    �����в�����һ���ַ����͵ڶ����ַ�����ʼ��ַ����Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.10 
 */
int strncmp(const char *s1, const char *s2, size_t n);

/*
 * @brief: 
 *    ��ָ��������ָ�����ַ�������в��ҵ�һ������ָ���ַ���Ԫ�ء�
 * @param[in]: src_void: ����Դ��ʼ��ַ
 * @param[in]: c: �����ַ�
 * @param[in]: length: ���ҳ���
 * @return: 
 *    ��һ������ָ���ַ���Ԫ�ص�ַ��<br>
 *    NULL: û���ҵ�ָ���ַ���<br>
 * @qualification method: ����/����
 * @derived requirement: ��
 * @function location: Internal
 * @implements: DR.1.20
 */
void *memchr(const void *src_void, int c, size_t length);


/**
 * @req
 * @brief: 
 *    ��ָ�����ַ����в��ҵ�һ������ָ���ַ����ַ���
 * @param[in]: s1: ����Դ�ַ�����ʼ��ַ
 * @param[in]: i: �����ַ�
 * @return: 
 *    ��һ������ָ���ַ����ַ��ĵ�ַ��
 *    NULL: ���ַ�����û���ҵ�ָ���ַ���
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @notes:
 *    �����в����Դ�ַ�����ʼ��ַ�Ͳ����ַ�����Ч�ԡ�<br>
 *    �ֳ�������ο�RR.1ע�͡�<br>
 * @implements: DR.1.6 
 */
char *strchr(const char *s1, int i);

#ifdef __cplusplus
}
#endif

#endif /* __INCstringh */
