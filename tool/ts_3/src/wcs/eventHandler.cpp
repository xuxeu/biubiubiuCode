/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  eventHandler.cpp
 * @Version        :  1.0.0
 * @Brief           :  处理华邦模拟器的回调事件
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:38:16
 * @History        : 
 *   
 */
 
/************************引用部分*****************************/

#include <list>
#include "eventHandler.h"
#include "log.h"
#include "txthread.h"
#include "time.h"
#include "wcs_interface.h"
#include "rsp_TcpServer.h"

using namespace std;

/*定义事件类型*/
#define     TARGET_MODE_SWITH                            1
#define     TARGET_LOG_NOTICE                            2

eventHander::eventHander()
{
    m_runFlag = false;
    char temp[MAX_PATH] = "\0";
    GetCurrentDirectoryA(sizeof(temp), temp);
    string logPath = string(temp) + string("\\log\\wcsLog\\");
    if(!CreateDirectoryA(logPath.data(), NULL))
    {   
        CreateDirectoryA((string(temp)+"\\log").data(), NULL);
        CreateDirectoryA(logPath.data(), NULL);
    }
    pWcsLogFile = _fsopen((logPath + string("wcs.log")).data(), "w", _SH_DENYWR);       //打开一个文件，以只读的方式共享
    filename = logPath + string("wcs.log");
}


eventHander::~eventHander()
{
    if(pWcsLogFile != NULL)
    {
        fclose(pWcsLogFile);
        pWcsLogFile = NULL;
    }
    list <void*>*pArgList = NULL;
    void *pArg = NULL;
    while(!m_eventArg.empty())
    {
        pArgList = m_eventArg.front();
        m_eventArg.pop_front();
        if(pArgList != NULL)
        {
            while(!pArgList->empty())
            {
                pArg = pArgList->front();
                pArgList->pop_front();
                if(pArg != NULL)
                {
                    delete pArg;
                    pArg = NULL;
                }
            }
            delete pArgList;
            pArgList = NULL;
        }
    }
}

void eventHander::run()
{
    m_runFlag = true;
    
    while(m_runFlag)
    {
        eventProc();
    }
    return;
}

void eventHander::stop()
{
    m_runFlag = false;
    return;
}

void eventHander::pushEvent(UINT32 eventType, list <void*> *pArgList)
{
    m_MtxEvent.lock();
    m_eventType.push_back(eventType);
    m_eventArg.push_back(pArgList);
    m_MtxEvent.unlock();
    m_MtxEventCondition.lock();
    m_eventCondition.wakeAll();
    m_MtxEventCondition.unlock();
    sysLoger(SLDEBUG, "eventHander::pushEvent: event:%d", eventType);
    return;
}

bool eventHander::popEvent(UINT32 * eventType, list < void * > * * eventArg)
{
    QMutexLocker locker(&m_MtxEvent);
    if(m_eventType.isEmpty())
    {
        *eventType = 0;
        *eventArg = NULL;
        return false;
    }
    *eventType = m_eventType.front();
    *eventArg = m_eventArg.front();
    m_eventType.pop_front();
    m_eventArg.pop_front();
    sysLoger(SLDEBUG, "eventHander::popEvent: event:%d", *eventType);
    return true;
}

void eventHander::wcsModeSwitch(UINT32 mode)
{
    sysLoger(SLDEBUG, "eventHander::wcsModeSwitch: target mode switch event triggered! New mode:%d", mode);

    list <void*> *pArgList = new list <void*>();
    UINT32 *pMode = new UINT32(mode);
    if((pArgList == NULL) || (pMode == NULL))
    {
        sysLoger(SLWARNING, "eventHander::wcsModeSwitch:alloc event argument list failure!");
        if(pArgList != NULL)
        {
            delete pArgList;
            pArgList = NULL;
        }
        if(pMode != NULL)
        {
            delete pMode;
            pMode = NULL;
        }
    }
    //*pMode = mode;
    pArgList->push_back((void*)pMode);
    pushEvent(TARGET_MODE_SWITH, pArgList);
    return;
}

