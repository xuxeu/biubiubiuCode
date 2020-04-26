/*******************************************************************************
 *                      ���������������޹�˾ ��Ȩ����
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: Packet.h,v $
  * Revision 1.1  2008/04/02 03:36:57  guojc
  * Bug ���  	0003005
  * Bug ���� 	ICE����ʧ��
  *
  * ���룺������
  * ���ӣ�������
  *
  * Revision 1.2  2007/04/02 02:56:53  guojc
  * �ύ���µ�IS���룬֧��������IDEʹ�ã�֧�������ļ�����
  *
  * Revision 1.1  2006/11/13 03:40:56  guojc
  * �ύts3.1����Դ��,��VC8�±���
  *
  * Revision 1.2  2006/07/17 09:30:01  tanjw
  * 1.����arm,x86 rsp��־�򿪺͹رչ���
  * 2.����arm,x86 rsp�ڴ��ȡ�Ż��㷨�򿪺͹رչ���
  *
  * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
  * no message
  *
  * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
  * TS��������
  *
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * �����ļ�;�����ļ��Ļ����ӿ�
  */

/**
* @file     packet.h
* @brief     
*     <li>���ܣ� �������</li>
* @author     ������
* @date     2006-02-20
* <p>���ţ�ϵͳ�� 
*/

#ifndef _PACKET_H
#define _PACKET_H


/************************************���ò���******************************************/
#include <QThread>
#include <QMap>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <cassert>
#include "sysTypes.h"
/************************************��������******************************************/


/************************************���岿��******************************************/

/**
* @brief
*    RSP_Packet�Ǳ��ػ������ICE��������࣬ÿ������ӵ��һ����С���ַ�����\n
* @author     ������
* @version 1.0
* @date     2006-02-20
* <p>���ţ�ϵͳ�� 
*/
class RSP_Packet
{
public:
    RSP_Packet();
    ~RSP_Packet();

    /**
    * @brief 
    *     ����: ���û�������ݵĴ�С
    * @param[in] wSize ��С
    */
    T_VOID SetSize(T_WORD wSize)
    {
        
        m_size = wSize;
    }
    
    /**
    * @brief 
    *     ����: �õ�������Ĵ�С
    * @return ��С
    */
    T_WORD GetSize()
    {
        return m_size;
    }

    /**
    * @brief 
    *     ����: ���û����Ŀ�ĵ�ַ����������ã�����Ĭ��ֵ���͵�ICE
    * @param[in] wSize ��С
    */
    T_VOID SetDesAid(T_HWORD wDesAid)
    {
        
        m_desAid = wDesAid;
    }
    
    /**
    * @brief 
    *     ����: �õ������Ŀ�ĵ�ַ
    * @return ��С
    */
    T_HWORD GetDesAid()
    {
        return m_desAid;
    }

    /**
    * @brief 
    *     ����: ���û����Ŀ�ĵ�ַ����������������ã�����Ĭ��ֵ���͵�ICE��ԭ�Ӳ����Ự
    * @param[in] wSize ��С
    */
    T_VOID SetDesSaid(T_HWORD wDesSaid)
    {
        
        m_desSaid = wDesSaid;
    }
    
    /**
    * @brief 
    *     ����: �õ������Ŀ�ĵ�ַ��������ID
    * @return ��С
    */
    T_HWORD GetDesSaid()
    {
        return m_desSaid;
    }

    /**
    * @brief 
    *     ����: ���û����Դ��ַ����������������ã�Ĭ��ֵΪRSP��������
    * @param[in] wSize ��С
    */
    T_VOID SetSrcSaid(T_HWORD wSrcSaid)
    {
        
        m_srcSaid = wSrcSaid;
    }
    
    /**
    * @brief 
    *     ����: �õ������Դ��ַ��������
    * @return ��С
    */
    T_HWORD GetSrcSaid()
    {
        return m_srcSaid;
    }

    /**
    * @brief 
    *     ����: �õ�����������ݻ���ָ��
    * @return ָ��
    */
    T_CHAR* GetData()
    {
        return m_data;
    }
private:
    T_MODULE T_CONST T_WORD PACKET_DATA_SIZE = 8192;
    T_WORD m_size;
    T_CHAR m_data[PACKET_DATA_SIZE];
    T_HWORD m_desAid;
    T_HWORD m_desSaid;
    T_HWORD m_srcSaid;
};

/**
* @brief
*    RSP_PacketManager��������������Ӷ�������Ƶ���ش������ͷŶ���\n
* @author     ������
* @version 1.0
* @date     2006-02-20
* <p>���ţ�ϵͳ�� 
*/
class RSP_PacketManager {
private:
    T_MODULE RSP_PacketManager* m_pm;
    T_CONST T_MODULE T_WORD LOW_LEVEL = 216;    //���ˮλ��־
    T_CONST T_MODULE T_WORD HIGH_LEVEL = 512;    //���λ��־
    T_MODULE QList<RSP_Packet*> m_freeList;        //������а�����
    T_MODULE T_WORD m_waterLevel;                //��ǰˮλֵ 
    T_MODULE QMutex m_mutex;                        //�������
    QWaitCondition m_bufferNotFull;        //��������,�����ж��Ƿ�ﵽ��ˮλ�ж�
public:

    /**
    * ����:�õ���������������ָ��
    * @return �ɹ����ظ�ָ�룬ʧ�ܷ���NULL
    */
    T_MODULE RSP_PacketManager* GetInstance() 
    {
        if (m_pm == NULL) 
        {    //���ؼ��һ��
            QMutexLocker lock(&m_mutex);
            if (m_pm == NULL)
                m_pm = new RSP_PacketManager; 

        }

        return m_pm;
    }
    
    /**
    * ����:����һ�������
    * @return �ɹ����ػ������ʧ�ܷ���NULL
    */
    RSP_Packet* Alloc() ;
    
    /**
    * ����:�ͷ�һ�������
    * @param pack һ��Packet����
    * @return �ɹ�����true��ʧ�ܷ���false
    */
    T_BOOL Free(RSP_Packet* tPack) ;
};

#endif