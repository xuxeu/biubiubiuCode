/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-2         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbBreakpoint.c
 * @brief:
 *             <li>调试管理模块的断点管理</li>
 */

/************************头 文 件******************************/
#include <string.h>
#include "dbMemory.h"
#include "dbBreakpoint.h"
#include "dbHdBp.h"
#include "dbAtom.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/*全局的断点管理信息表*/
typedef struct
{
    T_DB_Breakpoint   *bp_busy;  /* 已设置了断点的链表 */
    T_DB_Breakpoint   *bp_free;  /* 空闲断点链表 */
}T_DB_BreakpointManageInfo;

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/* 断点管理信息表 */
static T_DB_BreakpointManageInfo taBreakManageInfo={NULL,NULL};

/************************全局变量*****************************/

/* 断点表 */
static T_DB_Breakpoint dbBpTable[CONFIG_DB_BREAK_POINT];

/************************函数实现*****************************/

/*
 * @brief:
 *     初始化断点管理数据结构
 * @param[in] bpNumber :配置的断点个数
 * @return:
 *     无
 */
void taDebugBreakPointInit(UINT32 bpNumber)
{
    UINT32 idx = 0;
    
    /* 清空断点列表  */
    ZeroMemory(&dbBpTable, sizeof(dbBpTable));

    /* 初始化全局断点管理信息表空闲链为配置的断点表空间 */
    taBreakManageInfo.bp_free = dbBpTable;

    /* 初始化全局断点管理信息表的已设置断点表为NULL */
    taBreakManageInfo.bp_busy = NULL;

    /* 将所有断点的空间串起来形成单向链表 */
    for (idx = 0; (INT32)idx < (bpNumber - 1); idx++)
    {        
        (taBreakManageInfo.bp_free[idx]).next_addr = &(taBreakManageInfo.bp_free[idx + 1]);
    }
}

/*
 * @brief:
 *    查找匹配的断点信息
 * @param[in]: sessionID :调试会话ID
 * @param[in]: objID: 被调试对象ID
 * @param[in]: logicAddr: 断点逻辑地址
 * @param[out]: breakpointList: 存储查到的符合条件的断点列表,如果逻辑地址相同但sessionID,taskID不同，则返回第一个匹配的断点
 * @return:
 *         TA_OK:找到有匹配的断点
 *         TA_DB_BREAKPOINT_NOTFOUND:没有找到断点
 *         TA_DB_BREAKPOINT_DIFFER_ID:没有找到完全匹配的断点,地址相同但是sessionID,objID不同
 */
T_TA_ReturnCode taDebugLookUpBP(UINT32 sessionID, UINT32 objID, UINT32 logicAddr, T_DB_Breakpoint **breakpointList)
{
    T_DB_Breakpoint *pBreakpoint = taBreakManageInfo.bp_busy;

    /* 设置breakpointList为NULL */
    *breakpointList = NULL;

    /* 在已设置断点链表pBreakpoint中遍历查找逻辑地址等于<logicAddr>的断点 */
    while ((NULL != pBreakpoint) && (pBreakpoint->logicAddr != logicAddr))
    {
        /* 未找到则继续找链表中的下一个节点 */
        pBreakpoint = pBreakpoint->next_addr;
    }

    /* 如果没有找到逻辑地址相同的断点 */
    if (NULL == pBreakpoint)
    {
        return (TA_DB_BREAKPOINT_NOTFOUND);
    }

    /* 设置tpBpList指向相同地址断点头节点 */
    *breakpointList = pBreakpoint;

    /* 遍历具有相同断点地址的断点节点 */
    while(NULL != pBreakpoint)
    {
         /* 查找会话完全匹配的断点 */
        if ((sessionID == pBreakpoint->sessionID) && (objID == pBreakpoint->objID))
        {
            *breakpointList = pBreakpoint;

            return (TA_OK);
        }

        /* 未找到则继续找链表中的下一个节点 */
        pBreakpoint = pBreakpoint->same_addr;
    }

    /* 只找到逻辑地址匹配的断点 */
    return (TA_DB_BREAKPOINT_DIFFER_ID);
}

