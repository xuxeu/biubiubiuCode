/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-05-26         彭元志，北京科银京成技术有限公司
 *                           创建该文件。
 */

/**
 * @file  mdlCmdLib.c
 * @brief
 *       功能：
 *       <li>动态下载模块管理</li>
 */

/************************头 文 件******************************/
#include "mdlInitLib.h"
#include "mdlCmdLib.h"
#include "mdlLib.h"
#include "loadLib.h"
#include "symLib.h"
#include "sysSymTbl.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/*
 * @brief:
 *      加载模块,命令格式"ml:<modulePath>:textAdd:textSize:dataAdd:dataSize:bssAdd:bssSize"
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode LoadModule(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);
/*
 * @brief:
 *      卸载模块,命令格式"mu:moduleID"
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode UnloadModule(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      分配模块内存,命令格式"ma:fileSize",主机端通过该命令获取模块存放地址，
 *      然后根据该地址链接成绝对定位的ELF
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode AllocateModuleMemory(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);
/*
 * @brief:
 *      释放模块内存,命令格式"mf:address",一般用于分配模块内存后，
 *      主机端在链接成ELF过程中出现问题后，需要发送该命令释放目标机端分配的内存
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode FreeModuleMemory(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      添加模块符号到系统符号表中,命令格式为"ms:<symbolName>:symbolValue:symbolType:groupId:moduleId"
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode AddModuleSymbol(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      同步模块符号,命令格式为"mg:moduleId",如果模块ID为0表示获取内核映像模块符号否则为DKM项目模块符号
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode SyncModuleSymbol(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      获取所有模块信息,命令格式为"mi"，如果模块是通过代码从非易失性存储介质加载，调试时IDE需要通过模块
 *      信息链接成静态的ELF文件
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode GetAllModuleInfo(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      执行模块符号,命令格式为"mc:funcAdd:priority:size:options:arg...."
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode CallModuleFunc(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/*
 * @brief:
 *      执行模块C++的构造函数,命令格式为"mp:moduleId"
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode CallCplusplusCtors(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize);

/************************模块变量*****************************/

/* 查询命令表 */
static T_TA_CommandTable mdlModuleCommandTable[MDL_MODULE_COMMAND_NUM] =
{
	{"ml",  LoadModule,            TA_NEED_REPLY},/* 加载模块 */
    {"mu",  UnloadModule,          TA_NEED_REPLY},/* 卸载模块 */
    {"mf",  FreeModuleMemory,      TA_NEED_REPLY},/* 模块内存释放 */
    {"ma",  AllocateModuleMemory,  TA_NEED_REPLY},/* 分配模块内存 */
    {"ms",  AddModuleSymbol,       TA_NEED_REPLY},/* 添加模块符号 */
    {"mg",  SyncModuleSymbol,      TA_NEED_REPLY},/* 同步模块符号 */
    {"mi",  GetAllModuleInfo,      TA_NEED_REPLY},/* 获取所有模块信息 */
    {"mc",  CallModuleFunc,        TA_NEED_REPLY},/* 模块函数执行 */
    {"mp",  CallCplusplusCtors,    TA_NEED_REPLY},/* 模块的构造函数执行 */
    {NULL,  NULL,                  TA_NO_REPLY  }
};

/************************全局变量*****************************/

/************************函数实现*****************************/

/*
 * @brief:
 * 		动态加载代理消息包处理函数
 * @param[in]:packet: 要处理的消息包
 * @return:
 *     TA_OK: 命令执行成功
 *     TA_FAIL：命令执行失败
 */
static T_TA_ReturnCode mdlModuleCommandProc(T_TA_Packet *packet)
{
    /* 调用taAgentEntry处理收到的消息包 */
    return (taAgentEntry(packet,NULL,COMMANDTYPE_NONE,mdlModuleCommandTable,
    		MDL_MODULE_COMMAND_NUM,NULL,NULL));
}

/*
 * @brief:
 *     安装动态加载消息处理函数
 * @return:
 *     无
 */
void mdlModuleCommandInit(void)
{
	/* 安装动态加载消息处理函数 */
	taInstallMsgProc(TA_DYNAMIC_LOADER, mdlModuleCommandProc);
}

