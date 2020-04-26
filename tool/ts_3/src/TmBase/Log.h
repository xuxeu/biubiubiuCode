/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  log.h
 * @Version        :  1.0.0
 * @Brief           :  ��־�������
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006��11��29�� 17:45:06
 * @History        : 
 *   
 */

#ifndef _LOG_H
#define _LOG_H


/************************���ò���*****************************/
#include <map>
#include <string>
#include "mutex.h"
#include "windows.h"

#include "Packet.h"
/************************ǰ����������***********************/
using namespace std;

const char SYSTEMLOGFILE[] = "system.log"; 

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

/*
�ú����Ĺ����ǴӸ������ļ���filename ��
ȥ��·���󣬷��ز���·�����ļ���ָ��
*/
char* truncatPathFromFilename(char *filename);

/*
�ú����������ǽ�ָ�����ַ���str��
�е�oldch �ַ��滻�� newch �ַ���
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
/************************���岿��*****************************/
//class TS_Mutex;
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


/**************************��Ϣ��־*************************************/

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

        //������־�ļ�Ŀ¼
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
    //����Ľӿ�
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
    //��������ǰ�������־�ļ�Ϊ"xx.YYYY-MM-DD-HH-MM.log"
    void renameLogFile();
    //����Ϣд����־�ļ�
    void writePackToFile(PackInfo info, const Packet &pack);
    void writePackToFile(PackInfo info, char *pBuf, int len);

private:
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [begin]*/
    /*  Modifed brief: ����д��־�������־�򿪱�־*/
    string                  filename;
    string                  logPath;             //������־�ļ�·��
    TS_Mutex                mtxLog;
    TS_Mutex                mtxWrite;         //д��־����
    UI                       maxLogCount;
    UI                       curLogCount;
    FILE*                  pCurFile;
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [end]*/
};
#endif  /*_LOG_H*/

