/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  tsfsLog.h
* @brief  
*       ���ܣ�
*       <li>��־�������</li>
*/
/************************ͷ�ļ�********************************/
#ifndef _LOG_H
#define _LOG_H

#include <map>
#include <string>
#include "tsfsMutex.h"
#include "windows.h"
#include "..\tsApi\tclientapi.h"
using namespace std;

/************************�궨��********************************/
#define UC  unsigned char
#define UL  unsigned long
#define US  unsigned short
#define UI  unsigned int

#define MAX_NAME_SIZE   100

#define PACK_LOG_COUNT   3000        //һ����־�ļ����¼����Ϣ��־����
#define SYS_LOG_COUNT    3000        //һ����־�ļ��������¼����־����
//����ʱ��ӡϵͳ��־

#define _IS_DEBUG
#ifdef _IS_DEBUG
#define IS_PACK_LOG
#define IS_CURSYS_LOG_LEVEL        LDEBUG
#else
#define IS_CURSYS_LOG_LEVEL        LERROR
#endif

//����ϵͳ��־�ӿڵļ���ʽ
#define sysLoger  SystemLog::getInstance()->LogPrint

//����ȥ��·�����ļ���
#define __MYFILE__ truncatPathFromFilename(__FILE__)


/************************�ӿ�����******************************/
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

/************************���Ͷ���******************************/
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
    void openLogFile(string logfileName = string("IceServerSys"));
    
protected:
    
    void message_output();
    //�رղ���������־�ļ�
    void closeLogFile();
private:
    char *buf;
    string filename;              //��¼��ǰ�ļ���
    US bufsize;
    FILE *fp;
    UI  maxLogFileLine;
    UI  curLogFileLine;
    string                     logPath;                            //������־�ļ�·��
    LogLevel                curLogLevel;                //��ǰ�����������־����
    static SystemLog *plog;
    static TMutex *pInitmtx;
    TMutex logmtx;                          //��־�ļ���
};
#endif  /*_LOG_H*/
