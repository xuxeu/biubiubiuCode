/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file:taProtocol.c
 * @brief:
 *             <li>消息通信抽象协议实现</li>
 */

/************************头 文 件*****************************/
#include "taProtocol.h"

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************外部声明*****************************/

/************************前向声明*****************************/

/************************模块变量*****************************/

/* 指向协议操作表链的指针 */
static T_TA_TREATY_OPS *taTreatyOpsList = NULL;

/************************全局变量*****************************/

/************************实   现********************************/
/*
 * @brief:
 *      在协议操作表链中指定协议的协议操作表
 * @param[in]: protocolType: 协议类型
 * @return:
 *      ops: 协议操作表的指针
 *      NULL: 未找到协议操作表
 */
static T_TA_TREATY_OPS* taTreatyLookup(T_TA_COMMType protocolType)
{
    T_TA_TREATY_OPS *ops = taTreatyOpsList;

    /* 在协议操作表链中遍历查找指定协议类型的协议操作表 */
    for (ops = taTreatyOpsList; NULL != ops; ops = ops->next)
    {
        if (protocolType == ops->type)
        {
            return (ops);
        }
    }

    return (NULL);
}

/**
 * @brief:
 *      向协议操作表链中添加一个协议操作表
 * @param[in]:optable: 指向协议操作表的指针
 * @return:
 *      TA_OK: 添加协议成功
 *      TA_INVALID_PARAM:无效的输入参数
 */
T_TA_ReturnCode taTreatyAdd(T_TA_TREATY_OPS *optable)
{
    /* 参数合法性检查 */
    if (( NULL == optable )
	    || ( NULL == optable->getpkt )
	    || ( NULL == optable->putpkt )
	    || (optable->offset > TA_PKT_DATA_SIZE))
    {
        return (TA_INVALID_PARAM);
    }

    /* 将协议操作表插入链表 */
    optable->next = taTreatyOpsList;

    taTreatyOpsList = optable;

    return (TA_OK);
}

/**
 * @brief:
 *      通过物理通道发送数据包
 * @param[in]: commIf: 调用者向TA通信协议栈提供的设备读写操作接口
 * @param[in]: packet: 需要发送的数据包内容
 * @return:
 *      TA_OK：接收数据成功
 *      TA_INVALID_PROTOCOL:协议还没有安装 *      
 *      TA_INVALID_PARAM: 输入的缓冲指针不合法
 *      TA_FAIL:发送数据失败
 */
T_TA_ReturnCode taPutpkt(T_TA_COMMIf *commIf, T_TA_Packet *packet)
{
    T_TA_TREATY_OPS *ops = NULL;
	T_TA_ReturnCode ret = TA_OK;
	
    /* 参数合法性检查未通过 */
    if ((NULL == packet) || (NULL == commIf))
    {
         return (TA_INVALID_PARAM);
    }

    /* 根据协议类型查找协议操作表 */
    ops = taTreatyLookup(commIf->commType);
    if (NULL == ops)
    {
        return (TA_INVALID_PROTOCOL);
    }

    /* 调用发包函数，发送消息包 */
    ret = ops->putpkt(commIf, packet);

    return (ret);
}

/**
 * @brief:
 *      通过物理通道接收数据包，收到任何有效数据包都立即返回
 * @param[in]:commIf:  通信设备操作表
 * @param[out]:packet: 数据包内容
 * @return:
 *      TA_OK：接收数据成功
 *      TA_INVALID_PROTOCOL:协议还没有安装
 *      TA_INVALID_PARAM: 输出缓冲指针不合法
 *      TA_FAIL:收包失败
 */
T_TA_ReturnCode taGetpkt(T_TA_COMMIf *commIf, T_TA_Packet *packet)
{
    T_TA_TREATY_OPS* ops = NULL;
	T_TA_ReturnCode ret = TA_OK;

    /* 参数合法性检查 */
    if ((NULL == packet) || (NULL == commIf))
    {
        return (TA_INVALID_PARAM);
    }

    /* 根据协议类型查找协议操作表 */
    ops = taTreatyLookup(commIf->commType);
    if (NULL == ops)
    {
        return (TA_INVALID_PROTOCOL);
    }

    /* 调用收包函数，接收消息包 */
    ret = ops->getpkt(commIf, packet);
    
    return (ret);
}
