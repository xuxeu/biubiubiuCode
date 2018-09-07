/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-2         彭元志，北京科银京成技术有限公司
 *                           创建该文件。
 */

/**
 * @file  mdlLib.c
 * @brief
 *       功能：
 *       <li>动态下载模块管理</li>
 */

/************************头 文 件******************************/
#include "mdlInitLib.h"
#include "mdlLib.h"
#include "mdlCmdLib.h"
#include "unldLib.h"
#include "loadLib.h"
#include "sysSymTbl.h"
#include "moduleLib.h "
#include "symLibP.h"
#include "loadShareLibP.h"
#include "rtpLibCommon.h "
#include "rtpLibP.h"
#include "cplusLoadP.h"
#include "ta.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/* 模块符号数 */
static int mdlSymbolNumber = 0;

/* 存放模块符号ID */
SYMBOL **mdlModuleSymbol = NULL;
/************************全局变量*****************************/

/* 初始化任务ID，用于调试时运行初始化任务 */
int taInitTaskId = 0;

/************************函数实现*****************************/

/*
 * @brief
 *    加载模块
 * @param[in]: pModInfo: 模块信息
 * @param[out]: pModuleId: 模块ID
 * @return
 *    ERROR：加载模块失败
 *    OK：加载模块成功
 */
STATUS mdlLoadModule(MDL_MOD_INFO *pModInfo, MODULE_ID *pModuleId)
{
	MODULE_ID moduleId;
	STATUS status = OK;
	int i = 0;

	/* 创建模块 */
	moduleId = moduleCreate((char*)pModInfo->name, MODULE_ELF, pModInfo->flags);
	if (NULL == moduleId)
	{
		return (ERROR);
	}

	/* 添加模块段信息 */
    for (i = 0; i < MDL_MOD_NUM_SEGS; i++)
    {
        status = moduleSegAdd(moduleId, pModInfo->segment[i].type,
    						(void *)pModInfo->segment[i].address,
    						pModInfo->segment[i].size, pModInfo->segment[i].flags);
    	if (OK != status)
    	{
    		return (ERROR);
    	}
    }

    /* 记录模块ID */
    *pModuleId = moduleId;

    return (OK);
}

/*
 * @brief
 *    卸载模块
 * @param[in]: moduleId: 模块ID
 * @return
 *    ERROR：卸载模块失败
 *    OK：卸载模块成功
 */
STATUS mdlUnloadModule(MODULE_ID moduleId)
{
    /* 调用loader库的模块卸载接口卸载模块 */
    return unldByModuleId(moduleId, UNLD_FORCE | UNLD_CPLUS_XTOR_AUTO);
}

/*
 * @brief
 *    释放模块内存
 * @param[in]: address: 模块内存地址
 * @return
 *    无
 */
void mdlFreeModuleMem(void *address)
{
	/* 释放内存 */
	free(address);
}

/*
 * @brief
 *    分配模块内存
 * @param[in]: alignment: 分配内存对齐大小
 * @param[in]: size: 分配的内存大小
 * @return
 *    NULL：分配模块内存失败
 *    内存地址：分配模块内存成功
 */
void* mdlAllocateModuleMem(UINT32 alignment,UINT32 size)
{
	/* 调用系统的内存分配接口分配内存 */
	void * addr = loadShareTgtMemAlign(MDL_ALLOC_ALIGN_ADDRESS,MDL_ROUND_UP(size, alignment));

	/* 清空内存 */
	loadShareTgtMemSet(addr, 0, MDL_ROUND_UP(size, alignment));

	return (addr);
}

/*
 * @brief
 *    检查模块是否存在
 * @param[in]: moduleId: 模块ID
 * @return
 *    NULL：模块不存在
 *    模块ID：模块存在
 */
MODULE_ID mdlFigureModuleId(MODULE_ID moduleId)
{
	/* 检查模块是否存在 */
	return moduleIdFigure(moduleId);
}

