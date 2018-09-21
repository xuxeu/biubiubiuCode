/****************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *
 *          Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***************************************************************************/

/*
 * �޸ļ�¼��
 * 20090813    �����ļ�
 */

/**
 * @file     taTsi109eth.c
 * @brief
 *    <li>���ܣ�ʵ��Tsi109eth�����豸������PHY����DP83848IVV�� </li>
 * @date    20090813
 */

/**************************** ���ò��� *****************************************/
#include <taTypes.h>
#include "taErrorDefine.h"

/*************************** ǰ���������� ***************************************/
extern int eth_initialize(unsigned char *mac);
extern int eth_init(void);
extern int eth_send(volatile void *packet, int length);
extern int eth_rx(char *packet_buff);
extern int eth_get_rx_status();
extern int eth_enable_rx_int(int flag);
extern int eth_clear_rx_event();
extern unsigned char NetRxPacketsPtr;

/**************************** ���岿�� ******************************************/


/**************************** ʵ�ֲ��� *****************************************/
/**
 * @brief
 *    ����:
 *        �������豸��
 *
 *  ʵ�����ݣ�
 *        ��ʼ�������豸��
 *
 * @param[in] mac:ָ�룬mac��ֵַ
 *
 * @return ����ɹ�����SUCC�����򷵻�FAIL��
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
 *    ����:
 *        �ر������豸��
 *
 *  ʵ�����ݣ�
 *        ��ֹ�����豸��ع��ܡ�
 *
 * @param[in] ��
 *
 * @return �ޡ�
 */
void fnTA_net_close()
{

	return ;
}

/**
 * @brief
 *    ����:
 *        ��ȡ���ݰ���
 *
 *  ʵ�����ݣ�
 *        �������豸��ȡһ�����ݰ���
 *
 * @param[out] buf:�ַ�ָ�룬������ݰ��Ļ��塣
 * @param[out] offset:ָ�룬��Щ����������̫��֡��ǰ����Ӳ���״̬�֣���
 *        ״̬�ֲ�������̫��֡�����������ڲ�ͬ��������offset��ָ������
 *        �������ݵĳ��ȣ���֪ͨЭ��ջ�����ò������ݡ����豸Ϊ0��
 * 
 * @return ���ذ����ȡ�
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
 *    ����:
 *        �������ݰ���
 *
 *  ʵ�����ݣ�
 *        �������豸дһ�����ݰ���
 *
 * @param[in] buf:�ַ�ָ�룬������ݰ��Ļ��塣
 * @param[in] cnt:���ݰ�����
 * 
 * @return �ޡ�
 */
T_VOID taNetPutpkt(T_UBYTE minor,T_CHAR* buf,T_WORD cnt)
//void fnTA_net_putpkt(char* buf, int cnt)
{
	eth_send(buf, cnt);
}

/**
 * @brief
 *    ����:
 *        �����жϷ������
 *
 *  ʵ�����ݣ�
 *        �����жϡ�
 *
 * @param[in] callback:ָ�룬�ص�������
 * 
 * @return �ޡ�
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
 * ������:   fnTA_net_get_isr
 --------------------------------------------------------------------
 * ����:      ��ȡ�����ж�״̬
 --------------------------------------------------------------------
 * �������: ��
 --------------------------------------------------------------------
 * �������: ��
 --------------------------------------------------------------------
 * �������: ��
 --------------------------------------------------------------------
 * ����ֵ:   ���յ��ж�:TRUE;û�нӵ��ж�:FALSE
 --------------------------------------------------------------------
 * ������:   ȫ��
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
 *    ����:
 *       ���������״̬��
 *
 *  ʵ�����ݣ�
 *        �ж�����״̬��
 *
 * @param[in] �ޡ�
 * 
 * @return �����жϷ���1�����򷵻�0��
 */
int fnTA_net_get_status()
{
    return eth_get_rx_status();
}

/**
 * @brief
 *    ����:
 *        ʹ��/��ֹ���������жϡ�
 *
 *  ʵ�����ݣ�
 *        ���ݴ��������ʹ�ܻ��߽�ֹ���������жϡ�
 *
 * @param[in] flag:�Ƿ�ʹ�ܡ�
 * 
 * @return �ޡ�
 */
void fnTA_net_enable_rx_int(int flag)
{
    eth_enable_rx_int(flag);

    return ;
}

/**
 * @brief
 *    ����:
 *        ʹ��/��ֹ���������жϡ�
 *
 *  ʵ�����ݣ�
 *        ���ݴ��������ʹ�ܻ��߽�ֹ���������жϡ�
 *
 * @param[in] flag:�Ƿ�ʹ�ܡ�
 * 
 * @return �ޡ�
 */
void fnTA_net_enable_tx_int(int flag)
{

    return ;
}

/**
 * @brief
 *    ����:
 *       ��������Ľ����жϺš�
 *
 *  ʵ�����ݣ�
 *        ���������Ľ����жϺš�
 *
 * @param[in] �ޡ�
 * 
 * @return �����Ľ����жϺš�
 */
int  fnTA_net_get_rx_vector(void)
{
#if 1
    return 16;//����eTSEC3�����жϺ�
#else
	return (22);
#endif
}


int fnTA_net_space_attribute_set(void)
{
//    unsigned int attr = MM_DEFAULT_SYSTEM_IOADDR_ATTR ;
//    unsigned int ret = 0;
//
//    /* ����MMU���� */
//    ret = mslMemPageAttrbuteSet(mslMemCurrentPageContextGet(), NetRxPacketsPtr, NET_RX_BUF_SIZE, attr);
//    if(ret != 0)
//    {
//        return (TA_FAIL);
//    }

    return (TA_OK);
}

