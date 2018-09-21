
/*
* @file 	cpu.c
* @brief
*       功能：
*       <li>本模块主要提供cpu的管理。</li>
*/

/* 头文件 */
#include <taTypes.h>


/**************************** 全局变量部分 *********************************/

/**************************** 外部声明 *********************************/

/**************************** 宏定义 *********************************/

/**************************** 类型定义 *********************************/

/*************************** 前向声明部分 ******************************/


BOOL taArchInitFlag = FALSE;

void *mmPageDirBase = NULL;   /* 更新hash表时使用的页目录地址指针 */


/* 外部声明 */
extern void *_Core_Space_MMU_Base;
extern void *_Core_Space;

/*
* @brief
*       初始化BAT寄存器
* @param[in] bat: 要写入的初值
* @return
*       none
* @implements  DM.1.2.74,DM.1.3.74,DM.1.4.74
*/
extern void taArchInitializeBat( UINT32 bat );

/* 实现 */


/*
* @brief
*       初始化处理器。
* @return
*       none
* @implements  DM.1.2.93,DM.1.3.93,DM.1.4.93
*/
void taCpuInitialize( void )
{
    /* 初始化MMU页转换使用的hash表空间 */
//    _msl_arch_init_hashtable();
    
    /* 初始化BAT寄存器 */
	taArchInitializeBat(0);
}

/*
* @brief
*       屏蔽CPU中断。
* @param[out]    level:  指向状态寄存器值的指针
* @return
*       none
* @implements  DM.1.2.1,DM.1.3.1,DM.1.4.1
*/
void taCpuInterruptDisable( UINT32 *level )
{
    UINT32 disableMask = 0x8000;
    UINT32 tmpLevel = 0;

    /*
    * 保存当前状态寄存器的值到*level，通过将MSR寄存器中的
    * EE位设置为0关闭中断
    */
    asm volatile ("mfmsr %0; sync; andc %1,%0,%1; sync; isync; mtmsr %1; sync; isync" :
    "=&r" ((tmpLevel)), "=&r" ((disableMask)) :
    "0" ((tmpLevel)), "1" ((disableMask)));
    *level = tmpLevel;
}

/*
* @brief
*       使能CPU中断。
* @param[in]    level:  状态寄存器的值（正常情况应与
*                       _msl_cpu_interrupt_disable配对使用并使用其获取到的值）
* @return
*       none
* @implements  DM.1.2.2,DM.1.3.2,DM.1.4.2
*/
void taCpuInterruptEnable( UINT32 level )
{
    UINT32 tmpLevel = 0;

    /* 保存当前状态寄存器的值到tmpLevel */
    asm volatile ("mfmsr %0; sync;" :
    "=&r" ((tmpLevel))  :
    "0" ((tmpLevel)));

    /*  清EE位*/
    tmpLevel &= ~0x8000;

    /* 只获取EE位，清其它位 */
    level &= 0x8000;

    /*  指定状态寄存器的值 */
    tmpLevel |= level;

    /* 恢复tmpLevel指定的状态寄存器值到当前状态寄存器MSR */
    asm volatile ( "mtmsr %0; sync; isync" :    "=r" ((tmpLevel)) : "0" ((tmpLevel)));
}

/*
* @brief
*       快速开关一次CPU中断。
* @param[in]    level:  状态寄存器的值（正常情况应使用
*                       _msl_cpu_interrupt_disable获取到的值）
* @return
*       none
* @implements  DM.1.2.3,DM.1.3.3,DM.1.4.3
*/
void taCpuInterruptOpt(UINT32 level )
{
    UINT32 disableMask = 0x8000;
    UINT32 tmpLevel = 0;    

    /* 保存当前状态寄存器的值到tmpLevel */
    asm volatile ("mfmsr %0; sync;" :
    "=&r" ((tmpLevel))  :
    "0" ((tmpLevel)));
    
    /*  清EE位*/
    tmpLevel &= ~0x8000;

    /* 只获取EE位，清其它位 */
    level &= 0x8000;

    /*  指定状态寄存器的值 */
    tmpLevel |= level;    
    
    /*
    * 通过恢复tmpLevel指定的状态寄存器值到当前状态寄存器MSR来开中断;
    * 通过将MSR寄存器中的EE位设置为0关闭中断
    */
    asm volatile ("mtmsr %0; sync; isync; andc %1,%0,%1; mtmsr %1; sync; isync" :
    "=r" ((tmpLevel)), "=r" ((disableMask)) :
    "0" ((tmpLevel)), "1" ((disableMask)));
}

/*
 * @brief:
 *      Arch相关初始化
 * @return:
 *      无
 */
void taArchInit(void)
{
	/*初始化MSL异常栈*/
	taExceptionstackInit();

	/* 初始化向量表 */
	taInitVectorTable();
}


/*
*刷cache
*/
void CPU_FlushCaches(unsigned int addr, unsigned int len)
{

}
