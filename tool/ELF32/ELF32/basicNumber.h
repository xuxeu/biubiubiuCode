 /**************************************************************************
 *
 *                   �����������ɼ������޹�˾        ��Ȩ����
 *  CopyRight (C)  2000-2010   Coretek System Inc. All Rights Reserved
 *
***************************************************************************/

/*
 * �޸���ʷ��
 *        2010-8-4        amwyga        �����������ɼ������޹�˾
 *        �޸ļ�����:�������ļ�
 *********************************************************************
 */
 
 /**
 * @file  basicNumber.h
 * @brief
 *       <li>���ܣ��������ݶ��壬�������ֵ����Сֵ��</li>
 */

#ifndef BASICNUMBER_H_
#define BASICNUMBER_H_

/************************ ���ò��� ****************************/

/************************�� �� ��******************************/

    #ifndef TRUE
    #define TRUE        1
    #endif
    
    #ifndef FALSE
    #define FALSE        0
    #endif
    
    #ifndef NULL
    #define NULL        0
    #endif
    
    #define MAX_INT8    (0x7f)
    #define MAX_UINT8    (0xff)
    #define MIN_INT8    (0x80)
    #define MIN_UINT8    (0)
    
    #define MAX_HINT16    (0x7fff)
    #define MAX_UINT16    (0xffff)
    #define MIN_INT16    (0x8000)
    #define MIN_UINT16    (0)
    
    #define MAX_INT32    (0x7fffffff)
    #define MAX_UINT32    (0xffffffff)
    #define MIN_INT32    (0x80000000)
    #define MIN_UINT32    (0)
    
    #define MAX_INT64    (0x7fffffffffffffff)
    #define MAX_UINT64    (0xffffffffffffffff)
    #define MIN_INT64    (0x8000000000000000)
    #define MIN_UINT64    (0)
    
    #define MAX_FLOAT    (3.4E38)
    #define MIN_FLOAT    (3.4E-38)
    
    #define MIN_DOUBLE (1.7E-308)
    #define MAX_DOUBLE (1.7E308)
    
    #define KB_1 (0x400)
    #define KB_4 (0x1000)
    #define MB_1 (0x100000)
    #define GB_1 (0x40000000)
    
/*********************** �������� *****************************/

/*********************** ���岿�� ****************************/

#endif /*BASICTYPES_H_*/
