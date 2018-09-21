
/*
* @file 	cpu.c
* @brief
*       ���ܣ�
*       <li>��ģ����Ҫ�ṩcpu�Ĺ���</li>
*/

/* ͷ�ļ� */
#include <taTypes.h>


/**************************** ȫ�ֱ������� *********************************/

/**************************** �ⲿ���� *********************************/

/**************************** �궨�� *********************************/

/**************************** ���Ͷ��� *********************************/

/*************************** ǰ���������� ******************************/


BOOL taArchInitFlag = FALSE;

void *mmPageDirBase = NULL;   /* ����hash��ʱʹ�õ�ҳĿ¼��ַָ�� */


/* �ⲿ���� */
extern void *_Core_Space_MMU_Base;
extern void *_Core_Space;

/*
* @brief
*       ��ʼ��BAT�Ĵ���
* @param[in] bat: Ҫд��ĳ�ֵ
* @return
*       none
* @implements  DM.1.2.74,DM.1.3.74,DM.1.4.74
*/
extern void taArchInitializeBat( UINT32 bat );

/* ʵ�� */


/*
* @brief
*       ��ʼ����������
* @return
*       none
* @implements  DM.1.2.93,DM.1.3.93,DM.1.4.93
*/
void taCpuInitialize( void )
{
    /* ��ʼ��MMUҳת��ʹ�õ�hash��ռ� */
//    _msl_arch_init_hashtable();
    
    /* ��ʼ��BAT�Ĵ��� */
	taArchInitializeBat(0);
}

/*
* @brief
*       ����CPU�жϡ�
* @param[out]    level:  ָ��״̬�Ĵ���ֵ��ָ��
* @return
*       none
* @implements  DM.1.2.1,DM.1.3.1,DM.1.4.1
*/
void taCpuInterruptDisable( UINT32 *level )
{
    UINT32 disableMask = 0x8000;
    UINT32 tmpLevel = 0;

    /*
    * ���浱ǰ״̬�Ĵ�����ֵ��*level��ͨ����MSR�Ĵ����е�
    * EEλ����Ϊ0�ر��ж�
    */
    asm volatile ("mfmsr %0; sync; andc %1,%0,%1; sync; isync; mtmsr %1; sync; isync" :
    "=&r" ((tmpLevel)), "=&r" ((disableMask)) :
    "0" ((tmpLevel)), "1" ((disableMask)));
    *level = tmpLevel;
}

/*
* @brief
*       ʹ��CPU�жϡ�
* @param[in]    level:  ״̬�Ĵ�����ֵ���������Ӧ��
*                       _msl_cpu_interrupt_disable���ʹ�ò�ʹ�����ȡ����ֵ��
* @return
*       none
* @implements  DM.1.2.2,DM.1.3.2,DM.1.4.2
*/
void taCpuInterruptEnable( UINT32 level )
{
    UINT32 tmpLevel = 0;

    /* ���浱ǰ״̬�Ĵ�����ֵ��tmpLevel */
    asm volatile ("mfmsr %0; sync;" :
    "=&r" ((tmpLevel))  :
    "0" ((tmpLevel)));

    /*  ��EEλ*/
    tmpLevel &= ~0x8000;

    /* ֻ��ȡEEλ��������λ */
    level &= 0x8000;

    /*  ָ��״̬�Ĵ�����ֵ */
    tmpLevel |= level;

    /* �ָ�tmpLevelָ����״̬�Ĵ���ֵ����ǰ״̬�Ĵ���MSR */
    asm volatile ( "mtmsr %0; sync; isync" :    "=r" ((tmpLevel)) : "0" ((tmpLevel)));
}

/*
* @brief
*       ���ٿ���һ��CPU�жϡ�
* @param[in]    level:  ״̬�Ĵ�����ֵ���������Ӧʹ��
*                       _msl_cpu_interrupt_disable��ȡ����ֵ��
* @return
*       none
* @implements  DM.1.2.3,DM.1.3.3,DM.1.4.3
*/
void taCpuInterruptOpt(UINT32 level )
{
    UINT32 disableMask = 0x8000;
    UINT32 tmpLevel = 0;    

    /* ���浱ǰ״̬�Ĵ�����ֵ��tmpLevel */
    asm volatile ("mfmsr %0; sync;" :
    "=&r" ((tmpLevel))  :
    "0" ((tmpLevel)));
    
    /*  ��EEλ*/
    tmpLevel &= ~0x8000;

    /* ֻ��ȡEEλ��������λ */
    level &= 0x8000;

    /*  ָ��״̬�Ĵ�����ֵ */
    tmpLevel |= level;    
    
    /*
    * ͨ���ָ�tmpLevelָ����״̬�Ĵ���ֵ����ǰ״̬�Ĵ���MSR�����ж�;
    * ͨ����MSR�Ĵ����е�EEλ����Ϊ0�ر��ж�
    */
    asm volatile ("mtmsr %0; sync; isync; andc %1,%0,%1; mtmsr %1; sync; isync" :
    "=r" ((tmpLevel)), "=r" ((disableMask)) :
    "0" ((tmpLevel)), "1" ((disableMask)));
}

/*
 * @brief:
 *      Arch��س�ʼ��
 * @return:
 *      ��
 */
void taArchInit(void)
{
	/*��ʼ��MSL�쳣ջ*/
	taExceptionstackInit();

	/* ��ʼ�������� */
	taInitVectorTable();
}


/*
*ˢcache
*/
void CPU_FlushCaches(unsigned int addr, unsigned int len)
{

}
