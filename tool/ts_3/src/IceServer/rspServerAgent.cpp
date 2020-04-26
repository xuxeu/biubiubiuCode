/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: rspServerAgent.cpp,v $
  * Revision 1.7  2008/03/19 10:25:11  guojc
  * Bug 编号  	0002430
  * Bug 名称 	ts启动后造成Windows任务栏凝固，无法响应快捷键
  *
  * 编码：唐兴培
  * 检视：卿孝海,郭建川
  *
  * Revision 1.2  2008/03/19 09:03:00  guojc
  * Bug 编号  	0002430
  * Bug 名称 	ts启动后造成Windows任务栏凝固，无法响应快捷键
  *
  * 编码：唐兴培
  * 检视：卿孝海,郭建川
  *
  * Revision 1.1  2008/02/04 05:33:49  zhangxc
  * 设计报告名称：《ACoreOS CVS代码仓库管理》
  * 编写人员：张晓超
  * 检视人员：张宇旻
  * 其它：从现有最新ACoreOS仓库中取出最新的源代码，整理后提交到新的仓库目录结构中管理。
  * 第一次提交src模块。
  *
  * Revision 1.6  2007/06/21 11:37:29  guojc
  * 1. 修改了IS对simihost不删除硬件断点的处理
  * 2. 统一管理IS调试日志开关
  *
  * Revision 1.5  2007/04/20 07:10:50  guojc
  * 增加了对ICE DCC功能的支持
  * 增加了对目标板复位功能的支持
  * 增加了对内存宽度可配置的支持
  * 增加了对华邦模拟器调试兼容的支持
  *
  * Revision 1.4  2007/04/16 02:20:53  guojc
  * 修正ICE Server在协助semihosting上面出现的一些问题
  *
  * Revision 1.3  2007/04/02 02:56:53  guojc
  * 提交最新的IS代码，支持脱离于IDE使用，支持配置文件解析
  *
  * Revision 1.2  2007/02/25 06:05:14  guojc
  * 修正了ICE Server和TRA Server有垃圾日志文件的问题
  * 修正了TSAPI与测试代码不同步的问题
  *
  * Revision 1.1  2006/11/13 03:40:55  guojc
  * 提交ts3.1工程源码,在VC8下编译
  *
  * Revision 1.3  2006/07/03 07:18:20  tanjw
  * 1.修正iceserver与gdb建立连接后执行目标机上电初始化过程中收到gdb命令返回出错的bug
  * 2.修正同时启动多个iceserver日志文件共享冲突的bug
  * 3.修正ts发送线程日志文件记录点.由发送后记录改为发送前记录
  *
  * Revision 1.2  2006/06/06 12:08:11  pengh
  * no message
  *
  * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
  * no message
  *
  * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
  * TS代码整理
  *
  * Revision 1.4  2006/04/06 01:14:08  guojc
  * 修正了打开关闭的相关bug,确保动态库能够正确退出
  *
  * Revision 1.3  2006/03/29 08:38:14  guojc
  * 修正了关于软件断点指令插入检查
  *
  * Revision 1.1  2006/03/04 09:25:11  guojc
  * 首次提交ICE Server动态库的代码
  *
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * 创建文件;定义 DLL 应用程序的入口点
  */

/**
* @file     rspServerAgent.h
* @brief     
*     <li>功能： 实现TS标准动态库的基本接口</li>
* @author     郭建川
* @date     2006-02-20
* <p>部门：系统部 
*/

/************************************引用部分******************************************/
#include <windows.h>
#include <QMap>
#include "rspTcpServer.h"
#include "sysTypes.h"
#include "common.h"
/**********************************前向声明部分******************************************/
using namespace std;
/************************************定义部分******************************************/    
T_MODULE QMap<T_WORD, RSP_TcpServer*> vtRSP_Id2Server;  //ID到服务器的映射表
T_MODULE QMutex vtRSP_MapMutex;                            //对映射表区进行锁定
T_MODULE T_WORD count = 0;
/************************************实现部分******************************************/    



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{

    return TRUE;
}

static unsigned int int2hex(char *ptr, int size,int intValue)
{
    memset(ptr,'\0',size);    //设置大小,最大缓冲区的大小
    sprintf(ptr,"%x",intValue);
    return strlen(ptr);
}


/**
* @brief 
*    功能: 动态库向TS注册的回调函数, SA DLL的TX线程会调用此方法对收到数据进行处理
* @param[in] id sa的aid号
* @param[in] des_said 目标said号
* @param[in] src_aid  ca的aid号
* @param[in] src_said ca的said号
* @param pBuf 发送的缓冲区
* @param[in] size 缓冲区大小
* @return 0表示成功
*/
FUNC_INT putpkt(T_HWORD vwServerId,T_HWORD vhwDesSaid,T_HWORD vhwSrcAid,T_HWORD vwSrcSaid,T_CHAR* cpBuf,T_WORD vwSize)
{
    //检查参数合法性
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    //从表中找到对应的服务器对象，进行了加锁保护
    RSP_TcpServer *server = NULL;
    vtRSP_MapMutex.lock();
    server = vtRSP_Id2Server.value(vwServerId);
    vtRSP_MapMutex.unlock();
    
    if(NULL == server)
    {
        return FAIL;
    }

    int ret = server->Putpkt(vhwDesSaid, vhwSrcAid, vwSrcSaid, cpBuf, vwSize);

    return ret;
}

