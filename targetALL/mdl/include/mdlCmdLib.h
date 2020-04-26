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
 * @file  mdlCmdLib.h
 * @brief
 *       功能：
 *       <li>动态下载命令处理模块相关声明</li>
 */
#ifndef MDL_CMDLIB_H_
#define MDL_CMDLIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/************************头 文 件******************************/

/************************宏 定 义******************************/

/* 对齐字节数计算 */
#define MDL_ROUND_UP(x, align)	(((int) (x) + (align - 1)) & ~(align - 1))

/* 16字节对齐 */
#define	MDL_ALLOC_ALIGN_SIZE 16

/* 模块内存地址2^12对齐 */
//#define	MDL_ALLOC_ALIGN_ADDRESS 4096
#define	MDL_ALLOC_ALIGN_ADDRESS 65536

/* 动态加载模块支持的命令个数 */
#define MDL_MODULE_COMMAND_NUM  10

/* 模块信息最大长度 */
#define MDL_MODULE_MAX_INFO_LEN  350

/* 符号信息最大长度 */
#define MDL_SYMBOL_MAX_INFO_LEN  300

/************************类型定义*****************************/

/************************接口声明*****************************/

/*
 * @brief:
 *     安装动态加载消息处理函数
 * @return:
 *     无
 */
void mdlModuleCommandInit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MDL_CMDLIB_H_ */
