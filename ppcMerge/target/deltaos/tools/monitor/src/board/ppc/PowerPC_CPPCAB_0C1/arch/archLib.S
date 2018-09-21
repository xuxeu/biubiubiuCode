/*
* 变更历史：
* 2010-08-30  尹立孟  创建该文件。
*/

/*
* @file  archLib.S
* @brief
*       功能：
*       <li> 提供CPU寄存器操作。</li>
*/

#define _ASMLANGUAGE
#include <dbAsm.h>

/*
* @brief
*       设置SDR1寄存器的内容为sdr。
* @param[in]  sdr: 要设置的SDR1寄存器内容
* @return
*       none
* @implements  DM.1.2.73,DM.1.3.73,DM.1.4.73
*/ 
PUBLIC(_msl_arch_set_sdr1)
SYM (_msl_arch_set_sdr1):
	mtspr 25,r3
	blr

/*
* @brief
*       设置DBAT0L寄存器为batl。设置DBAT0U寄存器为batu。
* @param[in]  batl: 要设置的DBAT0L寄存器内容
* @param[in]  batu: 要设置的DBAT0U寄存器内容
* @return
*       none
* @implements  DM.1.2.57,DM.1.3.57,DM.1.4.57
*/
PUBLIC(_msl_arch_set_dbat0)
SYM (_msl_arch_set_dbat0):
	mtdbatl 0,3
	mtdbatu 0,4
	blr

/*
* @brief
*       设置DBAT1L寄存器为batl。设置DBAT1U寄存器为batu。
* @param[in]  batl: 要设置的DBAT1L寄存器内容
* @param[in]  batu: 要设置的DBAT1U寄存器内容
* @return
*      none
* @implements  DM.1.2.58,DM.1.3.58,DM.1.4.58
*/
PUBLIC(_msl_arch_set_dbat1)
SYM (_msl_arch_set_dbat1):
	mtdbatl 1,3
	mtdbatu 1,4
	blr

/*
* @brief
*       设置DBAT2L寄存器为batl。设置DBAT2U寄存器为batu。
* @param[in]  batl: 要设置的DBAT2L寄存器内容
* @param[in]  batu: 要设置的DBAT2U寄存器内容
* @return
*       none
* @implements  DM.1.2.59,DM.1.3.59,DM.1.4.59
*/
PUBLIC(_msl_arch_set_dbat2)
SYM (_msl_arch_set_dbat2):
	mtdbatl 2,3
	mtdbatu 2,4
	blr

/*
* @brief
*       设置DBAT3L寄存器为batl。设置DBAT3U寄存器为batu。
* @param[in]  batl: 要设置的DBAT3L寄存器内容
* @param[in]  batu: 要设置的DBAT3U寄存器内容
* @return
*       none
* @implements  DM.1.2.60,DM.1.3.60,DM.1.4.60
*/
PUBLIC(_msl_arch_set_dbat3) 
SYM (_msl_arch_set_dbat3):
	mtdbatl 3,3
	mtdbatu 3,4
	blr

/*
* @brief
*       设置IBAT0L寄存器为batl。设置IBAT0U寄存器为batu。
* @param[in]  batl: 要设置的IBAT0L寄存器内容
* @param[in]  batu: 要设置的IBAT0U寄存器内容
* @return
*       none
* @implements  DM.1.2.65,DM.1.3.65,DM.1.4.65
*/
PUBLIC(_msl_arch_set_ibat0)
SYM (_msl_arch_set_ibat0):
	mtibatl 0,3
	mtibatu 0,4
	blr
    
/*
* @brief
*       设置IBAT1L寄存器为batl。设置IBAT1U寄存器为batu。
* @param[in]  batl: 要设置的IBAT1L寄存器内容
* @param[in]  batu: 要设置的IBAT1U寄存器内容
* @return
*      none
* @implements  DM.1.2.66,DM.1.3.66,DM.1.4.66
*/
PUBLIC(_msl_arch_set_ibat1)
SYM (_msl_arch_set_ibat1):
	mtibatl 1,3
	mtibatu 1,4
	blr