void eventHander::wcsLogerNotice(UINT32 level, char * fmt, va_list argptr)
{
    sysLoger(SLDEBUG, "eventHander::wcsLogerNotice: target system logger event triggered! ");
    
    if(NULL == fmt)
    {
        sysLoger(SLWARNING, "eventHander::wcsLogerNotice:Argument fmt is bad pointer!");
        return;
    }
    if(level > wcsCurLogLevel)
    {
        return;
    }
    list <void*> *pArgList = new list <void*>();
    char *pMsgBuf = new char[4096];
    UINT32 *pLevel = new UINT32(level);
    if((pArgList == NULL) || (pMsgBuf == NULL) || (pLevel == NULL))
    {
        sysLoger(SLWARNING, "eventHander::wcsLogerNotice:alloc event argument list failure!");
        if(pArgList != NULL)
        {
            delete pArgList;
            pArgList = NULL;
        }
        if(pMsgBuf != NULL)
        {
            delete pMsgBuf;
            pMsgBuf = NULL;
        }
        if(pLevel != NULL)
        {
            delete pLevel;
            pLevel = NULL;
        }
        return;
    }
    vsPrintfToBuffer(false, pMsgBuf, 4096, level, fmt, argptr);
    pArgList->push_back((void*)pLevel);
    pArgList->push_back((void*)pMsgBuf);
    pushEvent(TARGET_LOG_NOTICE, pArgList);
    return;
}

void eventHander::vsPrintfToBuffer(bool bAddTime, char *pBuf, UINT32 bufsize, UINT32 level, char * fmt, va_list argptr)
{
    char time[11] = "\0";
    char date[11] = "\0";
    int ret = 0;

    if(NULL == pBuf || NULL == fmt)
    {
        sysLoger(SLWARNING, "eventHandler::vsPrintfToBuffer: Argument contained bad pointer!");
        return;
    }
    if(bAddTime)
    {
        _strtime_s(time, sizeof(time));
        _strdate_s(date, sizeof(date));
            
        //去掉fmt中的第一个\n符，将该\n符移动level前面
        if(strchr(fmt, '\n') == fmt)
        {
            fmt++;
            ret = sprintf_s(pBuf, bufsize, "\n%s %-10s ", date, time); 
        }
        else
        {
            ret = sprintf_s(pBuf, bufsize, "%s %-10s ", date, time);
        }
    }
    if(ret >=0 && ret < bufsize)
    {
        pBuf += ret;
        ret = vsnprintf_s(pBuf, bufsize -ret, bufsize -ret-1, fmt, argptr);
        pBuf[ret] = '\0';
    }

    return;
}

void eventHander::eventProc()
{
    UINT32 eventType = 0;
    list <void*> *pEventArg = NULL;
    bool ret = false;

    m_MtxEventCondition.lock();
    m_eventCondition.wait(&m_MtxEventCondition);
    m_MtxEventCondition.unlock();
    
    ret = popEvent(&eventType, &pEventArg);
    if(ret)
    {
        switch(eventType)
        {
            case TARGET_MODE_SWITH:
                handleTargetModeSwitch(pEventArg);
                break;
                
            case TARGET_LOG_NOTICE:
                handleTargetLogNotice(pEventArg);
                break;
                
            default:
                
                break;
        }
    }
    delete pEventArg;
    pEventArg = NULL;
    return;
}

void eventHander::handleTargetLogNotice(list < void * > * pArgList)
{
    if(NULL == pArgList)
    {
        sysLoger(SLWARNING, "eventHander::handleTargetLogNotice:Argument pArgList is bad pointer!");
        return;
    }
    if(pArgList->empty())
    {
        sysLoger(SLINFO, "eventHandler::handleTargetLogNotice:No argument in argList!");
        return;
    }
    char *pBuf = NULL;
    UINT32 *pLevel = (UINT32*)pArgList->front();
    pArgList->pop_front();
    pBuf = (char*)pArgList->front();
     pArgList->pop_front();
    if((pBuf == NULL) || (pLevel == NULL))
    {
        sysLoger(SLINFO, "eventHander::handleTargetLogNotice:log buffer is bad pointer!");
        if(pBuf != NULL)
        {
            delete pBuf;
        }
        if(pLevel != NULL )
        {
            delete pLevel;
        }
        return;
    }
    writeWcsLogToFile(*pLevel, pBuf);
    delete pBuf;
    delete pLevel;
    return;
}

