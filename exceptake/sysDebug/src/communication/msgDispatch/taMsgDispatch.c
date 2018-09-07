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
 * @file:taMsgDispatch.c
 * @brief:
 *             <li>通信模块消息派发实现</li>
 */

/************************头 文 件******************************/
#include "taMsgDispatch.h"
#include "taPacketComm.h"

/************************宏 定 义*******************************/

/************************类型定义******************************/

/* 定义消息派发表结构 */
typedef struct
{
    TaMsgProc proc;  /* 消息处理函数 */
}TA_MSG_DISPATCH_TAB;

/************************外部声明******************************/

/************************前向声明******************************/

/************************模块变量******************************/

/* 轮循收包标志 */
static BOOL taDispatchFlag  = FALSE;

/* 消息包模块变量 */
static T_TA_Packet taPacket;

/* 消息派发表 */
static TA_MSG_DISPATCH_TAB taMessageDispatchTable[TA_MAX_AGENT_NUM];

/************************全局变量******************************/

/* 记录目标机ID，用于主动上报和向TSMAP查询使用 */
static UINT16 m_targetID = 0xFFFF;

/************************函数实现******************************/

/*
 * @brief:
 *  	启动循环接收消息
 * @return:
 *      无
 */
void taStartLoopDispatch(void)
{
    taDispatchFlag = TRUE;
}

/*
 * @brief:
 *      停止循环接收消息
 * @return:
 *      无
 */
void taStopLoopDispatch(void)
{
    taDispatchFlag = FALSE;
}

/*
 * @brief:
 *      安装通信通道消息处理函数
 * @param[in]:aid: 通道号
 * @param[in]:proc: 消息处理函数指针
 * @return:
 *     TA_OK: 安装成功
 *     TA_INVALID_ID: 错误的通道号,正确的通道ID号范围应该是0~MAX_AGENT_NUM
 */
T_TA_ReturnCode taInstallMsgProc(UINT32 aid, TaMsgProc proc)
{
    if (aid >= TA_MAX_AGENT_NUM)
    {
        return (TA_INVALID_ID);
    }

    /* 保存通道号和通道消息处理函数之间的映射关系 */
    taMessageDispatchTable[aid].proc = proc;

    return (TA_OK);
}




/*
 *@brief
 *     设置目标机ID
 *@param[in] targetID: 目标机ID
 *@return
 *    none
 * @implements  DM.6.96
 */
void taSetTargetId(UINT16 targetID)
{
    m_targetID = targetID;
}

/*
 *@brief
 *     获取目标机ID
 *@return
 *    目标机ID
 * @implements  DM.6.92
 */
UINT32 taGetTargetId(void)
{
    return m_targetID;
}





/**
 * @brief:
 *      消息接收派发函数，收到消息包后会根据接收通道ID调用对应安装的消息处理
 *      函数，对消息包进行处理
 * @return:
 *      TA_OK:收包成功
 *      TA_FAIL:收包失败
 */
T_TA_ReturnCode taMessageDispatch(void)
{
    UINT32 desSAid = 0;
    T_TA_ReturnCode ret = TA_OK;

    do
    {
        /* 调用taCOMMGetPkt()接收数据包 */
        ret = taCOMMGetPkt(&taPacket);
        if (TA_OK == ret)
        {
            /* 获取目的通道ID */
            desSAid = taGetPktHeadRecvChannel(&(taPacket.header));

            /* 如果目的通道ID在允许的范围内，并且已注册消息处理函数 */
            if ((desSAid < TA_MAX_AGENT_NUM) && (NULL != taMessageDispatchTable[desSAid].proc))
            {
                /* 记录目标机ID */
            	taSetTargetId(taGetPktHeadReceiver(&(taPacket.header)));
                /* 向目的通道派发数据包 */
                taMessageDispatchTable[desSAid].proc(&taPacket);
            }
        }
    } while (TRUE == taDispatchFlag);

    return (ret);
}