/*
 * @brief:
 *        插入一个断点(软件断点和硬件断点)，如果已经存在逻辑地址相同的断点则插入相同地址链表否则插入taBreakManageInfo.bp_busy链头
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:breakpoint: 断点信息
 * @return:
 *         TA_OK                   :执行成功
 *         TA_DB_BREAKPOINT_FULL   :断点打满了
 *         TA_INVALID_PARAM        :非法的参数
 *         TA_DB_BREAKPOINT_EXIST  :相同会话,相同任务的相同地址已存在一个断点
 */
T_TA_ReturnCode taDebugInsertBP(T_DB_Info *debugInfo, T_DB_Breakpoint *breakpoint)
{
    T_DB_Breakpoint* pNewBP = NULL;
    T_DB_Breakpoint* pSameAddrBP = NULL;
    T_TA_ReturnCode ret = TA_OK;

    /* 从断点链中查看是否存在完全匹配的断点 */
    ret = taDebugLookUpBP(debugInfo->sessionID, debugInfo->objID, breakpoint->logicAddr, &pSameAddrBP);
    if (TA_OK == ret)
    {
        return (TA_OK);
    }

    /* 如果已经达到的允许的最大断点设置数量 */
    if (NULL == taBreakManageInfo.bp_free)
    {
        return (TA_DB_BREAKPOINT_FULL);
    }

    /* 从空断点链上获取一个节点 */
    pNewBP = taBreakManageInfo.bp_free;

    /* 空断点链表头节点指向下一个节点 */
    taBreakManageInfo.bp_free = taBreakManageInfo.bp_free->next_addr;

    /* 根据输入参数对新插入的断点节点赋值 */
    pNewBP->sessionID = debugInfo->sessionID;
    pNewBP->objID = debugInfo->objID;
    pNewBP->logicAddr = breakpoint->logicAddr;
    pNewBP->bpType = breakpoint->bpType;
    pNewBP->bpClass = breakpoint->bpClass;
    pNewBP->breakSize = breakpoint->breakSize;
    pNewBP->next_addr = NULL;
    pNewBP->same_addr = NULL;

    /* 如果是插入软件断点 */
    if (DB_BP_NORMAL == breakpoint->bpType || DB_BP_SOFT_STEP == breakpoint->bpType)
    {
        /* 插入软件断点 */
        ret = taDebugInsertSoftBp(debugInfo->objID, breakpoint->logicAddr, (UINT8*)(&(pNewBP->shadow)));
    }
    else
    {
        /* 插入硬件断点 */
        ret = taDebugInsertHardBp(debugInfo, breakpoint->logicAddr, breakpoint->bpType, breakpoint->breakSize);
    }

    /* 断点插入失败 */
    if (TA_OK != ret)
    {
        /* 插入断点失败，则将取下的断点写回free链表  */
        pNewBP->next_addr = taBreakManageInfo.bp_free;
        taBreakManageInfo.bp_free = pNewBP;
        return ret;
    }

    /* 如果链表中已存在相同逻辑地址的断点，但不在同一会话的断点 */
    if (NULL != pSameAddrBP)
    {
        /* 在相同地址链表头节点插入新加入的断点 */
        pNewBP->same_addr = pSameAddrBP->same_addr;
        pSameAddrBP->same_addr = pNewBP;
    }
    else
    {
        /* 不存在相同地址的断点，直接将新断点节点插入taBreakManageInfo.bp_busy链头 */
        pNewBP->next_addr = taBreakManageInfo.bp_busy;
        taBreakManageInfo.bp_busy = pNewBP;
    }

    return (TA_OK);
}

/*
 * @brief:
 *        删除一个断点
 * @param[in]:debugInfo :调试会话信息
 * @param[in]:logicAddr :断点地址
 * @param[in]:breakpointType :断点类型
 * @param[in]:breakpointClass :断点分类
 * @param[in]:size :断点宽度
 * @return:
 *         TA_OK :执行成功
 *         TA_FAIL :删除断点失败
 *         TA_DB_BREAKPOINT_EMPTY :没有打任何断点
 *         TA_INVALID_PARAM :非法的参数
 *         TA_DB_BREAKPOINT_NOTFOUND :没找到断点
 */