/*
 * @brief
 *    注册模块符号到系统符号表
 * @param[in]: symbolName: 符号名
 * @param[out]: symbolValue: 符号值
 * @param[out]: symbolType: 符号类型
 * @param[out]: groupId: 组ID
 * @param[out]: symRef: 符号引用，一般为模块ID
 * @return
 *    ERROR：符号注册失败
 *    OK：符号注册成功
 */
SYMBOL_ID mdlRegisterModuleSym(char *symbolName, char *symbolValue, SYM_TYPE symbolType, UINT16 groupId, UINT32 symRef)
{
	return symRegister(sysSymTbl, symbolName, symbolValue, symbolType, groupId, symRef, FALSE);
}

/*
 * @brief
 *    获取核心符号个数
 * @return
 *    核心符号个数
 */
UINT32 mdlGetSymbolsNum(void)
{
	return sysSymTbl->nsymbols;
}

/*
 * @brief
 *    获取模块符号ID
 * @param[in]: name: 符号名
 * @param[out]: val: 符号值
 * @param[out]: type: 符号类型
 * @param[out]: info: 匹配信息
 * @param[out]: group: 组ID
 * @param[out]: pSymbol: 符号节点ID
 * @return
 *    TRUE：继续执行
 *    FALSE：不继续执行
 */
BOOL mdlGetModuleSymId(char *name, int val, SYM_TYPE type, MDL_MATCH_INFO *info, UINT16 group, SYMBOL *pSymbol)
{
	/* 符号不属于该模块 */
    if (pSymbol->symRef != (UINT32)info->moduleId)
	{
    	return (TRUE);
	}

    /* 检查存储模块符号ID的内存地址是否为NULL */
    if (NULL == info->moduleSymbol)
    {
    	return (FALSE);
    }

    /* 设置符号ID */
    (info->moduleSymbol)[mdlSymbolNumber++] = pSymbol;

    return (TRUE);
}

/*
 * @brief
 *    获取模块符号ID列表
 * @param[in]: moduleId: 模块ID
 * @return
 *    模块符号数
 */
UINT32 mdlGetModuleSymIdList(MODULE_ID moduleId)
{
    MDL_MATCH_INFO matchInfo;
    UINT32 symbolNum = 0;

    /* 设置模块符号数为0 */
    mdlSymbolNumber = 0;

	/* 设置模块ID */
	matchInfo.moduleId = moduleId;

	/* 获取所有模块的符号个数 */
	symbolNum = mdlGetSymbolsNum();

	/* 分配内存，存放符号ID  */
	mdlModuleSymbol = (SYMBOL **)mdlAllocateModuleMem(MDL_ALLOC_ALIGN_SIZE, symbolNum * sizeof(SYMBOL*));

	/* 设置存放符号地址的地址 */
	matchInfo.moduleSymbol = mdlModuleSymbol;

	/* 获取所有符号ID  */
	symEach(sysSymTbl, (FUNCPTR)mdlGetModuleSymId, (int)&matchInfo);

	return (mdlSymbolNumber);
}

/*
 * @brief
 *    获取模块信息
 * @param[in]: moduleId: 模块ID
 * @param[out]: pModuleInfo: 存储模块信息
 * @return
 *    ERROR：模块ID无效
 *    OK：获取模块信息成功
 */
STATUS mdlGetModuleInfo(MODULE_ID moduleId, MODULE_INFO *pModuleInfo)
{
	/* 获取模块信息 */
	return moduleInfoGet(moduleId, pModuleInfo);
}

/*
 * @brief
 *    加载模块的C++构造函数
 * @param[in]: moduleId: 模块ID
 * @return
 *    ERROR：加载失败
 *    OK：加载成功
 */
STATUS mdlCplusCallCtors(MODULE_ID moduleId)
{
	return cplusLoadFixup(moduleId, sysSymTbl, XTOR_CALL);
}

/*
 * @brief
 *    解析创建MDL_CREATE_DESC结构体
 * @param[in]: inbuf: 输入数据
 * @param[out]: desc: 创建描述符
 * @return
 *    TRUE：创建成功
 *    FALSE：创建失败
 */
