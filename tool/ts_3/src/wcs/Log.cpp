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
#include "QMutex.h"
//#include "thread.h"
#include "log.h"
/************************前向声明部分***********************/



const char SYSTEMLOGFILE[] = "IceServerSys"; 
const char LEVELINFO[5][10] = {"LDEBUG","LINFO", "LWARNING", "LERROR", "LFATAL"};
/************************定义部分*****************************/

SystemLog* SystemLog::plog = NULL;
QMutex* SystemLog::pInitmtx = new QMutex();
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
SystemLog::SystemLog(unsigned int line, unsigned short size)
{
    
    
    bufsize = (size>4096)?size:4096;
    maxLogFileLine = (line>0)?line:SYS_LOG_COUNT;   
    curLogFileLine = 0;
    curLogLevel = SLDEBUG;          //默认输出INFO以上的级别
    fp = NULL;
    buf = (char*)malloc(bufsize);
    if(buf == NULL)
    {
        buf = (char*)malloc(bufsize);
    }
    memset(buf, 0, bufsize);
    //先创建系统日志目录
    char temp[MAX_PATH] = "\0";
    GetCurrentDirectoryA(sizeof(temp), temp);
    logPath = string(temp) + string("\\log\\systemLog\\");
    if(!CreateDirectoryA(logPath.data(), NULL))
    {   
        CreateDirectoryA((string(temp)+"\\log").data(), NULL);
        CreateDirectoryA(logPath.data(), NULL);
    }
    //openLogFile();
   
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
    fclose(fp);
    fp = NULL;
    //构造system+时间.log文件名
    char filename[MAX_PATH] = "\0";
    int idx1 = 0, idx2 = 0;
    idx1 = this->filename.find_last_of("\\");
    idx2 = this->filename.find_last_of(".log");
    
    strcat_s(filename, sizeof(filename), this->filename.substr(idx1, idx2 - idx1).data());
    strcat_s(filename, sizeof(filename)-strlen(filename), "_bak.log");
    //删除旧文件
    DeleteFileA(filename);
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
    if(buf == NULL)
    {
        free(buf);
        buf = NULL;
    }
    if(fp != NULL)
    {
        closeLogFile();
    }
    if(plog != NULL)
        delete plog;
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
SystemLog*SystemLog::getInstance()
{
    if(NULL == plog)
    {
        QMutexLocker locker(pInitmtx);
        if(NULL == plog)
        {
            plog = new SystemLog();
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
    //return;
    
    char time[11] = "\0";
    char date[11] = "\0";
    char tmpbuf[4096] = "\0";

    if(level < curLogLevel)
    {
        return;           //低于允许输出以上级别的就不再输出
    }
    _strtime_s(time, sizeof(time));
    _strdate_s(date, sizeof(date));
    
    QMutexLocker locker(&logmtx);

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
    
    if(fp)
    {
        
        if(curLogFileLine > maxLogFileLine)
        {
            closeLogFile();
            int idx = 0;
            idx = filename.find_last_of("\\");
            openLogFile(filename.substr(idx));
            curLogFileLine = 0;
        }
        fprintf_s(fp, buf);
        fflush(fp);
        curLogFileLine++;
    }    
    return;
}


#if 1
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
void PackLog::write(PackAct act, char *pbuf, int len)
{
    //return;

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
    //如果日志记录达到最大数量则重新打开一个文件
    if(curLogCount >= maxLogCount)
    {
        renameLogFile();
        open(this->filename.data());
    }
    
    //将消息写入日志文件
    mtxLog.lock();
    writePackToFile(info, pbuf, len);
    mtxLog.unlock();
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
    //先关闭文件
    if(pCurFile == NULL)
    {
        return;
    }
    fclose(pCurFile);
    pCurFile= NULL;
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
    DeleteFileA(filenm);
    //重命名文件
    rename(filename.data(), filenm);
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
    char path[MAX_PATH] = "\0";
    FILE *ptmp = NULL;
    char curpath[MAX_PATH] = "\0";
    if(NULL == filename)
    {
        sysLoger(SLWARNING, "PackLog::open: Parameter filename is Null pointer !");
        return;
    }
    if(this->filename.empty())
    {        
        this->filename = logPath + string(filename);
    }
    ptmp = _fsopen(this->filename.data(), "w", _SH_DENYWR);       //打开一个文件，以只读的方式共享
    if(NULL == ptmp)
    {
        ptmp  = _fsopen(this->filename.data(), "w", _SH_DENYWR);  //重试一次
        if(NULL == ptmp)
        {
            sysLoger(SLERROR, "PackLog:: open log file %s  failure! FILE:%s, LINE:%d", 
                                        this->filename.data(), __MYFILE__, __LINE__);   
            pCurFile = NULL;
            return;
        }
    }
    curLogCount = 0;
    pCurFile = ptmp;
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
    char strpackAct[3][10] = {"Sended!","Received!", "NULL"};
    unsigned char buf[20] = " ";
    //Packet tmpPack = const_cast<Packet &>(pack);
    if(NULL == pCurFile)
    {
        sysLoger(SLERROR, "PackLog: Invalid log file point encounted! FILE:%s, LINE:%d",
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
#endif
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
    close();
    if(logFileBack.find(".log") == logFileBack.length() - strlen(".log"))
    {
        logFileBack = filename.substr(0, filename.length() - strlen(".log"));
        logFileBack += "_bak.log";
    }

    DeleteFileA(filename.data());
    DeleteFileA(logFileBack.data());
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


