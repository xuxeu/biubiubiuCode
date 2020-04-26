/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: lambdaIce.h,v $
  * Revision 1.4  2008/03/19 10:25:11  guojc
  * Bug ���  	0002430
  * Bug ���� 	ts���������Windows���������̣��޷���Ӧ��ݼ�
  *
  * ���룺������
  * ���ӣ���Т��,������
  *
  * Revision 1.1  2008/02/04 05:33:49  zhangxc
  * ��Ʊ������ƣ���ACoreOS CVS����ֿ����
  * ��д��Ա��������
  * ������Ա������F
  * ����������������ACoreOS�ֿ���ȡ�����µ�Դ���룬������ύ���µĲֿ�Ŀ¼�ṹ�й���
  * ��һ���ύsrcģ�顣
  *
  * Revision 1.3  2007/04/20 07:10:50  guojc
  * �����˶�ICE DCC���ܵ�֧��
  * �����˶�Ŀ��帴λ���ܵ�֧��
  * �����˶��ڴ��ȿ����õ�֧��
  * �����˶Ի���ģ�������Լ��ݵ�֧��
  *
  * Revision 1.2  2007/04/02 02:56:53  guojc
  * �ύ���µ�IS���룬֧��������IDEʹ�ã�֧�������ļ�����
  *
  * Revision 1.1  2006/11/13 03:40:56  guojc
  * �ύts3.1����Դ��,��VC8�±���
  *
  * Revision 1.1.1.1  2006/04/19 06:29:13  guojc
  * no message
  *
  * Revision 1.2  2006/04/14 02:02:27  guojc
  * �����˶�CP15Э�������Ĵ����޸ĵ�֧��
  *
  * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
  * TS��������
  *
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * �����ļ�;�����ļ��Ļ����ӿ�
  */

/**
* @file     lambdaIce.h
* @brief     
*     <li>���ܣ� ICEԭ�Ӳ���Э���װ��</li>
* @author     ������
* @date     2006-02-20
* <p>���ţ�ϵͳ�� 
*/

#ifndef _LAMBDA_ICE_H
#define _LAMBDA_ICE_H


/************************************���ò���******************************************/
#include "sysTypes.h"         ///�����ض���
/************************************��������******************************************/

/************************************���岿��******************************************/
class RSP_TcpServer;

/**
* @brief
*    RSP_LambdaIce��ICE��ICE Server�ṩ��ԭ�Ӳ��������Э���װ���������ṩICE�ܹ��ṩ��һϵ��ԭ�Ӳ�������\n
*   ������װ����ICE����ԭ�Ӳ����Ŀ������ݰ���ͨ��RX�̷߳��͵�ICE\n
* @author     ������
* @version 1.0
* @date     2006-02-20
* <p>���ţ�ϵͳ�� 
*/
class RSP_LambdaIce
{
public:
    /**
    * @brief 
    *     ����: ���÷���������
    * @param[in] vtpServer ����������ָ��
    */
    T_VOID SetServer(RSP_TcpServer *vtpServer);
    
    /**
    * @brief 
    *     ����:��ѯICE��С��
    * @param[in] cpBuf ������,��Ҫ���͸�ICE��һЩ��Ϣ
    */
    T_VOID QueryTargetEndian(int IceID);

    /**
    * @brief 
    *     ����:ɾ����ICE�ĻỰ
    * @param[in] IceID ICE��ID��
    */
    T_VOID DeleteSeesion(int IceID);
    /**
    * @brief 
    *     ����:������ICE�ĻỰ
    * @param[in] cpBuf ������,��Ҫ���͸�ICE��һЩ��Ϣ
    */
    T_VOID CreateSeesion(T_CHAR *cpBuf, T_WORD cpBufSize, int IceID);
    /**
    * @brief 
    *     ����: ��װд�ڴ���ư�
    * @param[in] wAddr �ڴ��ַ
    * @param[in] cpBuf ������
    * @param[in] wOffset ƫ�Ƶ�ַ
    * @param[in] wSize Ҫд���ڴ��С
    */
    T_VOID WriteMemory(T_WORD wAddr, T_CHAR *cpBuf, T_WORD wOffset, T_WORD wSize);
    
    /**
    * @brief 
    *     ����: ��װ���ڴ���ư�
    * @param[in] wAddr �ڴ��ַ
    * @param[in] wSize Ҫ�����ڴ��С
    */
    T_VOID ReadMemory(T_WORD wAddr, T_WORD wSize);
    
    /**
    * @brief 
    *     ����: ��װд���мĴ������ư�
    * @param[in] waRegs �Ĵ�������
    * @param[in] waRegs �Ĵ������鳤��
    */
    T_VOID WriteAllRegisters(T_WORD *waRegs, T_WORD wNum);
    
    /**
    * @brief 
    *     ����: ��װ�����мĴ������ư�
    */
    T_VOID ReadAllRegisters();

    /**
    * @brief 
    *     ����: ��װ��������Ŀ������ư�
    */
    T_VOID ContinueTarget();

