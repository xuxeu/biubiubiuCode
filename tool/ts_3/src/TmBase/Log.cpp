/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  Log.cpp
 * @Version        :  1.0.0
 * @Brief           :  ʵ��ϵͳ����־����Ϣ����־
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006��11��29�� 16:52:27
 * @History        : 
 *   
 */




/************************���ò���*****************************/
#include "stdarg.h"
#include "stdlib.h"
#include "stdio.h"
#include "share.h"
#include "string.h"
#include "time.h"
#include "windows.h"
#include "mutex.h"
#include "log.h"
/************************ǰ����������***********************/


const char LEVELINFO[5][10] = {"LDEBUG","LINFO", "LWARNING", "LERROR", "LFATAL"};
/************************���岿��*****************************/

SystemLog* SystemLog::plog = NULL;
TS_Mutex* SystemLog::pInitmtx = new TS_Mutex();
//char* SystemLog::curFile = "System.log";
/************************ʵ�ֲ���****************************/

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
    bufsize = (size>4096)?size:4096;
    maxLogFileLine = (line>0)?line:65535;   
    curLogFileLine = 0;
    curLogLevel = level;          //Ĭ�����DEBUG���ϵļ���
    fp = NULL;
    buf = (char*)malloc(bufsize);
    if(buf == NULL)
    {
        buf = (char*)malloc(bufsize);
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
    *(strrchr(temp, '\\')) = '\0';
    logPath = string(temp) + string("\\ts\\log\\systemLog\\");
    if(!CreateDirectory(logPath.data(), NULL))
    {   
        CreateDirectory((string(temp)+"\\ts").data(), NULL);
        CreateDirectory((string(temp)+"\\ts\\log").data(), NULL);
        CreateDirectory(logPath.data(), NULL);
    }
    openLogFile();
   
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
    idx2 = this->filename.find_last_of(".");
    
    strcpy_s(filename, sizeof(filename), this->filename.substr(idx1, idx2 - idx1).data());
    strcat_s(filename, sizeof(filename)-strlen(filename), "_bak.log");
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
    if((level < curLogLevel) || (NULL == buf))
    {
        return;           //��������������ϼ���ľͲ������
    }
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [end]*/
    
    char time[11] = "\0";
    char date[11] = "\0";
    char tmpbuf[4096] = "\0";

    _strtime_s(time, sizeof(time));
    _strdate_s(date, sizeof(date));
    
    TS_MutexLocker locker(&logmtx);

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
    strcat_s(buf, bufsize-strlen(buf), tmpbuf);
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
    return;
}



/*******************��Ϣ��־***************************/

/**
 * @Funcname:  PackLog::write
 * @brief        :  ��Ϣ��־ģ�����ӿ�
 * @para1      : info,��־��Ϣ,���հ����Ƿ��Ͱ�
 * @para2      : pack ���ݰ�
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��6�� 16:11:10
 *   
**/
void PackLog::write(PackAct act, const Packet &pack)
{
    char stime[10] = "\0";
    char sdate[10] = "\0";
    PackInfo info;

    //������Ϣ����ʱ�估��Ϊ��Ϣ
    _strtime_s(stime, sizeof(stime));
    _strdate_s(sdate, sizeof(sdate));
    strcpy_s(info.strtime, sizeof(info.strtime), sdate);
    strcat_s(info.strtime, sizeof(info.strtime) - strlen(info.strtime), "\t");
    strcat_s(info.strtime, sizeof(info.strtime) - strlen(info.strtime), stime);
    info.pack_act = act;
    
    TS_MutexLocker locker(&mtxWrite);

    //����־�ѹر��򷵻�
    if(NULL == pCurFile)
    {
        return;
    }
    
    //�����־��¼�ﵽ������������´�һ���ļ�    
    if(curLogCount >= maxLogCount)
    {
        //���ݾ���־�ļ�
        renameLogFile();
        
        //������־�ļ�
        open(this->filename.data());
    }
    
    //����Ϣд����־�ļ�
    writePackToFile(info, pack);
    
    //��־��¼��������1
    curLogCount++;
        
    return;
}


