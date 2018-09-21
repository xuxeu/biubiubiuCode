/*
* 更改历史：
* 2004-12-6  唐兴培  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 卿孝海  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  tsfsLog.h
* @brief  
*       功能：
*       <li>日志类的声明</li>
*/
/************************头文件********************************/
#ifndef _LOG_H
#define _LOG_H

#include <map>
#include <string>
#include "tsfsMutex.h"
#include "windows.h"
#include "..\tsApi\tclientapi.h"
using namespace std;

/************************宏定义********************************/
#define UC  unsigned char
#define UL  unsigned long
#define US  unsigned short
#define UI  unsigned int

#define MAX_NAME_SIZE   100

#define PACK_LOG_COUNT   3000        //一个日志文件里记录的消息日志条数
#define SYS_LOG_COUNT    3000        //一个日志文件里允许记录的日志条数
//调试时打印系统日志

#define _IS_DEBUG
#ifdef _IS_DEBUG
#define IS_PACK_LOG
#define IS_CURSYS_LOG_LEVEL        LDEBUG
#else
#define IS_CURSYS_LOG_LEVEL        LERROR
#endif

//定义系统日志接口的简化形式
#define sysLoger  SystemLog::getInstance()->LogPrint

//定义去掉路径的文件名
#define __MYFILE__ truncatPathFromFilename(__FILE__)


/************************接口声明******************************/
/*
该函数的功能是从给定的文件名filename 中
去掉路径后，返回不带路径的文件名指针
*/
char* truncatPathFromFilename(char *filename);

/*
该函数的作用是将指定的字符串str中
中的oldch 字符替换成 newch 字符。
*/
char*  ReplaceChInstr(char *str, char oldch, char newch);

/************************类型定义******************************/
class SystemLog
{

public:
    SystemLog(LogLevel level = LDEBUG, unsigned int line = SYS_LOG_COUNT, unsigned short size = 4096);
    ~SystemLog();
    
    /*系统日志的对外接口函数，用于与printf很相像，
    不过它会自动在每一条日志记录前加回车换行符。
    */
    void LogPrint(LogLevel level, char *fmt, ...);

    static SystemLog *getInstance(LogLevel level = LDEBUG);
    
    void setLogLevel(LogLevel level)
    {
        curLogLevel = level;
    }
    LogLevel getLogLevel()
    {
        return curLogLevel;
    }
    void openLogFile(string logfileName = string("IceServerSys"));
    
protected:
    
    void message_output();
    //关闭并重命名日志文件
    void closeLogFile();
private:
    char *buf;
    string filename;              //记录当前文件名
    US bufsize;
    FILE *fp;
    UI  maxLogFileLine;
    UI  curLogFileLine;
    string                     logPath;                            //保存日志文件路径
    LogLevel                curLogLevel;                //当前允许输出的日志级别
    static SystemLog *plog;
    static TMutex *pInitmtx;
    TMutex logmtx;                          //日志文件锁
};
#endif  /*_LOG_H*/
