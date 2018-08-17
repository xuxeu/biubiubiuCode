
/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file: taCpuLib.c
 * @brief:
 *             <li>CPU��س�ʼ��</li>
 */

/************************ͷ �� ��******************************/
#include "taTypes.h"
#include "dbAsm.h"

/************************�� �� ��******************************/

typedef void (*DefaultExceptionHandler)(void);

/************************���Ͷ���*****************************/

/* �ж��������� */
typedef struct
{
    UINT32 low_offsets_bits : 16;
	UINT32 segment_selector : 16;
	UINT32 fixed_value_bits : 8;
	UINT32 gate_type	: 5;
	UINT32 privilege	: 2;
	UINT32 present		: 1;
	UINT32 high_offsets_bits: 16;
}T_Int_Desc;

/************************�ⲿ����*****************************/

/* ��ȡIDT�����ַ */
extern void taGetIdtBase(T_Int_Desc **table, UINT32 *limit);

/* �ϵ��쳣���� */
void taBpExceptionStub(void);

/* CPU�쳣������ں������� */
void exceptionEntry_0();
void exceptionEntry_1();
void exceptionEntry_2();
void exceptionEntry_3();
void exceptionEntry_4();
void exceptionEntry_5();
void exceptionEntry_6();
void exceptionEntry_7();
void exceptionEntry_8();
void exceptionEntry_9();
void exceptionEntry_10();
void exceptionEntry_11();
void exceptionEntry_12();
void exceptionEntry_13();
void exceptionEntry_14();
void exceptionEntry_16();
void exceptionEntry_17();
void exceptionEntry_18();
void exceptionEntry_19();

/************************ǰ������*****************************/

/************************ģ�����*****************************/

/* �쳣��ں����� */
static DefaultExceptionHandler exceptionEntryTable[] = 
{
exceptionEntry_0,
exceptionEntry_1,
exceptionEntry_2,
exceptionEntry_3,
exceptionEntry_4,
exceptionEntry_5,
exceptionEntry_6,
exceptionEntry_7,
exceptionEntry_8,
exceptionEntry_9,
exceptionEntry_10,
exceptionEntry_11,
exceptionEntry_12,
exceptionEntry_13,
exceptionEntry_14,
0,
exceptionEntry_16,
exceptionEntry_17,
exceptionEntry_18,
exceptionEntry_19,
};

/************************ȫ�ֱ���*****************************/

/************************����ʵ��*****************************/

/*
 * @brief:
 *      ��ȡ��ǰCS��ֵ
 * @return:
 *      CS�Ĵ�����ֵ
 */
static inline UINT16 taGetCS(void)
{
  register UINT16 segment = 0;

  asm volatile ( "movw %%cs,%0" : "=r" (segment) : "0" (segment) );

  return segment;
}

/*
 * @brief
 *    �����ж���������
 * @param[in] idtEntry: �ж�����������ָ��
 * @param[in] hdl: ƫ��λ
 * @param[in] type: �ж�������
 * @param[in] pri: ���ȼ�
 * @param[out] : ��
 * @return:
 *      ��
 */
static void taCreateInterruptGateDescriptor(T_Int_Desc* idtEntry, void* hdl, UINT8 type, UINT8 pri)
{
    idtEntry->low_offsets_bits    = (((UINT32) hdl) & 0xffff);
    idtEntry->segment_selector    = taGetCS();
    idtEntry->fixed_value_bits    = 0;
    idtEntry->gate_type           = type;
    idtEntry->privilege           = pri;
    idtEntry->present             = 1;
    idtEntry->high_offsets_bits   = ((((UINT32) hdl) >> 16) & 0xffff);
}

/*
 * @brief:
 *      IDT���ʼ��
 * @return:
 *      ��
 */
void taIDTInit(void)
{
    UINT32 i = 0;
    UINT32 limit = 0;
    T_Int_Desc idtEntry;
    T_Int_Desc *gdtBase = NULL;

    /* ��ȡIDT�Ļ���ַ */
    taGetIdtBase(&gdtBase, &limit);
    
    /* ��װ�쳣�� */
    for (i = 0; i < 20; i++)
    {
        if( i == 15 )
        {
            continue;
        }

        taCreateInterruptGateDescriptor(&idtEntry, exceptionEntryTable[i], 0xf,0); /* ������  */
        gdtBase[i] = idtEntry;
    }
}

/*
 * @brief:
 *     ��װ�쳣��������IDT����
 * @param[in] vector :�쳣��
 * @param[in] vectorHandle :�쳣������
 * @return:
 *     ��
 */
void taInstallVectorHandle(int vector, void *vectorHandle)
{
    UINT32 limit = 0;
    T_Int_Desc idtEntry;
    T_Int_Desc *gdtBase = NULL;
    
    if(vector > CONFIG_INTERRUPT_NUMBER_OF_VECTORS)
    {
        return;
    }
    
    /* ��ȡIDT�Ļ���ַ */
    taGetIdtBase(&gdtBase, &limit);
    
    if((vector == 1) || (vector == 3))
    {
        taCreateInterruptGateDescriptor(&idtEntry, vectorHandle,0xf,3); /* ������  */
    }
    else
    {
        taCreateInterruptGateDescriptor(&idtEntry, vectorHandle,0xf,0); /* ������  */
    }
    
    gdtBase[vector] = idtEntry;
}
	
/*
 * @brief:
 *     �쳣��ӡ
 * @param[in] context :�쳣������
 * @return:
 *     ��
 */
void taExceptionPrintk(int *regs)
{
    UINT32  edi = (*regs++);
    UINT32  esi = (*regs++);
    UINT32  ebp = (*regs++);
    UINT32  esp = (*regs++);
    UINT32  ebx = (*regs++);
    UINT32  edx = (*regs++);
    UINT32  ecx = (*regs++);
    UINT32  eax = (*regs++);
    UINT32  vector = (*regs++);
    UINT32  errCode = (*regs++);
    UINT32  pc = (*regs++);
    
    printk("edi:%x esi:%x ebp:%x esp:%x ebx:%x edx:%x ecx:%x eax:%x vector:%x errCode:%x pc:%x\n",edi,esi,ebp,esp,ebx
    ,edx,ecx,eax,vector,errCode,pc);
    while(1);
}

/*
 * @brief:
 *      Arch��س�ʼ��
 * @return:
 *      ��
 */
void taArchInit(void)
{
	/* ��ʼ��IDT�� */
	taIDTInit();
}
