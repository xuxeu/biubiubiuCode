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
 * @file  rseCpuInfoLib.c
 * @brief
 *       功能：
 *       <li>获取CPU信息</li>
 */

/************************头 文 件******************************/
#include "rseConfig.h"
#include "rseInitLib.h"
#include "rseCmdLib.h"
#include "taDebugMode.h"
#include "vxCpuLib.h"

/************************宏 定 义******************************/

/* CPU信息属性个数 */
#define RSE_CPU_INFO_ATTR_NUM 4


/* CPU字符串 */
#if defined( __MIPS__)
#define RSE_CPU_STRING "loongson"
#elif defined(__PPC__)
#define RSE_CPU_STRING "PowerPC"
#else
#define RSE_CPU_STRING "pentium4"
#endif


/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/*
 * @brief
 *      提供CPU信息的消息处理
 * @param[in] packet: 接收到的数据包
 * @return
 *      None
 */
static T_TA_ReturnCode rseCpuInfoHandler(T_TA_Packet* packet);

/*
 * @brief
 *      获取系统CPU的信息。
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
static void rseGetCpuInfo(T_TA_Packet* packet,UINT32* outSize);

/************************模块变量*****************************/

/************************全局变量*****************************/

/************************函数实现*****************************/

/**
  * @brief
  *     CPU信息初始化函数
  *@return
  *      None
  */
void rseCpuInfoLibInit(void)
{
	rseRegisterModuleHandler(RSE_TYPE_CPU_MODULE, rseCpuInfoHandler);
}

/*
 * @brief
 *      提供CPU信息的消息处理
 * @param[in] packet: 接收到的数据包
 * @return
 *      TA_OK:操作成功
 */
static T_TA_ReturnCode rseCpuInfoHandler(T_TA_Packet* packet)
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
        	rseGetCpuInfo(packet,&outSize);
            break;
        default:
        	outSize = (UINT32)sprintf((INT8 *)outbuf, "E%d",TA_INVALID_OPERATE_ID);
            break;
    }

    /* 发送数据包 */
    taCOMMSendPacket(packet, outSize);

    return TA_OK;
}

/*
 * @brief
 *      获取系统CPU的信息。
 * @param[in] packet: 接收到的数据包
 * @param[out] outSize: 写入数据包的字节数
 * @return
 *      None
 */
static void rseGetCpuInfo(T_TA_Packet* packet,UINT32* outSize)
{
    UINT8* inbuf = NULL;
    UINT8* outbuf = NULL;
    UINT8* ptr = NULL;
    UINT32 len = 0;

    /* 从包中获取实际数据 */
    inbuf = taGetPktUsrData(packet);

    /* 设置输出缓冲outbuf为inbuf */
    ptr = outbuf = inbuf;

    /* 清空数据包 */
    memset(ptr,0, TA_PKT_DATA_SIZE);

    /* 设置回复信息 */
    len = sprintf((INT8 *)ptr,"%s","OK");
    ptr += (len+2);

    /* 写入类型ID */
    RSE_PUTPKT_SHORT(RSE_TYPE_CPU, ptr);

    /* 写入对象ID */
    RSE_PUTPKT_LONG(RSE_NULL_OBJECTID, ptr);

    /* 写入标志位 */
    *ptr = 0;
    ptr += 1;

    /* 写入要显示标签 */
    len = sprintf((INT8 *)ptr,"%s", RSE_CPU_STRING);
    ptr += (len+1);

    /* 写入操作个数 */
    *ptr = RSE_NULL_OPER;
    ptr += 1;

    /* 写入属性个数 */
    *ptr = RSE_CPU_INFO_ATTR_NUM;
    ptr += 1;

    /* 写入属性表 */
    RSE_PUTPKT_SHORT(RSE_ATTR_NAME, ptr);
    len = sprintf((INT8 *)ptr,"%s", RSE_CPU_STRING);
    ptr += (len+1);

    /* 写入当前系统的调试模式*/
    RSE_PUTPKT_SHORT(RSE_ATTR_DEBUG_MODE, ptr);
    if(DEBUGMODE_SYSTEM == taDebugModeGet())
    {
        len = sprintf((INT8 *)ptr,"%s", "system");
    }
    else
    {
    	len = sprintf((INT8 *)ptr,"%s", "task");
    }
    ptr += (len+1);

    /* 写入当前系统使能的CPU核数 */
    RSE_PUTPKT_SHORT(RSE_ATTR_ENABLED_CPU, ptr);
    len = sprintf((INT8 *)ptr,"%d", vxCpuConfiguredGet());
    ptr += (len+1);

    /* 写入当前CPU状态  */
    RSE_PUTPKT_SHORT(RSE_ATTR_STATE, ptr);
    len = sprintf((INT8 *)ptr,"%s", "running");
    ptr += (len+1);

    /* 设置输出缓冲数据大小 */
    *outSize = ptr - outbuf;
}
