/*
* ������ʷ��
* 2006-11-29 qingxiaohai  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  log.h
* @brief
*       ���ܣ�
*       <li>��־�������</li>
*/



#ifndef _LOG_H
#define _LOG_H


/************************ͷ�ļ�********************************/
#include <map>
#include <string>
#include "mutex.h"
#include "windows.h"

using namespace std;

/************************�궨��********************************/
const char SYSTEMLOGFILE[] = "LambdaPRO.log";

#define UC  unsigned char
#define UL  unsigned long
#define US  unsigned short
#define UI  unsigned int

#define MAX_NAME_SIZE   100
#define PACK_LOG_COUNT       3000          //һ����־�ļ����¼����Ϣ��־����
#define SYS_LOG_COUNT          20000        //һ����־�ļ��������¼����־����

/*����ϵͳ�е���־*/
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


//����ϵͳ��־�ӿڵļ���ʽ
#define sysLoger SystemLog::getInstance()->LogPrint

//����ȥ��·�����ļ���
#define __MYFILE__ truncatPathFromFilename(__FILE__)


/************************���Ͷ���******************************/
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

    /*ϵͳ��־�Ķ���ӿں�����������printf������
    ���������Զ���ÿһ����־��¼ǰ�ӻس����з���
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

    //�رղ���������־�ļ�
    void closeLogFile();
private:
    char *buf;
    //char buftype;
    string filename;              //��¼��ǰ�ļ���
    //static char *curFile;         //��¼��ǰ�ļ���
    US bufsize;
    FILE *fp;
    UI  maxLogFileLine;
    UI  curLogFileLine;
    string                     logPath;                            //������־�ļ�·��
    LogLevel                curLogLevel;                //��ǰ�����������־����
    static SystemLog *plog;
    static TS_Mutex *pInitmtx;
    TS_Mutex logmtx;                          //��־�ļ���
};


#endif  /*_LOG_H*/