/*
 * @brief:
 *      卸载模块,命令格式"mu:moduleID"
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode UnloadModule(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 moduleId = 0;
    STATUS status = OK;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[2];

    /* 按"mu:moduleID"格式解析命令参数 */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&moduleId))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 卸载模块 */
    status = mdlUnloadModule((MODULE_ID)moduleId);
    if (OK != status)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_UNLOAD_MODULE_FAIL);
        return (TA_FAIL);
    }

    /* 设置输出缓冲为 "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return(TA_OK);
}

/*
 * @brief:
 *      执行模块C++的构造函数,命令格式为"mp:moduleId"
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode CallCplusplusCtors(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 moduleId = 0;
    STATUS status = OK;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[2];

    /* 按"mp:moduleID"格式解析命令参数 */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&moduleId))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 检查模块是否有效 */
    if (NULL == mdlFigureModuleId((MODULE_ID)moduleId))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_MODULE_ID);
        return (TA_FAIL);
    }

    /* 执行构造函数 */
    status = mdlCplusCallCtors((MODULE_ID)moduleId);
    if (OK != status)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 设置输出缓冲为 "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      分配模块内存,命令格式"ma:fileSize",主机端通过该命令获取模块存放地址，
 *      然后根据该地址链接成绝对定位的ELF
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode AllocateModuleMemory(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 allocSize = 0;
    void *allocAddr = NULL;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[2];

    /* 按"ma:fileSize"格式解析命令参数 */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&allocSize))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 分配模块内存 */
    allocAddr = mdlAllocateModuleMem(MDL_ALLOC_ALIGN_SIZE, allocSize);
    if (NULL == allocAddr)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_ALLOC_MODULE_MEM_FAIL);
        return (TA_FAIL);
    }

    /* 设置输出缓冲为 "OK:<allocAddr>" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%x",(UINT32)allocAddr);

    return (TA_OK);
}

/*
 * @brief:
 *      释放模块内存,命令格式"mf:address",一般用于分配模块内存后，
 *      主机端在链接成ELF过程中出现问题后，需要发送该命令释放目标机端分配的内存
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode FreeModuleMemory(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 address = 0;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[2];

    /* 按"mf:address"格式解析命令参数 */
    if (!((*(ptr++) == ':') && (0 != hex2int(&ptr,&address))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 释放模块内存 */
    mdlFreeModuleMem((void *)address);

    /* 设置输出缓冲为 "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return (TA_OK);
}

/*
 * @brief:
 *      加载模块,命令格式"ml:<modulePath>:textAdd:textSize:dataAdd:dataSize:bssAdd:bssSize"
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode LoadModule(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 textAdd = 0;
	UINT32 textSize = 0;
    UINT32 dataAdd = 0;
    UINT32 dataSize = 0;
    UINT32 bssAdd = 0;
    UINT32 bssSize = 0;
    char modulePath[PATH_MAX] = {0};
    char *pModulePath = modulePath;
    MODULE_ID moduleId;
    STATUS status = OK;
    MDL_MOD_INFO moduleInfo;
    const UINT8 *ptr = NULL;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[2];

    /* 按"ml:<modulePath>:textAdd:textSize:dataAdd:dataSize:bssAdd:bssSize"格式解析命令参数 */
    if (!((*(ptr++) == ':')
    	&& (0 != hex2str(&ptr, (UINT8 **)&pModulePath, PATH_MAX))
    	&& (*(ptr++) == ':')
    	&& (0 != hex2int(&ptr,&textAdd))
        && (*(ptr++) == ':' )
        && (0 != hex2int(&ptr, &textSize))
        && (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr,&dataAdd))
        && (*(ptr++) == ':' )
        && (0 != hex2int(&ptr, &dataSize))
        && (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr,&bssAdd))
        && (*(ptr++) == ':' )
        && (0 != hex2int(&ptr, &bssSize))))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 设置模块信息 */
    moduleInfo.name = modulePath;
    moduleInfo.flags = LOAD_GLOBAL_SYMBOLS;

    /* 设置代码段信息，SEG_FREE_MEMORY表示删除段时释放该段内存  */
    moduleInfo.segment[0].address = textAdd;
    moduleInfo.segment[0].size = textSize;
    moduleInfo.segment[0].type = SEGMENT_TEXT;
    moduleInfo.segment[0].flags = SEG_FREE_MEMORY;

    /* 设置数据段信息，SEG_FREE_MEMORY表示删除段时释放该段内存 */
    moduleInfo.segment[1].address = dataAdd;
    moduleInfo.segment[1].size = dataSize;
    moduleInfo.segment[1].type = SEGMENT_DATA;
    moduleInfo.segment[1].flags = 0;

    /* 设置BSS段信息，SEG_FREE_MEMORY表示删除段时释放该段内存 */
    moduleInfo.segment[2].address = bssAdd;
    moduleInfo.segment[2].size = bssSize;
    moduleInfo.segment[2].type = SEGMENT_BSS;
    moduleInfo.segment[2].flags = 0;

    /* 加载模块 */
    status = mdlLoadModule(&moduleInfo, &moduleId);
    if (OK != status)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_LOAD_MODULE_FAIL);
        return (TA_FAIL);
    }

    /* 设置回复信息为"OK:moduleId:groupId" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%x:%x",(UINT32)moduleId, moduleId->group);

    return (TA_OK);
}

/*
 * @brief:
 *      添加模块符号到系统符号表中,命令格式为"ms:<symbolName>:symbolValue:symbolType:groupId:moduleId"
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode AddModuleSymbol(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
	UINT32 symbolValue = 0;
	UINT32 symbolType = 0;
	UINT32 groupId = 0;
	UINT32 moduleId = 0;
	SYMBOL_ID pSymbol = NULL;
	const UINT8 *ptr = NULL;
    char symbolName[PATH_MAX] = {0};
    char *pSymbolName = symbolName;

	/* 跳过操作命令的命令字 */
	ptr = &inbuf[2];

	/* 按"ms:<symbolName>:symbolValue:symbolType:groupId:moduleId"格式解析命令参数 */
	if ( !((*(ptr++) == ':')
		&& (0 != hex2str(&ptr, (UINT8 **)&pSymbolName, PATH_MAX))
		&& (*(ptr++) == ':')
		&& (0 != hex2int(&ptr,&symbolValue))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr, &symbolType))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr,&groupId))
		&& (*(ptr++) == ':' )
		&& (0 != hex2int(&ptr,&moduleId))))
	{
		*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
		return (TA_FAIL);
	}

	/* 注册模块符号到系统符号表 */
	pSymbol = mdlRegisterModuleSym(symbolName, (char*)symbolValue, (SYM_TYPE)symbolType, (UINT16)groupId, moduleId);
    if (NULL == pSymbol)
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_ADD_SYMBOL_FAIL);
        return (TA_FAIL);
    }

    /* 设置输出缓冲为 "OK" */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK");

    return(TA_OK);
}

