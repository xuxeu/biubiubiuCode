/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  log.h
 * @Version        :  1.0.0
 * @Brief           :  日志类的声明
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006年11月29日 17:45:06
 * @History        : 
 *   
 */

#ifndef _LOG_H
#define _LOG_H


/************************引用部分*****************************/
#include <map>
#include <string>
#include "mutex.h"
#include "windows.h"

#include "Packet.h"
/************************前向声明部分***********************/
using namespace std;

const char SYSTEMLOGFILE[] = "system.log"; 

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
/************************定义部分*****************************/
//class TS_Mutex;
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


/**************************消息日志*************************************/

typedef enum{SEND_PACK, RECV_PACK}PackAct;
typedef struct _PackInfo
{
    PackAct pack_act;
    char strtime[50];
}PackInfo;

class PackLog //: public threadWithQueue
{
public:
    PackLog(UI logCount = PACK_LOG_COUNT)
    {
        maxLogCount = (logCount>0)?logCount:PACK_LOG_COUNT;
        curLogCount = 0;
        pCurFile = NULL;
        char temp[MAX_PATH] = "\0";
        GetModuleFileName(NULL, temp, sizeof(temp));
        *(strrchr(temp, '\\')) = '\0';
        logPath = string(temp) + string("\\ts\\log\\MsgLog\\");

        //创建日志文件目录
        if(!CreateDirectoryA(logPath.data(), NULL))
        {   
            CreateDirectoryA((string(temp)+"\\ts").data(), NULL);
            CreateDirectoryA((string(temp)+"\\ts\\log").data(), NULL);
            CreateDirectoryA(logPath.data(), NULL);
        }
    }
    ~PackLog()
    {
        close();
    }
    void open(const char* filename);
    //对外的接口
    void write(PackAct act, const Packet &pack);
    void write(PackAct act, char *pBuf, int len);
    void close()
    {
        mtxWrite.lock();
        renameLogFile();
        mtxWrite.unlock();
    }

    void deleteLogFile();
    
protected:
    //重命名当前对象的日志文件为"xx.YYYY-MM-DD-HH-MM.log"
    void renameLogFile();
    //将消息写入日志文件
    void writePackToFile(PackInfo info, const Packet &pack);
    void writePackToFile(PackInfo info, char *pBuf, int len);

private:
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [begin]*/
    /*  Modifed brief: 增加写日志互斥和日志打开标志*/
    string                  filename;
    string                  logPath;             //保存日志文件路径
    TS_Mutex                mtxLog;
    TS_Mutex                mtxWrite;         //写日志互斥
    UI                       maxLogCount;
    UI                       curLogCount;
    FILE*                  pCurFile;
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [end]*/
};
#endif  /*_LOG_H*/

