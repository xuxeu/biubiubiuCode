/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-29         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 *@file:taUtil.h
 *@brief:
 *             <li>���ú�������</li>
 */
#ifndef _TAUTIL_H
#define _TAUTIL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************ͷ�ļ�********************************/
#include "taTypes.h"

/************************�궨��********************************/

/* ���㶨�� */
#define ZeroMemory(addr,size) (memset((void *)(addr),0,(size)))

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/
extern const UINT8 taHexChars[17];

/*
 * @brief:
 *      ��32λ����wValueת�����ֽ���������bpDestָ���Ŀ�ʼλ�ã�ת�����ֽ�������󳤶�ΪwBytes
 * @param[in]: value: ��ת��������
 * @param[in]: bytes: �ֽ�������󳤶�
 * @param[out]: dest: ����ֽ�������ʼ��ַ
 * @return:
 *     FALSE: ת��ʧ��
 *     TRUE: ת���ɹ�
 */
BOOL __store_long(INT32 value, UINT8 *dest, INT32 bytes);

/*
 * @brief:
 *      ��bpSource��Ϊ��ʼ��ַ���ֽ�����wBytes���ֽ�ת�����������浽bpDestָ����λ��
 * @param[in]: source: ����ֽ�������ʼ��ַ
 * @param[in]: bytes: �ֽ����ĳ���
 * @param[out]: dest: ָ��ת�����������ָ��
 * @return:
 *      FALSE: ת��ʧ��
 *      TRUE: ת���ɹ�
 */
BOOL __fetch_long(INT32 *dest, const UINT8* source, INT32 bytes);

/*
 * @brief:
 *      ��bpSource��Ϊ��ʼ��ַ���ֽ�����wBytes���ֽ�ת�����������浽bpDestָ����λ��
 * @param[in]: source: ����ֽ�������ʼ��ַ
 * @param[in]: bytes: �ֽ����ĳ���
 * @param[out]: dest: ָ��ת�����������ָ��
 * @return:
 *      FALSE: ת��ʧ��
 *      TRUE: ת���ɹ�
 */
BOOL __fetch_short(UINT16 *dest, const UINT8* source, INT32 bytes);

/*
 * @brief:
 *      ��������ʮ������ASCII�ַ�ת���ɶ�Ӧ������
 * @param[in]: ch: ��ת�����ַ�
 * @return:
 *      ת�����Ӧ������
 *      -1: ch��Ϊʮ������ASCII�ַ�
 */
INT32 hex2num(UINT8 ch);

/*
 * @brief:
 *     ��һ���ڴ����е�����ת����ʮ�������ַ���������ָ�����ڴ�λ����
 * @param[in]: mem: �ڴ���ʼ��ַ
 * @param[in]: count: ת�����ڴ�������
 * @param[out]: buf: ����ʮ�������ַ����Ļ������ĵ�ַ
 * @return:
 *     ��
 */
void mem2hex(const UINT8 *mem, UINT8 *buf, UINT32 count);

/*
 * @brief:
 *      ��һ���ڴ����е�����ת����ʮ�������ַ�����
 * @param: buf: �������ݺ�����ַ�����ŵ���ʼ��ַ
 * @param[in]: count: ת�����ڴ�������
 * @return:
 *      ��
 */
void mem2hexEx(UINT8 *buf, UINT32 count);

/*
 * @brief:
 *      ��ʮ�������ַ���ת���ɶ�Ӧ�����ݱ�����ָ�����ڴ�λ����
 * @param[in]: buf: ָ��ʮ�������ַ����Ļ�������ָ��
 * @param[in]: count: ת�����ڴ�������
 * @param[out]: mem: ת�������ݵ��ڴ���ʼ��ַ
 * @return:
 *      ת�������ݵ��ڴ���ʼ��ַ
 */
UINT8* hex2mem(const UINT8 *buf, UINT8 *mem, UINT32 count);

/*
 * @brief:
 *      ��һ���ڴ�����ʮ�������ַ���ת���ɶ�Ӧ�����ݱ�����ָ�����ڴ�λ����
 * @param[in]: buf: �����ַ�����������ݴ�ŵ���ʼ��ַ
 * @param[in]: count: ת�����ַ�������
 * @param[out]: outbuf: �����ַ�����������ݴ�ŵ���ʼ��ַ
 * @return:
 *     -1:ת��ʧ��
 *      0:ת���ɹ�
 */
INT32 hex2memEx(const UINT8 *buf, UINT8 *outbuf, UINT32 count);

/*
 * @brief:
 *      ��ʮ�������ַ���ת���ɶ�Ӧ32λ����
 * @param: ptr: ����ʱΪָ��ʮ�������ַ����׵�ַ��ָ�룬���ʱΪת������ʱ��ָ��
 * @param[out]: intValue: ָ��ת�����32λ������ָ��
 * @return:
 *      ת�����ַ���
 */
UINT32 hex2int(const UINT8 **ptr, UINT32 *intValue);

/*
 * @brief:
 *      ��ʮ�������ַ���ת���ɶ�Ӧ64λ����
 * @param: ptr: ����ʱΪָ��ʮ�������ַ����׵�ַ��ָ�룬���ʱΪת������ʱ��ָ��
 * @param[out]: intValue: ָ��ת�����64λ������ָ��
 * @return:
 *      ת�����ַ���
 */
UINT32 hex2int64(const UINT8 **ptr, UINT64 *intValue);

/*
 * @brief:
 *      ��16λ����hValueת�����ֽ���������bpDestָ��Ļ����У�ת�����ֽ�������󳤶�ΪwBytes
 * @param[in]: value: ��ת��������
 * @param[in]: bytes: �ֽ�������󳤶ȣ���<bpDest>ָ��Ļ������󳤶�
 * @param[out]: dest: ����ֽ�������ʼ��ַ
 * @return:
 *      FALSE: ת��ʧ��
 *      TRUE: ת���ɹ�
 */
BOOL __store_short(INT16 value, UINT8 *dest, INT32 bytes);

/*
 * @brief:
 *      ��ȡ16�����ַ�����'<'��'>'֮���ַ���������ַ�����λ��Ϊ'<'�򷵻�0��
 * @param ubpPtr: ����ʱΪָ��ʮ�������ַ����׵�ַ��ָ�룬���ʱΪת������ʱ��ָ��
 * @param[in]:len:�����ַ�����'<'��'>'֮���ַ�������󳤶�
 * @param[out]:strAddr: ָ��ת��������ַ����׵�ַ��ָ��
 * @return:
 *      ����ַ����ַ�����
 */
UINT32 hex2str(const UINT8 **ubpPtr, UINT8 **strAddr, UINT32 len);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TAUTIL_H */