/*
* @brief
*       设置IBAT2L寄存器为batl。设置IBAT2U寄存器为batu。
* @param[in]  batl: 要设置的IBAT2L寄存器内容
* @param[in]  batu: 要设置的IBAT2U寄存器内容
* @return
*       none
* @implements  DM.1.2.67,DM.1.3.67,DM.1.4.67
*/
PUBLIC(_msl_arch_set_ibat2)
SYM (_msl_arch_set_ibat2):
	mtibatl 2,3
	mtibatu 2,4
	blr

/* 
* @brief
*       设置IBAT3L寄存器为batl。设置IBAT3U寄存器为batu。
* @param[in]  batl: 要设置的IBAT3L寄存器内容
* @param[in]  batu: 要设置的IBAT3U寄存器内容
* @return
*       none
* @implements  DM.1.2.68,DM.1.3.68,DM.1.4.68
*/
PUBLIC(_msl_arch_set_ibat3)
SYM (_msl_arch_set_ibat3):
	mtibatl 3,3
	mtibatu 3,4
	blr

#if defined(__PPC7450__) || defined(__PPC8640__)

/*
* @brief
*       设置DBAT4L寄存器为batl。设置DBAT4U寄存器为batu。
* @param[in]  batl: 要设置的DBAT4L寄存器内容
* @param[in]  batu: 要设置的DBAT4U寄存器内容
* @return
*      none
* @implements  DM.1.2.61,DM.1.4.61
*/
PUBLIC(_msl_arch_set_dbat4)
SYM (_msl_arch_set_dbat4):
	mtspr 569,3
	mtspr 568,4
	blr

/*
* @brief
*       设置DBAT5L寄存器为batl。设置DBAT5U寄存器为batu。
* @param[in]  batl: 要设置的DBAT5L寄存器内容
* @param[in]  batu: 要设置的DBAT5U寄存器内容
* @return
*       none
* @implements  DM.1.2.62,DM.1.4.62
*/
PUBLIC(_msl_arch_set_dbat5)
SYM (_msl_arch_set_dbat5):
	mtspr 571,3
	mtspr 570,4
	blr

/*
* @brief
*       设置DBAT6L寄存器为batl。设置DBAT6U寄存器为batu。
* @param[in]  batl: 要设置的DBAT6L寄存器内容
* @param[in]  batu: 要设置的DBAT6U寄存器内容
* @return
*       none
* @implements  DM.1.2.63,DM.1.4.63
*/
PUBLIC(_msl_arch_set_dbat6)
SYM (_msl_arch_set_dbat6):
	mtspr 573,3
	mtspr 572,4
	blr

/*
* @brief
*       设置DBAT7L寄存器为batl。设置DBAT7U寄存器为batu。
* @param[in]  batl: 要设置的DBAT7L寄存器内容
* @param[in]  batu: 要设置的DBAT7U寄存器内容
* @return
*       none
* @implements  DM.1.2.64,DM.1.4.64
*/
PUBLIC(_msl_arch_set_dbat7) 
SYM (_msl_arch_set_dbat7):
	mtspr 575,3
	mtspr 574,4
	blr

/*
* @brief
*       设置IBAT4L寄存器为batl。设置IBAT4U寄存器为batu。
* @param[in]  batl: 要设置的IBAT4L寄存器内容
* @param[in]  batu: 要设置的IBAT4U寄存器内容
* @return
*       none
* @implements  DM.1.2.69,DM.1.4.69
*/
PUBLIC(_msl_arch_set_ibat4)
SYM (_msl_arch_set_ibat4):
	mtspr 561,3
	mtspr 560,4
	blr
    
