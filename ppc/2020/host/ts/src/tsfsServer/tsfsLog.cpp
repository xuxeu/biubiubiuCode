/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsLog.cpp
* @brief  
*       功能：
*       <li> 实现系统类日志和消息包日志</li>
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
#include "tsfsMutex.h"
#include "tsfsLog.h"

/************************宏定义********************************/

/************************类型定义******************************/

/************************全局变量******************************/
const char SYSTEMLOGFILE[] = "IceServerSys"; 

const char LEVELINFO[5][10] = {"LDEBUG","LINFO", "LWARNING", "LERROR", "LFATAL"};

SystemLog* SystemLog::plog = NULL;

TMutex* SystemLog::pInitmtx = new TMutex();

/************************模块变量******************************/

/************************外部声明******************************/

/************************前向声明******************************/

/************************实现*********************************/
/**
 * @Funcname: SystemLog
 * @brief: 系统日志构造器,准备缓冲及文件
 * @Author:qingxiaohai
 * @History: Created this function on 2006年11月29日 17:34:59 
**/
SystemLog::SystemLog(LogLevel level, unsigned int line, unsigned short size)
{
    bufsize = (size>4096)?size:4096;
    maxLogFileLine = (line>0)?line:SYS_LOG_COUNT;   
    curLogFileLine = 0;
    curLogLevel = level; //默认输出INFO以上的级别
    fp = NULL;
    buf = (char*)malloc(bufsize);
    if(buf == NULL)
    {
        buf = (char*)malloc(bufsize);
    }

    memset(buf, 0, bufsize);

    //先创建系统日志目录
    char temp[MAX_PATH] = "\0";
    GetModuleFileName(NULL, temp, sizeof(temp));
    *(strrchr(temp, '\\')) = '\0';
    logPath = string(temp) + string("\\ts\\log\\systemLog\\");
    if(!CreateDirectory(logPath.data(), NULL))
    {
        if(!CreateDirectory((string(temp)+"\\ts\\log").data(), NULL))
        {
            CreateDirectory((string(temp)+"\\ts").data(), NULL);
            CreateDirectory((string(temp)+"\\ts\\log").data(), NULL);
        }
        CreateDirectory(logPath.data(), NULL);
    }

    return;
}

/**
 * @Funcname:openLogFile
 * @brief:  打开日志文件
 * @Author:  qingxiaohai
 * @History: 1.  Created this function on 2006年12月4日 14:32:29 
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

    //打开一个文件，以只读的方式共享
    fp = _fsopen((logPath + logfileName).data(), "w", _SH_DENYWR); 
    
	filename = logPath + logfileName;
}

void SystemLog::closeLogFile()
{
    //先关闭文件
    if(fp == NULL)
    {
        return;
    }

    fclose(fp);
    fp = NULL;

    //构造system+时间.log文件名
    char filename[MAX_PATH] = "\0";
    int idx2 = 0;
    idx2 = this->filename.find_last_of('.');
    
    strcat_s(filename, sizeof(filename), this->filename.substr(0, idx2).data());
    strcat_s(filename, sizeof(filename), "_bak.log");
    
	//删除旧文件
    DeleteFile(filename);

    //重命名文件
    rename(this->filename.data(), filename);
    return;
    
}

/**
 * @Funcname:  ~SystemLog
 * @brief : 系统日志析构器,释放缓冲和文件 
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月29日 17:35:42
**/
SystemLog::~SystemLog( )
{
    if(buf != NULL)
    {
        free(buf);
        buf = NULL;
    }

    if(fp != NULL)
    {
        closeLogFile();
    }

    if(plog != NULL)
	{
        delete plog;
	}
}

/**
 * @Funcname: getInstance
 * @brief : 获取系统日志的对象 
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月29日 17:22:51  
**/
SystemLog*SystemLog::getInstance(LogLevel level)
{
    if(NULL == plog)
    {
        TMutexLocker locker(pInitmtx);
        if(NULL == plog)
        {
            plog = new SystemLog(level);
        }
    }
    return plog;
}

/**
 * @Funcname:LogPrint
 * @brief:系统日志对外的接口函数
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年11月29日 17:22:51
**/
void SystemLog::LogPrint(LogLevel level, char *fmt,...)
{
    char time[11] = "\0";
    char date[11] = "\0";
    char tmpbuf[4096] = "\0";
    
    if((LBUTT == curLogLevel) || (level < curLogLevel))
    {
        return; //低于允许输出以上级别的就不再输出
    }
    _strtime_s(time, sizeof(time));
    _strdate_s(date, sizeof(date));
    
    TMutexLocker locker(&logmtx);

    va_list ap;
    va_start(ap, fmt); // use variable arg list
    
    //去掉fmt中的第一个\n符，将该\n符移动level前面
    if(strchr(fmt, '\n') == fmt)
    {
        fmt++;
        sprintf_s(buf, bufsize, "\n\n%s %-10s %-10s", date, time, LEVELINFO[level%5]); 
    }
    else
    {
        sprintf_s(buf, bufsize, "\n%s %-10s %-10s", date, time, LEVELINFO[level%5]);
    }
    vsnprintf_s(tmpbuf, sizeof(tmpbuf), sizeof(tmpbuf)-1, fmt, ap);
    strcat_s(buf, sizeof(tmpbuf), tmpbuf);
    buf[bufsize - 1] = '\0';
    va_end(ap);
    
    message_output();
    
    return;
}

/**
 * @Funcname:  message_output
 * @brief : 将日志信息写入日志文件
 * @Author:qingxiaohai
 * @History: 1.  Created this function on 2006年11月29日 17:25:08
**/
void SystemLog::message_output()
{
    if(fp)
    {
        if(curLogFileLine > maxLogFileLine)
        {
            closeLogFile();
            int idx = 0;
            idx = filename.find_last_of("\\");
            if(idx != string::npos)
            {
                filename = filename.substr(idx);     
            }
            openLogFile(filename);
            curLogFileLine = 0;
        }
        fprintf_s(fp, buf);
        fflush(fp);
        curLogFileLine++;
    }    
}

/**
 * @Funcname: truncatPathFromFilename
 * @brief: 将带有路径的文件名截取掉路径
 * @para1: filename, 文件名
 * @return: 返回不带路径的文件名, 以"\0"结束的字符串
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年12月1日 9:11:54
**/
char* truncatPathFromFilename(char *filename)
{
    UI  idx = 0;
    UI  namelen = strlen(filename);
    string name = string(filename);
    idx = name.find_last_of('\\');
    if(idx == string::npos)
    {
        return filename;
    }
    return &filename[idx+1];
}

/**
 * @Funcname: ReplaceChInstr
 * @brief:  在字符串中替换指定的字符
 * @para1: char *str, 源字符串
 * @para2: char oldch, 需要替换的字符
 * @para3: char newch 替换后的字符
 * @return: 0:表示成功,-1失败
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006年12月4日 14:24:05
**/
char*  ReplaceChInstr(char *str, char oldch, char newch)
{
    UI strLen = 0;
    UI idx = 0;

    if(str == NULL)
    {
        return NULL;
    }

    strLen = strlen(str);

    while(idx < strLen)
    {
        if(str[idx] == oldch)
        {
            str[idx] = newch;
        }
        idx++;
    }

    return str;
}