T_TA_ReturnCode taDebugRemoveBP(T_DB_Info *debugInfo, UINT32 logicAddr, T_DB_BpType breakpointType, T_DB_BpClass breakpointClass, UINT32 size)
{
    T_DB_Breakpoint* pMatchBP = taBreakManageInfo.bp_busy;
    T_DB_Breakpoint* pPreBusyBP = NULL;
    T_DB_Breakpoint* pPreSameBP = NULL;

    /* 查找具有相同地址的断点链，并记录前一个断点链 */
    while ((NULL != pMatchBP) && (logicAddr != pMatchBP->logicAddr))
    {
    	/* 记录前一个节点,便于删除 */
        pPreBusyBP = pMatchBP;

        /* 指向下一个节点元素 */
        pMatchBP = pMatchBP->next_addr;
    }

    /* 查找全匹配断点，并记录前一个断点节点 */
    while ((NULL != pMatchBP)
            && ((debugInfo->sessionID != pMatchBP->sessionID)
            || (debugInfo->objID != pMatchBP->objID)
            || ((pMatchBP->bpType == DB_BP_SOFT_STEP)?0:(breakpointType != pMatchBP->bpType))
            || (breakpointClass != pMatchBP->bpClass)))
    {
        /* 设置pPreSameBP为pMatchBP,记录前一个节点便于删除 */
        pPreSameBP = pMatchBP;

        /* 设置pMatchBP让其指向当前pMatchBP所链接的相同断点地址链 */
        pMatchBP = pMatchBP->same_addr;
    }


    /* 没找到的匹配断点 */
    if (NULL == pMatchBP)
    {
        //return (TA_DB_BREAKPOINT_NOTFOUND);
         /*查找不到断点返回OK*/
        return TA_OK;
    }

    /* 如果断点类型为软件断点 */
    if (DB_BP_NORMAL == breakpointType || DB_BP_SOFT_STEP == breakpointType)
    {
        /* 删除软件断点 */
        taDebugRemoveSoftBp(debugInfo->objID, logicAddr, (const UINT8*)(pMatchBP->shadow));
    }
    else
    {
        /* 删除硬件断点 */
        taDebugRemoveHardBp(debugInfo, logicAddr, breakpointType, size);
    }

    /* 如果找到的断点为相同地址链的最后一个节点(或只有一个节点) */
    if (NULL == pMatchBP->same_addr)
    {
    	/* 找到节点的前一个节点不为空表示相同地址链有多个断点，只需简单的将前一个节点的next指针设为空即从链表中去掉了找到的节点 */
        if (NULL != pPreSameBP)
        {
            /* 当前删除的断点节点为same链中的最后一个节点，设置pPreSameBP的same_addr为NULL */
            pPreSameBP->same_addr = NULL;
        }
        else
        {
        	/* 前一个断点为空，说明same链上只有一个断点,直接从不同地址链上摘除 */
            if (NULL == pPreBusyBP)
            {
                /* 当前删除的断点节点是taBreakManageInfo.bp_busy的头节点，设置taBreakManageInfo.bp_busy指向下一个节点 */
                taBreakManageInfo.bp_busy = taBreakManageInfo.bp_busy->next_addr;
            }
            else
            {
                /* 不是头接点，则将上一个节点指针指向找到结点的下一个结点，即从busy链表中摘除了 */
                pPreBusyBP->next_addr = pMatchBP->next_addr;
            }
        }
    }
    else
    {
    	/* 如果找到的断点不是相同地址链的最后一个节点 */
        if (NULL == pPreSameBP)
        {
        	/* 前一个节点为空，即找到的节点为相同地址链的头节点，如果找到的节点为整个busy链的第一个节点 */
        	if (NULL == pPreBusyBP)
        	{
        		/* busy链的头接点指向找到的节点的same链的下一个节点 */
                taBreakManageInfo.bp_busy = pMatchBP->same_addr;
        	}
        	else
        	{
            	/* 将找到节点的相同地址链挂到不同地址节点链上 */
            	pPreBusyBP->next_addr = pMatchBP->same_addr;
        	}
        	
        	/* 新的相同地址链的头节点在不同地址链上，指向原头结点的下一个节点*/
        	pMatchBP->same_addr->next_addr = pMatchBP->next_addr;
        }
        else
        {
            /* pPreSameBP的same_addr节点赋值为pMatchBP的same_addr节点 */
            pPreSameBP->same_addr = pMatchBP->same_addr;
        }
    }

    /* 将取下的空闲链表插入到bp_free链 */
    pMatchBP->next_addr = taBreakManageInfo.bp_free;
    taBreakManageInfo.bp_free = pMatchBP;

    return (TA_OK);
}

