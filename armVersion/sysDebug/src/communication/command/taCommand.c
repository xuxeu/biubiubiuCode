/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 * 修改历史:
 * 2013-06-29         彭元志，北京科银京成技术有限公司
 *                          创建该文件。 
 */

/*
 * @file:taCommand.c
 * @brief:
 *             <li>TA消息接口表管理实现</li>
 */

/************************头 文 件******************************/
#include <string.h>
#include "taCommand.h"
#include "taManager.h"
#include "db.h"
#include "ta.h"

/************************宏 定 义*******************************/

/************************类型定义******************************/

/************************外部声明******************************/

/* 查询命令表 */
extern T_TA_CommandTable taManagerCommandTable[TA_MANAGER_COMMANDNUM];

/************************前向声明******************************/

/************************模块变量******************************/

/************************全局变量******************************/

/************************函数实现******************************/

/*
 * @brief:
 *      从命令表中搜索指定命令
 * @param[in]:cmdTable: 要搜索的命令表
 * @param[in]:cmdStr: 命令字符串
 * @param[in]:tableLength: 命令表长度
 * @return:
 *     NULL: 未找到相应命令
 *     找到相应命令返回命令数据结构指针
 */
static T_TA_CommandTable* taSearchCommand(T_TA_CommandTable *cmdTable,
                                   const UINT8 *cmdStr,
                                   UINT32 tableLength)
{
    INT32 idx = 0;
    INT32 cmdIdx = -1;

    /* 进行参数合法性检  */
    if (( NULL == cmdTable ) || (NULL == cmdStr ) || (0 == cmdStr[0]))
    {
        return (NULL);
    }

    /* 依次遍历命令表，找匹配命令 */
    while ((idx < (INT32)tableLength) && (NULL != cmdTable[idx].ubCmdStr))
    {
        /* 需要完全匹配关键字 */
		if (0 == strncmp((const char *)cmdStr, (const char*)(cmdTable[idx].ubCmdStr), strlen(cmdTable[idx].ubCmdStr)))
		{
            /* 记录查询到元素的索引号 */
            cmdIdx = idx;
			break;
		}
        idx++;
    }

    return ((cmdIdx == -1) ? NULL : &(cmdTable[cmdIdx]));
}

/*
 * @brief:
 *     解析执行调试器命令
 * @param[in]:   debugInfo: 调试代理的基本信息
 * @param[in]:   inbuf:输入的待处理的包体，无包头
 * @param[in]:   inSize: 包的大小
 * @param[in]:   commandType: 命令类型
 * @param[in]:   cmdTable: 在命令表 cmdTable 中查询命令
 * @param[out]:  outbuf: 处理完后待输出的包体，无包头
 * @param[out]:  outSize: 处理完后回复数据的字节长度
 * @param[out]:  isNeedReply: 是否需要回复
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_FAIL: 命令执行失败
 *     TA_DB_NEED_IRET：需要从异常中返回
 *     TA_COMMAND_NOTFOUND: 命令没找到
 */
T_TA_ReturnCode taProcessCommand(T_DB_Info *debugInfo,
                                 UINT8 *inbuf,
                                 UINT32 inSize,
                                 UINT8 *outbuf,
                                 UINT32 *outSize,
                                 BOOL *isNeedReply,
                                 T_TA_CommandType commandType,
                                 T_TA_CommandTable *cmdTable,
                                 UINT32 tableLength)
{
	UINT32 ret = TA_OK;
	T_TA_CommandTable *cmdHandle = NULL;

    /* 如果命令表cmdTable为空，则在默认命令表中查询 */
    if (NULL == cmdTable)
    {
        switch (commandType)
        {
			/* 如果是查询命令，则从查询表中查找*/
			case COMMANDTYPE_QUERY:
			{
				/* 从信息查询命令表中搜索指定命令 */
				cmdHandle = taSearchCommand(taManagerCommandTable, (const UINT8 *)inbuf, TA_MANAGER_COMMANDNUM);
			}
            break;
			case COMMANDTYPE_DEBUG:
			{
				/* 从RSP命令表中搜索指定命令 */
				cmdHandle = taSearchCommand(taRSPCmdTable, (const UINT8 *)inbuf, TA_RSP_TABLE_LEN);
			}
			break;
        default:
            break;
        }
    }
    else
    {
        /* 已提供命令表，在命令表中查询 */
        cmdHandle = taSearchCommand(cmdTable, (const UINT8 *)inbuf,tableLength);
    }

    /* 未找到相应命令 */
    if (NULL == cmdHandle)
    {
        *isNeedReply = TA_NEED_REPLY;

        return (TA_COMMAND_NOTFOUND);
    }

    /* 调用命令处理函数处理输入的命令 */

    ret = cmdHandle->cmdProc(debugInfo, inbuf, inSize, outbuf, outSize);

    /* 设置回复标志 */
    *isNeedReply = cmdHandle->ubNeedReply;

    return (ret);
}
