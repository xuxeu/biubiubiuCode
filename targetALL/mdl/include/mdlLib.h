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
 * @file  mdlLib.h
 * @brief
 *       功能：
 *       <li>动态下载模块相关类型定义和接口声明</li>
 */
#ifndef MDL_LIB_H_
#define MDL_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/************************头 文 件******************************/
#include <vxWorks.h>
#include "moduleLib.h"
#include "ta.h"

/************************宏 定 义******************************/

/* 模块的段个数 */
#define	MDL_MOD_NUM_SEGS  3

/* 最大的模块ID数 */
#define MDL_MAX_MODULE_ID_NUM  1024

#ifdef __X86__

/*存放函数参数buffer*/
#define STRING_BUFF_LEN  (1024)
#endif
/* 默认任务栈大小 */
#define MDL_TASK_STACK_SIZE 0x10000
/************************类型定义*****************************/

/* 段信息结构体  */
typedef struct
{
	UINT32 type;	 /* 段类型 */
	UINT32 address;  /* 段地址 */
	UINT32 size;	 /* 段大小 */
	UINT32 flags;	 /* 段标志 */
} MDL_SEG_INFO;

/* 模块信息 */
typedef struct
{
	char *name;	  /* 模块名 */
	UINT32 flags; /* 加载标志 */
    MDL_SEG_INFO segment[MDL_MOD_NUM_SEGS];	/* 段信息 */
} MDL_MOD_INFO;

/* 匹配信息 */
typedef struct
{
	MODULE_ID moduleId;
	SYMBOL **moduleSymbol;
}MDL_MATCH_INFO;

/* 创建类型 */
typedef enum
{
    MDL_CRT_TASK	    = 0, /* 任务 */
    MDL_CRT_RTP		    = 1, /* RTP */
    MDL_CRT_TYPE_NUM
} MDL_CREATE_TYPE;

/* 创建描述符 */
typedef struct
{
	MDL_CREATE_TYPE contextType; /* 创建类型 */
	char name[PATH_MAX];/* 名字 */
	UINT32	redirIn;	/* 重定向输入I/O */
	UINT32	redirOut;	/* 重定向输出I/O */
	UINT32	redirErr;	/* 重定向错误I/O */
	UINT32	baseAddr;	/* 虚拟基地址 */
	UINT32  priority;	/* 任务优先级  */
	UINT32  stackSize;	/* 任务栈大小  */
	UINT32  options;	/* 任务属性 */

	union
	{
		/* 任务创建信息 */
		struct
		{
			UINT32 entry;        /* 任务入口点 */
			UINT32 numArgs;	     /* 参数个数 */
			UINT32 arg[MAX_ARGS];/* 参数列表 */
		} task;

		/* RTP创建信息 */
		struct
		{
			UINT32	options;	/* RTP属性 */
			UINT32	numArgv;	/* 参数个数 */
			char**	argv;		/* RTP参数 */
			UINT32	numEnvp;	/* 环境变量个数 */
			char**  envp;		/* 环境变量 */
		 } rtp;
	}crtSpecific;
} MDL_CREATE_DESC;

/************************接口声明*****************************/

/* 存放模块符号ID */
extern SYMBOL **mdlModuleSymbol;

/*
 * @brief
 *    加载模块
 * @param[in]: pModInfo: 模块信息
 * @param[out]: pModuleId: 模块ID
 * @return
 *    ERROR：加载模块失败
 *    OK：加载模块成功
 */
STATUS mdlLoadModule(MDL_MOD_INFO *pModInfo, MODULE_ID *pModuleId);

/*
 * @brief
 *    卸载模块
 * @param[in]: moduleId: 模块ID
 * @return
 *    ERROR：卸载模块失败
 *    OK：卸载模块成功
 */
STATUS mdlUnloadModule(MODULE_ID moduleId);

/*
 * @brief
 *    释放模块内存
 * @param[in]: address: 模块内存地址
 * @return
 *    无
 */
void mdlFreeModuleMem(void *address);

/*
 * @brief
 *    分配模块内存
 * @param[in]: alignment: 分配内存对齐大小
 * @param[in]: size: 分配的内存大小
 * @return
 *    NULL：分配模块内存失败
 *    内存地址：分配模块内存成功
 */
void* mdlAllocateModuleMem(UINT32 alignment,UINT32 size);

/*
 * @brief
 *    检查模块是否存在
 * @param[in]: moduleId: 模块ID
 * @return
 *    NULL：模块不存在
 *    模块ID：模块存在
 */
MODULE_ID mdlFigureModuleId(MODULE_ID moduleId);

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
SYMBOL_ID mdlRegisterModuleSym(char *symbolName, char *symbolValue,
							SYM_TYPE symbolType, UINT16 groupId, UINT32 symRef);

/*
 * @brief
 *    获取核心符号个数
 * @return
 *    核心符号个数
 */
UINT32 mdlGetSymbolsNum(void);

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
BOOL mdlGetModuleSymId(char *name, int val, SYM_TYPE type, MDL_MATCH_INFO *info, UINT16 group, SYMBOL *pSymbol);

/*
 * @brief
 *    获取模块符号ID列表
 * @param[in]: moduleId: 模块ID
 * @return
 *    模块符号数
 */
UINT32 mdlGetModuleSymIdList(MODULE_ID moduleId);

/*
 * @brief
 *    获取模块信息
 * @param[in]: moduleId: 模块ID
 * @param[out]: pModuleInfo: 存储模块信息
 * @return
 *    ERROR：模块ID无效
 *    OK：获取模块信息成功
 */
STATUS mdlGetModuleInfo(MODULE_ID moduleId, MODULE_INFO *pModuleInfo);

/*
 * @brief
 *    加载模块的C++构造函数
 * @param[in]: moduleId: 模块ID
 * @return
 *    ERROR：加载失败
 *    OK：加载成功
 */
STATUS mdlCplusCallCtors(MODULE_ID moduleId);

/*
 * @brief
 *    解析创建MDL_CREATE_DESC结构体
 * @param[in]: inbuf: 输入数据
 * @param[out]: desc: 创建描述符
 * @return
 *    TRUE：创建成功
 *    FALSE：创建失败
 */
BOOL mdlCtxCreateDesc(const UINT8 *inbuf, MDL_CREATE_DESC *desc);

/*
 * @brief
 *    创建任务执行函数或者创建任务执行RTP
 * @param[in]: pCtxCreate: 创建描述符
 * @return
 *    ERROR：创建任务执行函数失败
 *    tid：创建任务执行函数成功,返回任务ID
 */
INT32 mdlCreateTask(MDL_CREATE_DESC *pTaskCreate);

/*
 * @brief
 *    创建任务执行RTP
 * @param[in]: pCtxCreate: 创建描述符
 * @return
 *    ERROR：创建任务执行RTP失败
 *    pid：创建任务执行RTP成功,返回RTP ID
 */
INT32 mdlCreateRtp(MDL_CREATE_DESC *pCtxCreate, UINT32 *textStart);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MDL_LIB_H_ */
