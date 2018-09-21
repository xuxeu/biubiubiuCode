/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:dbStandardRsp.c
 * @brief:
 *             <li>GDB的RSP协议实现</li>
 */

/************************头 文 件*****************************/
#include <string.h>
#include "dbStandardRsp.h"
#include "taErrorDefine.h"
#include "dbAtom.h"
#include "dbBreakpoint.h"
#include "dbContext.h"
#include "sdaCpuLib.h"
#include "dbMemory.h"
#include "taUtil.h"
#include "taCommand.h"
#include "db.h"
#include "taDebugMode.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/* 产生trap异常 */
extern void taTrap(void);

/* 通过线程ID获取CPU ID */
extern UINT32 sdaGetCpuIdByThreadId(UINT32 threadId);

/* 初始化任务ID */
extern int taInitTaskId;

#ifdef __MIPS__
/*单步断点任务ID*/
extern int stepThreadId;
#endif
/************************前向声明******************************/

/************************模块变量******************************/

/* 二值写内存时0x7d转换的缓冲 */
static UINT8 m_Content_0x7d[TA_WRITE_M_MAX_DATA_SIZE * 2];

/* 暂停命令标志 */
static BOOL m_StopCommandTag = FALSE;

/* 多线程信息回复标志 */
static BOOL m_MultiThreadInfoTag = TRUE;

/************************全局变量******************************/

/* NOTE:根据GJB5369标准,数组必须有下标,如果增加或删除了命令表，请注意修改配置 */
T_DB_RspCommandTable taRSPCmdTable[TA_RSP_TABLE_LEN] =
{
    {"?",                RspQueryEvent,                TA_NEED_REPLY},
    {"M",                RspWriteMemory,               TA_NEED_REPLY},
    {"m",                RspReadMemory,                TA_NEED_REPLY},
    {"X",                RspWriteBinaryMemory,         TA_NEED_REPLY},
    {"x",                RspReadBinaryMemory,          TA_NEED_REPLY},
    {"P",                RspWriteSingleRegister,       TA_NEED_REPLY},
    {"G",                RspWriteAllRegister,          TA_NEED_REPLY},
    {"g",                RspReadAllRegister,           TA_NEED_REPLY},
    {"p",                RspReadSingleRegister,        TA_NEED_REPLY},
    {"Z",                RspSetBreakpoint,             TA_NEED_REPLY},
    {"z",                RspRemoveBreakpoint,          TA_NEED_REPLY},
    {"!",                RspBreakDebugProc,            TA_NEED_REPLY},
    {"c",                RspContinue,                  TA_NO_REPLY  },
    {"s",                RspStep,                      TA_NO_REPLY  },
    {"qC",    	         RspGetCurThreadID,            TA_NEED_REPLY},
    {"qfThreadInfo",     RspGetThreadIdList,           TA_NEED_REPLY},
    {"qsThreadInfo",     RspGetThreadIdList,           TA_NEED_REPLY},
    {"Hg",    	         RspSetThreadSubsequentOptHg,  TA_NEED_REPLY},
    {"T",    	         RspThreadIsActive,            TA_NEED_REPLY},
    {"qThreadExtraInfo", RspThreadExtraInfo,           TA_NEED_REPLY},
    {"iThread",    	     RspSetMultiThreadInfo,        TA_NEED_REPLY},
    {NULL,               NULL,                         TA_NO_REPLY  }
};

/* 查询异常上下文 */
T_DB_ExceptionContext taGdbGetExpContext;

/************************实   现*******************************/

/*
 * @brief:
 *     基本调试初始化
 * @return:
 *     TRUE: 初始化成功
 *     FALSE: 初始化失败
 */
BOOL taDBInit(void)
{
    return (taDebugArchInit());
}

/*
 * @brief:
 *      设置是否是暂停命令标志
 * @return:
 *     无
 */
void taDebugSetStopCommandTag(BOOL stopTag)
{
    /* 设置当前暂停标志 */
    m_StopCommandTag = stopTag;
}

