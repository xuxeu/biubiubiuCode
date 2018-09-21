/*
* ������ʷ��
* 2008-09-24 zhangxu  ���������������޹�˾
*                        �������ļ���
*/

/**
* @file  tsSymManComm.h
* @brief
*       ���ܣ�
*       <li>>tsSymManComm�����������궨��</li>
*/

/************************ͷ�ļ�********************************/


#ifndef  _TSSYMMAN_COMM_H
#define _TSSYMMAN_COMM_H

#include "sysTypes.h"

/************************�궨��********************************/
#define MAX_PATH_LEN    260

/************************���Ͷ���******************************/

/************************�ӿ�����******************************/

/**
* ����: ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
* @param ch    ������16����ASCII�ַ�
* @return ����16�����ַ���Ӧ��ASCII��
*/
extern int hex2num(unsigned char ch);

/**
* ����: ��16�����ַ���ת���ɶ�Ӧ��32λ������
* @param ptr[OUT] ����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
*        �������ת������ʱ��ָ�롣
* @param intValue[OUT]   ת�����32λ����
* @return ����ת����16�����ַ������ȡ�
*/
extern unsigned int hex2int(char **ptr, int *intValue);

/**
* ����: ��intת����16���Ƶ��ַ���,���뱣֤size��С����ת������ַ�����С
* @param ptr ����16�����ַ����Ļ�����
* @param size   �������Ĵ�С
* @param intValue   Ҫת��������
* @return ת���Ĵ�С
*/
extern unsigned int int2hex(char *ptr, int size,int intValue);

/**
 * @Funcname: CheckFolderExist
 * @brief        : ���Ŀ¼�Ƿ����
 * @para[IN]   : strPathĿ¼��
 * @return      : Ŀ¼�������
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008��9��24�� 13:34:11
 *
**/
extern int CheckFolderExist(char* strPath);

/**
 * @Funcname: CheckFileExist
 * @brief        : ����ļ��Ƿ����
 * @para[IN]   : strPath�ļ���
 * @return      : �ļ��������
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008��9��24�� 13:34:11
 *
**/
extern int CheckFileExist(char *strFile);

/**
 * @Funcname: DeleteDirectory
 * @brief        : ɾ��Ŀ¼(������Ŀ¼�ͷǿ�Ŀ¼)
 * @para[IN]   : strPathĿ¼��
 * @return      : ɾ���Ƿ�ɹ�
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008��9��24�� 13:34:11
 *
**/
extern int DeleteDirectory(char *DirName);

/**
 * @Funcname: DeleteFolderAllFile
 * @brief        : ɾ���ļ����������ļ�
 * @para[IN]   : strPathĿ¼��
 * @return      : ɾ���Ƿ�ɹ�
 * @Author     : zhangxu
 *
 * @History: 1.  Created this function on 2008��9��24�� 13:34:11
 *
**/
extern int DeleteFolderAllFile(char *strPath);



#endif

