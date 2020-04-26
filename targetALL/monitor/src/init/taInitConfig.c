/***************************************************************************
 *				北京科银京成技术有限公司 版权所有
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-06-27         彭元志，北京科银京成技术有限公司
 *                         创建该文件。
 */

/*
 * @file:taInitConfig.c
 * @brief:
 *             <li>TA设备配置相关初始化</li>
 */
 
/************************头 文 件******************************/
#include "ta.h"
#include "config_ta.h"
#include "taUdpLib.h"

/************************宏 定 义********************************/

/************************类型定义*******************************/

/************************外部声明*******************************/

/************************前向声明*******************************/

/************************模块变量*******************************/

/* 通信设备操作接口 */
static T_TA_COMMIf taCOMMIfDevice;

/*网络*/
#ifdef CONFIG_TA_CHANNEL_NET
    static T_TA_NET_CONFIG taNetConfig = CONFIG_TA_CHANNEL_NET;
#endif

/************************全局变量*******************************/

/************************函数实现*******************************/

/*
 * @brief:
 *      初始化调试设备
 * @return:
 *      无
 */
void taDebugDeviceInit(void)
{
    /* 网络设备初始化 */
    taUDPInit(&taNetConfig);
    
    /* 初始化通信协议 */
    taCOMMIfDevice.read = taUdpRead;
    taCOMMIfDevice.write = taUdpWrite;

    /* 初始化通信设备 */
    taCOMMIfDevice.commType = TA_COMM_UDP;
    taCOMMInit(&taCOMMIfDevice);
      
    /* 协议初始化 */
    taTreatyUDPInit();
}

/*
 * @brief:
 *      显示通道信息
 * @return:
 *      无
 */
void taDisplayChannelInfo(void)
{
#ifdef CONFIG_TA_CHANNEL_NET
  TA_xprint("\n\t CHANNEL UDP:    IP:%d.%d.%d.%d, PORT:%d\n",
      taNetConfig.ip[IP0],taNetConfig.ip[IP1],taNetConfig.ip[IP2],taNetConfig.ip[IP3],taNetConfig.port);
#endif

#ifdef CONFIG_TA_CHANNEL_UART
  if(COM1 == taUartConfig.minor)
  {
      TA_xprint("\n\t CHANNEL UART:    NAME:COM1, BAUD:%d\n",taUartConfig.baud);
  }
  else
  {
      TA_xprint("\n\t CHANNEL UART:    NAME:COM2, BAUD:%d\n",taUartConfig.baud);
  }
#endif
}
 
/*
 * @brief:
 *      显示启动信息
 * @return:
 *      无
 */
void taDisplayStartedInfo(void)
{
#ifdef CONFIG_TA_DISP
    /*打印启动信息*/
    TA_xprint("========================================================");
    TA_xprint("\n\r  Lambda Debug Tool Series                   ");
    TA_xprint("\n\r  Target Rom Agent for DeltaOS6.0            ");
    TA_xprint("\n\r  Version:%s","0.0.1");
    TA_xprint("\n\r                                             ");
    TA_xprint("\n\r  @Copyright reserved by Coretek Systems       ");
    TA_xprint("\n\r  http://www.coretek.com.cn                  ");
    TA_xprint("\n\r========================================================");

    /* 显示通道信息 */
    taDisplayChannelInfo();
#endif
}

char * versionNumber()
{

	return 0;

}