/*
 * @brief:
 *      获取暂停命令标志
 * @return:
 *     TRUE: 是暂停命令
 *     FALSE: 不是暂停命令
 */
BOOL taDebugGetStopCommandTag(void)
{
    /* 返回当前暂停标志 */
    return (m_StopCommandTag);
}

/*
 * @brief:
 *     激活初始化任务
 * @return:
 *     无
 */
void taDebugTaskActivate(T_DB_Info *debugInfo)
{
	/* 启动初始化任务 */
	if (taInitTaskId != 0)
	{
		/* 激活初始化任务 */
		if ((debugInfo->ctxType == TA_CTX_TASK_RTP) || (debugInfo->ctxType == TA_CTX_SYSTEM_RTP))
		{
			taTaskCont(taInitTaskId);
		}
		else
		{
			taTaskActivate(taInitTaskId);
		}
		taInitTaskId = 0;
	}
}

/*
 * @brief:
 *      处理查询当前产生异常的事件
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲,格式为?
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 */
T_TA_ReturnCode RspQueryEvent(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    UINT32 signal = 0;

    /* 将vector转换为POSIX信号 */
    signal = taDebugVector2Signal(debugInfo->context->vector, debugInfo->context);

    /* 设置输出缓冲为"SXX"，XX为signal的十六进制表示 */
    outbuf[0] = 'S';
    outbuf[1] = taHexChars[(signal >> 4) & 0xF];
    outbuf[2] = taHexChars[signal % 16];
    outbuf[3] = '\0';

    /* 设置输出字符串长度 */
    *outSize = strlen((const char *)outbuf);

    return (TA_OK);
}

/*
 * @brief:
 *      处理M命令写内存
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲,参数格式为address,size:xxxxxx
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:bpOutbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspWriteMemory(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    const UINT8* inPtr = NULL;
    UINT8 *outPtr = outbuf;
    UINT32 writeSize = 0;
    UINT32 writeAddr = 0;
    T_TA_ReturnCode ret = TA_FAIL;

    /* 跳过操作命令中的命令字 */
    inPtr = &(inbuf[1]);

    /* 按格式"address,size:xxxxxx"解析命令参数 */
    if ( !(( 0 != hex2int(&inPtr,&writeAddr))
        && ( *(inPtr++) == ',' )
        && ( 0 != hex2int(&inPtr, &writeSize))
        && ( *(inPtr++) == ':' ) ))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* 写内存字符串出出现非法的十六进制数 */
    if (-1 == hex2memEx(inPtr, outPtr, writeSize))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d", GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* 将转换后的内存数据写到writeAddr地址处 */
    ret = taDebugWriteMemory(debugInfo->objID, writeAddr, (const UINT8 *)outPtr, writeSize, Align_None);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* 刷新Cache */
    taDebugFlushCache(debugInfo->objID, writeAddr, writeSize);

    /* 设置OK至输出缓冲 */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *     处理读内存操作命令
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspReadMemory(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    const UINT8 *ptr = NULL;
    UINT32 readAddr = 0;
    UINT32 readSize = 0;
    T_TA_ReturnCode ret = TA_FAIL;

    /* 跳过操作命令中的命令字 */
    ptr = &(inbuf[1]);

    /* 按格式"m<addr>,<size>"解析命令参数 */
    if ( !(( 0 != hex2int(&ptr,&readAddr))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr,&readSize))
        && ( *ptr == '\0' )))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* 读取的长度大于一次读取允许读取的最大长度 */
    if (readSize > (TA_PKT_DATA_SIZE /2))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_READ_SIZE_ERROR);

        return (TA_FAIL);
    }

    /* 清空缓存 */
    ZeroMemory((void *)outbuf, (UINT32)(readSize*2));

    /* 读内存到输出缓冲中*/
    ret = taDebugReadMemory(debugInfo->objID, readAddr, outbuf, readSize, Align_None);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* 将读取的内存数据转换为十六进值字符流到输出缓冲中 */
    mem2hexEx(outbuf, readSize);

    /* 设置输出的字符串长度 */
    *outSize = readSize*DB_MEM_TO_CHAR_LEN;

    return (TA_OK);
}