/*
* @brief
*       设置IBAT5L寄存器为batl。设置IBAT5U寄存器为batu。
* @param[in]  batl: 要设置的IBAT5L寄存器内容
* @param[in]  batu: 要设置的IBAT5U寄存器内容
* @return
*       none
* @implements  DM.1.2.70,DM.1.4.70
*/
PUBLIC(_msl_arch_set_ibat5)
SYM (_msl_arch_set_ibat5):
	mtspr 563,3
	mtspr 562,4
	blr

/*
* @brief
*       设置IBAT6L寄存器为batl。设置IBAT6U寄存器为batu。
* @param[in]  batl: 要设置的IBAT6L寄存器内容
* @param[in]  batu: 要设置的IBAT6U寄存器内容
* @return
*      none
* @implements  DM.1.2.71,DM.1.4.71
*/
PUBLIC(_msl_arch_set_ibat6)
SYM (_msl_arch_set_ibat6):
	mtspr 565,3
	mtspr 564,4
	blr

/*
* @brief
*       设置IBAT7L寄存器为batl。设置IBAT7U寄存器为batu。
* @param[in]  batl: 要设置的IBAT7L寄存器内容
* @param[in]  batu: 要设置的IBAT7U寄存器内容
* @return
*       none
* @implements  DM.1.2.72,DM.1.4.72
*/
PUBLIC(_msl_arch_set_ibat7)
SYM (_msl_arch_set_ibat7):
	mtspr 567,3
	mtspr 566,4
	blr
#endif

/*
* @brief
*       使ea指定的指令TLB和数据TLB无效。
* @param[in]  ea: 指定的逻辑地址
* @return
*       none
* @implements  DM.1.2.56,DM.1.3.56,DM.1.4.56
*/
PUBLIC(_msl_arch_invalidate_tlb)
SYM (_msl_arch_invalidate_tlb):
	tlbie 3
	blr
 
/*
* @brief
*       初始化BAT寄存器。
* @param[in]  value: 要写入的初值。
* @return
*       none
* @implements  DM.1.2.74,DM.1.3.74,DM.1.4.74
*/
PUBLIC(taArchInitializeBat)
SYM (taArchInitializeBat):
    mtibatu 0,3
    mtibatu 1,3
    mtibatu 2,3
    mtibatu 3,3
    mtdbatu 0,3
    mtdbatu 1,3
    mtdbatu 2,3
    mtdbatu 3,3
#if defined(__PPC7450__) || defined(__PPC8640__)

    /*编译器编译mtdbatu指令是，bat寄存器的索引只能为0-3，因此只能使用mtspr初始化扩展的bat寄存器*/
    mtspr 560,3
    mtspr 562,3
    mtspr 564,3
    mtspr 566,3
    mtspr 568,3
    mtspr 570,3
    mtspr 572,3
    mtspr 574,3
#endif
    blr
	
/*
* @brief
*       通过设置MSR寄存器的IR位和DR位为1使能MMU。
* @return
*       none
* @implements  DM.1.2.54,DM.1.3.54,DM.1.4.54
*/
PUBLIC(taArchEnableMMU)
SYM (taArchEnableMMU):
    mfmsr 3
    ori 3,3,0x30
    sync
    isync
    mtmsr 3
    sync
    isync
    blr

/*
* @brief
*       通过设置MSR寄存器的IR位和DR位为0禁止MMU。
* @return
*       none
* @implements  DM.1.2.94,DM.1.3.94,DM.1.4.94
*/
PUBLIC(taArchDisableMMU)
SYM (taArchDisableMMU):
    mfmsr 3
    li  4, 0x30
    andc 3,3,4
    sync
    isync
    mtmsr 3
    sync
    isync
    blr
 
/*
* @brief
*       获取hid0寄存器的值。
* @return
*       获取到的hid0寄存器的值
* @implements  DM.1.2.48,DM.1.3.48,DM.1.4.48
*/
PUBLIC(taArchCacheGetHid0)
SYM (taArchCacheGetHid0):
	mfspr 3, 1008
	blr

