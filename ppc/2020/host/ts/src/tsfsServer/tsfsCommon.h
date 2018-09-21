/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  tsfsCommon.h
* @brief  
*       ���ܣ�
*       <li>�������ֵ��������</li>
*/
/************************ͷ�ļ�********************************/
#ifndef _TSFSCOMMON_H
#define _TSFSCOMMON_H

#include "stdafx.h"
#include "windows.h"
#include "tsfsLog.h"
#include "..\tsApi\tclientapi.h"
#include "tsfsServer.h"
#include "tsfsWorker.h"

/******************************* �������� ************************************/

//����ʹ��ȫ��Server����
extern TSFS_Server *pTSFS_Server;

/************************�궨��********************************/
//���õ���ģʽ
//#define DEBUG

//TSMAP AID
#define TSFS_TSMAP_ID                   1  

//�ظ�����
#define TSFS_SUCCESS                    "OK"                

#define TSFS_SERVER_EXIT                0       //TSFS Server�����˳�

#define TSFS_SEND_REGDATA_FAILED        -1      //��TSMAP����ע�����ݰ�ʧ��
#define TSFS_DID_NOT_LOGOUT             -2      //û����TSMAPע��
#define TSFS_LOGOUT_FAILED              -3      //��TSMAPע��ʧ��
#define TSFS_REGISTER_FAILED            -4      //��TSMAPע��ʧ��
#define TSFS_INVALID_INIT_ARGUMENT      -5      //��ʼ����������
#define TSFS_CONNECT_TS_FAILED          -6      //����TSʧ��
#define TSFS_GET_PID_FAILED             -7      //��ȡ����̨PIDʧ��
#define TSFS_CREATE_SERVER_FAILED       -8      //����Serverʧ��
#define TSFS_SEND_LOGOUT_DATA_FAILED    -9      //��TSMAP����ע�����ݰ�ʧ��

//��������
const char TSFS_CONTROL_FLAG[] = ";";
const char TSFS_COMM_CONTROL_FLAG[] =  ":";
const char TSFS_FILE_NAME_BEGIN[] = "<";
const char TSFS_FILE_NAME_END[] = ">";

//֧�ֽӿ�����
#define TSFS_OPEN_FILE       'o'
#define TSFS_CLOSE_FILE      'c'
#define TSFS_READ_FILE       'r'
#define TSFS_WRITE_FILE      'w'
#define TSFS_CREATE_FILE     'n'
#define TSFS_DELETE_FILE     'd'
#define TSFS_IO_CONTROL      'i'

//IO��������
#define TSFS_FIONREAD           1
#define TSFS_FIOSEEK            7
#define TSFS_FIOWHERE           8
#define TSFS_FIORENAME          10
#define TSFS_FIOMKDIR           31
#define TSFS_FIORMDIR           32
#define TSFS_FIOREADDIR         37
#define TSFS_FIOFSTATGETBYFD    38
#define TSFS_FIOFSTATGETBYNAME  39
#define TSFS_FIOTRUNC           43
#define TSFS_FIOTIMESET         45  
#define TSFS_FIOFSTATFSGET      47
#define TSFS_FIOCHECKROOT       49

//acoreos flag
#define	_FAPPEND	0x0008	/* append (writes guaranteed at the end) */
#define	_FCREAT		0x0200	/* open with file create */
#define	_FTRUNC		0x0400	/* open with truncation */
#define	_FEXCL		0x0800	/* error on open if file exists */
#define _FBINARY    0x10000
#define _FTEXT      0x20000


/************************�ӿ�����********************************/
/**
 * ����: ��int64ת����16���Ƶ��ַ���, ���뱣֤size��С����ת������ַ�����С
 * @param ptr ����16�����ַ����Ļ�����
 * @param size   �������Ĵ�С
 * @param intValue   Ҫת��������
 * @return ת���Ĵ�С
*/
unsigned int intToHex(char *ptr,  int size, __int64 intValue);

/**
* ����: ��16�����ַ���ת���ɶ�Ӧ��32λ������
* @param ptr[out] ����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
*        �������ת������ʱ��ָ�롣
* @param intValue[out]   ת�����32λ����
* @return ����ת����16�����ַ������ȡ�
*/
unsigned int hexToInt(char **ptr, int *intValue);

/**
* ����: ��16�����ַ���ת���ɶ�Ӧ��64λ������
* @param ptr[out] ����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
*        �������ת������ʱ��ָ�롣
* @param intValue[out]   ת�����64λ����
* @return ����ת����16�����ַ������ȡ�
*/
unsigned int hexToInt64(char **ptr, __int64 *intValue);

/**
* ����: ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
* @param ch    ������16����ASCII�ַ�
* @param intValue[out]   ת�����32λ����
* @return ����ת����16�����ַ������ȡ�
*/
int hexToNum(unsigned char ch);

/**
 * @brief
 *   �˳�TSFS Server
 * @param CtrlType[in]:    �ػ��ź�
 */
BOOL WINAPI tsfs_serverExit( DWORD CtrlType );

/**
 * @brief
 *   �˳�TSFS Server
 * @param signal[in]:    �ź�,δʹ��
 */
void tsfs_serverSignalExit( int signal );

#endif

