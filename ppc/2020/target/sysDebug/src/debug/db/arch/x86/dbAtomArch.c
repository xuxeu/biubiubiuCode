/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/**
 * @file  dbAtomArch.c
 * @brief:
 *             <li>操作用户程序寄存器相关的接口</li>
 */

/************************头文件******************************/
#include "taErrorDefine.h"
#include "dbVector.h"
#include "dbAtom.h"
#include "dbSignal.h"
#include "taUtil.h"
#include "ta.h"

/************************宏定义******************************/

/************************类型定义****************************/

/************************全局变量****************************/
/************************外部声明*****************************/

/* 断点异常处理 */
extern void taBpExceptionStub(void);
/************************前向声明****************************/

/************************模块变量****************************/

/************************函数实现*****************************/

/*
 * @brief:
 *      体系结构初始化
 * @return:
 *     TRUE: 初始化成功
 */
BOOL taDebugArchInit(void)
{
    /* 安装单步异常 */
	taInstallVectorHandle(GENERAL_DB_INT, taTraceExceptionStub);

    /* 安装软件断点异常 */
	taInstallVectorHandle(GENERAL_TRAP_INT, taBpExceptionStub);

    return (TRUE);
}

/*
 * @Brief
 *     vector号转化成标准信号表示形式
 * @param[in]: vector:向量号
 * @param[in]: context:异常上下文
 * @return
 *     返回向量号对应的POSIX信号
 */