/*
 * @brief:
 *      处理写二进制形式内存命令，GDB传送0x23(ASCII #),0x24(ASCII $)等扩展字符的格式为(0x7d 原始字符异或0x20),
 *      例如 0x7d以两个字节(0x7d 0x5d)传送。所以目标机段接受时需进行解析。如果当前字符为0x7d,则下个字符为(*src^ 0x20)
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲,参数格式为addr,size:xxxx
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspWriteBinaryMemory(T_DB_Info *debugInfo,
                                            const UINT8 *inbuf,
                                            UINT32 inSize,
                                            UINT8 *outbuf,
                                            UINT32 *outSize)
{
    const UINT8* ptr = NULL;
    const UINT8* src = NULL;
    UINT8* des = NULL;
    UINT32 writeSize = 0;
    UINT32 writeAddr = 0;
    UINT32 cnt = 0;
    UINT8 tempValue = 0;
    T_TA_ReturnCode ret = TA_FAIL;

    /* 跳过操作命令的命令字 */
    ptr = &(inbuf[1]);

    /* 按格式"Xaddr,size:xxxx…"解析命令参数 */
    if ( !( ( 0 != hex2int(&ptr,&writeAddr))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr,&writeSize))
        && ( *(ptr++) == ':' )
        && ( writeSize <= sizeof(m_Content_0x7d))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* 将参数中的二进制内存值中的被转义的字节还原为原值 */
    src = ptr;
    des = m_Content_0x7d;
    ZeroMemory((void *)m_Content_0x7d, (UINT32)sizeof(m_Content_0x7d));
    cnt = writeSize;

    /*
     * 处理转义字符算法如下:
     * 遍历待写入内存数据的所有字节, 将值为0x7d的字节作如下转换:
     * 将值为0x7d字节的下一个字节值与 0x20异或得出转义前的值。
     */
    while (0 != (cnt--))
    {
        if (*src == 0x7d)
        {
            src++;
            tempValue = (*src) ^ ((UINT8)0x20);
            *des = tempValue;
            src++;
            des++;
        }
        else
        {
            *des = *src;
            src++;
            des++;
        }
    }

    /* 将转换后的内存数据写到writeAddr内存地址起始处 */
    ret = taDebugWriteMemory(debugInfo->objID, writeAddr, m_Content_0x7d, writeSize, Align_None);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf,"E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL); 
    }

    /* 刷新Cache */
    taDebugFlushCache(debugInfo->objID, writeAddr, writeSize);

    /* 设置"OK"到输出缓冲 */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      处理二进制形式读内存命令
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲,参数格式为addr,size
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspReadBinaryMemory(T_DB_Info *debugInfo,
                                           const UINT8 *inbuf,
                                           UINT32 inSize,
                                           UINT8 *outbuf,
                                           UINT32 *outSize)
{
    const UINT8* ptr = NULL;
    UINT32 readSize = 0;
    UINT32 readAddr = 0;
    T_TA_ReturnCode ret = TA_FAIL;
    UINT32 maxOutputSize = *outSize - (UINT32)(1);/* 最大输出大小 */

    /* 跳过操作命令的命令字 */
    ptr = &(inbuf[1]);

    /* 按格式"x<addr>,<size>"解析操作参数 */
    if ( !(( 0 != hex2int(&ptr,&readAddr))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr,&readSize))
        && ( *(ptr++) == '\0')
        && ( readSize > 0)
        && (readSize <= maxOutputSize)))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* 读取内存 */
    ret = taDebugReadMemory(debugInfo->objID, readAddr, outbuf, readSize,Align_None);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf,"E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* 设置输出数据大小 */
    *outSize = readSize;

    return (TA_OK);
}

