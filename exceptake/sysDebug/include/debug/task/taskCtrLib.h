/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-08-28         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 * @file: taskCtrLib.h
 * @brief:
 *             <li>会话管理</li>
 */
#ifndef _TASK_CTRL_LIB_H_
#define _TASK_CTRL_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头 文 件******************************/
#include "taTypes.h"
#include "taskSllLib.h"
#include "sdaMain.h"
#include "taErrorDefine.h"

/************************类型定义*****************************/

/************************宏 定 义******************************/

/************************接口声明*****************************/

/*
 * @brief:
 *      释放任务节点内存
 * @param[in]: pTaskNode: 任务节点
 * @param[in]: arg: 函数参数
 * @return:
 *     FALSE: 继续操作链表
 */
BOOL taTaskNodeFree(SLL_NODE *pTaskNode, INT32 arg);

/*
 * @brief:
 *      查找任务节点
 * @param[in]: tid: 任务ID
 * @param[out]: pSid: 会话ID
 * @return:
 *     NULL:未找到对应的任务节点
 *     T_TASK_NODE:任务节点
 */
SLL_NODE* taTaskNodeFind(INT32 tid, UINT32 *pSid);

/*
 * @brief:
 *     任务是否需要绑定，如果创建的新任务为绑定调试的实时进程创建或者绑定的调试任务创建，自动绑定该任务到调试会话中
 * @param[in]: tid: 任务ID
 * @param[out]: pSid: 会话ID
 * @return:
 *     FALSE: 无需绑定任务到调试会话
 *     TRUE：需绑定任务到调试会话
 */
BOOL taTaskIsAttach(int tid, UINT32 *pSid);

/*
 * @brief:
 *      获取调试会话中绑定的任务ID列表
 * @param[in]: sid: 会话ID
 * @param[out]: idList: 任务ID列表
 * @param[in]: maxTasks: 保存的最大任务个数
 * @return:
 *     idx:任务个数
 */
INT32 taGetTaskIdList(UINT32 sid, INT32 idList[], INT32 maxTasks);

/*
 * @brief:
 *      绑定单个任务
 * @param[in]: sid: 会话ID
 * @param[in]: tid: 任务ID
 * @return:
 *     TA_OK:绑定任务成功
 *     TA_INVALID_TASK_ID:无效任务ID
 *     TA_TASK_ALREADY_ATTACH:任务已经绑定
 *     TA_ALLOC_MEM_FAIL:分配内存失败
 */
T_TA_ReturnCode taAttachTask(UINT32 sid, INT32 tid);

/*
 * @brief:
 *      解绑任务
 * @param[in]: tid: 任务ID
 * @return:
 *     TA_OK:解绑任务成功
 *     TA_TASK_NO_ATTACH:任务未绑定
 */
T_TA_ReturnCode taDetachTask(INT32 tid);

/*
 * @brief:
 *      调试绑定任务
 * @param[in]: ctxType: 上下文类型
 * @param[in]: taskId: 任务ID
 * @param[out]: pSid: 会话ID
 * @return:
 *     TA_OK: 绑定任务成功
 *     TA_INVALID_TASK_ID:无效任务ID
 *     TA_TASK_ALREADY_ATTACH:任务已经绑定
 *     TA_ALLOC_MEM_FAIL:分配内存失败
 *     TA_ALLOCATE_SESSION_FAIL：分配调试会话失败
 */
UINT32 taTaskAttach(TA_CTX_TYPE ctxType, UINT32 taskId, UINT32 *pSid);

/*
 * @brief:
 *      调试解绑任务
 * @param[in]: sid: 会话ID
 * @return:
 *     TA_OK: 解绑成功
 */
UINT32 taTaskDetach(UINT32 sid);

/*
 * @brief:
 *      根据恢复任务节点任务的运行
 * @param[in]: pTaskNode: 任务节点
 * @param[in]: arg: 函数参数
 * @return:
 *     FALSE: 表示恢复链表所有任务运行
 */
BOOL taTaskNodeResume(SLL_NODE *pTaskNode, INT32 arg);

/*
 * @brief:
 *      挂起任务节点的任务
 * @param[in]: pTaskNode: 任务节点
 * @param[in]: arg: 函数参数
 * @return:
 *     FALSE: 表示挂起链表所有任务运行
 */
BOOL taTaskNodeSuspend(SLL_NODE *pTaskNode, INT32 arg);

/*
 * @brief:
 *      运行调试会话任务链表中的所有任务
 * @param[in]: sid: 会话ID
 * @return:
 *     OK:运行所有任务成功
 */
STATUS taTaskAllResume(INT32 sid);

/*
 * @brief:
 *      暂停调试会话任务链表中的所有任务
 * @param[in]: sid: 会话ID
 * @return:
 *     OK:挂起所有任务成功
 */
STATUS taTaskAllSuspend(INT32 sid);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* _TASK_CTRL_LIB_H_ */