/**
 * @Funcname:  PackLog::write
 * @brief        :  ��Ϣ��־ģ�����ӿ�
 * @para1      : info,��־��Ϣ,���հ����Ƿ��Ͱ�
 * @para2      : pack ���ݰ�
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��6�� 16:11:10
 *   
**/
void PackLog::write(PackAct act, char *pbuf, int len)
{    
    char stime[10] = "\0";
    char sdate[10] = "\0";
    PackInfo info;

    //������Ϣ����ʱ�估��Ϊ��Ϣ
    _strtime_s(stime, sizeof(stime));
    _strdate_s(sdate, sizeof(sdate));
    strcpy_s(info.strtime, sizeof(info.strtime), sdate);
    strcat_s(info.strtime, sizeof(info.strtime) - strlen(info.strtime), "\t");
    strcat_s(info.strtime, sizeof(info.strtime) - strlen(info.strtime), stime);
    info.pack_act = act;

    TS_MutexLocker locker(&mtxWrite);

    //����־�ѹر��򷵻�
    if(NULL == pCurFile)
    {
        return;
    }
    
    //�����־��¼�ﵽ������������´�һ���ļ�
    if(curLogCount >= maxLogCount)
    {
        //���ݾ���־�ļ�
        renameLogFile();
        
        //������־�ļ�
        open(this->filename.data());
    }
    
    //����Ϣд����־�ļ�
    writePackToFile(info, pbuf, len);
    
    //��־��¼��������1
    curLogCount++;
    return;
}

/**
 * @Funcname:  renameLogFile
 * @brief        :  ��������Ϣ��־�ļ�
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��1��5�� 9:48:18
 *   
**/
void PackLog::renameLogFile()
{   
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [begin]*/
    /*  Modifed brief: ��������־ʱ���ӻ���*/
    TS_MutexLocker locker(&mtxLog);
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [end]*/
    
    //�ȹر��ļ�
    if(pCurFile == NULL)
    {
        return;
    }

    if (0 != fclose(pCurFile))
    {
        SystemLog::getInstance()->LogPrint(LERROR, "PackLog::renameLogFile: close log file failed, [%s], [error_code :%d]",
                            this->filename.data(), GetLastError());
    }
    else
    {
        //��־�ѹر�
        pCurFile = NULL;
    }    
    
    //����system+ʱ��.log�ļ���
    char filenm[MAX_PATH] = "\0";
    std::string::size_type startIdx, endIdx;    

    //filename_bak.log
    //��ȡ�ļ���(����·���ͺ�׺)
    startIdx = this->filename.find_last_of('\\');
    endIdx = this->filename.find_last_of('.');
    startIdx++;
    
    memcpy(filenm, this->filename.data(), endIdx);
    //��װ���ļ���
    strcat_s(filenm, sizeof(filenm)-strlen(filenm), "_bak");
    strcat_s(filenm, sizeof(filenm)-strlen(filenm), ".log");
    //ɾ���ɵı���
    if(!DeleteFile(filenm))
    {
        SystemLog::getInstance()->LogPrint(LWARNING, "PackLog::renameLogFile: delete log back file failed, [%s], [error_code :%d]",
                            filenm, GetLastError() );
    }
    //�������ļ�
    if(0 != rename(filename.data(), filenm))
    {
        SystemLog::getInstance()->LogPrint(LERROR, "PackLog::renameLogFile: rename log file failed, oldNm[%s], newNm[%s], [error_code :%d]",
                            this->filename.data(), filenm, GetLastError());
    }
        
    return;
}


/**
 * @Funcname:  void PackLog::openLogFile
 * @brief        :  ����־�ļ�
 * @para1      : filename, �ļ���
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��6�� 11:37:15
 *   
**/
void PackLog::open(const char *filename)
{
    TS_MutexLocker locker(&mtxLog);
    
    if(NULL != pCurFile)
    {
        return;
    }
    
    if(NULL == filename)
    {
        sysLoger(LWARNING, "PackLog::open: Parameter filename is Null pointer !");
        return;
    }
    if(this->filename.empty())
    {        
        this->filename = logPath + string(filename);
    }
    
    pCurFile = _fsopen(this->filename.data(), "w", _SH_DENYWR);       //��һ���ļ�����ֻ���ķ�ʽ����
    if(NULL == pCurFile)
    {
        pCurFile  = _fsopen(this->filename.data(), "w", _SH_DENYWR);  //����һ��
        if(NULL == pCurFile)
        {
            sysLoger(LERROR, "PackLog:: open log file %s  failure! error code :%d", 
                                        this->filename.data(), GetLastError());
            return;
        }
    }
    curLogCount = 0;
    return;
}