/*
 * @brief:
 *      处理写单个寄存器操作命令
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲,参数格式为regNum=value
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspWriteSingleRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    UINT32 regNum = 0;
    T_TA_ReturnCode ret = TA_FAIL;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[1];

    /* 按"P<regNum>=<value>"格式解析命令参数 */
    if (!(( 0 != hex2int(&ptr,&regNum))
        && (*(ptr++) == '=' )))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* 写寄存器regNum */
    ret = taDebugWriteSingleRegister(debugInfo->context, regNum, ptr);
    if ( TA_OK != ret )
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* 拷贝上下文,用于RSP协议命令读取,否则会导致IDE显示的寄存器未修改 */
    memcpy((void *)&taGdbGetExpContext, debugInfo->context, sizeof(T_DB_ExceptionContext));

    /* 设置输出缓冲为 "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      处理写所有寄存器操作命令
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspWriteAllRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    T_TA_ReturnCode ret = TA_FAIL;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[1];

    /* 写所有寄存器 */
    ret = taDebugWriteAllRegisters(debugInfo->context, ptr, strlen((const char *)ptr));
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* 拷贝上下文,用于RSP协议命令读取,否则会导致IDE显示的寄存器未修改 */
    memcpy((void *)&taGdbGetExpContext, debugInfo->context, sizeof(T_DB_ExceptionContext));

    /* 设置 "OK"到输出缓冲 */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      处理读所有寄存器操作命令
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspReadAllRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    T_TA_ReturnCode ret = TA_FAIL;

    /* 读取所有寄存器的值放到输出缓冲 */
    ret = taDebugReadAllRegisters(&taGdbGetExpContext, outbuf, outSize);
    if (TA_OK != ret)
    {
    	*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    return (TA_OK);
}

/*
 * @brief:
 *      处理读单个寄存器操作命令
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK:执行成功
 *    TA_FAIL: 执行失败
 */
T_TA_ReturnCode RspReadSingleRegister(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    T_TA_ReturnCode ret = TA_FAIL;
    const UINT8* ptr = NULL;
    UINT32 regNum = 0;

    /* 跳过操作命令中的命令字 */
    ptr = &(inbuf[1]);

    /* 按格式"p<regnum>"解析操作参数 */
    if ( !(( 0 != hex2int(&ptr,&regNum))
        && (*ptr == '\0')))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* 读取指定寄存器的值到输出缓冲中 */
    ret = taDebugReadSingleRegister(&taGdbGetExpContext, regNum, outbuf, outSize);
    if (TA_OK != ret)
    {
    	*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    return (TA_OK);
}

/*
 * @brief:
 *      处理设置断点命令,在指定地址处设置断点(软件/硬件断点)
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲,命令参数格式为type,address,size
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *     TA_OK：执行成功
 *     TA_FAIL:执行失败
 */
T_TA_ReturnCode RspSetBreakpoint(T_DB_Info *debugInfo,
                                        const UINT8 *inbuf,
                                        UINT32 inSize,
                                        UINT8 *outbuf,
                                        UINT32 *outSize)
{
    UINT32 type = 0;
    UINT32 address = 0;
    UINT32 size = 0;
    T_TA_ReturnCode ret = TA_FAIL;
    const UINT8 *ptr = NULL;
    T_DB_Breakpoint breakPoint;

    /* 清空断点信息 */
    ZeroMemory((void *)(&breakPoint), sizeof(breakPoint));

    /* 跳过操作命令中的命令字 */
    ptr = &inbuf[1];

    /* 按格式"Z<type>,<addr>,<size>"解析参数 */
    if ( !( ( 0 != hex2int(&ptr,&type))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr, &address))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr, &size))
        && ( (*ptr == '\0')  || (*ptr == ',')  || (*ptr == ' ') )))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* 设置断点信息 */
	breakPoint.logicAddr = address;
	breakPoint.bpType= type;
	breakPoint.objID = debugInfo->objID;
	breakPoint.sessionID = debugInfo->sessionID;
	breakPoint.bpClass = BPClass_NORMAL;
	breakPoint.breakSize = size;
