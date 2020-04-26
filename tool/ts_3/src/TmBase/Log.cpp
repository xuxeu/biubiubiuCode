/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  Log.cpp
 * @Version        :  1.0.0
 * @Brief           :  实现系统类日志和消息包日志
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2006年11月29日 16:52:27
 * @History        : 
 *   
 */




/************************引用部分*****************************/
#include "stdarg.h"
#include "stdlib.h"
#include "stdio.h"
#include "share.h"
#include "string.h"
#include "time.h"
#include "windows.h"
#include "mutex.h"
#include "log.h"
/************************前向声明部分***********************/


const char LEVELINFO[5][10] = {"LDEBUG","LINFO", "LWARNING", "LERROR", "LFATAL"};
/************************定义部分*****************************/

SystemLog* SystemLog::plog = NULL;
TS_Mutex* SystemLog::pInitmtx = new TS_Mutex();
//char* SystemLog::curFile = "System.log";
/************************实现部分****************************/

/**
 * @Funcname:  SystemLog
 * @brief        :  系统日志构造器,准备缓冲及文件
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年11月29日 17:34:59
 *   
**/
SystemLog::SystemLog(LogLevel level , unsigned int line, unsigned short size)
{   
    bufsize = (size>4096)?size:4096;
    maxLogFileLine = (line>0)?line:65535;   
    curLogFileLine = 0;
    curLogLevel = level;          //默认输出DEBUG以上的级别
    fp = NULL;
    buf = (char*)malloc(bufsize);
    if(buf == NULL)
    {
        buf = (char*)malloc(bufsize);
    }
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: 增加缓冲的检查*/
    if(buf)
    {
        memset(buf, 0, bufsize);
    }
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [end]*/
    
    //先创建系统日志目录
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
 * @brief        :  打开日志文件
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月4日 14:32:29
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
    fp = _fsopen((logPath + logfileName).data(), "w", _SH_DENYWR);       //打开一个文件，以只读的方式共享
    filename = logPath + logfileName;
}