/*
 * @brief:
 *      向地址addr处插入一个断点指令，同时将addr处原有的内容保存到shadow中
 * @param[in]: ctxId: 上下文ID
 * @param[in]: addr: 插入断点指令的地址
 * @param[in]: shadow: 用来保存断点处原有内容的缓存
 * @return:
 *        TA_OK: 操作成功
 *        TA_FAIL: 操作失败
 */
T_TA_ReturnCode taDebugInsertSoftBp(UINT32 ctxId,  UINT32 addr, UINT8 *shadow)
{
	UINT32 inst = DB_ARCH_BREAK_INST;
    T_TA_ReturnCode ret = TA_OK;

    /* 从addr地址读取DB_ARCH_BREAK_INST_LEN长度的内容保存到shadow中 */
    ret = taDebugReadMemory(ctxId, addr, shadow, DB_ARCH_BREAK_INST_LEN, Align_None);
    if (TA_OK != ret)
    {
        return (TA_FAIL);
    }

    /* 当前内存是否是断点指令 */
    if (taDebugBpInstructCheck((UINT8 *)shadow) != FALSE)
    {
    	return TA_OK;
    }

    /* 将断点指令写入到addr处 */
    ret = taDebugWriteMemory(ctxId, addr, (const UINT8*)&inst, DB_ARCH_BREAK_INST_LEN, Align_None);
    if (TA_OK != ret)
    {
        return (TA_FAIL);
    }

    /* 刷新Cache */
    taDebugFlushCache(ctxId, addr, DB_ARCH_BREAK_INST_LEN);

    return (TA_OK);
}

/*
 * @brief:
 *        从地址addr处删除一个断点指令，同时恢复addr处原有的内容
 * @param[in]: ctxId: 上下文ID
 * @param[in]: addr: 插入断点指令的地址
 * @param[in]: shadow: 用来保存断点处原有内容的缓存
 * @return:
 *        TA_OK: 操作成功
 *        TA_FAIL:  操作失败
 */
T_TA_ReturnCode taDebugRemoveSoftBp(UINT32 ctxId, UINT32 addr, const UINT8 *shadow)
{
    T_TA_ReturnCode ret = TA_OK;
    UINT8 buffer[DB_ARCH_BREAK_INST_LEN];

    /* 从addr地址读取DB_ARCH_BREAK_INST_LEN长度的内容保存到shadow中 */
    ret = taDebugReadMemory(ctxId, addr, buffer, DB_ARCH_BREAK_INST_LEN, Align_None);
    if (TA_OK != ret)
    {
        return (TA_FAIL);
    }

     /* 当前内存是否是断点指令 */
    if (taDebugBpInstructCheck((UINT8 *)buffer) != TRUE)
    {

    	return TA_OK;
    }

    /* 将shadow的内容写入到addr处 */
    ret = taDebugWriteMemory(ctxId, addr, shadow, DB_ARCH_BREAK_INST_LEN, Align_None);
    if(TA_OK != ret)
    {
        return (TA_FAIL);
    }

    /* 刷新Cache */
    taDebugFlushCache(ctxId, addr, DB_ARCH_BREAK_INST_LEN);

    return (TA_OK);
}