    /**
    * @brief 
    *     ����: ��װֹͣĿ������ư�
    */
    T_VOID StopTarget();
    /**
    * @brief 
    *     ����: ��װ����Ŀ������ư�
    *  @param[in]: US resetType ����������
    *   resetType��0  CPUӲ����λ��ֻ�ǽ�Ŀ���CPU����Ӳ����λ������Ŀ�����Χ�豸���и�λ��
    *   resetType��1  ����Ӳ����λ��ֻ�ǽ�Ŀ������ΧӲ���豸���и�λ������CPU���и�λ��
    *   resetType��2  CPU����ͬʱӲ����λ��
    */
    T_VOID ReSetTarget(T_UWORD resetType);
    
    /**
    * @brief 
    *     ����: ��װ��ѯĿ���CPUID CODE
    */
    T_VOID QueryCPUIDCode();
    
    /**
    * @brief 
    *     ����: ��װ��ѯĿ������ư�
    */
    T_VOID QueryTarget();
    
    /**
    * @brief 
    *     ����: ��װ��DCC���ܵĿ��ư�
    */
    T_VOID OpenDccFunction(T_WORD dccAddr);

    /**
    * @brief 
    *     ����: ��װ����ϵ��۲��
    * @param[in] type �ϵ�����
    *                           0������ϵ�
    *                           1��Ӳ���ϵ�
    *                           2��д�۲��
    *                           3�����۲��
    *                           4�����ʹ۲��
    * @param[in] wAddr �ϵ��ַ
    * @param[in] wWidth �ϵ���
    */
    T_VOID InsertBreakPointORWatchPoint(T_WORD type, T_WORD wAddr, T_WORD wWidth);
    /**
    * @brief 
    *     ����: ��װɾ���ϵ��۲��
    * @param[in] type �ϵ�����
    *                           0������ϵ�
    *                           1��Ӳ���ϵ�
    *                           2��д�۲��
    *                           3�����۲��
    *                           4�����ʹ۲��
    * @param[in] wAddr �ϵ��ַ
    * @param[in] wWidth �ϵ���
    */
    T_VOID DeleteBreakPointORWatchPoint(T_WORD type, T_WORD wAddr, T_WORD wWidth);
#if 0
    T_VOID InsertBreakPoint(T_WORD wAddr, T_WORD wWidth);
    /**
    * @brief 
    *     ����: ��װ����۲����ư�
    * @param[in] wAddr �۲���ַ
    * @param[in] wWidth �۲����
    * @param[in] wAccess �۲������
    */
    T_VOID InsertWatchPoint(T_WORD wAddr, T_WORD wWidth, T_WORD wAccess);

    /**
    * @brief 
    *     ����: ��װɾ���۲����ư�
    * @param[in] wAddr �۲���ַ
    * @param[in] wWidth �۲����
    * @param[in] wAccess �۲������
    */
    T_VOID RemoveWatchPoint(T_WORD wAddr, T_WORD wWidth, T_WORD wAccess);
    /**
    * @brief 
    *     ����: ��װɾ��Ӳ���ϵ���ư�
    * @param[in] wAddr �ϵ��ַ
    * @param[in] wWidth �ϵ���
    */
    T_VOID RemoveBreakPoint(T_WORD wAddr, T_WORD wWidth);

#endif

    /**
    * @brief 
    *     ����: ��װд�ڴ���ư�
    * @param[in] wAddr �ڴ��ַ
    * @param[in] cpBuf ������
    * @param[in] wOffset ƫ�Ƶ�ַ
    * @param[in] wSize Ҫд���ڴ��С
    * @param[in] wDelay ��ʱʱ�䳤��
    */
    T_VOID WriteMemoryWithDelay(T_WORD wAddr, T_CHAR *cpBuf, T_WORD wOffset, T_WORD wSize, T_WORD wDelay);


    /**
    * @brief 
    *     ����: ��װдCP15�Ĵ����Ŀ��ư�
    * @param[in] wNum CP15�Ĵ�����
    * @param[in] wValue �Ĵ���ֵ
    */
    T_VOID WriteCp15Reg(T_WORD wNum, T_WORD wValue);

    /**
    * @brief 
    *     ����: ��װ��CP15�Ĵ����Ŀ��ư�
    * @param[in] wNum CP15�Ĵ�����
    * @param[in] wValue �Ĵ���ֵ
    */
    T_VOID ReadCp15Reg(T_WORD wNum);
    
private:
    T_BOOL m_iceBigEndian;

    RSP_TcpServer *m_server;

    /*
    * ����: ��intת����16���Ƶ��ַ���,���뱣֤size��С����ת������ַ�����С
    * @param[out] vcpPtr ����16�����ַ����Ļ�����
    * @param[in] vwSize   �������Ĵ�С
    * @param[in] vwIntValue   Ҫת��������
    * @return ת���Ĵ�С
    */
    T_UWORD Int2hex(T_CHAR *vcpPtr, T_WORD vwSize,T_WORD vwIntValue);
};


#endif