UINT32 taDebugVector2Signal(UINT32 vector, T_DB_ExceptionContext *context)
{
    UINT32 signal = SIGNAL_TERM;
    
    /* 根据vector号转换成标准POSIX信号 */
    switch (vector)
    {
        /* 0 号中断——除法错异常 */
        case GENERAL_ZDIV_INT:
            signal = SIGNAL_FPE;
            break;

        /* 1 号中断——调试异常*/
        case GENERAL_DB_INT:
            signal = SIGNAL_TRAP;
            break;

        /* 2 号中断——非屏蔽中断*/
        case GENERAL_NMI_INT:
            signal = SIGNAL_INT;
            break;

        /* 3 号中断——断点异常（#BP）*/
        case GENERAL_TRAP_INT:
            signal = SIGNAL_TRAP;
            break; 

        /* 4 号中断——运算溢出（#DE）*/
        case GENERAL_OVERF_INT:
            signal = SIGNAL_NON;
            break;  

        /* 5 号中断——Bound指令范围越界*/
        case GENERAL_BOUND_INT:
            signal = SIGNAL_NON;
            break;  

        /* 6 号中断——非法操作码异常*/
        case GENERAL_ILLINS_INT:
            signal = SIGNAL_ILL;
            break;

        /* 7 号中断——协处理器不可用异常*/
        case GENERAL_DEV_INT:
            signal = SIGNAL_FPE;
            break;

        /* 8 号中断——双故障异常*/
        case GENERAL_DF_INT:
            signal = SIGNAL_EMT;
            break;

        /* 9 号中断——协处理器段越限*/
        case GENERAL_CSO_INT:
            signal = SIGNAL_SEGV;
            break;
        
        /* 10 号中断——非法TSS(Task State Segment)异常*/
        case GENERAL_TSS_INT:
            signal = SIGNAL_SEGV;
            break;

        /* 11 号中断——段不存在*/
        case GENERAL_SEGMENT_INT:
            signal = SIGNAL_SEGV;
            break;

        /* 12 号中断——栈故障异常*/
        case GENERAL_SF_INT:
            signal = SIGNAL_SEGV;
            break;

        /* 13 号中断——一般保护异常*/
        case GENERAL_GP_INT:
            signal = SIGNAL_SEGV;
            break;
            
        /* 14 号中断——页故障异常*/
        case GENERAL_PAGE_INT:
            signal = SIGNAL_SEGV;
            break;

        /* 16 号中断——数字运算错误异常*/
        case GENERAL_MF_INT:
            signal = SIGNAL_EMT;
            break;

        /* 17 号中断——对齐检验异常*/
        case GENERAL_AE_INT:
            signal = SIGNAL_BUS;
            break;
            
        /* 18 号中断——Machine Check异常*/
        case GENERAL_MC_INT:
            signal = SIGNAL_EMT;
            break;

        /* 19 号中断——SIMD浮点异常*/
        case GENERAL_SIMD_INT:
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
 * @param[in]: context:异常上下文
 * @param[in]: registerNum:寄存器号
 * @param[in]: inBuf:输入buffer
 * @return
 *     TA_OK:操作成功
 *     TA_FAIL:操作失败
 *     TA_INVALID_PARAM:非法的参数
 */
T_TA_ReturnCode taDebugWriteSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, const UINT8 *inBuf)
{
    UINT32 value = 0;

    /* 参数有效性判断 */
    if ((registerNum >= DB_X86_REGISTER_SUM))
    {
        /* 如果有参数为空,返回TA_INVALID_PARAM */
        return (TA_INVALID_PARAM);
    }

    /* 将输入字符流转换为整形数 */
    if (hex2int(&inBuf, &value) > 0 )
    {
        /* 写指定的值到context中指定寄存器 */
        context->registers[registerNum] = swap(value);

        return (TA_OK);
    }

    /* 输入字符流转换失败 */
    return (TA_FAIL);
}

/*
 * @brief
 *     读单个的用户寄存器
 * @param[in]: context:异常上下文
 * @param[in]: registerNum:寄存器号
 * @param[out]: outBuf:输出buffer
 * @param[out]: outBufferLen::输出缓冲的长度
 * @return
 *     TA_OK:操作成功
 *     TA_INVALID_PARAM:非法的参数
 */
T_TA_ReturnCode taDebugReadSingleRegister(T_DB_ExceptionContext *context, UINT32 registerNum, UINT8 *outBuf, UINT32 *outBufferLen)
{
    /* 参数有效性判断 */
    if ((registerNum >= DB_X86_REGISTER_SUM))
    {
        /* 如果有参数为空,返回TA_INVALID_PARAM */
        return (TA_INVALID_PARAM);
    }
    
    mem2hex((UINT8*)&context->registers[registerNum],outBuf,DB_REGISTER_UNIT_LEN);
    
	*outBufferLen = DB_REGISTER_UNIT_LEN * DB_MEM_TO_CHAR_LEN;

    return (TA_OK);
}

/*
 * @brief:
 *     清除被调试应用上下文中PS寄存器的单步标志位，确保返回到被调试应用后可以连续运行
 * @param[in]: context: 调试异常上下文
 * @return:
 *     TA_OK: 操作成功
 */
T_TA_ReturnCode taDebugContinue(T_DB_ExceptionContext *context)
{
    /* 清除PS寄存器的单步标志位 */
    context->registers[PS] &= (~DB_ARCH_PS_TRAP_BIT);

    return (TA_OK);
}

/*
 * @brief:
 *     设置被调试应用上下文中PS寄存器的单步标志位,确保返回到被调试应用运行执行一条指令以后触发单步异常
 * @param[in]: context:调试异常上下文
 * @return:
 *     TA_OK:操作成功
 */
T_TA_ReturnCode taDebugStep(T_DB_ExceptionContext* context)
{
    /* 设置PS寄存器的单步标志位 */
    context->registers[PS] |= DB_ARCH_PS_TRAP_BIT;

    return (TA_OK);
}

/*
 * @brief:
 *     跳过产生异常指令
 * @param[in]: context:调试异常上下文
 * @return:
 *     TA_OK:操作成功
 */
T_TA_ReturnCode taDebugJumpOverTrap(T_DB_ExceptionContext* context)
{
    /* 跳过断点长度 */
    context->registers[PC] += DB_ARCH_BREAK_INST_LEN;

    return (TA_OK);
}
