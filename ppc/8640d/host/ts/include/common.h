/*
* ������ʷ��
* 2008-03-18  ������  ���������������޹�˾
*                        �������ļ���
*/

/**
* @file  common.h
* @brief
*       ���ܣ�
*       <li>������־,���ݰ��͹�������ͷ�ļ�,�����ݺ궨��,ȷ���Ƿ�ʹ��QT�߳�</li>
*/


/************************ͷ�ļ�********************************/
#ifndef _COMMONWITHTHREAD_H
#define _COMMONWITHTHREAD_H

#include "../src/TmBase/Util.h"
#include "../src/TmBase/Log.h"
#include "../src/TmBase/Packet.h"

//��Ҫʹ��QT�߳�,������Ŀ��C\C++��Command Line�к궨��TS_USE_QT,Ĭ�϶���ΪTS_NOT_USE_QT,����ʹ��QT�߳�

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
/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************�ӿ�����******************************/
#endif