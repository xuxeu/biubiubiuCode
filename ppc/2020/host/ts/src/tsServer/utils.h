/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	utils.h
 * @brief	���ú����⺯������
 * @author 	����F
 * @date 	2007��12��17��
 * @History
 */

/**
 * @brief       �������в������л�ȡָ��ѡ���Ӧ���ַ�������
 * @param[in]	argc  ��������
 * @param[in]	argv  ��������
 * @param[in]	opt   ѡ��
 * @return      �����ɹ�����ָ���ַ���������ָ�룬����ʧ�ܷ���NULL
**/
char* ArgGetStr(int argc, char* argv[],char* opt);

/**
 * @brief       �������в������л�ȡָ��ѡ���Ӧ����������
 * @param[in]	argc  ��������
 * @param[in]	argv  ��������
 * @param[in]	opt   ѡ��
 * @return      �����ɹ���������������Ӧ������������ʧ�ܷ���-1
**/
int ArgGetInt(int argc, char* argv[],char* opt);

/**
 * @brief       ȷ�������в��������Ƿ����ĳ��ѡ��
 * @param[in]	argc  ��������
 * @param[in]	argv  ��������
 * @param[in]	opt   ѡ��
 * @return      ���ڸ�ѡ���TRUE��ʧ�ܷ���FALSE
**/
int ArgInclude(int argc, char* argv[],char* opt);

/**
 * @brief       ���ת����С��
 * @param[in]	cnt  ��ת�������size
 * @param[in]	ptr  ָ��ת�������ָ��
 * @return      ��
**/
void big2little(int cnt, void *ptr);

/**
 * @brief       ��ˢ�µĿ���̨��ӡ
 * @param[in]	form  ��ӡ��ʽ����printf��ȫһ��
 * @param[in]	...  �ɱ������
 * @return      ��
**/
void SL_Print(char *form,...);

int __fetch_long(int *dest, char* source, int bytes);

int IsSameWord( char *str1, char *str2 );
int FindItem( char *ItemStr, int argc, char *argv[] );

int fnUtility_ParaGetInt( char *ItemStr, int argc,char *argv[] );
char* fnUtility_ParaGetStr( char *ItemStr, int argc, char *argv[] );

