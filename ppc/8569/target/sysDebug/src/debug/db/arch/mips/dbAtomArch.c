/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2011-09-23         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbAtomArch.c
 * @brief:
 *             <li>ta调试管理体系结构相关的原子操作</li>
 */

/************************头 文 件*****************************/
#include "taErrorDefine.h"
#include "dbContext.h"
#include "taUtil.h"
#include "dbAtom.h"
#include "dbSignal.h"
#include "dbVector.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/* 断点异常处理 */
extern void taBpExceptionStub(void);

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *      体系结构初始化，目前主要完成体系结构相关RSP命令处理
 * @return:
 *     TRUE: 初始化成功
 *     FALSE: 初始化失败
 */
BOOL taDebugArchInit(void)
{
    /* 安装软件断点异常 */
	taInstallVectorHandle(IV_BP_VEC, taBpExceptionStub);

    return (TRUE);
}

/*
 * @Brief
 *     vector号转化成标准信号表示形式
 * @param[in] vector:向量号
 * @param[in] context:异常上下文
 * @return
 *     返回向量号对应的信号
 */
UINT32 taDebugVector2Signal(UINT32 vector, T_DB_ExceptionContext *context)
{
    UINT32 signal = SIGNAL_TERM;
    
    /* 根据vector号转换成标准POSIX信号 */
    switch (vector)
    {
        /* 0 号中断——中断 */
        case IV_LOW_VEC:
            signal = SIGNAL_INT;
            break;      

		/* 6 号中断——指令错误异常 */
		case IV_IBUS_VEC:
			signal = SIGNAL_BUS;
			break;

		/* 7 号中断——数据错误异常 */
		case IV_DBUS_VEC:
			signal = SIGNAL_BUS;
			break;

	 	/* 9 号中断——断点异常*/
        case IV_BP_VEC:
            signal = SIGNAL_TRAP;
            break;

		/* 13 号中断——TRAP */
		case IV_TRAP_VEC:
			signal = SIGNAL_TRAP;
			break;

		/* 13 号中断——浮点异常 */
		case IV_FPE_VEC:
			signal = SIGNAL_FPE;
			break;

        /*  其它返回SIGNAL_EMT */
        default:
            signal = SIGNAL_EMT;
            break;
    }

    return (signal); 
} 

/*
 * @brief
 *     写单个的用户寄存器
 * @param[in] tpContext:异常上下文
 * @param[in] uwRegisterNum:寄存器号
 * @param[in] ubInBuf:输入buffer
 * @param[in] uwInLen:输入数据长度(单位字节)
 * @return
 *     TA_OK:操作成功
 *     TA_FAIL:操作失败
 *     TA_INVALID_PARAM:非法的参数
 */
T_TA_ReturnCode taDebugWriteSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, const UINT8 *inBuf)
{
	UINT64 value = 0;

    /* 参数有效性检查 */
    if (registerNum >= REGISTER_SUM)
    {
        /* 如果参数无效,返回TA_INVALID_PARAM */
        return (TA_INVALID_PARAM);
    }

    /* 寄存器编号在浮点寄存器范围内,数组下标号从38开始，不支持浮点寄存器修改 */
    if(registerNum > GPR_SUM + SPR_SUM)
    {
    	return (TA_INVALID_PARAM);
    }

    /* 寄存器编号不在浮点寄存器范围内,先将字符流转换为整形 */
    if (0 != hex2int64(&inBuf, &value) )
    {	
		/* 将转换后的值保存到通用寄存器中 */
		context->registers[registerNum] = swap64(value);
		
		return (TA_OK);
    }

    /* 转换失败,返回TA_FAIL */
    return (TA_FAIL);
}

/*
 * @brief
 *     读单个的用户寄存器
 * @param[in] context:异常上下文
 * @param[in] registerNum:寄存器号
 * @param[out] outBuf:输出buffer
 * @param[out] outBufferLen::输出缓冲的长度
 * @return
 *     TA_OK:操作成功
 *     TA_INVALID_PARAM:非法的参数
 */
T_TA_ReturnCode taDebugReadSingleRegister(T_DB_ExceptionContext* context, UINT32 registerNum, UINT8 *outBuf, UINT32 *outBufferLen)
{
    /* 参数有效性判断 */
    if ((registerNum >= REGISTER_SUM))
    {
        return (TA_INVALID_PARAM);
    }
    
    mem2hex((UINT8*)&context->registers[registerNum],outBuf,DB_REGISTER_UNIT_LEN);
    
	*outBufferLen = DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN;

    return (TA_OK);
}

/*
 * @brief:
 *     MIPS不支持单步位设置
 * @param[in]:context: 调试异常上下文
 * @return:
 *     TA_OK: 操作成功
 */
T_TA_ReturnCode taDebugContinue(T_DB_ExceptionContext *context)
{
    return (TA_OK);
}

/*
 * @brief:
 *     MIPS不支持单步位设置
 * @param[in]:context:调试异常上下文
 * @return:
 *     TA_OK:操作成功
 */
T_TA_ReturnCode taDebugStep(T_DB_ExceptionContext* context)
{
    return (TA_OK);
}

/*
 * @brief:
 *     跳过产生异常指令
 * @param[in]:context:调试异常上下文
 * @return:
 *     TA_OK:操作成功
 */
T_TA_ReturnCode taDebugJumpOverTrap(T_DB_ExceptionContext* context)
{
    /* 跳过断点长度 */
    context->registers[PC] += DB_ARCH_BREAK_INST_LEN;

    return (TA_OK);
}