#ifdef __MIPS__
	/*如果为软件单步断点则保存当前的任务ID*/
	if(breakPoint.bpType == DB_BP_SOFT_STEP)
	{
		stepThreadId = taTaskIdSelf();
	}
	else
	{
		stepThreadId = 0;
	}
#endif
    /* 插入断点 */
    ret = taDebugInsertBP(debugInfo, &breakPoint);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* 设置"OK"到输出缓冲  */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      处理删除断点操作命令。
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲,参数格式为type,address,size
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspRemoveBreakpoint(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    UINT32 type = 0;
    UINT32 address = 0;
    UINT32 size = 0;
    T_TA_ReturnCode ret = TA_FAIL;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令中的命令字 */
    ptr = &inbuf[1];

    /* 按格式"z<type>,<address>,<size>"解析参数 */
    if ( !( ( 0 != hex2int(&ptr, &type))
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr, &address) )
        && ( *(ptr++) == ',' )
        && ( 0 != hex2int(&ptr, &size))
        && ( *ptr == '\0' )))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        return (TA_FAIL);
    }

    /* 删除断点 */
    ret = taDebugRemoveBP(debugInfo, address, type, BPClass_NORMAL, size);
    if (TA_OK != ret)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* 设置OK到输出buffer */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      处理调试暂停命令
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *     TA_DB_NEED_IRET:执行成功
 */
T_TA_ReturnCode RspBreakDebugProc(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	/* 如果当前处于异常状态，则不执行trap函数 */
	if (!TA_DEBUG_IS_CONTINUE_STATUS(debugInfo))
	{
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
	}

	/* 设置暂停命令标志为TRUE */
	taDebugSetStopCommandTag(TRUE);

	/* trap进入异常 */
	taTrap();

    return (TA_OK);
}

/*
 * @brief:
 *    处理继续运行操作命令,返回到被调试程序继续运行
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *     TA_DB_NEED_IRET: 执行成功
 */
T_TA_ReturnCode RspContinue(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    /* 清除单步位 */
    taDebugContinue(debugInfo->context);

    /* 激活初始化任务 */
    taDebugTaskActivate(debugInfo);

    /* 需要从异常退出 */
    return (TA_DB_NEED_IRET);
}

/*
 * @brief:
 *      处理单步操作命令,置位标志寄存器的单步标志，使程序运行时产生单步异常
 * @param[in]:tpInfo:调试会话信息
 * @param[in]:bpInbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]: bpOutbuf:存储操作结果的数据缓冲
 * @param[out]: outSize:存储操作结果数据长度
 * @return:
 *     TA_DB_NEED_IRET: 需要从中断异常返回
 */
T_TA_ReturnCode RspStep(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
#ifdef __PPC85XX__
       /* 处理单步命令置位标志寄存器的单步标志位 */
	int ret =  taDebugStep(debugInfo->context);
	if(ret != TA_OK)
	{
		return (TA_FAIL);
	}
#else
       /* 处理单步命令置位标志寄存器的单步标志位 */
	taDebugStep(debugInfo->context);
#endif

    /* 激活初始化任务 */
	taDebugTaskActivate(debugInfo);

    /* 需要从异常退出 */
    return (TA_DB_NEED_IRET);
}

/*
 * @brief:
 *      处理"qC"命令,获取当前线程ID
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspGetCurThreadID(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	int threadId = 0;

	/* 获取当前调试线程ID */
	threadId = TA_GET_CUR_THREAD_ID(debugInfo);
	if (0 == threadId)
	{
	    /* 设置"l"到输出缓冲 */
	    *outSize = (UINT32)sprintf((INT8*)(outbuf), "l");
	    return (TA_FAIL);
	}

    /* 设置"QCthreadId"到输出缓冲 */
    *outSize = (UINT32)sprintf((INT8*)(outbuf), "QC%x",threadId);

	return (TA_OK);
}

