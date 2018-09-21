/*
* 更改历史：
* 2008-03-18  唐兴培  北京科银技术有限公司
*                        创建该文件。
*/

/**
* @file  common.h
* @brief
*       功能：
*       <li>包含日志,数据包和公共函数头文件,并根据宏定义,确定是否使用QT线程</li>
*/


/************************头文件********************************/
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
/************************宏定义********************************/
/************************类型定义******************************/
/************************接口声明******************************/
#endif