BOOL mdlCtxCreateDesc(const UINT8 *inbuf, MDL_CREATE_DESC *desc)
{
	const UINT8 *ptr = NULL;
	char *pTmpStr = NULL;
	char *pTmpStr1 = NULL;
	char *pTmpStr2 = NULL;
	char **rtpArgs = NULL;
	int count = 0;
    char *pName = desc->name;
    int i = 0;
#ifdef __X86__
    char *pArgBuf = NULL; //指向函数参数buffer指针
    int  offset = 0; //偏移量
#endif
	/* 跳过操作命令的命令字 */
    ptr = &inbuf[2];

    /* 解析参数 */
    if (!((*(ptr++) == ':')
    	&& (0 != hex2int(&ptr,&desc->contextType))
    	&& (*(ptr++) == ':')
    	&& (0 != hex2str(&ptr, (UINT8 **)&pName, PATH_MAX))
    	&& (*(ptr++) == ':')
    	&& (0 != hex2int(&ptr,&desc->redirIn))
    	&& (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr, &desc->redirOut))
    	&& (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr,&desc->redirErr))
    	&& (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr,&desc->priority))
    	&& (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr,&desc->stackSize))
    	&& (*(ptr++) == ':' )
    	&& (0 != hex2int(&ptr,&desc->options))))
    {
        return (FALSE);
    }

    /* 根据类型解析参数 */
    switch (desc->contextType)
    {
    	case MDL_CRT_TASK:
#ifdef __MIPS__
            if (!((*(ptr++) == ':')
    	    	&& (0 != hex2int(&ptr,&(desc->crtSpecific.task.entry)))
    	    	&& (*(ptr++) == ':' )
    	    	&& (0 != hex2int(&ptr, &(desc->crtSpecific.task.numArgs)))))
    	    {
    	        return (FALSE);
    	    }

    	    /* 解析函数参数 */
    	    for (i = 0; i < desc->crtSpecific.task.numArgs; i++)
    	    {
    	    	if (!((*(ptr++) == ':') && hex2int(&ptr,&(desc->crtSpecific.task.arg[i]))))
    	    	{
    	    		 return (FALSE);
    	    	}
    	    }
#else
    		pArgBuf = (char*)malloc(sizeof(char)*STRING_BUFF_LEN);
    		UINT32 tmp = 0;
    		memset(pArgBuf,0,STRING_BUFF_LEN);

    	    if (!((*(ptr++) == ':')
    	    	&& (0 != hex2int(&ptr,&(desc->crtSpecific.task.entry)))
    	    	&& (*(ptr++) == ':' )
    	    	&& (0 != hex2int(&ptr, &(desc->crtSpecific.task.numArgs)))))
    	    {
    	        return (FALSE);
    	    }

    	    /* 解析函数参数 */
    	    for (i = 0; i < desc->crtSpecific.task.numArgs; i++)
    	    {

			    desc->crtSpecific.task.arg[i] = (int)(pArgBuf+offset);

			    if (*ptr == ':')
			    {
			      ptr++;
				}
				else if(*ptr == '\0')
				{
						  break;
				}

				while((*ptr != ':')&&(*ptr != '\0'))
				{
					*(pArgBuf+offset++) = *ptr++;
				}

			    *(pArgBuf+offset++) = '\0';//存入结束符号。
    	    }

    	    //转换参数
    	    for (i = 0; i < desc->crtSpecific.task.numArgs; i++)
    	    {
    	    	char *ptr = (char*)(desc->crtSpecific.task.arg[i]);
    	    	char *ptrSave = ptr;
                if(*ptr != '"')
                {
                	//跳过“0x”
                	if((*ptr == '0')&&((*(ptr+1) == 'x')||(*(ptr+1) == 'X')))
                	{
                        ptr = ptr+2;
                        ptrSave = ptr;
                	}
                	if(hex2int(&ptr,&tmp) == strlen(ptrSave)) 
                	{
                		desc->crtSpecific.task.arg[i] = tmp;
                	}
                }
                else
                {
                	//去掉双引号
                	desc->crtSpecific.task.arg[i] = (int)(ptr+1);
                	*(pArgBuf+strlen(ptr)-1) = '\0';
                }
    	    }
#endif
    		break;
    	case MDL_CRT_RTP:
    		/* 解析RTP参数 */
    	    if (!((*(ptr++) == ':')
    	    	&& (0 != hex2int(&ptr,&(desc->crtSpecific.rtp.options)))
    	    	&& (*(ptr++) == ':')
    	    	&& (0 != hex2int(&ptr,&(desc->crtSpecific.rtp.numArgv)))
    	    	&& (*(ptr++) == ':')))
    	    {
    	        return (FALSE);
    	    }

    	    /* 设置参数个数 */
    	    count = desc->crtSpecific.rtp.numArgv;

    	    /* 分配内存 */
    	    rtpArgs = (char **)mdlAllocateModuleMem(sizeof(char*), ((count+1) * sizeof(char*)));

    	    /* 设置参数地址 */
    	    desc->crtSpecific.rtp.argv = rtpArgs;

    	    /* 查找参数字符串 */
    	    pTmpStr1 = strchr((const char*)ptr, '<');
    	    pTmpStr2 = strchr((const char*)ptr, '>');
    	    if ((pTmpStr1 == NULL) || (pTmpStr2 == NULL))
    	    {
    	    	return (FALSE);
    	    }

    	    pTmpStr2[0] = EOS;
    	    pTmpStr1++;

    	    /* 解析参数 */
    	    for (i = 0; i < count; i++)
    	    {
    	    	rtpArgs[i] = pTmpStr1;
    	    	pTmpStr = strchr(pTmpStr1, ' ');
    	    	if(pTmpStr == NULL || (pTmpStr > pTmpStr2))
    	    	{
        	    	break;
    	    	}

    	    	pTmpStr[0] = EOS;
    	    	pTmpStr1 = ++pTmpStr;
    	    }

    	    /* 参数以空结束 */
    	    rtpArgs[count] = NULL;

    	    ptr = (const UINT8 *)(++pTmpStr2);

    	    /* 解析环境变量 */
    	    if (!((*(ptr++) == ':')
    	    	&& (0 != hex2int(&ptr,&(desc->crtSpecific.rtp.numEnvp)))
    	    	&& (*(ptr++) == ':')))
    	    {
    	        return (FALSE);
    	    }

    	    /* 设置环境变量个数 */
    	    count = desc->crtSpecific.rtp.numEnvp;

    	    /* 分配环境变量内存 */
    	    rtpArgs = (char **)mdlAllocateModuleMem(sizeof(char*), ((count+1) * sizeof(char*)));

    	    /* 分配内存 */
    	    desc->crtSpecific.rtp.envp = rtpArgs;

    	    /* 查找参数字符串 */
    	    pTmpStr1 = strchr((const char*)ptr, '<');
    	    pTmpStr2 = strchr((const char*)ptr, '>');
    	    if ((pTmpStr1 == NULL) || (pTmpStr2 == NULL))
    	    {
    	    	return (FALSE);
    	    }

    	    pTmpStr2[0] = EOS;
    	    pTmpStr1++;

    	    /* 解析环境变量 */
    	    for (i = 0; i < count; i++)
    	    {
    	    	rtpArgs[i] = pTmpStr1;
    	    	pTmpStr = strchr(pTmpStr1, ' ');
    	    	if (pTmpStr == NULL || (pTmpStr > pTmpStr2))
    	    	{
        	    	break;
    	    	}

    	    	pTmpStr[0] = EOS;
    	    	pTmpStr1 = ++pTmpStr;
    	    }

    	    /* 环境变量以空结束 */
    	    rtpArgs[count] = NULL;

    		break;
    	default:
    	    break;
    }

	return (TRUE);
}

