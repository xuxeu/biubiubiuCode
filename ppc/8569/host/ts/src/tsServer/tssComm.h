/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file             :  tssComm.h
 * @Version        :  1.0.0
 * @brief           :  *   <li> ����</li>tsmap�����������궨��
                                <li>���˼��</li> 
                                <p> n     ע�����n</p> 
 *   
 * @author        :  zhangxu
 * @create date:  2008��9��24�� 15:14:57
 * @History        : 
 *   
 */


#ifndef  _TSSYMMAN_COMM_H
#define _TSSYMMAN_COMM_H


#include "sysTypes.h"


#define SUCC    0
#define FAIL     -1

#define MAX_PATH_LEN    260


#define TARGET_ARCH_X86  0
#define TARGET_ARCH_PPC  2
#define TARGET_ARCH_ARM  1
#define TARGET_ARCH_MAX  3

#define TOOL_CHAIN_VER_296   0
#define TOOL_CHAIN_VER_344   1
#define TOOL_CHAIN_VER_MAX   2

#define DES_TA_MANAGER_ID (0)
#define TS_DEFAULT_TIMEOUT 128



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
 * @Funcname: ExtendESC
 * @brief        : ��չ�ļ�ȫ·�����е�\\�ַ�����һ��\��չΪ\\
 * @para[IN|OUT]   : pString�ļ�ȫ·����
 * @para[IN]   : ������峤��
 * @return      :��չ�Ƿ�ɹ�
 * @Author     : zhangxu
 *  
 * @History: 1.  Created this function on 2008��9��24�� 13:34:11
 *   
**/
extern int ExtendESC(char* pString, int len);

/**
 * @Funcname: ResumeBlank
 * @brief        : ���ַ����е�Э��ASC��ָ�Ϊ�ո�
 * @para[IN|OUT]   : pString�ַ���
 * @return      :�ָ��Ƿ�ɹ�
 * @Author     : tangxp
 *  
 * @History: 1.  Created this function on 2009��2��12�� 13:34:11
 *   
**/
extern int ResumeBlank(char* pString);

#endif