/*
 * @brief:
 *      处理"qfThreadInfo"和"qsThreadInfo"命令,获取所有线程信息，调试器首先发送qfThreadInfo命令获取第一个线程信息
 *      然后发送qsThreadInfo命令获取剩下的线程信息，当目标机端回复'l'表示线程信息回复完毕
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspGetThreadIdList(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    static INT32 threadIdList[MAX_TASKS_NUM] = {0};
    static UINT32 count = 0;
    static UINT32 index = 0;
	
    /* 如果是qfThreadInfo命令，表示获取第一个线程ID，通过taskIdListGet获取线程ID列表  */
    if (!strncmp((const char *)inbuf, "qfThreadInfo", strlen("qfThreadInfo")))
    {
    	if (m_MultiThreadInfoTag == FALSE)
    	{
    		if(TA_GET_CUR_THREAD_ID(debugInfo) != 0)
    		{
    			count = 1;
    			threadIdList[0] = TA_GET_CUR_THREAD_ID(debugInfo);
    		}
    	}
    	else
    	{
			/* 获取所有线程ID列表 */
			count = taTaskIdListGet(threadIdList,MAX_TASKS_NUM);
    	}
    }

    /* 线程ID列表结束或者无线程 */
	if (count == index || count == 0)
	{
		/* 设置"l"到输出缓冲 */
		outbuf[0] = 'l';
		index = 0;
		*outSize = 1;
	}
	else
	{
		/* 输出剩余线程信息，设置"m<threadId>"到输出缓冲 */
	    *outSize = (UINT32)sprintf((INT8*)(outbuf), "m%x",threadIdList[index++]);
	}

    return (TA_OK);
}

/*
 * @brief:
 *      处理"Hg"命令,设置GDB要对线程后续的操作,Hg表示调试器后续要发送g命令读取所有寄存器
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspSetThreadSubsequentOptHg(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	const UINT8 *ptr = &inbuf[2];
    UINT32 threadId = 0;
    UINT32 cpuIndex = 0;
	T_DB_ExceptionContext *context = &taGdbGetExpContext;

    /* 按格式"Hg<threadID>"解析参数 */
    if (!(0 != hex2int(&ptr, &threadId)))
    {
        ptr = &inbuf[2];
    	if('-' == *ptr)
	    {
	  	    ptr++;
		    hex2int(&ptr, &threadId);
		    threadId = 0-threadId;
	    }
    	else
    	{
            *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
            return (TA_FAIL);
    	}
    }

    /* 如果线程ID为0，获取当前线程寄存器信息 */
    if (threadId == 0)
    {
		/* 获取当前调试线程ID */
		threadId = taTaskIdSelf();
    }

    /* 线程是否处于激活状态 */
    if (taTaskIdVerify(threadId) != OK)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

	/* 如果是获取当前产生异常的任务寄存器，拷贝异常上下文到taGdbGetExpContext中 */
    if (((cpuIndex = sdaGetCpuIdByThreadId(threadId)) != 0xffffffff) && (DEBUGMODE_SYSTEM == taDebugModeGet()))
	{
        memcpy((void *)context, (void *)taSDAExpContextPointerGet(cpuIndex), sizeof(T_DB_ExceptionContext));
	}
    else if ((TA_GET_CUR_THREAD_ID(debugInfo) == threadId) && (DEBUGMODE_TASK == taDebugModeGet()))
    {
    	memcpy((void *)context, debugInfo->context, sizeof(T_DB_ExceptionContext));
    }
	else
	{
		/* 其他任务通过操作系统的接口获取任务切换时的上下文 */
		if (OK != taTaskContextGet(threadId, context))
		{
    		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
    		return (TA_OK);
		}
	}

    /* 设置"OK"到输出缓冲 */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      处理"T"命令,查看线程是否处于激活状态
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspThreadIsActive(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	const UINT8 *ptr = &inbuf[1];
	UINT32 threadId = 0;

    /* 按格式"T<threadID>"解析参数 */
    if (!(0 != hex2int(&ptr, &threadId)))
    {
        ptr = &inbuf[1];
    	if('-' == *ptr)
	    {
	 	    ptr++;
	 	    hex2int(&ptr, &threadId);
	 	    threadId = 0-threadId;
	    }
    	else
    	{
            *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
            return (TA_FAIL);
    	}
    }



    /* 如果显示单个线程信息，查询其它线程回复线程未激活 */
    if (m_MultiThreadInfoTag == FALSE)
    {
    	if (threadId != TA_GET_CUR_THREAD_ID(debugInfo))
    	{
    		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
    		return (TA_OK);
    	}
    }

    /* 线程是否处于激活状态 */
    if (taTaskIdVerify(threadId) != OK)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_EXEC_COMMAND);
        return (TA_FAIL);
    }

    /* 设置"OK"到输出缓冲 */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "OK");

	return (TA_OK);
}