/*
 * @brief
 *    创建任务执行函数或者创建任务执行RTP
 * @param[in]: pCtxCreate: 创建描述符
 * @return
 *    ERROR：创建任务执行函数失败
 *    tid：创建任务执行函数成功,返回任务ID
 */
INT32 mdlCreateTask(MDL_CREATE_DESC *pCtxCreate)
{
	UINT32 i = 0;
	UINT32 stackSize = 0;

	/* 参数个数检查 */
    if (pCtxCreate->crtSpecific.task.numArgs > MAX_ARGS)
    {
    	return ERROR;
    }

    /* 设置其余参数为0 */
    for (i = pCtxCreate->crtSpecific.task.numArgs; i < MAX_ARGS; i++)
    {
    	pCtxCreate->crtSpecific.task.arg[i] = 0;
    }

    /* 设置任务栈大小 */
    if ((stackSize = pCtxCreate->stackSize) == 0)
    {
    	stackSize = MDL_TASK_STACK_SIZE;
    }


    /* 如果是调试任务则调用taskCreate函数创建任务 */
    if ((pCtxCreate->options & VX_TASK_NOACTIVATE) != 0)
    {
		/* 创建任务执行函数 */
    	taInitTaskId = taskCreate(NULL,pCtxCreate->priority,
					pCtxCreate->options,stackSize,
					(FUNCPTR)pCtxCreate->crtSpecific.task.entry,
					pCtxCreate->crtSpecific.task.arg[0], pCtxCreate->crtSpecific.task.arg[1],
					pCtxCreate->crtSpecific.task.arg[2], pCtxCreate->crtSpecific.task.arg[3],
					pCtxCreate->crtSpecific.task.arg[4], pCtxCreate->crtSpecific.task.arg[5],
					pCtxCreate->crtSpecific.task.arg[6], pCtxCreate->crtSpecific.task.arg[7],
					pCtxCreate->crtSpecific.task.arg[8], pCtxCreate->crtSpecific.task.arg[9]);
    	return (taInitTaskId);
    }
    else
    {
		/* 创建任务执行函数 */
		return taskSpawn(NULL,pCtxCreate->priority,
					pCtxCreate->options,stackSize,
					(FUNCPTR)pCtxCreate->crtSpecific.task.entry,
					pCtxCreate->crtSpecific.task.arg[0], pCtxCreate->crtSpecific.task.arg[1],
					pCtxCreate->crtSpecific.task.arg[2], pCtxCreate->crtSpecific.task.arg[3],
					pCtxCreate->crtSpecific.task.arg[4], pCtxCreate->crtSpecific.task.arg[5],
					pCtxCreate->crtSpecific.task.arg[6], pCtxCreate->crtSpecific.task.arg[7],
					pCtxCreate->crtSpecific.task.arg[8], pCtxCreate->crtSpecific.task.arg[9]);
    }
}

