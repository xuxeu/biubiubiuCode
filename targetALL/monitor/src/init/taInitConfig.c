/***************************************************************************
 *				�����������ɼ������޹�˾ ��Ȩ����
 * 	 Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-06-27         ��Ԫ־�������������ɼ������޹�˾
 *                         �������ļ���
 */

/*
 * @file:taInitConfig.c
 * @brief:
 *             <li>TA�豸������س�ʼ��</li>
 */
 
/************************ͷ �� ��******************************/
#include "ta.h"
#include "config_ta.h"
#include "taUdpLib.h"

/************************�� �� ��********************************/

/************************���Ͷ���*******************************/

/************************�ⲿ����*******************************/

/************************ǰ������*******************************/

/************************ģ�����*******************************/

/* ͨ���豸�����ӿ� */
static T_TA_COMMIf taCOMMIfDevice;

/*����*/
#ifdef CONFIG_TA_CHANNEL_NET
    static T_TA_NET_CONFIG taNetConfig = CONFIG_TA_CHANNEL_NET;
#endif

/************************ȫ�ֱ���*******************************/

/************************����ʵ��*******************************/

/*
 * @brief:
 *      ��ʼ�������豸
 * @return:
 *      ��
 */
void taDebugDeviceInit(void)
{
    /* �����豸��ʼ�� */
    taUDPInit(&taNetConfig);
    
    /* ��ʼ��ͨ��Э�� */
    taCOMMIfDevice.read = taUdpRead;
    taCOMMIfDevice.write = taUdpWrite;

    /* ��ʼ��ͨ���豸 */
    taCOMMIfDevice.commType = TA_COMM_UDP;
    taCOMMInit(&taCOMMIfDevice);
      
    /* Э���ʼ�� */
    taTreatyUDPInit();
}

/*
 * @brief:
 *      ��ʾͨ����Ϣ
 * @return:
 *      ��
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
 *      ��ʾ������Ϣ
 * @return:
 *      ��
 */
void taDisplayStartedInfo(void)
{
#ifdef CONFIG_TA_DISP
    /*��ӡ������Ϣ*/
    TA_xprint("========================================================");
    TA_xprint("\n\r  Lambda Debug Tool Series                   ");
    TA_xprint("\n\r  Target Rom Agent for DeltaOS6.0            ");
    TA_xprint("\n\r  Version:%s","0.0.1");
    TA_xprint("\n\r                                             ");
    TA_xprint("\n\r  @Copyright reserved by Coretek Systems       ");
    TA_xprint("\n\r  http://www.coretek.com.cn                  ");
    TA_xprint("\n\r========================================================");

    /* ��ʾͨ����Ϣ */
    taDisplayChannelInfo();
#endif
}

char * versionNumber()
{

	return 0;

}

