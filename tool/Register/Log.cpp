/*
* ������ʷ��
* 2006-11-29 qingxiaohai  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  log.cpp
* @brief
*       ���ܣ�
*       <li>ʵ��ϵͳ����־����Ϣ����־</li>
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
#include "mutex.h"
#include "log.h"
/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/
const char LEVELINFO[5][10] = {"LDEBUG","LINFO", "LWARNING", "LERROR", "LFATAL"};


SystemLog* SystemLog::plog = NULL;
TS_Mutex* SystemLog::pInitmtx = new TS_Mutex();

/**
 * @Funcname:  SystemLog
 * @brief        :  ϵͳ��־������,׼�����弰�ļ�
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006��11��29�� 17:34:59
 *
**/
SystemLog::SystemLog(LogLevel level , unsigned int line, unsigned short size)
{
    char *pStr = new char[0];
    char *pIde = new char[0];
    bufsize = (size>4096)?size:4096;
    maxLogFileLine = (line>0)?line:65535;
    curLogFileLine = 0;
    curLogLevel = level;          //Ĭ�����DEBUG���ϵļ���
    fp = NULL;
    buf = (char*)malloc(bufsize);

    if(buf == NULL)
    {
        return;
    }

    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: ���ӻ���ļ��*/
    if(buf)
    {
        memset(buf, 0, bufsize);
    }

    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [end]*/

    //�ȴ���ϵͳ��־Ŀ¼
    char temp[MAX_PATH] = "\0";
    GetModuleFileName(NULL, temp, sizeof(temp));
    pStr = temp;
    while ((pStr = strstr(pStr, "\\ide\\"))!=NULL)
    {
        pIde = pStr;
        pStr++;
    }
    *pIde = '\0';
    logPath = string(temp) + string("\\pub\\ts\\log\\systemLog\\");

    if(!CreateDirectory(logPath.data(), NULL))
    {
        CreateDirectory((string(temp)+"\\pub\\ts").data(), NULL);
        CreateDirectory((string(temp)+"\\pub\\ts\\log").data(), NULL);
        CreateDirectory(logPath.data(), NULL);
    }
    
    return;
}

/**
 * @Funcname:  openLogFile
 * @brief        :  ����־�ļ�
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006��12��4�� 14:32:29
 *
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

    fp = _fsopen((logPath + logfileName).data(), "w", _SH_DENYWR);       //��һ���ļ�����ֻ���ķ�ʽ����

    if (fp == NULL)
    {
        return;
    }

    filename = logPath + logfileName;
}

void SystemLog::closeLogFile()
{
    //�ȹر��ļ�
    if(fp == NULL)
    {
        return;
    }

    //�ر���־
    fclose(fp);
    fp = NULL;

    //����system+ʱ��.log�ļ���
    char filename[MAX_PATH] = "\0";
    int idx1 = 0, idx2 = 0;
    idx2 = (UI)(this->filename.find_last_of("."));

    strcpy(filename, this->filename.substr(idx1, idx2 - idx1).data());
    strcat(filename, "_bak.log");
    //ɾ�����ļ�
    DeleteFile(filename);
    //�������ļ�
    rename(this->filename.data(), filename);
    return;

}
/**
 * @Funcname:  ~SystemLog
 * @brief        :  ϵͳ��־������,�ͷŻ�����ļ�
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006��11��29�� 17:35:42
 *
**/
SystemLog::~SystemLog( )
{
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: �޸��ͷŻ����ж�*/
    if(buf != NULL)
    {
        free(buf);
        buf = NULL;
    }

    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [end]*/
    if(fp != NULL)
    {
        closeLogFile();
    }

    plog = NULL;
    return;
}

/**
 * @Funcname:  getInstance
 * @brief        :  ��ȡϵͳ��־�Ķ���
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006��11��29�� 17:22:51
 *
**/
SystemLog*SystemLog::getInstance(LogLevel level)
{
    if(NULL == plog)
    {
        TS_MutexLocker locker(pInitmtx);

        if(NULL == plog)
        {
            plog = new SystemLog(level);
        }
    }

    return plog;
}
/**
 * @Funcname:  LogPrint
 * @brief        :  ϵͳ��־����Ľӿں���
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006��11��29�� 17:22:51
 *
**/
void SystemLog::LogPrint(LogLevel level, char *fmt,...)
{
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: ���ӻ���ļ��*/
    if((level < curLogLevel) || (NULL == buf) || fp == NULL)
    {
        return;           //��������������ϼ���ľͲ������
    }

    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [end]*/

    char time[11] = "\0";
    char date[11] = "\0";
    char tmpbuf[4096] = "\0";

    _strtime(time);
    _strdate(date);

    TS_MutexLocker locker(&logmtx);

    va_list ap;
    va_start(ap, fmt); // use variable arg list

    //ȥ��fmt�еĵ�һ��\n��������\n���ƶ�levelǰ��
    if(strchr(fmt, '\n') == fmt)
    {
        fmt++;
        sprintf(buf, "\n\n%s %-10s %-10s", date, time, LEVELINFO[level%5]);
    }

    else
    {
        sprintf(buf, "\n%s %-10s %-10s", date, time, LEVELINFO[level%5]);
    }

    _vsnprintf(tmpbuf, sizeof(tmpbuf)-1, fmt, ap);
    strcat(buf, tmpbuf);
    buf[bufsize - 1] = '\0';
    va_end(ap);

    message_output();

    return;
}

/**
 * @Funcname:  message_output
 * @brief        :  ����־��Ϣд����־�ļ�
 * @return      :
 * @Author     :  qingxiaohai
 *
 * @History: 1.  Created this function on 2006��11��29�� 17:25:08
 *
**/
void SystemLog::message_output()
{
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: ���ӻ���ļ��*/
    if(NULL == buf)
    {
        return;
    }

    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [end]*/
    if(fp)
    {
        if(curLogFileLine > maxLogFileLine)
        {
            closeLogFile();
            int idx = 0;
            idx = (UI)(filename.find_last_of("\\"));

            if(idx != string::npos)
            {
                filename = filename.substr(idx);
            }

            openLogFile(filename);
            curLogFileLine = 0;
        }

		fprintf(fp, "%s", buf);
        fflush(fp);
        curLogFileLine++;
    }

    return;
}

