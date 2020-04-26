/*
* 更改历史：
* 2006-11-29 qingxiaohai  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  log.h
* @brief
*       功能：
*       <li>日志类的声明</li>
*/



#ifndef _LOG_H
#define _LOG_H


/************************头文件********************************/
#include <map>
#include <string>
#include "mutex.h"
#include "windows.h"

using namespace std;

/************************宏定义********************************/
const char SYSTEMLOGFILE[] = "LambdaPRO.log";

#define UC  unsigned char
#define UL  unsigned long
#define US  unsigned short
#define UI  unsigned int

#define MAX_NAME_SIZE   100
#define PACK_LOG_COUNT       3000          //一个日志文件里记录的消息日志条数
#define SYS_LOG_COUNT          20000        //一个日志文件里允许记录的日志条数

/*定义系统中的日志*/
//#define _TS_DEBUG

#ifdef _TS_DEBUG

#define CLIENT_AGENT_DEBUG
#define TS_CURSYS_LEVEL       LDEBUG

//ICEServer
#define IS_PACK_LOG
#define IS_CURSYS_LOG_LEVEL        LDEBUG
#else
#define TS_CURSYS_LEVEL       LERROR

//ICEServer
#define IS_CURSYS_LOG_LEVEL        LERROR
#endif


//定义系统日志接口的简化形式
#define sysLoger SystemLog::getInstance()->LogPrint

//定义去掉路径的文件名
#define __MYFILE__ truncatPathFromFilename(__FILE__)


/************************类型定义******************************/
#ifndef LOGLEVE
#define LOGLEVE
enum _LogLevel
{
    LDEBUG,
    LINFO,
    LWARNING,
    LERROR,
    LFATAL,
    LBUTT = 255
};
typedef _LogLevel LogLevel;
#endif

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
    void openLogFile(string logfileName = string(SYSTEMLOGFILE));

protected:

    void message_output();

    //关闭并重命名日志文件
    void closeLogFile();
private:
    char *buf;
    //char buftype;
    string filename;              //记录当前文件名
    //static char *curFile;         //记录当前文件名
    US bufsize;
    FILE *fp;
    UI  maxLogFileLine;
    UI  curLogFileLine;
    string                     logPath;                            //保存日志文件路径
    LogLevel                curLogLevel;                //当前允许输出的日志级别
    static SystemLog *plog;
    static TS_Mutex *pInitmtx;
    TS_Mutex logmtx;                          //日志文件锁
};


#endif  /*_LOG_H*/