/*
 * @brief:
 *      同步模块符号,命令格式为"mg:moduleId",如果模块ID为0表示获取内核模块所有符号否则为DKM项目模块符号，
 *      主机通过该命令获取目标机端符号，用于符号调试和链接DKM模块，因为DKM模块可能存在许多外部链接符号，
 *      符号定位该符号地址。
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode SyncModuleSymbol(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
    UINT32 moduleId = 0;
	UINT32 len = 0;
    const UINT8 *ptr = NULL;
    UINT8 *pTemp = outbuf;
    SYMBOL *pSymbol = NULL;
    static UINT32 count = 0;
	static UINT32 index = 0;

    /* 跳过操作命令的命令字 */
    ptr = &inbuf[2];

    /* 按"mg:moduleId"格式解析命令参数 */
    if (!((*(ptr++) == ':') && 0 != hex2int(&ptr,&moduleId)))
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 检查模块是否存在,如果模块ID为0表示获取内核映像模块符号,无需检查模块是否存在 */
    if ((NULL == mdlFigureModuleId((MODULE_ID)moduleId)) && (moduleId != 0) )
    {
        *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
        return (TA_FAIL);
    }

    /* 如果count为0表示获取所有符号ID */
    if (count == 0)
    {
    	count = mdlGetModuleSymIdList((MODULE_ID)moduleId);
    }

    /* 设置回复结果 */
    len = sprintf((INT8*)pTemp,"%s","OK");
    pTemp += len;

    while (index < count)
    {
    	if (((pTemp) - (outbuf) + MDL_SYMBOL_MAX_INFO_LEN) > TA_PKT_DATA_SIZE)
    	{
    		break;
    	}

    	/* 获取符号信息 */
    	pSymbol = mdlModuleSymbol[index++];

    	/* 设置回复信息为"OK,name,val,type,group" */
    	len = (UINT32)sprintf((INT8*)pTemp, ":%s,%x,%x,%x", pSymbol->name, pSymbol->value, pSymbol->type, pSymbol->group);

    	pTemp += len;
    }

    /* 设置输出缓冲为 "$",表示符号信息发送完毕 */
    if (index == count)
    {
    	*pTemp = '$';
    	pTemp += 1;
    	index = count = 0;
    	mdlFreeModuleMem(mdlModuleSymbol);
    }

    /* 设置输出缓冲数据大小 */
    *outSize = pTemp - outbuf;

    return (TA_OK);
}

