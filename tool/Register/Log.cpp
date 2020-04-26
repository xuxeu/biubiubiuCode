/*
* 更改历史：
* 2006-11-29 qingxiaohai  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  log.cpp
* @brief
*       功能：
*       <li>实现系统类日志和消息包日志</li>
*/





/************************头文件********************************/
#include "stdafx.h"
#include "stdarg.h"
#include "stdlib.h"
#include "stdio.h"
#include "share.h"
#include "string.h"
#include "time.h"
#include "windows.h"
#include "mutex.h"
#include "log.h"
/************************宏定义********************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/
const char LEVELINFO[5][10] = {"LDEBUG","LINFO", "LWARNING", "LERROR", "LFATAL"};


SystemLog* SystemLog::plog = NULL;
TS_Mutex* SystemLog::pInitmtx = new TS_Mutex();

/**
 * @Funcname:  SystemLog
 * @brief        :  系统日志构造器,准备缓冲及文件
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006年11月29日 17:34:59
 *
**/
SystemLog::SystemLog(LogLevel level , unsigned int line, unsigned short size)
{
    char *pStr = new char[0];
    char *pIde = new char[0];
    bufsize = (size>4096)?size:4096;
    maxLogFileLine = (line>0)?line:65535;
    curLogFileLine = 0;
    curLogLevel = level;          //默认输出DEBUG以上的级别
    fp = NULL;
    buf = (char*)malloc(bufsize);

    if(buf == NULL)
    {
        return;
    }

    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: 增加缓冲的检查*/
    if(buf)
    {
        memset(buf, 0, bufsize);
    }

    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [end]*/

    //先创建系统日志目录
    char temp[MAX_PATH] = "\0";
    GetModuleFileName(NULL, temp, sizeof(temp));
    pStr = temp;
    while ((pStr = strstr(pStr, "\\ide\\"))!=NULL)
    {
        pIde = pStr;
        pStr++;
    }
    *pIde = '\0';
    logPath = string(temp) + string("\\pub\\ts\\log\\systemLog\\");

    if(!CreateDirectory(logPath.data(), NULL))
    {
        CreateDirectory((string(temp)+"\\pub\\ts").data(), NULL);
        CreateDirectory((string(temp)+"\\pub\\ts\\log").data(), NULL);
        CreateDirectory(logPath.data(), NULL);
    }
    
    return;
}

/**
 * @Funcname:  openLogFile
 * @brief        :  打开日志文件
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006年12月4日 14:32:29
 *
**/
void SystemLog::openLogFile(string logfileName)
{
    if(LBUTT == curLogLevel)
    {
        return;
    }

    if(fp != NULL)
    {
        closeLogFile();
    }

    fp = _fsopen((logPath + logfileName).data(), "w", _SH_DENYWR);       //打开一个文件，以只读的方式共享

    if (fp == NULL)
    {
        return;
    }

    filename = logPath + logfileName;
}

void SystemLog::closeLogFile()
{
    //先关闭文件
    if(fp == NULL)
    {
        return;
    }

    //关闭日志
    fclose(fp);
    fp = NULL;

    //构造system+时间.log文件名
    char filename[MAX_PATH] = "\0";
    int idx1 = 0, idx2 = 0;
    idx2 = (UI)(this->filename.find_last_of("."));

    strcpy(filename, this->filename.substr(idx1, idx2 - idx1).data());
    strcat(filename, "_bak.log");
    //删除旧文件
    DeleteFile(filename);
    //重命名文件
    rename(this->filename.data(), filename);
    return;

}
/**
 * @Funcname:  ~SystemLog
 * @brief        :  系统日志析构器,释放缓冲和文件
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006年11月29日 17:35:42
 *
**/
SystemLog::~SystemLog( )
{
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: 修改释放缓冲判断*/
    if(buf != NULL)
    {
        free(buf);
        buf = NULL;
    }

    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [end]*/
    if(fp != NULL)
    {
        closeLogFile();
    }

    plog = NULL;
    return;
}

/**
 * @Funcname:  getInstance
 * @brief        :  获取系统日志的对象
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006年11月29日 17:22:51
 *
**/
SystemLog*SystemLog::getInstance(LogLevel level)
{
    if(NULL == plog)
    {
        TS_MutexLocker locker(pInitmtx);

        if(NULL == plog)
        {
            plog = new SystemLog(level);
        }
    }

    return plog;
}
/**
 * @Funcname:  LogPrint
 * @brief        :  系统日志对外的接口函数
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006年11月29日 17:22:51
 *
**/
void SystemLog::LogPrint(LogLevel level, char *fmt,...)
{
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: 增加缓冲的检查*/
    if((level < curLogLevel) || (NULL == buf) || fp == NULL)
    {
        return;           //低于允许输出以上级别的就不再输出
    }

    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [end]*/

    char time[11] = "\0";
    char date[11] = "\0";
    char tmpbuf[4096] = "\0";

    _strtime(time);
    _strdate(date);

    TS_MutexLocker locker(&logmtx);

    va_list ap;
    va_start(ap, fmt); // use variable arg list

    //去掉fmt中的第一个\n符，将该\n符移动level前面
    if(strchr(fmt, '\n') == fmt)
    {
        fmt++;
        sprintf(buf, "\n\n%s %-10s %-10s", date, time, LEVELINFO[level%5]);
    }

    else
    {
        sprintf(buf, "\n%s %-10s %-10s", date, time, LEVELINFO[level%5]);
    }

    _vsnprintf(tmpbuf, sizeof(tmpbuf)-1, fmt, ap);
    strcat(buf, tmpbuf);
    buf[bufsize - 1] = '\0';
    va_end(ap);

    message_output();

    return;
}

/**
 * @Funcname:  message_output
 * @brief        :  将日志信息写入日志文件
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006年11月29日 17:25:08
 *
**/
void SystemLog::message_output()
{
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: 增加缓冲的检查*/
    if(NULL == buf)
    {
        return;
    }

    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [end]*/
    if(fp)
    {
        if(curLogFileLine > maxLogFileLine)
        {
            closeLogFile();
            int idx = 0;
            idx = (UI)(filename.find_last_of("\\"));

            if(idx != string::npos)
            {
                filename = filename.substr(idx);
            }

            openLogFile(filename);
            curLogFileLine = 0;
        }

		fprintf(fp, "%s", buf);
        fflush(fp);
        curLogFileLine++;
    }

    return;
}