/*
 * @brief:
 *      删除当前调试会话所有断点
 * @param[in] debugInfo: 调试会话
 * @return:
 *      无
 */
void taDebugRemoveAllBp(T_DB_Info *debugInfo)
{
	T_DB_Breakpoint* pBreakpoint = taBreakManageInfo.bp_busy;
	T_DB_Breakpoint* pSameBreakpoint = NULL;
	T_DB_Breakpoint* pPreSameBreakpoint = NULL;

    /* 遍历所有断点 */
    while (NULL != pBreakpoint)
    {
        /* 设置same节点 */
        pSameBreakpoint = pBreakpoint;

        /* 指向下一个断点元素 */
        pBreakpoint = pBreakpoint->next_addr;

        /* 遍历具有相同断点地址的断点节点 */
        while(NULL != pSameBreakpoint)
        {
            /* 记录前一个same节点*/
            pPreSameBreakpoint = pSameBreakpoint;

            /* 指向下一个具有相同断点地址的节点，这里先指向下一个断点，
             * 否则删除后断点被回收，无法找到下一个断点 */
            pSameBreakpoint = pSameBreakpoint->same_addr;

            /* 删除断点 */
            taDebugRemoveBP(debugInfo, pPreSameBreakpoint->logicAddr, pPreSameBreakpoint->bpType, BPClass_NORMAL, pPreSameBreakpoint->breakSize);
        }
    }
}

/*
 * @brief:
 *      禁止所有软件断点
 * @return:
 *      无
 */
void taDebugBpAllDisable(void)
{
	T_DB_Breakpoint* pBreakpoint = taBreakManageInfo.bp_busy;
	T_DB_Breakpoint* pSameBreakpoint = NULL;
	T_DB_Breakpoint* pPreSameBreakpoint = NULL;

    /* 遍历所有断点 */
    while (NULL != pBreakpoint)
    {
        /* 设置same节点 */
        pSameBreakpoint = pBreakpoint;

        /* 指向下一个断点元素 */
        pBreakpoint = pBreakpoint->next_addr;

        /* 遍历具有相同断点地址的断点节点 */
        while(NULL != pSameBreakpoint)
        {
            /* 记录前一个same节点*/
            pPreSameBreakpoint = pSameBreakpoint;

            /* 指向下一个具有相同断点地址的节点，这里先指向下一个断点，
             * 否则删除后断点被回收，无法找到下一个断点 */
            pSameBreakpoint = pSameBreakpoint->same_addr;

            /* 删除软件断点 */
            taDebugRemoveSoftBp(pPreSameBreakpoint->objID, pPreSameBreakpoint->logicAddr, (const UINT8*)(pPreSameBreakpoint->shadow));
        }
    }
}

/*
 * @brief:
 *      使能所有软件断点
 * @return:
 *      无
 */
void taDebugBpAllEnable(void)
{
	T_DB_Breakpoint* pBreakpoint = taBreakManageInfo.bp_busy;
	T_DB_Breakpoint* pSameBreakpoint = NULL;
	T_DB_Breakpoint* pPreSameBreakpoint = NULL;

    /* 遍历所有断点 */
    while (NULL != pBreakpoint)
    {
        /* 设置same节点 */
        pSameBreakpoint = pBreakpoint;

        /* 指向下一个断点元素 */
        pBreakpoint = pBreakpoint->next_addr;

        /* 遍历具有相同断点地址的断点节点 */
        while(NULL != pSameBreakpoint)
        {
            /* 记录前一个same节点*/
            pPreSameBreakpoint = pSameBreakpoint;

            /* 指向下一个具有相同断点地址的节点，这里先指向下一个断点，
             * 否则删除后断点被回收，无法找到下一个断点 */
            pSameBreakpoint = pSameBreakpoint->same_addr;

            /* 插入断点 */
            taDebugInsertSoftBp(pPreSameBreakpoint->objID, pPreSameBreakpoint->logicAddr, (UINT8*)(&(pPreSameBreakpoint->shadow)));
        }
    }
}

/*
 * @brief:
 *      查看是否为软件单步设置的断点
 * @return:
 *      无
 */
