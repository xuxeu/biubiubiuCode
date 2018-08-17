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
 * @file  rseInitLib.h
 * @brief
 *       功能：
 *       <li>RSE初始化相关声明</li>
 */
#ifndef RSE_INITLIB_H_
#define RSE_INITLIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/************************头 文 件******************************/

/************************宏 定 义******************************/

/************************类型定义*****************************/

/************************接口声明*****************************/

/*
 * @brief:
 *     安装RSE消息处理函数
 * @return:
 *     无
 */
void rseCommandInit(void);

/**
  * @brief
  *     核心任务信息初始化函数
  *@return
  *      None
  */
void rseKernelTaskLibInit(void);

/**
  * @brief
  *     CPU信息初始化函数
  *@return
  *      None
  */
void rseCpuInfoLibInit(void);

/**
  * @brief
  *     模块信息初始化函数
  *@return
  *      None
  */
void rseModuleLibInit(void);

/**
  * @brief
  *     RTP消息处理函数初始化
  *@return
  *      None
  */
void rseRtpLibInit(void);

/*
 * @brief
 *    rse库初始化
 * @return
 *    ERROR：初始化失败
 *    OK：初始化成功
 */
STATUS rseLibInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RSE_INITLIB_H_ */
