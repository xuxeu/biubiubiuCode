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
#include "QMutex.h"
#include "windows.h"
//#include "thread.h"
//#include "Packet.h"
/************************前向声明部分***********************/
using namespace std;

#define UC  unsigned char
#define UL  unsigned long
#define US  unsigned short
#define UI  unsigned int

#define MAX_NAME_SIZE   100

#define PACK_LOG_COUNT       3000          //一个日志文件里记录的消息日志条数
#define SYS_LOG_COUNT          20000        //一个日志文件里允许记录的日志条数
//调试时打印系统日志
#define ICE_DEBUG 1
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

typedef enum
{
    SLDEBUG,
    SLINFO,
    SLWARNING,
    SLERROR,
    SLFATAL,
    SLBUTT
}LogLevel;


/************************定义部分*****************************/
//class QMutex;
class SystemLog
{

public:
    SystemLog(unsigned int line = SYS_LOG_COUNT, unsigned short size = 4096);
    ~SystemLog();
    
    /*系统日志的对外接口函数，用于与printf很相像，
    不过它会自动在每一条日志记录前加回车换行符。
    */
    void LogPrint(LogLevel level, char *fmt, ...);

    static SystemLog *getInstance();
    
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
    static QMutex *pInitmtx;
    QMutex logmtx;                          //日志文件锁
};

#if 1 
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
        GetCurrentDirectoryA(sizeof(temp), temp);
        logPath = string(temp) + string("\\log\\MsgLog\\");
        //创建日志文件目录
        if(!CreateDirectoryA(logPath.data(), NULL))
        {   
            CreateDirectoryA((string(temp)+"\\log").data(), NULL);
            CreateDirectoryA(logPath.data(), NULL);
        }
    }
    ~PackLog()
    {
        renameLogFile();
        close();
    }
    void open(const char* filename);
    //对外的接口
    void write(PackAct act, char *pBuf, int len);

    void close()
    {
        if(pCurFile != NULL)
        {
            mtxLog.lock();
            fclose(pCurFile);
            renameLogFile();
            mtxLog.unlock();
            pCurFile = NULL;
        }
    }

    void deleteLogFile();
    
protected:
    //重命名当前对象的日志文件为"xx.YYYY-MM-DD-HH-MM.log"
    void renameLogFile();
    //将消息写入日志文件
    void writePackToFile(PackInfo info, char *pBuf, int len);

private:

    string                  filename;
    string                  logPath;             //保存日志文件路径
    QMutex                  mtxLog;
    UI                       maxLogCount;
    UI                       curLogCount;
    FILE*                  pCurFile;
};
#endif
#endif  /*_LOG_H*/