BOOL taIsSoftStepBreakPoint(UINT32 addr)
{
		
	T_DB_Breakpoint* pBreakpoint = taBreakManageInfo.bp_busy;


    /* 遍历所有断点 */
    while (NULL != pBreakpoint)
    {
     

	if(pBreakpoint->logicAddr ==addr &&  pBreakpoint->bpType == DB_BP_SOFT_STEP)
	{
		return TRUE;
	}

        /* 指向下一个断点元素 */
        pBreakpoint = pBreakpoint->next_addr;

     
    }

	return FALSE;
}
/*
 * @brief:
 *     检查内存地址存放的是否是断点指令
 * @return:
 *     FALSE: 非断点指令
 *     TRUE:断点指令
 */
BOOL taDebugBpInstructCheck(UINT8 *addr)
{
	UINT32 bpInstr = DB_ARCH_BREAK_INST;
	UINT8 *bpAddr = (UINT8 *)&bpInstr;
    UINT32 index = 0;

    for (index = 0; index < DB_ARCH_BREAK_INST_LEN; index++)
	{
    	if (*addr++ != *bpAddr++)
    	{
    		return (FALSE);
    	}
	}

    return (TRUE);
}

 /**********************下面代码用于同步断点列表*****************************/
#ifdef _KERNEL_DEBUG_
/*
 * @brief:
 *    获取断点信息
 * @return:
 *     FALSE: 非断点指令
 *     TRUE:断点指令
 */
unsigned int* taGetBreakInfo()
{
    return (unsigned int*)(&taBreakManageInfo);
}
#endif

/*
 * @brief:
 *     同步断点信息
 * @return:
 *     FALSE: 非断点指令
 *     TRUE:断点指令
 */
BOOL taSyncBreakInfo(unsigned int *BreakManageInfo)
{
    T_DB_BreakpointManageInfo *taBreakInfo = (T_DB_BreakpointManageInfo*)BreakManageInfo;
    T_DB_Breakpoint* pBreakpoint = taBreakInfo->bp_busy;
    T_DB_Breakpoint* pNewBP = NULL;

    /* 遍历所有断点 */
    while (NULL != pBreakpoint)
    {	
	     /* 从空断点链上获取一个节点 */
	    pNewBP = taBreakManageInfo.bp_free;
	    /* 空断点链表头节点指向下一个节点 */
	    taBreakManageInfo.bp_free = taBreakManageInfo.bp_free->next_addr;

           memset(pNewBP,0,sizeof(T_DB_Breakpoint));
	    /* 根据输入参数对新插入的断点节点赋值 */
	    pNewBP->sessionID = pBreakpoint->sessionID;
        pNewBP->objID = pBreakpoint->objID;
	    pNewBP->logicAddr = pBreakpoint->logicAddr;
	    pNewBP->bpType = pBreakpoint->bpType;
	    pNewBP->bpClass = pBreakpoint->bpClass;
	    pNewBP->breakSize = pBreakpoint->breakSize;
           memcpy(pNewBP->shadow,pBreakpoint->shadow,DB_ARCH_BREAK_INST_LEN);
	    pNewBP->next_addr = NULL;
	    pNewBP->same_addr = NULL;

	    /* 直接将新断点节点插入taBreakManageInfo.bp_busy链头 */
          pNewBP->next_addr = taBreakManageInfo.bp_busy;
          taBreakManageInfo.bp_busy = pNewBP;

	    /* 指向下一个断点元素 */
        pBreakpoint = pBreakpoint->next_addr;

	}
}

#ifdef __PPC__
//刷新
void taUpdateCacheBreakList()
{
	T_DB_Breakpoint* pBreakpoint = taBreakManageInfo.bp_busy;

	/* 遍历所有断点 */
	while (NULL != pBreakpoint)
	{
		taCacheTextUpdata(pBreakpoint->logicAddr,sizeof(T_DB_Breakpoint));

		/* 指向下一个断点元素 */
		pBreakpoint = pBreakpoint->next_addr;
	}
}
#endif