/*
 * @brief
 *    创建任务执行RTP
 * @param[in]: pCtxCreate: 创建描述符
 * @return
 *    ERROR：创建任务执行RTP失败
 *    pid：创建任务执行RTP成功,返回RTP ID
 */
INT32 mdlCreateRtp(MDL_CREATE_DESC *pCtxCreate, UINT32 *textStart)
{
	RTP_ID rtpId = 0;
    UINT32 stackSize = 0;

    /* 设置任务栈大小 */
    if ((stackSize = pCtxCreate->stackSize) == 0)
    {
    	stackSize = MDL_TASK_STACK_SIZE;
    }

    /* 创建RTP */
    rtpId = rtpSpawn(pCtxCreate->name,
    		    (const char **)pCtxCreate->crtSpecific.rtp.argv,
    		    (const char **)pCtxCreate->crtSpecific.rtp.envp,
    			pCtxCreate->priority,stackSize,
    			pCtxCreate->crtSpecific.rtp.options,
    			pCtxCreate->options);
    if ((RTP_ID)ERROR == rtpId)
    {
    	return (ERROR);
    }

    /* 设置RTP初始化任务 */
    taInitTaskId = (((WIND_RTP *)rtpId)->initTaskId);

    /* 设置代码段起始地址 */
    *textStart = (UINT32)((WIND_RTP *)rtpId)->binaryInfo.textStart;

    return ((INT32)rtpId);
}