/**
* @brief 
*     功能: 动态库向TS注册的回调函数, SA DLL的RX线程会调用此方法接收动态库的数据进行转发
* @param[in] id sa的源said号
* @param[out] des_aid 目标aid号
* @param[out] des_said 目标said号
* @param[out] src_said ca的said号
* @param pBuf 发送的缓冲区
* @param[in] size 缓冲区大小
* @return 实际得到的数据的大小
*/
FUNC_INT getpkt(T_HWORD vwServerId,T_HWORD *vwpDesAid,T_HWORD *vwpDesSaid,T_HWORD *vwpSrcSaid,T_CHAR* cpBuf,T_WORD vwSize)
{
    //检查参数合法性
    if(NULL == cpBuf)
    {
        return FAIL;
    }

    //从表中找到对应的服务器对象，进行了加锁保护
    RSP_TcpServer *server = NULL;
    vtRSP_MapMutex.lock();
    server = vtRSP_Id2Server.value(vwServerId);
    vtRSP_MapMutex.unlock();
    
    if(NULL == server)
    {
        return FAIL;
    }

    int ret = server->Getpkt(vwpDesAid, vwpDesSaid, vwpSrcSaid, cpBuf, vwSize);

    return ret;
}

/**
* @brief 
*    功能: 关闭动作,便于动态库内部释放占用资源
* @param[in] id sa的aid号
* @return 0表示成功
*/
FUNC_INT close(T_HWORD hwId)
{
    //首先检查该ID对应的服务器对象是否存在
    vtRSP_MapMutex.lock();

    T_BOOL ret = vtRSP_Id2Server.contains(hwId);

    //如果不存在，直接返回成功
    if(ret == FALSE)
    {
        vtRSP_MapMutex.unlock();
        return TRUE;
    }

    RSP_TcpServer *tcpServer = vtRSP_Id2Server.take(hwId);
    

    tcpServer->Close();
    //确保动态库关闭,接收线程退出
    {
        int killCount = 0;
        QMutex mutex;                //对命令缓冲区进行锁定

        QWaitCondition waitCondition;  //条件变量，用来判断关闭方法是否可以正常返回

        while (tcpServer->isRunning() && killCount < 3) {
            killCount++;
            QMutexLocker lock(&mutex);
            waitCondition.wait(&mutex, 1);
        }
    }

    delete tcpServer;

    count--;
    if(count == 0)
    {
        /*SystemLogger* logger = SystemLogger::getInstance();
        if(NULL != logger)
        {    
            logger->close();
            delete logger;
            logger = NULL;
        }*/
    }
    vtRSP_MapMutex.unlock();


    return TRUE;
}

/**
* @brief 
*    功能: 打开动作,根据解析pBuf创建RSP服务器，便于动态库内部做初始化工作
* @param[in] hwId TS为其创建的ServerAgent的ID号
* @param[in] cpBuf 传递的参数缓冲区
* @return 0表示成功,
*/
FUNC_INT  open(T_HWORD hwId,T_CHAR* cpBuf, TCHAR* cpOutBuf)
{
    //首先检查是否已经有ID注册
    vtRSP_MapMutex.lock();
    if(count == 0)
    {
        /*SystemLogger* logger = SystemLogger::getInstance();
        if(NULL != logger)
        {
            logger->open();
        }
        else
        {
            return FAIL;
        }*/
    }
    count++;

    T_BOOL ret = vtRSP_Id2Server.contains(hwId);
    vtRSP_MapMutex.unlock();
    
    if(ret == TRUE)
    {
        return FALSE;
    }
    char tmp[100] = "\0";
    sprintf(tmp, "%s%d", "sysIceServer",  hwId);
    //创建系统日志
    SystemLog::getInstance()->openLogFile(string(tmp));
    SystemLog::getInstance()->setLogLevel(IS_CURSYS_LOG_LEVEL);
    //根据ID创建服务器对象
    RSP_TcpServer *tcpServer = new RSP_TcpServer();
    int result = tcpServer->Open(hwId, cpBuf);
    if(result == FAIL)
    {
        //释放创建的资源
        tcpServer->forceExit();
        tcpServer->Close();
        delete tcpServer;
        return FAIL;
    }
    
    //创建成功，加入到列表中去
    vtRSP_MapMutex.lock();
    vtRSP_Id2Server.insert(hwId, tcpServer);
    vtRSP_MapMutex.unlock();

    int2hex(cpOutBuf, 4, result);
    
    return result;
}

