/*
* ������ʷ��
* 2007-11-02  qingxiaohai ���������������޹�˾
*                           �������ļ���
*/

/**
* @file  tsmapComm.h
* @brief
*       ���ܣ�
*       <li>tsmap�����������궨��</li>
*/


/************************ͷ�ļ�********************************/

/************************�궨��********************************/
#ifndef  _TSMAP_COMM_H
#define _TSMAP_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

#define SUCC         0
#define FAIL          -1

#define US unsigned short
#define UI unsigned int
#define UC unsigned char
#define UL unsigned long

#define  TSMAP_REGISTER               'r'
#define  TSMAP_UNREGISTER             'u'
#define  TSMAP_QUERY                  'q'
#define  TSMAP_EXCEPTION              'e'

#define  TSMAP_SPLIT_STR              ";"
#define   TSMAP_SPLIT_STR_LEN       1
#define   MAX_TBLNM_LEN                 100

#define  EINVCMD                            -2
#define  EISTFAIL                            -3             //�������ݱ�ʧ��
#define  ENOTPMT                           -4             //�������ظ�ע��
#define  ENEXIT                               -5            //������ƥ�������

#define  ESTR_SYS                         "E01;"        //ϵͳ�ڲ�����
#define  ESTR_INVCMD                  "E02;"        //��Ч�������ʽ
#define  ESTR_ISTFAIL                  "E03;"        //ϵͳ�������ݴ���
#define  ESTR_NOTPMT                 "E04;"        //�������ظ��洢����������ֻ������һ������
#define  ESTR_NEXIT                     "E05;"        //δ�ҵ�ƥ����


    /************************���Ͷ���******************************/

    /************************�ӿ�����******************************/

    /**
     * @brief        :  ͨ������ֵ��ȡ��Ӧ�Ļظ��ַ���
     * @para[IN]  :errno, ������
     * @return      : ���ش������Ӧ�Ļظ��ַ���
    */
    char* getAckStr(int eno);

    /**
    * ����: ��������16����ASCII�ַ�ת���ɶ�Ӧ�����֡�
    * @param ch    ������16����ASCII�ַ�
    * @return ����16�����ַ���Ӧ��ASCII��
    */
    int hex2num(unsigned char ch);

    /**
    * ����: ��16�����ַ���ת���ɶ�Ӧ��32λ������
    * @param ptr[OUT] ����ָ��16�����ַ�����ָ�룬ת��������ָ��ͬ��ǰ
    *        �������ת������ʱ��ָ�롣
    * @param intValue[OUT]   ת�����32λ����
    * @return ����ת����16�����ַ������ȡ�
    */
    unsigned int hex2int(char **ptr, int *intValue);

    /**
    * ����: ��intת����16���Ƶ��ַ���,���뱣֤size��С����ת������ַ�����С
    * @param ptr ����16�����ַ����Ļ�����
    * @param size   �������Ĵ�С
    * @param intValue   Ҫת��������
    * @return ת���Ĵ�С
    */
    unsigned int int2hex(char *ptr, int size,int intValue);

#ifdef __cplusplus
}
#endif

#endif

