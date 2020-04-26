/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                         创建该文件。
 */

/*
 * @file:lmain.c
 * @brief:
 *             <li>ta模块初始化</li>
 */
 
/************************头 文 件******************************/

/************************宏 定 义********************************/

/************************类型定义*******************************/

/************************外部声明*******************************/
extern char _bss_start[];
extern char _bss_size[];
extern void *_vectors_table_end();
extern void taGeneralExpAsmHandler(void);
extern void CPU_FlushCacheL1Data(void);
extern void CPU_FlushCacheL2(void);
extern void sys_init(void);

/************************前向声明*******************************/

/************************模块变量*******************************/

/************************全局变量*******************************/

/************************函数实现*******************************/

 /*
 * @brief:
 *      上电后第一个C入口,清除BSS段，复制向量表，初始化TA
 * @return:
 *      无
 */
void lmain(void)
{
    unsigned int *bstart,bsize,i;

    bstart = (unsigned int*)&_bss_start;
    bsize = (unsigned int)&_bss_size;
    for (i = 0;i < bsize;i+=4)
    {
        *bstart++ = 0;
    }

    /* 复制向量表 */
    memcpy((char *)0x80000180,
        (char *)taGeneralExpAsmHandler,
        (char *)_vectors_table_end - (char *)taGeneralExpAsmHandler);

    CPU_FlushCacheL2();
    CPU_FlushCacheL1Data();
        
    sys_init();
}