/*
* @brief
*       设置hid0寄存器的值。
* @param[in]  hid0: 写入hid0寄存器的值
* @return
*       none
* @implements  DM.1.2.53,DM.1.3.53,DM.1.4.53
*/ 
PUBLIC(taArchCacheSetHid0)
SYM (taArchCacheSetHid0):
	sync
	mtspr 1008,3
	sync
	isync
	blr

/*
* @brief
*       根据传入的参数设置hid0寄存器。
* @param[in]  first: 写入hid0寄存器的值
* @param[in]  second: 写入hid0寄存器的值
* @return
*       none
* @implements  DM.1.2.52,DM.1.3.52,DM.1.4.52
*/ 
PUBLIC(taArchCacheInvalidByhid0)
SYM (taArchCacheInvalidByhid0):
	sync
	mtspr 1008,3 /* 根据first的值设置hid0寄存器 */
	mtspr 1008,4 /* 根据second的值设置hid0寄存器 */
	sync
	isync
	blr


/*
* @brief
*       刷新从指定起始地址开始,指定大小内存在cache中的内容。
* @param[in]  startAddr: 刷新起始地址
* @param[in]  cnt: cache的总块数
* @param[in]  lineSize: 每个cache块的大小
* @return
*      none
* @implements  DM.1.2.46,DM.1.3.46,DM.1.4.46
*/
PUBLIC(taArchCacheFlush)
SYM (taArchCacheFlush):

cache_flush_loop:
	addic.  4,4,-1 /* 将cnt自减1再存至cnt */
	dcbst   0,3   /* 刷新以start_addr开始的cache块 */
	sync
	add     3,3,5  /* 将start_addr加上line_size再存至start_addr */
	bgt     cache_flush_loop /* 如果cnt大于0则跳转至cache_flush_loop继续执行 */  
	blr


/*
* @brief
*       刷新所有cache
* @param[in]  startAddr: 刷新起始地址
* @param[in]  cnt: cache的总块数
* @param[in]  lineSize: 每个cache块的大小
* @param[in]  null: 当前地址对应内存的内容
* @return
*       none
* @implements  DM.1.2.47,DM.1.3.47,DM.1.4.47
*/ 
PUBLIC(tarchCacheFlushAll)
SYM (tarchCacheFlushAll):

cache_flush_all_loop:
	addic.  4,4,-1 /* 将cnt自减1再存至cnt */
	lwz     6,0(3) /* 将当前地址对应内存中的内容存至null */
	sync
	dcbst   0,3  /* 刷新以start_addr开始的cache块 */
	sync
	add     3,3,5  /* 将start_addr加上line_size再存至start_addr */
	bgt     cache_flush_all_loop  /* 如果cnt大于0则跳转至cache_flush_loop继续执行 */
	blr

/*
* @brief
*       无效从指定起始地址开始,指定大小的数据cache。
* @param[in]  startAddr: 无效起始地址
* @param[in]  cnt: cache的总块数
* @param[in]  lineSize: 每个cache块的大小
* @return
*       none
* @implements  DM.1.2.49,DM.1.3.49,DM.1.4.49
*/ 
PUBLIC(taArchCacheInvalidateData)
SYM (taArchCacheInvalidateData):

cache_invalidate_data_loop:
	addic.  4,4,-1 /* 将cnt自减1再存至cnt */
	dcbi    0,3  /* 无效以start_addr开始的数据cache块 */
	sync
	isync
	add     3,3,5  /* 将start_addr加上line_size再存至start_addr */
	bgt     cache_invalidate_data_loop  /* 如果cnt大于0则跳转至cache_invalidate_data_loop继续执行 */  
	blr

