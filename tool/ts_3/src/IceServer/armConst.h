/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: armConst.h,v $
  * Revision 1.5  2008/03/19 10:25:11  guojc
  * Bug ���  	0002430
  * Bug ���� 	ts���������Windows���������̣��޷���Ӧ��ݼ�
  *
  * ���룺������
  * ���ӣ���Т��,������
  *
  * Revision 1.1  2008/02/04 05:33:50  zhangxc
  * ��Ʊ������ƣ���ACoreOS CVS����ֿ����
  * ��д��Ա��������
  * ������Ա������F
  * ����������������ACoreOS�ֿ���ȡ�����µ�Դ���룬������ύ���µĲֿ�Ŀ¼�ṹ�й���
  * ��һ���ύsrcģ�顣
  *
  * Revision 1.4  2007/04/20 07:10:50  guojc
  * �����˶�ICE DCC���ܵ�֧��
  * �����˶�Ŀ��帴λ���ܵ�֧��
  * �����˶��ڴ��ȿ����õ�֧��
  * �����˶Ի���ģ�������Լ��ݵ�֧��
  *
  * Revision 1.3  2007/04/16 02:20:53  guojc
  * ����ICE Server��Э��semihosting������ֵ�һЩ����
  *
  * Revision 1.2  2007/04/02 02:56:53  guojc
  * �ύ���µ�IS���룬֧��������IDEʹ�ã�֧�������ļ�����
  *
  * Revision 1.1  2006/11/13 03:40:56  guojc
  * �ύts3.1����Դ��,��VC8�±���
  *
  * Revision 1.2  2006/07/03 07:18:20  tanjw
  * 1.����iceserver��gdb�������Ӻ�ִ��Ŀ����ϵ��ʼ���������յ�gdb����س����bug
  * 2.����ͬʱ�������iceserver��־�ļ������ͻ��bug
  * 3.����ts�����߳���־�ļ���¼��.�ɷ��ͺ��¼��Ϊ����ǰ��¼
  *
  * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
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
* @file 	armConst.h
* @brief 	
*     <li>���ܣ� ARM������������</li>
* @author 	������
* @date 	2006-02-20
* <p>���ţ�ϵͳ�� 
*/

#ifndef _ARM_CONST_H
#define _ARM_CONST_H


/************************************���ò���******************************************/
#include "sysTypes.h"
/************************************��������******************************************/


/************************************���岿��******************************************/

