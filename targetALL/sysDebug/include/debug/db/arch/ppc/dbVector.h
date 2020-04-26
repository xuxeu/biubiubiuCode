/*
* �����ʷ��
* 2013-01-22  ���        ��˫б�ܵ�ע�ͷ�ʽ�޸�Ϊб���ǵ�ע�ͷ�ʽ��
* 2012-05-25  ��ѧǿ      ��Ծ�̬���������Ժ�ֵ�������š�(BUG6540)
* 2012-04-21  ��ѧǿ      ���Ӵ����ʱ�����ж��쳣���������ĵĿռ���ʼ��ַ�ꡣ(BUG6549)
* 2010-08-30  ������      �����ļ���
*/

/*
* @file 	vectorPpc.h
* @brief
*	   ���ܣ�
*      <li>����ppcCPU��صĺꡢ���ݽṹ�������ӿڵ�</li>
*/

#ifndef _VECTORPPC_H
#define _VECTORPPC_H

#ifdef __cplusplus
 extern "C" {
#endif

/*�궨��*/
/*�쳣�ź�x86ƥ��*/
/* #define GENERAL_ZDIV_INT      0 */ /*������0�쳣*/
#define GENERAL_DB_INT          (1) /*���Ե����쳣*/
#define GENERAL_NMI_INT         (2) /*�������ж�,ppc�쳣���Ƿ������ж�*/
#define GENERAL_TRAP_INT        (3) /* ����ϵ��쳣��0700:  trap program exception */
/* #define GENERAL_OVERF_INT     4 */ /*�������*/
/* #define GENERAL_BOUND_INT     5 */ /*Boundָ�ΧԽ��*/
#define GENERAL_ILLINS_INT 	    (6)/*�Ƿ��������쳣*/
/* #define GENERAL_DEV_INT 		 7 */ /*�豸�������쳣*/
/* #define GENERAL_DF_INT 		 8 */ /*˫�����쳣*/
/* #define GENERAL_CSO_INT 		 9 */ /*Э�������ϳ���*/
/* #define GENERAL_TSS_INT 		 10 */ /*�Ƿ�TSS(Task State Segment)�쳣*/
/* #define GENERAL_SEGMENT_INT	 11 */ /*�β������쳣*/
/* #define GENERAL_SF_INT 		 12 */ /*ջ�����쳣*/
/* #define GENERAL_GP_INT 		 13 */ /*һ�㱣���쳣*/
#define GENERAL_PAGE_INT  		(14)/*ҳ�����쳣*/
#define GENERAL_IPAGE_INT  		(15)/*��ȡָ����ɵ�ҳ�����쳣*/
#define GENERAL_MF_INT			(16)/*�����쳣 0700:  IEEE floating-point enabled program exception*/
#define GENERAL_AE_INT          (17)/*��������쳣 0600:  ALIGNMENT���� */
#define GENERAL_MC_INT          (18)/*Machine Check�쳣 0200:  �������*/
#define GENERAL_SIMD_INT 		(19)/*SIMD �����쳣 */

/* ppc�������쳣 */
#define GENERAL_RST_INT         (20) /* 0100:  ϵͳ��λ*/
#define GENERAL_DAE_INT         (21) /* 0300:  DSI�쳣������PPC750����HASH�������⴦�� */
#define GENERAL_PRIVI_INT       (22) /* 0700:  privileged instruction program exception */
#define GENERAL_IAE_INT         (23) /* 0400:  ISI�쳣������PPC750����HASH�������⴦�� */
#define GENERAL_UF_INT          (24) /* 0800:  ���㲻�����쳣 */
#define GENERAL_DEC_INT         (25) /* 0900:  DEC�жϣ�DECһ�����ڼ�ʱ��ϵͳ���ڸ��ж�ֱ�ӷ��ء� */
#define GENERAL_PM_INT          (26) /* 0F00:  ���ܼ�� */
#define GENERAL_HBP_INT         (27) /* 0300:  Ӳ�����ݶϵ��쳣 */

/*����ppc��������쳣*/
#define GENERAL_ITM_INT        (28) /* 1000:  ָ��ת����ʧ������PPC603e����TLB�������⴦��*/
#define GENERAL_DLTM_INT       (29) /* 1100:  ���ݼ���ת����ʧ������PPC603e����TLB�������⴦��*/
#define GENERAL_DSTM_INT       (30) /* 1200:  ���ݴ洢ת����ʧ   ������PPC603e����TLB�������⴦��*/
#define GENERAL_IAB_INT        (31) /* 1300:  Ӳ��ָ��ϵ��쳣 */

#define GENERAL_SM_INT         (0x83) /* 1400:  ϵͳ�����ж� */
#define GENERAL_TH_INT         (0x84) /* 1700:  �¶ȹ����ж� */
/*����ppc��������쳣*/

#define GENERAL_SC_INT          (0x80) /* 0C00:  ϵͳ�����ж�*/
#define GENERAL_CALLLIB_INT		(0x81) /* 0C00:  CALLLIB�����ж�*/
#define GENERAL_CALLLIB_ERROR_INT		(0x82) /* 0C00:  CALLLIB����ʧ���ж�*/
#define GENERAL_ILLSC_INT       (0x85)    /* 0C00:  ϵͳ̬�·�������ϵͳ�����ж�*/

#define GENERAL_STD_IRQ_LAST   (32) /* ͨ���쳣�ŵ����޺�x86ͳһ������ppc�����쳣�������� */

/*ʹ�ô˵�ַ����MSLӳ���OSӳ����Ҫʹ�õ��쳣��ʱջ�ռ�*/
#define EXP_TMP_STACK_BASE (0xBA0)

/**************PPC�쳣����*********************************/
 //#define Reserved 0x00000
 #define EXC_SYSTEM_RESET 						0x00100
 #define EXC_MACHINE_CHECK 						0x00200
 #define EXC_DSI 								0x00300
 #define EXC_ISI 								0x00400
 #define EXC_EXTERNAL_INTERRUPT 				0x00500
 #define EXC_ALIGNMENT 							0x00600
 #define EXC_PROGRAM 							0x00700
 #define EXC_FLOATING-POINT_UNAVAILABLE 		0x00800
 #define EXC_DECREMENTER 						0x00900
 //#define RESERVED 0x00A00�C00BFF
 #define EXC_SYSTEM_CALL 						0x00C00
 #define EXC_TRACE 								0x00D00
 #define EXC_PERFORMANCE_MONITOR 				0x00F00
 #define EXC_ALTIVEC_UNAVAILABLE 				0x00F20
 #define EXC_ITLB_MISS 							0x01000
 #define EXC_DTLB_MISS_ON_LOAD 					0x01100
 #define EXC_DTLB_MISS_ON_STORE 				0x01200
 #define EXC_INSTRUCTION_ADDRESS_BREAKPOINT 	0x01300
 #define EXC_SYSTEM_MANAGEMENT_INTERRUPT 		0x01400
 #define EXC_ALTIVEC_ASSIST 					0x01600
/*
  * ��ΪEContext_Control�ṹ�еĳ�ԱfpscrΪdouble�ͣ����Խṹ��С����Ϊ8�ֽڶ���
  */
#define EXCEPTION_FRAME_SIZE  (0xB0)
#ifdef __cplusplus
}
#endif


#endif


