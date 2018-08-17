/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-07-08         彭元志，北京科银京成技术有限公司
 *                           创建该文件。
 */

/**
 * @file  rseModuleLib.c
 * @brief
 *       功能：
 *       <li>内核模块信息查询</li>
 */

/************************头 文 件******************************/
#include "rseConfig.h"
#include "moduleLib.h"
#include "rseCmdLib.h"
#include "rseInitLib.h"

/************************宏 定 义******************************/

/* 模块属性个数 */
#define RSE_MODULE_INFO_ATTR_NUM 7

/* 最大模块数 */
#define RSE_MAX_MODULE_NUM 1000

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/*
 * @brief
 *      提供模块信息的消息处理
 * @param[in] packet: 接收到的数据包
 * @return
 *      None
 */
static T_TA_ReturnCode rseModuleHandler(T_TA_Packet* packet);

/*
 * @brief
 *      获取模块信息
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
static void rseGetAllModuleInfo(T_TA_Packet* packet, UINT32* outSize);

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/**
  * @brief
  *     模块信息初始化函数
  *@return
  *      None
  */
void rseModuleLibInit(void)
{
	rseRegisterModuleHandler(RSE_TYPE_IMAGE_MODULE, rseModuleHandler);
}

/*
 * @brief
 *      提供模块信息的消息处理
 * @param[in] packet: 接收到的数据包
 * @return
 *      TA_OK:操作成功
 */
static T_TA_ReturnCode rseModuleHandler(T_TA_Packet* packet)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT32 outSize = 0;
    UINT8 operateId = 0;

    /* 从包中获取实际数据 */
    inbuf = taGetPktUsrData(packet);

    /* 设置输出缓冲outbuf为inbuf */
    outbuf = inbuf;

    /* 获取操作ID */
    operateId = inbuf[2];
    switch (operateId)
    {
        case RSE_OPR_QUERY:
        	rseGetAllModuleInfo(packet, &outSize);
            break;
        default:
        	outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d", TA_INVALID_OPERATE_ID);
            break;
    }

    /* 发送数据包 */
    taCOMMSendPacket(packet, outSize);

    return TA_OK;
}

/*
 * @brief
 *      获取模块信息
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
static void rseGetAllModuleInfo(T_TA_Packet* packet, UINT32* outSize)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT8* ptr = NULL;
    UINT32 len = 0;
	MODULE_INFO moduleInfo;
	MODULE_ID moduleId;
	static UINT32 count = 0;
	static UINT32 idx = 0;
	static MODULE_ID moduleIdList[RSE_MAX_MODULE_NUM] = {0};

    /* 从包中获取实际数据 */
	inbuf = taGetPktUsrData(packet);

    /* 设置输出缓冲outbuf为inbuf */
	ptr = outbuf = inbuf;

    /* 获取模块ID列表 */
    if(count == 0)
    {
    	count = moduleIdListGet(moduleIdList, RSE_MAX_MODULE_NUM);
    }

    /* 清空数据包 */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    while (idx < count)
    {
    	/* 先检查剩余包长是否可以存储模块信息 */
    	RSE_CHECK_OVER_PKTSIZE(ptr,outbuf, RSE_MODULE_MAX_INFO_LEN);

        /* 设置回复结果 */
        len = sprintf((INT8 *)ptr,"%s","OK");
        ptr += (len+2);

        /* 取得模块ID */
    	moduleId = moduleIdList[idx++];

		/* 写入类型ID */
		RSE_PUTPKT_SHORT(RSE_TYPE_IMAGE_MODULE, ptr);

		/* 获取模块信息 */
		moduleInfoGet(moduleId, &moduleInfo);

		/* 写入对象ID */
		RSE_PUTPKT_LONG((ulong_t)moduleId, ptr);

		/* 写入标志位 */
		*ptr = 0;
		ptr += 1;

		/* 写入显示标签 */
		len = sprintf((INT8 *)ptr,"%s", moduleId->name);
		ptr += (len+1);

		/* 写入操作个数 */
		*ptr = RSE_NULL_OPER;
		ptr += 1;

		/* 写入属性个数 */
		*ptr = RSE_MODULE_INFO_ATTR_NUM;
		ptr += 1;

		/* 写入属性表,依次为模块ID、模块名、模块大小、模块起始地址、模块状态、模块TOSID、模块类型、模块路径 */
		RSE_PUTPKT_SHORT(RSE_ATTR_ID, ptr);
		len = sprintf((INT8 *)ptr,"%x", moduleId->group);
		ptr += (len+1);

		/* 写入模块名属性 */
		RSE_PUTPKT_SHORT(RSE_ATTR_NAME,ptr);
		len = sprintf((INT8 *)ptr,"%s", moduleId->name);
		ptr += (len+1);

		/* 写入模块大小属性 */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_SIZE, ptr);
	#ifdef CONFIG_TA_LP64
		len = sprintf((INT8 *)ptr,"%lx", (moduleInfo.segInfo.textSize + moduleInfo.segInfo.dataSize + moduleInfo.segInfo.bssSize));
	#else
		len = sprintf((INT8 *)ptr,"%x", (moduleInfo.segInfo.textSize + moduleInfo.segInfo.dataSize + moduleInfo.segInfo.bssSize));
	#endif
		ptr += (len+1);

		/* 写入模块起始地址属性 */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_START_ADDR, ptr);
	#ifdef CONFIG_TA_LP64
		len = sprintf((INT8 *)ptr,"%lx", moduleInfo.segInfo.textAddr);
	#else
		len = sprintf((INT8 *)ptr,"%x", moduleInfo.segInfo.textAddr);
	#endif
		ptr += (len+1);

		/* 写入模块状态属性 */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_STATE, ptr);
		len = sprintf((INT8 *)ptr,"%s", "module loaded on target - symbols loaded");
		ptr += (len+1);

		/* 写入模块TOSID属性 */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_TOSID, ptr);
	#ifdef CONFIG_TA_LP64
		len = sprintf((INT8 *)ptr,"%lx", (ulong_t)moduleId);
	#else
		len = sprintf((INT8 *)ptr,"%x", (ulong_t)moduleId);
	#endif
		ptr += (len+1);

		/* 写入模块类型属性 */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_TYPE, ptr);
		len = sprintf((INT8 *)ptr,"%s", "Module");
		ptr += (len+1);

		/* 写入模块路径属性 */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_PATH, ptr);
		len = sprintf((INT8 *)ptr,"%s", moduleId->path);
		ptr += (len+1);

		/* 写入模块代码段起始地址属性 */
		RSE_PUTPKT_SHORT(RSE_ATTR_MODULE_TEXT_ADDR, ptr);
	#ifdef CONFIG_TA_LP64
		len = sprintf((INT8 *)ptr,"%lx", moduleInfo.segInfo.textAddr);
	#else
		len = sprintf((INT8 *)ptr,"%x", moduleInfo.segInfo.textAddr);
	#endif
		ptr += (len+1);
    }

    /* 设置输出缓冲为 "$",表示任务信息发送完毕 */
    if(idx == count)
    {
    	*ptr = RSE_END_STRING;
    	ptr += 1;
    	idx = count = 0;
    }

    /* 设置输出缓冲数据大小 */
    *outSize = ptr - outbuf;
}