void eventHander::handleTargetModeSwitch(list < void * > * pArgList)
{
    if(NULL == pArgList)
    {
        sysLoger(SLWARNING, "eventHander::handleTargetModeSwitch:Argument pArgList is bad pointer!");
        return;
    }
    if(pArgList->empty())
    {
        sysLoger(SLINFO, "eventHandler::handleTargetModeSwitch:No argument in argList!");
        return;
    }

    UINT32 *pMode = NULL;
    pMode = (UINT32*)pArgList->front();
    pArgList->pop_front();
    if(NULL == pMode)
    {
        sysLoger(SLINFO, "eventHandler::handleTargetModeSwitch:target switch mode is null!");
        return;
    }
    sysLoger(SLDEBUG, "eventHander::handleTargetModeSwitch: Switch to mode:%d", *pMode);
    
    if(WCS_DEBUG_MODE == *pMode)
    {
        sendData("S05", strlen("S05"));
    }
    delete pMode;
    pMode = NULL;
    return;
}


void eventHander::writeWcsLogToFile(UINT32 level, char * pBuf)
{
    if(pBuf == NULL)
    {
        sysLoger(SLWARNING, "eventHander::writeWcsLogToFile:Argument pBuf is bad pointer!");
        return;
    }
    if(pWcsLogFile == NULL)
    {
        sysLoger(SLWARNING, "eventHander::writeWcsLogToFile:Has not opened the log file!");
        return;
    }
    try
    {
        char sdate[11] = "\0";
        char stime[11] = "\0";
        _strtime_s(stime, sizeof(stime));
        _strdate_s(sdate, sizeof(sdate));

        fprintf_s(pWcsLogFile, "\n!ENTRY  WCS Simulator %d %d %s %s", level, 0, sdate, stime);
        fprintf_s(pWcsLogFile, "\n!MESSAGE %s", pBuf);
        fflush(pWcsLogFile);
    }
    catch(...)
    {
        sysLoger(SLWARNING, "eventHandler::writeWcsLogToFile: Catch an error when write log file!");
    }

    return;
}


void eventHander::sendData(char * buf, UINT32 len)
{
    if(NULL == buf)
    {
        sysLoger(SLWARNING, "eventHander::sendData:Argument buf is bad pointer!");
        return;
    }
    Packet *pack = PacketManager::getInstance()->alloc();
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "eventHander::sendData:Alloc packet for repling failure!");
        return;
    }
    char *pBuf = pack->getData();
    memcpy(pBuf, buf, len);
    pBuf += len;
    pack->setSize(len);
    pack->setDes_aid(RspTcpServer::getInstance()->m_IceSrvId);    
    
    TxThread::getInstance()->sendPack(pack);
    return;
}


void eventHander::wcsModeSwitchCallback(UINT32 mode)
{
    //RspTcpServer::getInstance()->getEventHandler()->wcsModeSwitch(mode);
    printf("\nwcsModeSwitchCallback function called! New Mode:%d", mode);
    if(mode == WCS_DEBUG_MODE)
    {
        RspTcpServer::getInstance()->getEventHandler()->sendData("S05", strlen("S05"));
    }
    return;
}

void eventHander::wcsLogerNoticeCallback(WCS_LOG_LEVEL level, char * fmt,...)
{
    char buf[1000]="\0";
    printf("\nwcsLogerNoticeCallback function called!");
    va_list ap;
    va_start(ap, fmt);
    vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
    printf("\n%s", buf);
    RspTcpServer::getInstance()->getEventHandler()->wcsLogerNotice(level, fmt, ap);
    va_end(ap);
    
    return;
}