/*
 * @brief:
 *      获取所有模块信息,命令格式为"mi"
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode GetAllModuleInfo(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
	static MODULE_ID moduleIdList[MDL_MAX_MODULE_ID_NUM] = {0};
	static UINT32 count = 0;
	static UINT32 index = 0;
	UINT8* ptr = outbuf;
	UINT32 len = 0;
	MODULE_ID moduleId = 0;
	MODULE_INFO moduleInfo;

    /* 获取模块ID列表  */
    if (count == 0)
    {
    	/* 获取所有模块ID */
    	count = moduleIdListGet(moduleIdList, MDL_MAX_MODULE_ID_NUM);
    }

    /* 设置回复结果 */
    len = sprintf((INT8*)ptr,"%s","OK");
    ptr += len;

    while (index < count)
    {
    	if (((ptr) - (outbuf) + MDL_MODULE_MAX_INFO_LEN) > TA_PKT_DATA_SIZE)
    	{
    		break;
    	}

		/* 设置模块ID */
		moduleId = moduleIdList[index++];

		/* 获取模块信息 */
		mdlGetModuleInfo(moduleId, &moduleInfo);

		/* 设置模块信息为"OK:moduleID,groupID,modulePath,textAdd,dataAdd,bssAddr,textSize,dataSize,bbsSize" */
		len = (UINT32)sprintf((INT8*)ptr, ";%x,%x,%s,%s,%x,%x,%x,%x,%x,%x",(UINT32)moduleId, moduleId->group,moduleId->name,moduleId->path,
						moduleInfo.segInfo.textAddr, moduleInfo.segInfo.dataAddr, moduleInfo.segInfo.bssAddr,
						moduleInfo.segInfo.textSize,moduleInfo.segInfo.dataSize,moduleInfo.segInfo.bssSize);

		ptr += len;
    }

    /* 设置输出缓冲为 "$",表示模块信息发送完毕 */
    if (index == count)
    {
    	*ptr = '$';
    	ptr += 1;
    	index = count = 0;
    }

    /* 设置输出缓冲数据大小 */
    *outSize = ptr - outbuf;

    return (TA_OK);
}

/*
 * @brief:
 *      执行模块符号,命令格式为"mc:funcAdd:priority:size:options:arg...."
 * @param[in]:debugInfo:调试会话信息
 * @param[in]:inbuf:包含操作命令的数据缓冲
 * @param[in]:inSize:操作命令数据长度
 * @param[out]:outbuf:存储操作结果的数据缓冲
 * @param[out]:outSize:存储操作结果数据长度
 * @return:
 *      TA_OK: 操作成功
 *      TA_FAIL:执行失败
 */
static T_TA_ReturnCode CallModuleFunc(T_DB_Info* debugInfo,
                                    const UINT8 *inbuf,
                                    UINT32 inSize,
                                    UINT8 *outbuf,
                                    UINT32 *outSize)
{
	MDL_CREATE_DESC mdlCreateDesc;
	UINT32 textStart = 0;
	INT32 status = OK;

	/* 创建描述符 */
    if (!mdlCtxCreateDesc(inbuf, &mdlCreateDesc))
    {
    	 *outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
    	 return (TA_FAIL);
    }

    /* 根据类型调用不同的创建函数 */
    if (MDL_CRT_TASK == mdlCreateDesc.contextType)
    {
    	status = mdlCreateTask(&mdlCreateDesc);
    }
    else if (MDL_CRT_RTP == mdlCreateDesc.contextType)
    {
    	status = mdlCreateRtp(&mdlCreateDesc,&textStart);
    }
    else
    {
    	*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
    	return (TA_FAIL);
    }

    /* 执行模块函数或者创建RTP失败 */
    if (ERROR == status)
    {
    	*outSize = (UINT32)sprintf((INT8*)outbuf, "E%d",TA_INVALID_INPUT);
    	return (TA_FAIL);
    }

    /* 设置输出缓冲 */
    *outSize = (UINT32)sprintf((INT8*)outbuf, "OK:%x:%x",textStart,status);

    return (TA_OK);
}