/*
 * @brief:
 *      处理"qThreadExtraInfo,tid"命令,获取线程额外信息,额外信息包括线程ID,线程名和核信息
 * @param[in]:debugInfo: 调试会话信息
 * @param[in]:inbuf: 包含操作命令的数据缓冲
 * @param[in]:inSize: 操作命令数据长度
 * @param[out]:outbuf: 存储操作结果的数据缓冲
 * @param[out]:outSize: 存储操作结果数据长度
 * @return:
 *    TA_OK：执行成功
 *    TA_FAIL:执行失败
 */
T_TA_ReturnCode RspThreadExtraInfo(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
	const UINT8 *ptr = &inbuf[17];
	UINT32 cpuAffinity = 0;
	UINT32 threadId = 0;

    /* 按格式"qThreadExtraInfo,threadID"解析参数 */
    if (!(0 != hex2int(&ptr, &threadId)))
    {
        ptr = &inbuf[17];
    	if('-' == *ptr)
	    {
	 	    ptr++;
	 	    hex2int(&ptr, &threadId);
	 	    threadId = 0-threadId;
	    }
    	else
    	{
    		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%02d",GDB_INVALID_PARAM);
        	return (TA_FAIL);
    	}
    }

	/* 获取CPU亲和力 */
	cpuAffinity = sdaGetCpuIdByThreadId(threadId);

    /* 设置"OK"到输出缓冲 */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "%x,%s,%x",threadId,taTaskName(threadId),cpuAffinity);

    /* 转换为十六进制字符串 */
    mem2hexEx(outbuf, *outSize);

    /* 设置输出的字符串长度 */
    *outSize = (*outSize)*DB_MEM_TO_CHAR_LEN;

	return (TA_OK);
}

/*
 * @brief:
 *      设置多线程信息，如果多线程标志为TRUE回复多线程信息，否则只回复当前产生异常的线程信息
 * @param[in]:tpInfo:调试会话信息
 * @param[in]:bpInbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:bpOutbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *     TA_FAIL: 操作失败
 *     TA_OK: 操作成功
 */
T_TA_ReturnCode RspSetMultiThreadInfo(T_DB_Info *debugInfo,
                   const UINT8 *inbuf,
                   UINT32 inSize,
                   UINT8 *outbuf,
                   UINT32 *outSize)
{
    const UINT8* ptr = NULL;
    UINT32 value = 0;

    /* 跳过操作命令的命令字 */
    ptr = &(inbuf[7]);

    /* 按格式解析操作参数 */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&value))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 设置多线程信息标志 */
    m_MultiThreadInfoTag = value;

    /* 设置"OK"到输出缓冲 */
    *outSize = (UINT32)sprintf((INT8 *)outbuf, "OK");

	 return (TA_OK);
}

/**********************下面代码用于同步多线程标志*****************************/
#ifdef _KERNEL_DEBUG_
/*
 * @brief:
 *      获取多线程标志
 * @return:
 *     多线程标志
 */
BOOL taGetMultiThreadInfoTag()
{
	return m_MultiThreadInfoTag;
}

#else

/*
 * @brief:
 *      设置多线程标志
 * @param[in]:tag: 多线程标志
 * @return:
 *     无
 */
void taSetMultiThreadInfoTag(BOOL tag)
{
    m_MultiThreadInfoTag = tag;
}
#endif


