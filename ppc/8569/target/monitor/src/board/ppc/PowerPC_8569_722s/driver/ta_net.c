/****************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *
 *          Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * 修改记录：
 * 20090813    创建文件
 */

/**
 * @file     taTsi109eth.c
 * @brief
 *    <li>功能：实现Tsi109eth网络设备驱动。PHY采用DP83848IVV。 </li>
 * @date    20090813
 */

/**************************** 引用部分 *****************************************/
#include <taTypes.h>
#include "taErrorDefine.h"

/*************************** 前向声明部分 ***************************************/
extern int eth_initialize(unsigned char *mac);
extern int eth_init(void);
extern int eth_send(volatile void *packet, int length);
extern int eth_rx(char *packet_buff);
extern int eth_get_rx_status();
extern int eth_enable_rx_int(int flag);
extern int eth_clear_rx_event();
extern unsigned char NetRxPacketsPtr;

/**************************** 定义部分 ******************************************/


/**************************** 实现部分 *****************************************/
/**
 * @brief
 *    功能:
 *        打开网络设备。
 *
 *  实现内容：
 *        初始化网络设备。
 *
 * @param[in] mac:指针，mac地址值
 *
 * @return 如果成功返回SUCC，否则返回FAIL。
 */
T_BOOL taNetOpen(T_UBYTE minor, UINT8 * mac)
//int  fnTA_net_open(unsigned char* mac)
{
#if 1
	int ret = 0;
    ret = kbspRawNetOpen(0, mac);
    return TA_OK;
    if (0 == ret)
    {
        return TA_OK;
    }
    return TA_FAIL;
#else
    eth_initialize(mac);

    ret = eth_init();
    if(ret == 1)
        return TA_OK;

    return TA_FAIL;
#endif
}

/**
 * @brief
 *    功能:
 *        关闭网络设备。
 *
 *  实现内容：
 *        禁止网络设备相关功能。
 *
 * @param[in] 无
 *
 * @return 无。
 */
void fnTA_net_close()
{

	return ;
}

/**
 * @brief
 *    功能:
 *        读取数据包。
 *
 *  实现内容：
 *        从网络设备读取一个数据包。
 *
 * @param[out] buf:字符指针，存放数据包的缓冲。
 * @param[out] offset:指针，有些网卡会在以太网帧的前面添加部分状态字，该
 *        状态字不属于以太网帧，并且依赖于不同的网卡，offset就指明了这
 *        部分数据的长度，以通知协议栈丢弃该部分数据。本设备为0。
 * 
 * @return 返回包长度。
 */
T_WORD taNetGetpkt(T_UBYTE minor,T_CHAR* buf,T_WORD* offset)
//int  fnTA_net_getpkt(char* buf, int* offset)
{
	*offset = 0;
	int ret=0;

	ret = eth_rx(buf);
	if(ret == -1)
		return 0;

	return ret;
}

/**
 * @brief
 *    功能:
 *        发送数据包。
 *
 *  实现内容：
 *        向网络设备写一个数据包。
 *
 * @param[in] buf:字符指针，存放数据包的缓冲。
 * @param[in] cnt:数据包长度
 * 
 * @return 无。
 */
T_VOID taNetPutpkt(T_UBYTE minor,T_CHAR* buf,T_WORD cnt)
//void fnTA_net_putpkt(char* buf, int cnt)
{
	eth_send(buf, cnt);
}

/**
 * @brief
 *    功能:
 *        网卡中断服务程序。
 *
 *  实现内容：
 *        处理中断。
 *
 * @param[in] callback:指针，回调函数。
 * 
 * @return 无。
 */
void fnTA_net_isr(T_VOID(*callback)(T_VOID))
{
	eth_clear_rx_event();
	if(NULL != callback)
	{
		callback();
	}
}

/********************************************************************
 * 函数名:   fnTA_net_get_isr
 --------------------------------------------------------------------
 * 功能:      获取网络中断状态
 --------------------------------------------------------------------
 * 输入参数: 无
 --------------------------------------------------------------------
 * 输出参数: 无
 --------------------------------------------------------------------
 * 出入参数: 无
 --------------------------------------------------------------------
 * 返回值:   接收到中断:TRUE;没有接到中断:FALSE
 --------------------------------------------------------------------
 * 作用域:   全局
 ********************************************************************/
int  fnTA_net_get_isr(void)
{
    if(1 == eth_get_rx_status())
    {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief
 *    功能:
 *       获得网卡的状态。
 *
 *  实现内容：
 *        判断网卡状态。
 *
 * @param[in] 无。
 * 
 * @return 网卡中断返回1；否则返回0。
 */
int fnTA_net_get_status()
{
    return eth_get_rx_status();
}

/**
 * @brief
 *    功能:
 *        使能/禁止网卡接收中断。
 *
 *  实现内容：
 *        根据传输参数，使能或者禁止网卡接收中断。
 *
 * @param[in] flag:是否使能。
 * 
 * @return 无。
 */
void fnTA_net_enable_rx_int(int flag)
{
    eth_enable_rx_int(flag);

    return ;
}

/**
 * @brief
 *    功能:
 *        使能/禁止网卡发送中断。
 *
 *  实现内容：
 *        根据传输参数，使能或者禁止网卡发送中断。
 *
 * @param[in] flag:是否使能。
 * 
 * @return 无。
 */
void fnTA_net_enable_tx_int(int flag)
{

    return ;
}

/**
 * @brief
 *    功能:
 *       获得网卡的接收中断号。
 *
 *  实现内容：
 *        返回网卡的接收中断号。
 *
 * @param[in] 无。
 * 
 * @return 网卡的接收中断号。
 */
int  fnTA_net_get_rx_vector(void)
{
#if 1
    return 16;//网卡eTSEC3接收中断号
#else
	return (22);
#endif
}


int fnTA_net_space_attribute_set(void)
{
//    unsigned int attr = MM_DEFAULT_SYSTEM_IOADDR_ATTR ;
//    unsigned int ret = 0;
//
//    /* 设置MMU属性 */
//    ret = mslMemPageAttrbuteSet(mslMemCurrentPageContextGet(), NetRxPacketsPtr, NET_RX_BUF_SIZE, attr);
//    if(ret != 0)
//    {
//        return (TA_FAIL);
//    }

    return (TA_OK);
}

