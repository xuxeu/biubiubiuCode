/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  listen.h
* @brief
*       功能：
*       <li>监听线程</li>
*/




#ifndef    LISTEN1_INCLUDE
#define LISTEN1_INCLUDE
/************************头文件********************************/
#include "tsInclude.h"
#include "tcpDevice.h"


/************************宏定义********************************/
/************************类型定义******************************/
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

/************************接口声明******************************/
#endif
