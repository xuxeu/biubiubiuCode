/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
* @file     common.h
* @brief     
*     <li>功能： 包含日志,数据包和公共函数头文件,并根据宏定义,确定是否使用QT线程</li>
* @author     唐兴培
* @date     2008-3-18
* <p>部门：系统部 
*/

#ifndef _COMMONWITHTHREAD_H
#define _COMMONWITHTHREAD_H

#include "../src/TmBase/Util.h"
#include "../src/TmBase/Log.h"
#include "../src/TmBase/Packet.h"

//若要使用QT线程,需在项目的C\C++的Command Line中宏定义TS_USE_QT,默认定义为TS_NOT_USE_QT,即不使用QT线程

#ifdef TS_USE_QT
    #include <QThread>
    #include <QMutex>
    #include <QWaitCondition>

    #define baseThread QThread
    #define TMutex QMutex
    #define TMutexLocker QMutexLocker
    #define waitCondition QWaitCondition
#else
    #include "../src/TmBase/Thread.h"
    #include "../src/TmBase/mutex.h"

    #define baseThread TS_Thread
    #define TMutex TS_Mutex
    #define TMutexLocker TS_MutexLocker
    #define waitCondition TS_WaitCondition
#endif	

#endif