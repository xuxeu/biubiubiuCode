/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: Packet.h,v $
  * Revision 1.1  2008/04/02 03:36:57  guojc
  * Bug 编号  	0003005
  * Bug 名称 	ICE调试失败
  *
  * 编码：唐兴培
  * 检视：郭建川
  *
  * Revision 1.2  2007/04/02 02:56:53  guojc
  * 提交最新的IS代码，支持脱离于IDE使用，支持配置文件解析
  *
  * Revision 1.1  2006/11/13 03:40:56  guojc
  * 提交ts3.1工程源码,在VC8下编译
  *
  * Revision 1.2  2006/07/17 09:30:01  tanjw
  * 1.增加arm,x86 rsp日志打开和关闭功能
  * 2.增加arm,x86 rsp内存读取优化算法打开和关闭功能
  *
  * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
  * no message
  *
  * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
  * TS代码整理
  *
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * 创建文件;定义文件的基本接口
  */

/**
* @file     packet.h
* @brief     
*     <li>功能： 缓冲包类</li>
* @author     郭建川
* @date     2006-02-20
* <p>部门：系统部 
*/

#ifndef _PACKET_H
#define _PACKET_H


/************************************引用部分******************************************/
#include <QThread>
#include <QMap>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <cassert>
#include "sysTypes.h"
/************************************声明部分******************************************/


/************************************定义部分******************************************/

/**
* @brief
*    RSP_Packet是本地缓存控制ICE命令包的类，每个对象都拥有一个大小和字符数组\n
* @author     郭建川
* @version 1.0
* @date     2006-02-20
* <p>部门：系统部 
*/
class RSP_Packet
{
public:
    RSP_Packet();
    ~RSP_Packet();

    /**
    * @brief 
    *     功能: 设置缓冲包数据的大小
    * @param[in] wSize 大小
    */
    T_VOID SetSize(T_WORD wSize)
    {
        
        m_size = wSize;
    }
    
    /**
    * @brief 
    *     功能: 得到缓冲包的大小
    * @return 大小
    */
    T_WORD GetSize()
    {
        return m_size;
    }

    /**
    * @brief 
    *     功能: 设置缓冲包目的地址，如果不设置，就用默认值发送到ICE
    * @param[in] wSize 大小
    */
    T_VOID SetDesAid(T_HWORD wDesAid)
    {
        
        m_desAid = wDesAid;
    }
    
    /**
    * @brief 
    *     功能: 得到缓冲包目的地址
    * @return 大小
    */
    T_HWORD GetDesAid()
    {
        return m_desAid;
    }

    /**
    * @brief 
    *     功能: 设置缓冲包目的地址二级代理，如果不设置，就用默认值发送到ICE的原子操作会话
    * @param[in] wSize 大小
    */
    T_VOID SetDesSaid(T_HWORD wDesSaid)
    {
        
        m_desSaid = wDesSaid;
    }
    
    /**
    * @brief 
    *     功能: 得到缓冲包目的地址二级代理ID
    * @return 大小
    */
    T_HWORD GetDesSaid()
    {
        return m_desSaid;
    }

    /**
    * @brief 
    *     功能: 设置缓冲包源地址二级代理，如果不设置，默认值为RSP解析服务
    * @param[in] wSize 大小
    */
    T_VOID SetSrcSaid(T_HWORD wSrcSaid)
    {
        
        m_srcSaid = wSrcSaid;
    }
    
    /**
    * @brief 
    *     功能: 得到缓冲包源地址二级代理
    * @return 大小
    */
    T_HWORD GetSrcSaid()
    {
        return m_srcSaid;
    }

    /**
    * @brief 
    *     功能: 得到缓冲包的数据缓冲指针
    * @return 指针
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
*    RSP_PacketManager负责管理缓冲区，从而避免了频繁地创建和释放对象\n
* @author     郭建川
* @version 1.0
* @date     2006-02-20
* <p>部门：系统部 
*/
class RSP_PacketManager {
private:
    T_MODULE RSP_PacketManager* m_pm;
    T_CONST T_MODULE T_WORD LOW_LEVEL = 216;    //最低水位标志
    T_CONST T_MODULE T_WORD HIGH_LEVEL = 512;    //最高位标志
    T_MODULE QList<RSP_Packet*> m_freeList;        //保存空闲包链表
    T_MODULE T_WORD m_waterLevel;                //当前水位值 
    T_MODULE QMutex m_mutex;                        //互斥对象
    QWaitCondition m_bufferNotFull;        //条件变量,用来判断是否达到高水位判断
public:

    /**
    * 功能:得到缓冲包管理的引用指针
    * @return 成功返回该指针，失败返回NULL
    */
    T_MODULE RSP_PacketManager* GetInstance() 
    {
        if (m_pm == NULL) 
        {    //多重检查一次
            QMutexLocker lock(&m_mutex);
            if (m_pm == NULL)
                m_pm = new RSP_PacketManager; 

        }

        return m_pm;
    }
    
    /**
    * 功能:申请一个缓冲包
    * @return 成功返回缓冲包，失败返回NULL
    */
    RSP_Packet* Alloc() ;
    
    /**
    * 功能:释放一个缓冲包
    * @param pack 一个Packet对象
    * @return 成功返回true，失败返回false
    */
    T_BOOL Free(RSP_Packet* tPack) ;
};

#endif