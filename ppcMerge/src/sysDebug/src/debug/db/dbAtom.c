/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-01         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file: dbAtom.c
 * @brief:
 *             <li>调试原子操作，包括读取所有寄存器、修改所有寄存器、获取当前断点地址操作</li>
 */
/************************头 文 件******************************/
#include "taErrorDefine.h"
#include "dbAtom.h"
#include "taUtil.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief
 *     读所有的用户寄存器
 * @param[in] tpContext:异常上下文
 * @param[out] ubOutBuf:输出buffer
 * @param[in] uwOutBufferLen:输出buffer的长度
 * @return
 *     TA_OK:操作成功
 *     TA_OUT_OF_BUFFER:输出缓冲长度不足
 */
T_TA_ReturnCode taDebugReadAllRegisters(T_DB_ExceptionContext* context, UINT8 *outBuf, UINT32 *outBufferLen)
{
    /*  判断输出缓冲长度是否足够(按双精度浮点处理,浮点寄存器占64位)  */
    if ((*outBufferLen) < DB_ARCH_REGISTER_SUM_SIZE)
    {
        return (TA_OUT_OF_BUFFER);
    }
#ifdef __PPC85XX__
    //拷贝通用寄存器
    mem2hex((const UINT8*)(context->registers), outBuf, GPR_SUM*DB_REGISTER_SIZE);
    //不拷贝浮点寄存器
    //拷贝特殊寄存器
    mem2hex((const UINT8*)(&(context->registers[PC])), outBuf+GPR_SUM*DB_REGISTER_SIZE, SPR_SUM*DB_REGISTER_SIZE);
    /* 输出数据长度 */
    (*outBufferLen) = (GPR_SUM+SPR_SUM)*DB_REGISTER_UNIT_LEN*DB_MEM_TO_CHAR_LEN;

#else
    /* 按GDB上下文,先保存寄存器*/
    mem2hex((const UINT8*)(context->registers), outBuf, DB_REGISTER_SUM_SIZE);

    /* 输出数据长度 */
    (*outBufferLen) = DB_ARCH_REGISTER_SUM_SIZE;
#endif
    return (TA_OK);
}

/*
 * @brief
 *     写所有用户寄存器
 * @param[in] tpContext:异常上下文
 * @param[in] ubInBuf:输入buffer
 * @param[in] uwInLen:输入数据长度
 * @return
 *     TA_OK:操作成功
 *     TA_OUT_OF_BUFFER:输入缓冲长度非法
 */
T_TA_ReturnCode taDebugWriteAllRegisters(T_DB_ExceptionContext *context, const UINT8 *inBuf, UINT32 inLen)
{
    /*  输入字节流长度是否超出范围 */
    if (inLen != DB_ARCH_REGISTER_SUM_SIZE)
    {
        /*  超出范围则返回DB_INVALID_PARAM_LENGTH  */
        return (TA_OUT_OF_BUFFER);
    }
    UINT8* saveBuf = inBuf;
#ifdef __PPC85XX__
    //拷贝通用寄存器
    //mem2hex((const UINT8*)(context->registers), outBuf, GPR_SUM*DB_REGISTER_SIZE);
    hex2memEx(inBuf, (UINT8*)(context->registers), GPR_SUM*DB_REGISTER_SIZE);
    //不拷贝浮点寄存器
    inBuf += GPR_SUM*DB_REGISTER_SIZE;//跳过32个浮点，浮点是32位。
    //拷贝特殊寄存器
    hex2memEx(inBuf, (const UINT8*)(&(context->registers[PC])), FPSCR*DB_REGISTER_SIZE);
    //mem2hex((const UINT8*)(&(context->registers[PC])), outBuf, FPSCR*DB_REGISTER_SIZE);
#else
    /* 向寄存器值写入上下文中 */
    hex2memEx(inBuf, (UINT8*)(context->registers), DB_REGISTER_SUM_SIZE);
#endif
    return (TA_OK);
}

/*
 * @brief:
 *    获取被调试程序当前断点触发的地址，当被调试程序触发断点异常后,PC寄存器的值将指向当前断点的地址
 * @param[in]:  context: 调试异常上下文
 * @param[out]: address:存储触发断点指令的地址
 * @return:
 *     TA_OK:操作成功
 */
T_TA_ReturnCode taDebugGetCurrentBpAddress(T_DB_ExceptionContext *context, UINT32 *address)
{
    /* 获取被调试程序当前断点触发的地址  */
    *address = context->registers[PC];

    return (TA_OK);
}