/**
 * @Funcname:  void PackLog::writePackToFile
 * @brief        :  ����Ϣд����־�ļ�
 * @para1      : pack:��Ҫд����־����Ϣ
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��6�� 13:48:01
 *   
**/
void PackLog::writePackToFile(PackInfo info, const Packet &pack)
{
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [begin]*/
    /*  Modifed brief: д��־ʱ���ӻ���*/
    TS_MutexLocker locker(&mtxLog);
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [end]*/
    
    char strpackAct[3][10] = {"Sended!","Received!", "NULL"};
    unsigned char buf[20] = " ";
    Packet tmpPack = const_cast<Packet &>(pack);
    
    if(NULL == pCurFile)
    {
        sysLoger(LERROR, "PackLog: Invalid log file point encounted!");
        return;
    }
    //�����Ϣ��ͷ������ʱ�����Ϣ
    fprintf_s(pCurFile, "\n\n%s", info.strtime);
    fprintf_s(pCurFile, "  %s  srcaid=%d,desaid=%d,srcSaid=%d,desSaid=%d,serial=%d,size=%d",
                strpackAct[info.pack_act], tmpPack.getSrc_aid(), tmpPack.getDes_aid(), tmpPack.getSrc_said(),
                tmpPack.getDes_said(), tmpPack.getSerialNum(), tmpPack.getSize());

    //�����Ϣ������
    int idx = 0;
    int chperline = 16;
    int cpylen = 0;
    int chCountInLine = 0;
    fprintf_s(pCurFile, "\nMessage Content:");
    
    while(idx < tmpPack.getSize())
    {
        memset(buf, 0, sizeof(buf));
        chCountInLine = 0;
        if(idx > tmpPack.getSize())
        {
            break;
        }
        //cpylen = ((idx+chperline)>pack.getSize())?(pack.getSize()-idx):chperline;
        cpylen = min((tmpPack.getSize()-idx), chperline);
        //ÿ�δ���Ϣ����������bytecount ���ֽڳ���                        
        memcpy_s(buf, sizeof(buf), &tmpPack.getData()[idx], cpylen);
            
        
        idx += cpylen;
        //��buf�е������������־
        fprintf_s(pCurFile, "\n");
        for(int i = 0; i<cpylen; i++)
        {
             //ʮ�������ֽ���ʽ
            fprintf_s(pCurFile, "%-3x", buf[i]);  
             chCountInLine++;
             
        }

        //����16���ֽڵ�λ��
        if(chCountInLine < 16)
        {
            for(int k=16-chCountInLine; k>0; k--)
            {
                fprintf_s(pCurFile, "%-3c", ' ');
            }
        }
        //�ַ�����ʽ
        fprintf_s(pCurFile, "\t\t");
        for(int i = 0; i<cpylen; i++)
        {
            if(isprint(buf[i]) != 0)
            {
                //�ǿɴ�ӡ�ַ�
                fprintf_s(pCurFile, "%c", buf[i]);
            }
            else
            {
                //���ɴ�ӡ�ַ�����. ����
                fprintf_s(pCurFile, ".");
            }
        }
        
    }
    fprintf_s(pCurFile, "\n");
    fflush(pCurFile);

    return;
}

