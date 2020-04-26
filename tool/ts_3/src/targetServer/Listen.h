/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit, including
*    copying it, modifying it, compiling it, and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice, this paragraph, and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* 
*
* @file     listen.h
* @brief     监听线程
* @author     彭宏
* @date     2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/


#ifndef    LISTEN1_INCLUDE
#define LISTEN1_INCLUDE

#include "tsInclude.h"
#include "tcpDevice.h"

class DeviceBase;
class ClientAgent;
/*Modified by tangxp for BUG No.0002430 on 2007.12.29 [begin]*/
/*  Modifed brief:用单独写的线程类替换QT线程类*/
class Listen :    public baseThread 
/*Modified by tangxp for BUG No.0002430 on 2007.12.29 [end]*/
{
private:
    DeviceBase* pDeviceBase;
    int port;                                    //端口号
//    QMap<int ,ClientAgent*> mapID2CA;            //ID号到ClientAgent的映射
    /* 功能：根据申请创建ClientAgent线程*/
    bool isRun;    //线程运行标志
public:
    Listen(int port);
    /*功能: 关闭监听线程*/
    void close();
    /**
    * 功能：线程运行函数
    */
    void run();

};
#endif
