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
#include "QMutex.h"
#include "windows.h"
//#include "thread.h"
//#include "Packet.h"
/************************ǰ����������***********************/
using namespace std;

#define UC  unsigned char
#define UL  unsigned long
#define US  unsigned short
#define UI  unsigned int

#define MAX_NAME_SIZE   100

#define PACK_LOG_COUNT       3000          //һ����־�ļ����¼����Ϣ��־����
#define SYS_LOG_COUNT          20000        //һ����־�ļ��������¼����־����
//����ʱ��ӡϵͳ��־
#define ICE_DEBUG 1
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

typedef enum
{
    SLDEBUG,
    SLINFO,
    SLWARNING,
    SLERROR,
    SLFATAL,
    SLBUTT
}LogLevel;


/************************���岿��*****************************/
//class QMutex;
class SystemLog
{

public:
    SystemLog(unsigned int line = SYS_LOG_COUNT, unsigned short size = 4096);
    ~SystemLog();
    
    /*ϵͳ��־�Ķ���ӿں�����������printf������
    ���������Զ���ÿһ����־��¼ǰ�ӻس����з���
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
    static QMutex *pInitmtx;
    QMutex logmtx;                          //��־�ļ���
};

#if 1 
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
        GetCurrentDirectoryA(sizeof(temp), temp);
        logPath = string(temp) + string("\\log\\MsgLog\\");
        //������־�ļ�Ŀ¼
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
    //����Ľӿ�
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
    //��������ǰ�������־�ļ�Ϊ"xx.YYYY-MM-DD-HH-MM.log"
    void renameLogFile();
    //����Ϣд����־�ļ�
    void writePackToFile(PackInfo info, char *pBuf, int len);

private:

    string                  filename;
    string                  logPath;             //������־�ļ�·��
    QMutex                  mtxLog;
    UI                       maxLogCount;
    UI                       curLogCount;
    FILE*                  pCurFile;
};
#endif
#endif  /*_LOG_H*/

