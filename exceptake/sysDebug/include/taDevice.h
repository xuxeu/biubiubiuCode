/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-23         彭元志，北京科银京成技术有限公司
 *                          创建该文件。
 */

/*
 *@file:taDevice.h
 *@brief:
 *             <li>设备读写接口表定义</li>
 */
#ifndef _TADEVICE_H
#define _TADEVICE_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************头文件********************************/
#include "taTypes.h"
#include "taErrorDefine.h"

/************************宏定义********************************/

/************************类型定义******************************/

/* TA支持的通信方式*/
typedef enum
{
    TA_COMM_UDP  = 0x1,   /* 适用于UDP等以包为数据发送接收的通信类型*/
    TA_COMM_UART = 0x2    /* 适用于串口等字符设备的流通信协议*/
}T_TA_COMMType;

/* 通信设备操作表结构体定义 */
typedef struct
{
     /* 设备类型,目前支持TA_COMM_UDP和TA_COMM_UART */
    T_TA_COMMType commType;

    /* 指向读数据的函数指针 */
    T_TA_ReturnCode (*read)(UINT8 *buf, UINT32 bufLen, UINT32 *receSize);

    /* 指向写数据的函数指针 */
    T_TA_ReturnCode (*write)(UINT8 *buf, UINT32 bufLen, UINT32 *writeSize);
 }T_TA_COMMIf;

/************************接口声明******************************/

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*_TADEVICE_H */
