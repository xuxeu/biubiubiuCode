/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2011 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2011-02-23         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbAtomArch.c
 * @brief:
 *             <li>ta调试管理体系结构相关的原子操作</li>
 */

/************************头 文 件*****************************/
#include "taErrorDefine.h"
#include "taUtil.h"
#include "dbAtom.h"
#include "dbSignal.h"

/************************宏 定 义******************************/
#define GENERAL_DB_INT          1  /* 调试异常，自陷*/
#define GENERAL_TRAP_INT        3  /* 断点异常 0700:  trap program exception */
#define GENERAL_SM_INT          4  /* 1400:  系统管理中断 */
#define GENERAL_TH_INT          5  /* 1700:  温度管理中断 */
#define GENERAL_IAE_INT         6  /* 非法操作码异常*/
#define GENERAL_PAGE_INT        14 /* 页故障异常*/
#define GENERAL_MF_INT          16 /* 浮点异常 0700:  IEEE floating-point enabled program exception*/
#define GENERAL_AE_INT          17 /* 对齐检验异常 0600:  ALIGNMENT错误 */
#define GENERAL_MC_INT          18 /* Machine Check异常 0200:  机器检查*/
#define GENERAL_RST_INT         20 /* 0100:  系统复位*/
#define GENERAL_DAE_INT         21 /* 0300:  DSI异常，对于更新HASH表是特殊处理。 */
#define GENERAL_PRIVI_INT       22 /* 0700:  privileged instruction program exception */
#define GENERAL_ILLINS_INT      23 /* 0700:  illegal instruction program exception */
#define GENERAL_UF_INT          24 /* 0800:  浮点不可用异常 */
#define GENERAL_DEC_INT         25 /* 0900:  DEC中断，DEC一般用于计时，系统对于该中断直接返回。 */
#define GENERAL_PM_INT          26 /* 0F00:  性能监控 */
#define GENERAL_HBP_INT         27 /* 0300:  硬件断点异常 */
#define GENERAL_IAB_INT         31 /* 1300:  指令地址断点 */

/************************类型定义*****************************/

/************************外部声明*****************************/

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
    return (TRUE);
}

/*
 * @brief:
 *     将异常向量号转化成标准POSIX信号表示形式
 * @param[in]:vector:异常向量号
 * @param[in]:context:调试异常上下文
 * @return:
 *     返回向量号对应的POSIX信号
 */