/**
 * @Funcname:  void PackLog::writePackToFile
 * @brief        :  ����Ϣд����־�ļ�
 * @para1      : pack:��Ҫд����־����Ϣ
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��6�� 13:48:01
 *   
**/
void PackLog::writePackToFile(PackInfo info, char *pBuf, int len)
{
    TS_MutexLocker locker(&mtxLog);
    
    char strpackAct[3][10] = {"Sended!","Received!", "NULL"};
    unsigned char buf[20] = " ";

    if(NULL == pCurFile)
    {
        sysLoger(LERROR, "PackLog: Invalid log file point encounted! FILE:%s, LINE:%d",
                                        __MYFILE__, __LINE__);
        return;
    }
    //�����Ϣ��ͷ������ʱ�����Ϣ
    fprintf_s(pCurFile, "\n\n%s\t%s\tSize=%d", info.strtime, strpackAct[info.pack_act], len);
    /*fprintf_s(pCurFile, "  %s  srcaid=%d,desaid=%d,srcSaid=%d,desSaid=%d,serial=%d,size=%d",
                strpackAct[info.pack_act], tmpPack.getSrc_aid(), tmpPack.getDes_aid(), tmpPack.getSrc_said(),
                tmpPack.getDes_said(), tmpPack.getSerialNum(), tmpPack.getSize());*/

    //�����Ϣ������
    int idx = 0;
    int chperline = 16;
    int cpylen = 0;
    int chCountInLine = 0;
    fprintf_s(pCurFile, "\nMessage Content:");
    
    while(idx < len)
    {
        memset(buf, 0, sizeof(buf));
        chCountInLine = 0;
        if(idx > len)
        {
            break;
        }
        //cpylen = ((idx+chperline)>pack.getSize())?(pack.getSize()-idx):chperline;
        cpylen = min((len-idx), chperline);
        //ÿ�δ���Ϣ����������bytecount ���ֽڳ���                        
        memcpy_s(buf, sizeof(buf), &pBuf[idx], cpylen);
            
        
        idx += cpylen;
        //��buf�е������������־
        fprintf_s(pCurFile, "\n");
        for(int i = 0; i<cpylen; i++)
        {
             //ʮ�������ֽ���ʽ
            fprintf_s(pCurFile, "%-3x", buf[i]);  
             chCountInLine++;
             
        }

        //����16���ֽڵ�λ��
        if(chCountInLine < 16)
        {
            for(int k=16-chCountInLine; k>0; k--)
            {
                fprintf_s(pCurFile, "%-3c", ' ');
            }
        }
        //�ַ�����ʽ
        fprintf_s(pCurFile, "\t\t");
        for(int i = 0; i<cpylen; i++)
        {
            if(isprint(buf[i]) != 0)
            {
                //�ǿɴ�ӡ�ַ�
                fprintf_s(pCurFile, "%c", buf[i]);
            }
            else
            {
                //���ɴ�ӡ�ַ�����. ����
                fprintf_s(pCurFile, ".");
            }
        }
        
    }
    fprintf_s(pCurFile, "\n");
    fflush(pCurFile);

    return;
}

/**
 * @Funcname:  deleteLogFile
 * @brief        :  ɾ����־�ļ�
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007��4��14�� 10:41:40
 *   
**/
void PackLog::deleteLogFile()
{
    string logFileBack;
    logFileBack = filename;
    
    //�ر���־�ļ�
    close();
    
    if(logFileBack.find(".log") == logFileBack.length() - strlen(".log"))
    {
        logFileBack = filename.substr(0, filename.length() - strlen(".log"));
        logFileBack += "_bak.log";
    }
    
    mtxLog.lock();
    
    DeleteFileA(filename.data());
    DeleteFileA(logFileBack.data());
    
    mtxLog.unlock();
    
    return;
}
/**********************************��������****************************/

/**
 * @Funcname:  truncatPathFromFilename
 * @brief        :  ������·�����ļ�����ȡ��·��
 * @para1      : filename, �ļ���
 * @return      : ���ز���·�����ļ���, ��"\0"�������ַ���
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��1�� 9:11:54
 *   
**/
char* truncatPathFromFilename(char *filename)
{
    UI  idx = 0;
    string name = string(filename);
    idx = name.find_last_of('\\');
    if(idx == string::npos)
    {
        return filename;
    }
    return &filename[idx+1];
}

/**
 * @Funcname:  ReplaceChInstr
 * @brief        :  ���ַ������滻ָ�����ַ�
 * @para1      : char *str,   Դ�ַ���
 * @para2      : char oldch, ��Ҫ�滻���ַ�
 * @para3      : char newch �滻����ַ�
 * @return      : 0:��ʾ�ɹ�,-1ʧ��
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006��12��4�� 14:24:05
 *   
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


