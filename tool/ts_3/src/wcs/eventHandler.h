/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  eventHandler.h
 * @Version        :  1.0.0
 * @Brief           :  处理华邦模拟器的回调事件
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:37:45
 * @History        : 
 *   
 */

#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

#include "qthread.h"
#include "qmap.h"
#include "qstring.h"
#include "qmutex.h"
#include "qsemaphore.h"
#include "qwaitCondition.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysType.h"
#include "comfunc.h"
#include <string>
#include "windows.h"
#include "wcs_interface.h"

using namespace std;



class eventHander: public QThread
{
private:

    bool                  m_runFlag;
    QMutex             m_MtxEventCondition;
    QWaitCondition m_eventCondition;
    QMutex             m_MtxEvent;
    QList<UINT32> m_eventType;
    list <list <void*>*> m_eventArg;

    FILE *pWcsLogFile;
    string filename;
    UINT32       wcsCurLogLevel;
public:

    eventHander();
    ~eventHander();
        
    void run();
    void stop();

    void pushEvent(UINT32 eventType, list <void*> *eventArg);
    bool popEvent(UINT32 *eventType, list <void*>**eventArg);
    void eventProc();

    void wcsModeSwitch(UINT32 mode);
    void wcsLogerNotice(UINT32 level, char *fmt, va_list argptr);
    void vsPrintfToBuffer(bool bAddTime, char *pBuf, UINT32 bufsize, UINT32 level, char * fmt, va_list argptr);

    void handleTargetModeSwitch(list <void*> *pArgList);
    
    void handleTargetLogNotice(list <void*> *pArgList);

    void writeWcsLogToFile(UINT32 level, char *pBuf);

    void sendData(char *buf, UINT32 len);

    /*定义事件回调函数*/
    static void wcsModeSwitchCallback(UINT32 mode);
    static void wcsLogerNoticeCallback(WCS_LOG_LEVEL level, char * fmt,...);

};

#endif /*_EVENT_HANDLER_H_*/
