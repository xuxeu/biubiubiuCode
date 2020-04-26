/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ��Т��  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  tsfsLog.cpp
* @brief  
*       ���ܣ�
*       <li> ʵ��ϵͳ����־����Ϣ����־</li>
*/
/************************ͷ�ļ�********************************/
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

/************************�궨��********************************/

/************************���Ͷ���******************************/

/************************ȫ�ֱ���******************************/
const char SYSTEMLOGFILE[] = "IceServerSys"; 

const char LEVELINFO[5][10] = {"LDEBUG","LINFO", "LWARNING", "LERROR", "LFATAL"};

SystemLog* SystemLog::plog = NULL;

TMutex* SystemLog::pInitmtx = new TMutex();

/************************ģ�����******************************/

/************************�ⲿ����******************************/

/************************ǰ������******************************/

/************************ʵ��*********************************/
/**
 * @Funcname: SystemLog
 * @brief: ϵͳ��־������,׼�����弰�ļ�
 * @Author:qingxiaohai
 * @History: Created this function on 2006��11��29�� 17:34:59 
**/
SystemLog::SystemLog(LogLevel level, unsigned int line, unsigned short size)
{
    bufsize = (size>4096)?size:4096;
    maxLogFileLine = (line>0)?line:SYS_LOG_COUNT;   
    curLogFileLine = 0;
    curLogLevel = level; //Ĭ�����INFO���ϵļ���
    fp = NULL;
    buf = (char*)malloc(bufsize);
    if(buf == NULL)
    {
        buf = (char*)malloc(bufsize);
    }

    memset(buf, 0, bufsize);

    //�ȴ���ϵͳ��־Ŀ¼
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
 * @brief:  ����־�ļ�
 * @Author:  qingxiaohai
 * @History: 1.  Created this function on 2006��12��4�� 14:32:29 
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

    //��һ���ļ�����ֻ���ķ�ʽ����
    fp = _fsopen((logPath + logfileName).data(), "w", _SH_DENYWR); 
    
	filename = logPath + logfileName;
}

void SystemLog::closeLogFile()
{
    //�ȹر��ļ�
    if(fp == NULL)
    {
        return;
    }

    fclose(fp);
    fp = NULL;

    //����system+ʱ��.log�ļ���
    char filename[MAX_PATH] = "\0";
    int idx2 = 0;
    idx2 = this->filename.find_last_of('.');
    
    strcat_s(filename, sizeof(filename), this->filename.substr(0, idx2).data());
    strcat_s(filename, sizeof(filename), "_bak.log");
    
	//ɾ�����ļ�
    DeleteFile(filename);

    //�������ļ�
    rename(this->filename.data(), filename);
    return;
    
}

/**
 * @Funcname:  ~SystemLog
 * @brief : ϵͳ��־������,�ͷŻ�����ļ� 
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��29�� 17:35:42
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
 * @brief : ��ȡϵͳ��־�Ķ��� 
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��29�� 17:22:51  
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
 * @brief:ϵͳ��־����Ľӿں���
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��11��29�� 17:22:51
**/
void SystemLog::LogPrint(LogLevel level, char *fmt,...)
{
    char time[11] = "\0";
    char date[11] = "\0";
    char tmpbuf[4096] = "\0";
    
    if((LBUTT == curLogLevel) || (level < curLogLevel))
    {
        return; //��������������ϼ���ľͲ������
    }
    _strtime_s(time, sizeof(time));
    _strdate_s(date, sizeof(date));
    
    TMutexLocker locker(&logmtx);

    va_list ap;
    va_start(ap, fmt); // use variable arg list
    
    //ȥ��fmt�еĵ�һ��\n��������\n���ƶ�levelǰ��
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
 * @brief : ����־��Ϣд����־�ļ�
 * @Author:qingxiaohai
 * @History: 1.  Created this function on 2006��11��29�� 17:25:08
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
 * @brief: ������·�����ļ�����ȡ��·��
 * @para1: filename, �ļ���
 * @return: ���ز���·�����ļ���, ��"\0"�������ַ���
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��12��1�� 9:11:54
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
 * @brief:  ���ַ������滻ָ�����ַ�
 * @para1: char *str, Դ�ַ���
 * @para2: char oldch, ��Ҫ�滻���ַ�
 * @para3: char newch �滻����ַ�
 * @return: 0:��ʾ�ɹ�,-1ʧ��
 * @Author: qingxiaohai
 * @History: 1.  Created this function on 2006��12��4�� 14:24:05
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