/*
* @brief
*       无效从指定起始地址开始,指定大小的指令cache。
* @param[in]  startAddr: 无效起始地址
* @param[in]  cnt: cache的总块数
* @param[in]  lineSize: 每个cache块的大小
* @return
*       none
* @implements  DM.1.2.50,DM.1.3.50,DM.1.4.50
*/
PUBLIC(taArchCacheInvalidateInst)
SYM (taArchCacheInvalidateInst):

cache_invalidate_inst_loop:
	addic.  4,4,-1  /* 将cnt自减1再存至cnt */
	icbi    0,3   /* 无效以start_addr开始的指令cache块 */
	isync
	add     3,3,5  /* 将start_addr加上line_size再存至start_addr */
	bgt     cache_invalidate_inst_loop  /* 如果cnt大于0则跳转至cache_invalidate_inst_loop继续执行 */
	blr

/*
* @brief 
*       无效从指定起始地址开始,指定大小的所有类型的cache。
* @param[in]  startAddr: 无效起始地址
* @param[in]  cnt: cache的总块数
* @param[in]  lineSize: 每个cache块的大小
* @return
*       none
* @implements  DM.1.2.51,DM.1.3.51,DM.1.4.51
*/
PUBLIC(taArchCacheInvalidateInstdata)
SYM (taArchCacheInvalidateInstdata):

cache_invalidate_inst_and_loop:
	addic.  4,4,-1  /* 将cnt自减1再存至cnt */
	dcbi    0,3    /* 无效以start_addr开始的数据cache块 */
	sync
	icbi    0,3    /* 无效以start_addr开始的指令cache块 */
	sync
	isync
	add     3,3,5  /* 将start_addr加上line_size再存至start_addr */
	bgt     cache_invalidate_inst_and_loop  /* 如果cnt大于0则跳转至cache_invalidate_inst_and_loop继续执行 */ 
	blr
    
/*
* @brief 
*       设置系统栈。
* @param[in]  sysStack: 系统栈空间地址。
* @returns
*       none
* @implements  DM.1.2.77,DM.1.3.77,DM.1.4.77
*/
/* T_VOID setSysStack(T_VOID *sysStack)*/
.text
.p2align  4
PUBLIC(taArchSetSysstack)
SYM (taArchSetSysstack):
	subi r3,r3,16
	rlwinm  r3,r3,0,0,28 /* r3 & 0xfffffff8 */
	mtspr SPRG0,r3		/* spgr0 保存当前VM的系统栈 */
	blr

/*
* @brief 
*       设置段上下文。
* @param[in]  srBase: 段上下文空间基地址。
* @return
*       none
* @implements  DM.1.2.76,DM.1.3.76,DM.1.4.76
*/
/* void setSegmentContext(T_VOID *srBase)*/
.text
.p2align  4
PUBLIC(taArchSetSegmentContext)
SYM (taArchSetSegmentContext):
	li	r0,16		
	mtctr	r0
	li	r4,0
1:
	lwz r5,0(r3)
	mtsrin	r5,r4
	sync
	isync
	addi	r3,r3,4			/* next sr */
	addis	r4,r4,0x1000	/* address of next segment */
	bdnz	1b
	blr

/*
* @brief 
*       RAW Vector默认处理函数。
* @return
*       none 
* @implements  DM.1.2.75,DM.1.3.75,DM.1.4.75
*/
.text
.p2align 4
PUBLIC(taNullRawHandler)
SYM (taNullRawHandler):
	blr


/*
 * @brief 
 *    判断mmu是否使能。
 * @return
 *     TRUE:mmu已使能
 *     FALSE:mmu未使能
 * @implements  DM.1.2.90,DM.1.3.90,DM.1.4.90
 */
.text
.p2align 4
PUBLIC(taArchMMUIsEnabled)
SYM (taArchMMUIsEnabled):
    mfmsr r3
    andi. r3,r3,0x30 /* 检查[IR][DR]位 */
	beq .mmuDisabled
	li	r3,1
	blr
.mmuDisabled:
    li	r3,0
    blr
