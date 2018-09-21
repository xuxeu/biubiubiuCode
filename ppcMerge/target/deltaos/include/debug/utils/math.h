/************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * �޸���ʷ��
 * 2011-02-23         ���࣬�����������ɼ������޹�˾
 *                               �������ļ���
 * 2011-04-15	  ���£������������ɼ������޹�˾
 * 				  ��������ʽ���Ż����롢�޸�GBJ5369Υ���
*/

/*
 * @file�� math.h
 * @brief��
 *	    <li>������ѧ����صĺ궨��ͽӿ�������</li>
 * @implements: DR.2
 */

#ifndef _MATH_H
#define _MATH_H

/************************ͷ�ļ�********************************/
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/************************�궨��********************************/
#define ZERO 	0
#define REAL 	1
#define INTEGER 2
#define INF 	3
#define NAN 	4


/************************���Ͷ���******************************/
/************************�ӿ�����******************************/


/**
 * @req
 * @brief: 
 *    �зָ�������
 * @param[in]: value: ���зֵĸ�����
 * @param[out]: pIntPart: ����value����������
 * @return: 
 *    ����С�����֣����������<value>��ͬ��
 * @qualification method: ����
 * @derived requirement: ��
 * @function location: API
 * @implements: DR.2.16
 */
double tamodf(double value, double *pIntPart);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