class RSP_ArmConst
{
public:
	
///�������Ĵ���ģʽ
T_MODULE T_CONST T_WORD ARM_USER_MODE = 16;
T_MODULE T_CONST T_WORD ARM_FIQ_MODE = 17;
T_MODULE T_CONST T_WORD ARM_IRQ_MODE = 18;
T_MODULE T_CONST T_WORD ARM_SUPER_MODE = 19;
T_MODULE T_CONST T_WORD ARM_ABORT_MODE = 23;
T_MODULE T_CONST T_WORD ARM_UNDEFINED_MODE = 27;
T_MODULE T_CONST T_WORD ARM_SYSTEM_MODE = 31;

T_MODULE T_CONST T_WORD CORE_RUNNING = 0;        ///�ں���������
T_MODULE T_CONST T_WORD CORE_DEBUG_DBGREQ = 1;   ///��������ʹ�ں�ֹͣ
T_MODULE T_CONST T_WORD CORE_DEBUG_BKWTPT = 2;   ///�ϵ�,�۲��ʹ�ں�ֹͣ
T_MODULE T_CONST T_WORD CORE_DEBUG_WTPTECP = 3;  ///�۲��with�쳣ʹ�ں�ֹͣ
T_MODULE T_CONST T_WORD CORE_POWER_OFF = 4;      ///����״̬
T_MODULE T_CONST T_WORD CORE_INVALID = 5;      ///	��ȷ��״̬


T_MODULE T_CONST T_WORD ARM_REG_NUM = 37;


//ARM37���Ĵ���
T_MODULE T_CONST T_WORD JTAG_R0	= 0;
T_MODULE T_CONST T_WORD JTAG_R1	= 1;
T_MODULE T_CONST T_WORD JTAG_R2	= 2;
T_MODULE T_CONST T_WORD JTAG_R3	= 3;
T_MODULE T_CONST T_WORD JTAG_R4	= 4;
T_MODULE T_CONST T_WORD JTAG_R5	= 5;
T_MODULE T_CONST T_WORD JTAG_R6	= 6;
T_MODULE T_CONST T_WORD JTAG_R7	= 7;
T_MODULE T_CONST T_WORD JTAG_R8	= 8;
T_MODULE T_CONST T_WORD JTAG_R9	= 9;
T_MODULE T_CONST T_WORD JTAG_R10 = 10;
T_MODULE T_CONST T_WORD JTAG_R11 = 11;
T_MODULE T_CONST T_WORD JTAG_R12 = 12;
T_MODULE T_CONST T_WORD JTAG_R13 = 13;				/* use this for the stack pointer */
T_MODULE T_CONST T_WORD JTAG_R14 = 14;				/* use this for link pointer	*/
T_MODULE T_CONST T_WORD JTAG_R15 = 15;				/* use this for the program counter */
T_MODULE T_CONST T_WORD JTAG_FIQ_R8 = 16;
T_MODULE T_CONST T_WORD JTAG_FIQ_R9 = 17;
T_MODULE T_CONST T_WORD JTAG_FIQ_R10 = 18;
T_MODULE T_CONST T_WORD JTAG_FIQ_R11 = 19;
T_MODULE T_CONST T_WORD JTAG_FIQ_R12	= 20;
T_MODULE T_CONST T_WORD JTAG_FIQ_R13	=	21;
T_MODULE T_CONST T_WORD JTAG_FIQ_R14	=	22;
T_MODULE T_CONST T_WORD JTAG_IRQ_R13	=	23;
T_MODULE T_CONST T_WORD JTAG_IRQ_R14	=	24;
T_MODULE T_CONST T_WORD JTAG_ABT_R13	=	25;
T_MODULE T_CONST T_WORD JTAG_ABT_R14	=	26;
T_MODULE T_CONST T_WORD JTAG_SVC_R13	=	27;		/* same as user mode */
T_MODULE T_CONST T_WORD JTAG_SVC_R14	=	28;
T_MODULE T_CONST T_WORD JTAG_UND_R13	=	29;
T_MODULE T_CONST T_WORD JTAG_UND_R14	=	30;
T_MODULE T_CONST T_WORD JTAG_CPSR		=	31;
T_MODULE T_CONST T_WORD JTAG_FIQ_SPSR	=	32;
T_MODULE T_CONST T_WORD JTAG_IRQ_SPSR	=	33;
T_MODULE T_CONST T_WORD JTAG_ABT_SPSR	=	34;
T_MODULE T_CONST T_WORD JTAG_SVC_SPSR	=	35;
T_MODULE T_CONST T_WORD JTAG_UND_SPSR	=	36;
T_MODULE T_CONST T_WORD JTAG_REG_CNTS	=	37;

T_MODULE T_CONST T_WORD RSP_REG_NUM = 26;  //rspЭ���мĴ���������

//��������ϵ��thumb��arm̬
T_MODULE T_CONST T_WORD  RSP_SB_ARM_STATE = 4;
T_MODULE T_CONST T_WORD  RSP_SB_THUMB_STATE = 2;


//��������
T_MODULE T_CONST T_WORD CMD_WRITE_MEM = 0;  ///д�ڴ�
T_MODULE T_CONST T_WORD CMD_READ_MEM = 1;  ///���ڴ�
T_MODULE T_CONST T_WORD CMD_READ_REG = 2;  ///���Ĵ���
T_MODULE T_CONST T_WORD CMD_WRITE_REG = 3;  ///д�Ĵ���
T_MODULE T_CONST T_WORD CMD_RUN_TRT = 4;  ///����Ŀ���
T_MODULE T_CONST T_WORD CMD_STOP_TRT = 5;  ///ֹͣĿ���
T_MODULE T_CONST T_WORD CMD_QUERY_TRT = 6;  ///��ѯĿ���
T_MODULE T_CONST T_WORD CMD_INSERT_HBP = 7;  ///����Ӳ���ϵ�
T_MODULE T_CONST T_WORD CMD_REMOVE_HBP = 8;  ///ɾ��Ӳ���ϵ�
T_MODULE T_CONST T_WORD CMD_WRITE_INT_MEM = 9;  ///д�����ڴ�
T_MODULE T_CONST T_WORD CMD_INSERT_WBP = 10;  ///����۲��
T_MODULE T_CONST T_WORD CMD_REMOVE_WBP = 11;  ///ɾ���۲��
T_MODULE T_CONST T_WORD CMD_INIT_CP15 = 12; //����CP15�Ĵ���
T_MODULE T_CONST T_WORD CMD_QUERY_ICEENDIAN = 13; //��ѯICE��С��
/*ICE 3.2 ����[begin]*/
T_MODULE T_CONST T_WORD CMD_CREATE_SESSION = 14; //�����Ự
T_MODULE T_CONST T_WORD CMD_DELETE_SESSION = 15; //ɾ���Ự
T_MODULE T_CONST T_WORD CMD_RESET_TARGET = 16; //����Ŀ���
T_MODULE T_CONST T_WORD CMD_QUERY_TARGET_CPUID = 17; //��ѯĿ���CPUID
T_MODULE T_CONST T_WORD CMD_GET_CP15  = 18; //��ȡCP15�Ĵ���
T_MODULE T_CONST T_WORD CMD_OPEN_DCC_FUNCTION = 19; //����DCC����
//T_MODULE T_CONST T_WORD CMD_DELETE_SESSION = 20; //ɾ���Ự
/*ICE3.2����[end]*/

T_MODULE T_CONST T_WORD CMD_NEED_ICE = 100;  ///�Ƿ���Ҫ��ICE�����Ľ���
T_MODULE T_CONST T_WORD CMD_REPLY_WRITE_MEM = 101;  ///�ظ�GDBд�ڴ�
T_MODULE T_CONST T_WORD CMD_REPLY_S05 = 102;  ///�ظ�GDBS05
T_MODULE T_CONST T_WORD CMD_UPDATE_READ_REG = 103;  ///���¼Ĵ�����ֵ
T_MODULE T_CONST T_WORD CMD_UPDATE_READ_MEM = 104;  ///�����ڴ滺������ֵ
T_MODULE T_CONST T_WORD CMD_REPLY_READ_MEM = 105;  ///�ظ�GDB���ڴ�
T_MODULE T_CONST T_WORD CMD_REPLY_STOP_TRT = 106;  ///����ֹͣĿ������
T_MODULE T_CONST T_WORD CMD_SET_TRT_STATUS = 107;  ///����Ŀ���״̬
T_MODULE T_CONST T_WORD CMD_ANALY_RESULT = 108;  ///��������ִ�н��
T_MODULE T_CONST T_WORD CMD_DELAY = 109;  ///�����ӳ٣���ʼ��ʱ�ӵ�ʱ����Ҫ
T_MODULE T_CONST T_WORD CMD_UPDATE_TRT_STATUS = 110;  ///���ݲ�ѯĿ����������Ŀ���״̬�����ƶ�����
T_MODULE T_CONST T_WORD CMD_INSERT_SBP = 111;  ///��������ϵ�
T_MODULE T_CONST T_WORD CMD_SIMPLE_REPLY_GDB = 112;  ///�򵥻ظ�GDB�������
T_MODULE T_CONST T_WORD CMD_ANALY_INSERT_SBP = 113; //������������ϵ������Ƿ�ɹ�
T_MODULE T_CONST T_WORD CMD_UPDATE_CPSR = 114; //����CPSR
/*ICE 3.2 ����[begin]*/
T_MODULE T_CONST T_WORD CMD_ANALY_QUERY_ICEENDIAN = 115; //������ѯICE��С�˽��
T_MODULE T_CONST T_WORD CMD_ANALY_CREATE_SESSION = 116; //���������Ự���
T_MODULE T_CONST T_WORD CMD_ANALY_DELETE_SESSION = 117; //����ɾ���Ự���
T_MODULE T_CONST T_WORD CMD_ANALY_RESET_TARGET  = 118; //��������Ŀ������
T_MODULE T_CONST T_WORD CMD_REPLY_IDE_RESET_TARGET = 119;  // �ظ�IDE����Ŀ����Ľ��
T_MODULE T_CONST T_WORD CMD_ANALY_QUER_TARGET_CPUID = 120; //������ѯCPUID���
T_MODULE T_CONST T_WORD CMD_ANALY_BP_READ_MEM = 121; //��������ϵ�Ķ��ڴ�����������ϵ�
T_MODULE T_CONST T_WORD CMD_ANALY_SIMIHOST_BREAK = 122; //����simihost�ϵ���
T_MODULE T_CONST T_WORD CMD_ANALY_OPEN_DCC = 123; //��������DCC���ܵĽ��
/*ICE3.2����[end]*/

};


#endif