void SystemLog::closeLogFile()
{
    //先关闭文件
    if(fp == NULL)
    {
        return;
    }
    
    //关闭日志
    fclose(fp);
    fp = NULL;
    
    //构造system+时间.log文件名
    char filename[MAX_PATH] = "\0";
    int idx1 = 0, idx2 = 0;
    idx2 = this->filename.find_last_of(".");
    
    strcpy_s(filename, sizeof(filename), this->filename.substr(idx1, idx2 - idx1).data());
    strcat_s(filename, sizeof(filename)-strlen(filename), "_bak.log");
    //删除旧文件
    DeleteFile(filename);
    //重命名文件
    rename(this->filename.data(), filename);
    return;
    
}
/**
 * @Funcname:  ~SystemLog
 * @brief        :  系统日志析构器,释放缓冲和文件
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年11月29日 17:35:42
 *   
**/
SystemLog::~SystemLog( )
{
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: 修改释放缓冲判断*/
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
 * @brief        :  获取系统日志的对象
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年11月29日 17:22:51
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
 * @brief        :  系统日志对外的接口函数
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年11月29日 17:22:51
 *   
**/
void SystemLog::LogPrint(LogLevel level, char *fmt,...)
{
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: 增加缓冲的检查*/
    if((level < curLogLevel) || (NULL == buf))
    {
        return;           //低于允许输出以上级别的就不再输出
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
    
    //去掉fmt中的第一个\n符，将该\n符移动level前面
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
 * @brief        :  将日志信息写入日志文件
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年11月29日 17:25:08
 *   
**/
void SystemLog::message_output()
{
    /*Modified by tangxp for BUG NO.0002925 on 2008.3.17 [begin]*/
    /*  Modifeide brief: 增加缓冲的检查*/
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



/*******************消息日志***************************/

/**
 * @Funcname:  PackLog::write
 * @brief        :  消息日志模块对外接口
 * @para1      : info,日志信息,接收包还是发送包
 * @para2      : pack 数据包
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月6日 16:11:10
 *   
**/
void PackLog::write(PackAct act, const Packet &pack)
{
    char stime[10] = "\0";
    char sdate[10] = "\0";
    PackInfo info;

    //构造消息包的时间及行为信息
    _strtime_s(stime, sizeof(stime));
    _strdate_s(sdate, sizeof(sdate));
    strcpy_s(info.strtime, sizeof(info.strtime), sdate);
    strcat_s(info.strtime, sizeof(info.strtime) - strlen(info.strtime), "\t");
    strcat_s(info.strtime, sizeof(info.strtime) - strlen(info.strtime), stime);
    info.pack_act = act;
    
    TS_MutexLocker locker(&mtxWrite);

    //若日志已关闭则返回
    if(NULL == pCurFile)
    {
        return;
    }
    
    //如果日志记录达到最大数量则重新打开一个文件    
    if(curLogCount >= maxLogCount)
    {
        //备份旧日志文件
        renameLogFile();
        
        //打开新日志文件
        open(this->filename.data());
    }
    
    //将消息写入日志文件
    writePackToFile(info, pack);
    
    //日志记录计数器加1
    curLogCount++;
        
    return;
}


/**
 * @Funcname:  PackLog::write
 * @brief        :  消息日志模块对外接口
 * @para1      : info,日志信息,接收包还是发送包
 * @para2      : pack 数据包
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月6日 16:11:10
 *   
**/
void PackLog::write(PackAct act, char *pbuf, int len)
{    
    char stime[10] = "\0";
    char sdate[10] = "\0";
    PackInfo info;

    //构造消息包的时间及行为信息
    _strtime_s(stime, sizeof(stime));
    _strdate_s(sdate, sizeof(sdate));
    strcpy_s(info.strtime, sizeof(info.strtime), sdate);
    strcat_s(info.strtime, sizeof(info.strtime) - strlen(info.strtime), "\t");
    strcat_s(info.strtime, sizeof(info.strtime) - strlen(info.strtime), stime);
    info.pack_act = act;

    TS_MutexLocker locker(&mtxWrite);

    //若日志已关闭则返回
    if(NULL == pCurFile)
    {
        return;
    }
    
    //如果日志记录达到最大数量则重新打开一个文件
    if(curLogCount >= maxLogCount)
    {
        //备份旧日志文件
        renameLogFile();
        
        //打开新日志文件
        open(this->filename.data());
    }
    
    //将消息写入日志文件
    writePackToFile(info, pbuf, len);
    
    //日志记录计数器加1
    curLogCount++;
    return;
}

/**
 * @Funcname:  renameLogFile
 * @brief        :  重命名消息日志文件
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年1月5日 9:48:18
 *   
**/
void PackLog::renameLogFile()
{   
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [begin]*/
    /*  Modifed brief: 重命名日志时增加互斥*/
    TS_MutexLocker locker(&mtxLog);
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [end]*/
    
    //先关闭文件
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
        //日志已关闭
        pCurFile = NULL;
    }    
    
    //构造system+时间.log文件名
    char filenm[MAX_PATH] = "\0";
    std::string::size_type startIdx, endIdx;    

    //filename_bak.log
    //截取文件名(除开路径和后缀)
    startIdx = this->filename.find_last_of('\\');
    endIdx = this->filename.find_last_of('.');
    startIdx++;
    
    memcpy(filenm, this->filename.data(), endIdx);
    //组装新文件名
    strcat_s(filenm, sizeof(filenm)-strlen(filenm), "_bak");
    strcat_s(filenm, sizeof(filenm)-strlen(filenm), ".log");
    //删除旧的备份
    if(!DeleteFile(filenm))
    {
        SystemLog::getInstance()->LogPrint(LWARNING, "PackLog::renameLogFile: delete log back file failed, [%s], [error_code :%d]",
                            filenm, GetLastError() );
    }
    //重命名文件
    if(0 != rename(filename.data(), filenm))
    {
        SystemLog::getInstance()->LogPrint(LERROR, "PackLog::renameLogFile: rename log file failed, oldNm[%s], newNm[%s], [error_code :%d]",
                            this->filename.data(), filenm, GetLastError());
    }
        
    return;
}


/**
 * @Funcname:  void PackLog::openLogFile
 * @brief        :  打开日志文件
 * @para1      : filename, 文件名
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月6日 11:37:15
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
    
    pCurFile = _fsopen(this->filename.data(), "w", _SH_DENYWR);       //打开一个文件，以只读的方式共享
    if(NULL == pCurFile)
    {
        pCurFile  = _fsopen(this->filename.data(), "w", _SH_DENYWR);  //重试一次
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
 * @brief        :  将消息写入日志文件
 * @para1      : pack:需要写入日志的消息
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月6日 13:48:01
 *   
**/
void PackLog::writePackToFile(PackInfo info, const Packet &pack)
{
    /*Modifed by tangxp for BUG NO. 0002432 on 2008.1.4 [begin]*/
    /*  Modifed brief: 写日志时增加互斥*/
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
    //输出消息包头及接收时间等信息
    fprintf_s(pCurFile, "\n\n%s", info.strtime);
    fprintf_s(pCurFile, "  %s  srcaid=%d,desaid=%d,srcSaid=%d,desSaid=%d,serial=%d,size=%d",
                strpackAct[info.pack_act], tmpPack.getSrc_aid(), tmpPack.getDes_aid(), tmpPack.getSrc_said(),
                tmpPack.getDes_said(), tmpPack.getSerialNum(), tmpPack.getSize());

    //输出消息包内容
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
        //每次从消息包内容里拷贝bytecount 个字节出来                        
        memcpy_s(buf, sizeof(buf), &tmpPack.getData()[idx], cpylen);
            
        
        idx += cpylen;
        //将buf中的内容输出到日志
        fprintf_s(pCurFile, "\n");
        for(int i = 0; i<cpylen; i++)
        {
             //十六进制字节形式
            fprintf_s(pCurFile, "%-3x", buf[i]);  
             chCountInLine++;
             
        }

        //补齐16个字节的位置
        if(chCountInLine < 16)
        {
            for(int k=16-chCountInLine; k>0; k--)
            {
                fprintf_s(pCurFile, "%-3c", ' ');
            }
        }
        //字符串形式
        fprintf_s(pCurFile, "\t\t");
        for(int i = 0; i<cpylen; i++)
        {
            if(isprint(buf[i]) != 0)
            {
                //是可打印字符
                fprintf_s(pCurFile, "%c", buf[i]);
            }
            else
            {
                //不可打印字符，用. 代替
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
 * @brief        :  将消息写入日志文件
 * @para1      : pack:需要写入日志的消息
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月6日 13:48:01
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
    //输出消息包头及接收时间等信息
    fprintf_s(pCurFile, "\n\n%s\t%s\tSize=%d", info.strtime, strpackAct[info.pack_act], len);
    /*fprintf_s(pCurFile, "  %s  srcaid=%d,desaid=%d,srcSaid=%d,desSaid=%d,serial=%d,size=%d",
                strpackAct[info.pack_act], tmpPack.getSrc_aid(), tmpPack.getDes_aid(), tmpPack.getSrc_said(),
                tmpPack.getDes_said(), tmpPack.getSerialNum(), tmpPack.getSize());*/

    //输出消息包内容
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
        //每次从消息包内容里拷贝bytecount 个字节出来                        
        memcpy_s(buf, sizeof(buf), &pBuf[idx], cpylen);
            
        
        idx += cpylen;
        //将buf中的内容输出到日志
        fprintf_s(pCurFile, "\n");
        for(int i = 0; i<cpylen; i++)
        {
             //十六进制字节形式
            fprintf_s(pCurFile, "%-3x", buf[i]);  
             chCountInLine++;
             
        }

        //补齐16个字节的位置
        if(chCountInLine < 16)
        {
            for(int k=16-chCountInLine; k>0; k--)
            {
                fprintf_s(pCurFile, "%-3c", ' ');
            }
        }
        //字符串形式
        fprintf_s(pCurFile, "\t\t");
        for(int i = 0; i<cpylen; i++)
        {
            if(isprint(buf[i]) != 0)
            {
                //是可打印字符
                fprintf_s(pCurFile, "%c", buf[i]);
            }
            else
            {
                //不可打印字符，用. 代替
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
 * @brief        :  删除日志文件
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月14日 10:41:40
 *   
**/
void PackLog::deleteLogFile()
{
    string logFileBack;
    logFileBack = filename;
    
    //关闭日志文件
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
/**********************************公共函数****************************/

/**
 * @Funcname:  truncatPathFromFilename
 * @brief        :  将带有路径的文件名截取掉路径
 * @para1      : filename, 文件名
 * @return      : 返回不带路径的文件名, 以"\0"结束的字符串
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月1日 9:11:54
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
 * @brief        :  在字符串中替换指定的字符
 * @para1      : char *str,   源字符串
 * @para2      : char oldch, 需要替换的字符
 * @para3      : char newch 替换后的字符
 * @return      : 0:表示成功,-1失败
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2006年12月4日 14:24:05
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