UINT32 taDebugVector2Signal(UINT32 vector, T_DB_ExceptionContext *context)
{
    UINT32 signal = SIGNAL_TERM;

    /* 判断向量号类型 */
    switch(vector)
    {
        case GENERAL_MC_INT:
            signal = SIGNAL_SEGV;
            break;

        case GENERAL_DAE_INT:
            signal = SIGNAL_SEGV;
            break;

        case GENERAL_PAGE_INT:
            signal = SIGNAL_SEGV;
            break;

        case GENERAL_AE_INT:
            signal = SIGNAL_BUS;
            break;

        case GENERAL_TRAP_INT:
           signal = SIGNAL_TRAP;
           break;

        case GENERAL_MF_INT:
           signal = SIGNAL_FPE;
           break;

        case GENERAL_ILLINS_INT:
           signal = SIGNAL_ILL;
           break;

        case GENERAL_UF_INT:
            signal = SIGNAL_FPE;
            break;

        case GENERAL_DB_INT:
            signal = SIGNAL_TRAP;
            break;

        case GENERAL_IAB_INT:
            signal = SIGNAL_TRAP;
            break;

        default:
            break;
    }

    return (signal);
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
 *     TA_OUT_OF_BUFFER:输出缓冲长度不足
 */
T_TA_ReturnCode taDebugReadSingleRegister(T_DB_ExceptionContext* context, UINT32 registerNum, UINT8 *outBuf, UINT32 *outBufferLen)
{
    /* 寄存器号不在支持的范围内 */
    if (registerNum >= REGISTER_SUM)
    {
        return (TA_INVALID_PARAM);
    }

    /* 读取的寄存器为通用寄存器 */
    if ( registerNum < GPR_SUM )
    {
        /* 根据寄存器号从tpContext中读数据到ubOutBuf缓冲中 */
        mem2hex( (UINT8*)&(context->registers[registerNum]), outBuf, DB_REGISTER_UNIT_LEN );

        *outBufferLen = DB_REGISTER_SIZE;

        return (TA_OK);
    }

    /* 读取的寄存器为浮点寄存器 */
    if ( (registerNum >= GPR_SUM) && (registerNum < GPR_SUM+FGPR_SUM) )
    {
        /* 根据寄存器号从tpContext中读数据到ubOutBuf缓冲中 */
        mem2hex( (UINT8*)&(context->registers[registerNum*2-GPR_SUM]), outBuf, DB_FLOAT_REGISTER_UNIT_LEN);

        *outBufferLen = DB_FLOAT_REGISTER_SIZE;

        return (TA_OK);
    }

    /* 设置的寄存器为特殊寄存器,数组下标从64到69 */
    if ( registerNum >= GPR_SUM+FGPR_SUM )
    {
    	/* 根据寄存器号从tpContext中读数据到ubOutBuf缓冲中 */
         mem2hex( (UINT8*)&(context->registers[registerNum+FGPR_SUM]), outBuf, DB_REGISTER_UNIT_LEN );

        *outBufferLen = DB_REGISTER_SIZE;

        return (TA_OK);
    }

    return (TA_FAIL);
}

/*
 * @brief:
 *     写被调试应用上下文中单个寄存器的值
 * @param[in]:context: 调试异常上下文
 * @param[in]:registerNum:寄存器号，寄存器号范围在0~REGISTER_SUM  0~GPR_SUM为通用整型寄存器 GPR_SUM~(GPR_SUM+FGPR_SUM)为浮点寄存器 (GPR_SUM+FGPR_SUM)~REGISTER_SUM为特殊寄存器
 * @param[in]:inBuf:输入数据缓冲，输入数据为写入寄存器的值(十六进值字符串表示)
 * @return:
 *     TA_OK:操作成功
 *     TA_INVALID_PARAM:非法的参数
 *     TA_INVALID_PARAM_LENGTH:输入缓冲长度非法
 *     TA_FAIL:转换失败
 */
T_TA_ReturnCode  taDebugWriteSingleRegister(T_DB_ExceptionContext* context, UINT32 registerNum, const UINT8 *inBuf)
{
    UINT32 value = 0;
    UINT64 doubleValue = 0;

    /* 参数有效性检查 */
    if (registerNum >= REGISTER_SUM)
    {
        return (TA_INVALID_PARAM);
    }

    /* 寄存器编号在浮点寄存器范围内,数组下标号从32到63 */
    if ( (registerNum >= GPR_SUM) && (registerNum < GPR_SUM+FGPR_SUM) )
    {
        if (0 != hex2int64(&inBuf, &doubleValue) && '\0' == (*inBuf))
        {
            /* 将输入字符流转换为64位值,并写到对应的浮点寄存器  */
            context->registers[registerNum*2 - GPR_SUM] = ((DOUBLE)doubleValue);

            return (TA_OK);
        }

        return (TA_FAIL);
    }

    /* 寄存器编号不在浮点寄存器范围内,先将字符流转换为整形 */
    if ( 0 != hex2int(&inBuf, &value) && '\0' == (*inBuf))
    {
    	/* 寄存器编号在通用寄存器范围内,数组下标从0到31  */
        if (registerNum < GPR_SUM)
        {
            context->registers[registerNum] = value;

            return (TA_OK);
        }

        /* 寄存器编号在特殊寄存器范围内,数组下标从64到69 */
        if (registerNum >= GPR_SUM + FGPR_SUM)
        {
        	/* 将转换后的值保存到特殊寄存器中 */
            context->registers[registerNum+FGPR_SUM] = value;

            return (TA_OK);
        }
    }

    return (TA_FAIL);
}

/*
 * @brief:
 *     清除被调试应用上下文中PS寄存器的单步标志位SE，确保返回到被调试应用后可以连续运行
 * @param[in]:context: 调试异常上下文
 * @return:
 *     TA_OK: 操作成功
 */
T_TA_ReturnCode taDebugContinue(T_DB_ExceptionContext *context)
{
    /* 设置PS寄存器的单步标志位SE为0 */
    context->registers[PS] &= (~DB_ARCH_PS_TRAP_BIT);

    return (TA_OK);
}

/*
 * @brief:
 *     设置被调试应用上下文中PS寄存器的单步标志位SE为1,确保返回到被调试应用运行执行一条指令以后触发单步异常
 * @param[in]:context:调试异常上下文
 * @return:
 *     TA_OK:操作成功
 */
T_TA_ReturnCode taDebugStep(T_DB_ExceptionContext* context)
{
    /* 设置PS寄存器的SE标志位为1 */
    context->registers[PS] |= DB_ARCH_PS_TRAP_BIT;

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
