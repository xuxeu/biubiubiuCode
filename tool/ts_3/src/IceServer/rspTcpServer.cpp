/************************************************************************
*                北京科银京成技术有限公司 版权所有
*    Copyright (C) 2000-2006 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * $Log: rspTcpServer.cpp,v $
 * Revision 1.12  2008/03/19 10:25:11  guojc
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
 * Revision 1.11  2007/06/21 11:37:28  guojc
 * 1. 修改了IS对simihost不删除硬件断点的处理
 * 2. 统一管理IS调试日志开关
 *
 * Revision 1.10  2007/06/07 09:59:47  guojc
 * IS增加对cache等原子操作的支持
 *
 * Revision 1.9  2007/05/29 03:27:41  guojc
 * 修正了如果调试过程中有断点，可能导致读寄存器失败的问题
 *
 * Revision 1.8  2007/04/30 06:47:35  guojc
 * 修正了release版本assert不起作用的问题，通过重新定义assert解决
 *
 * Revision 1.7  2007/04/30 01:38:30  guojc
 * 对日志文件进行了修正
 * 对目标机复位判断进行了修正
 *
 * Revision 1.6  2007/04/20 07:10:50  guojc
 * 增加了对ICE DCC功能的支持
 * 增加了对目标板复位功能的支持
 * 增加了对内存宽度可配置的支持
 * 增加了对华邦模拟器调试兼容的支持
 *
 * Revision 1.5  2007/04/16 02:20:53  guojc
 * 修正ICE Server在协助semihosting上面出现的一些问题
 *
 * Revision 1.4  2007/04/06 03:48:15  guojc
 * 修正了主动上报的动态库在IDE重启后无法与目标机UDP服务关联的问题
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
 * Revision 1.17  2006/10/31 03:54:24  zhangym
 * 提交ICE Server内存优化算法，默认关闭优化算法
 *
 * Revision 1.16  2006/10/23 01:59:48  zhangym
 * 关闭ICE Server的内存优化算法
 *
 * Revision 1.15  2006/09/30 01:52:53  tanjw
 * P命令修改cpsr寄存器时,增加处理器模式检查
 *
 * Revision 1.14  2006/09/28 09:01:46  tanjw
 * no message
 *
 * Revision 1.13  2006/07/17 09:30:01  tanjw
 * 1.增加arm,x86 rsp日志打开和关闭功能
 * 2.增加arm,x86 rsp内存读取优化算法打开和关闭功能
 *
 * Revision 1.11  2006/07/06 02:12:24  tanjw
 * 修改gdb连接is后,is执行上电初始化操作等待时间
 *
 * Revision 1.9  2006/06/30 01:39:08  tanjw
 * 与gdb建立连接后自动初始化目标机
 *
 * Revision 1.8  2006/06/26 09:09:30  tanjw
 * 1.修改arm和x86内存读取出错bug
 * 2.iceserver去掉wsocket
 *
 * Revision 1.3  2006/06/06 12:08:11  pengh
 * no message
 *
 * Revision 1.2  2006/05/08 03:57:21  guojc
 * 修正了初始化协处理器的问题
 *
 * Revision 1.1.1.1  2006/04/19 06:29:13  guojc
 * no message
 *
 * Revision 1.7  2006/04/18 07:05:43  guojc
 * 修正了读大数据量内存会失败的问题
 *
 * Revision 1.6  2006/04/18 06:24:27  guojc
 * 加入了对目标机状态的更新
 *
 * Revision 1.5  2006/04/17 05:57:19  guojc
 * 修正了编译时出现的警告信息
 *
 * Revision 1.4  2006/04/14 02:02:28  guojc
 * 增加了对CP15协处理器寄存器修改的支持
 *
 * Revision 1.3  2006/04/12 09:57:53  guojc
 * 取消断点检查断言
 *
 * Revision 1.2  2006/04/06 12:36:02  guojc
 * 修正了读大块内存失败的问题
 *
 * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
 * TS代码整理
 *
 * Revision 1.5  2006/04/06 01:14:07  guojc
 * 修正了打开关闭的相关bug,确保动态库能够正确退出
 *
 * Revision 1.4  2006/03/30 01:29:56  guojc
 * 在插入断点指令之前清空了本地缓存，避免本地缓存有断点指令内容
 *
 * Revision 1.3  2006/03/29 08:38:13  guojc
 * 修正了关于软件断点指令插入检查
 *
 * Revision 1.1  2006/03/04 09:25:10  guojc
 * 首次提交ICE Server动态库的代码
 *
 * Revision 1.0  2006/01/12 01:10:14  guojc
 * 创建文件；实现RSP_TcpServer类的基本接口。
 */

/**
 * @file     rspTcpServer.cpp
 * @brief
 *    <li>功能：实现TCP通信方式的基于RSP协议的服务器 </li>
 * @author     郭建川
 * @date     2006-02-20
 * <p>部门：系统部
 */

/**************************** 引用部分 *********************************/
#include <cassert>
#include <iostream>            /// 使用IO输入输出流
#include <utility>
#include "rspTcpServer.h"
#include "wsocket.h"
//#include "util.h"
#include "armConst.h"
#include "iceServerComm.h"
#include "common.h"
#include "XmlHandler.h"
#include "util.h"
/*************************** 前向声明部分 ******************************/

#define PUTPKT_RETURN(flag)     {\
                                                            m_quitMutex.lock();\
                                                            m_txExitReady = TRUE;\
                                                            m_quitMutex.unlock();\
                                                            return flag;\
                                                         }

T_MODULE T_WORD Int2Hex(T_CHAR *cpBuf, T_WORD wNum, T_WORD wWidth);



int count = 0;
/**************************** 定义部分 *********************************/
T_MODULE T_CONST T_WORD PACKET_DATA_SIZE = 8192;
T_MODULE T_CONST T_WORD waCurUsrRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //用户模式下的寄存器映射表
T_MODULE T_CONST T_WORD waCurFiqRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 20, 21, 22, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //FIQ模式下的寄存器映射表
T_MODULE T_CONST T_WORD waCurIrqRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 23, 24, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //IRQ模式下的寄存器映射表
T_MODULE T_CONST T_WORD waCurSvcRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 27, 28, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //SVC模式下的寄存器映射表
T_MODULE T_CONST T_WORD waCurAbtRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 25, 26, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //ABT模式下的寄存器映射表
T_MODULE T_CONST T_WORD waCurUndRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 29, 30, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //UND模式下的寄存器映射表
T_MODULE T_CONST T_WORD waCurSysRegs[26] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 31};  //SYS模式下的寄存器映射表

T_CONST T_CHAR vcaHexchars[] = "0123456789abcdef";
/**************************** 实现部分 *********************************/

/**
 * @brief
 *    构造函数
 */
RSP_TcpServer::RSP_TcpServer()
{
    //初始化插入软件断点重试次数
    m_retryTimes = 5;

    //启动错误日志记录
    //m_errLogger = SystemLogger::getInstance();

    //默认打开内存优化算法
    SetMop(FALSE);
    //默认关闭数据包日志功能
#ifdef IS_PACK_LOG
    SetLog(TRUE);
#endif
    m_sClient = NULL;

    m_waitForIce = FALSE;
    m_hardBp = FALSE;
    m_iceEndian = FALSE;
    //暂时不实现
    m_lambdaIce.SetServer(this);

    m_rxExitReady = FALSE;
    m_localExitReady = FALSE;
    m_txExitReady = TRUE;

    m_validDeleWp = FALSE;
    m_validForWp = FALSE;
    memset(m_ICEName, 0, sizeof(m_ICEName));
    SetTargetStatus(RSP_ArmConst::CORE_POWER_OFF);
    
    m_SimiRegisterFlag = FALSE;
    m_SimiPCAddr = 0;
    m_NeedReplyS05 = TRUE;
    m_targetType = LAMBDA_JTAG_ICE;
}

/**
 * @brief
 *    析构函数
 */
RSP_TcpServer::~RSP_TcpServer()
{
    //释放资源
    //1.内存资源
    RefreshMemory();

    //2.初始化内存资源
    while(!m_memCfg.empty())
    {
        pair<T_WORD, T_WORD> *mem = m_memCfg.takeFirst();
        if(NULL != mem)
        {
            delete mem;
        }
    }

    //3. 数据包缓冲队列
    while(!m_packetQueue.empty())
    {
        RSP_Packet *pack = m_packetQueue.takeFirst();
        if(NULL != pack)
        {
            delete pack;
        }
    }

    //4.
    if(m_sClient != NULL){
        delete m_sClient;
    }
    m_sClient = NULL;

#ifdef IS_PACK_LOG
    m_logger.deleteLogFile();
#endif
}

/**
* @brief
*    打开操作，进行协议解析，得到需要的参数信息
* @param[in] id 对应服务器所绑定的AID号
* @param[in] cpBuf 参数缓冲指针,服务器可以从中解析到所需的参数
* @return 操作成功返回监听的端口号，失败返回FAIL
*/
int RSP_TcpServer::Open(T_WORD wId, T_CHAR *cpBuf)
{
    /*buf中的格式
    *     ICEID, 是否支持私有扩展, 配置文件名 
    */
    T_CHAR *ptr = cpBuf;
    //iceaid
    T_CHAR *pdest = strstr(ptr,CONTROL_FLAG);        //是否有分割符
    if(NULL == pdest)
    {
        sysLoger(LWARNING, "RSP_TcpServer::Open: Lost the ICEID part!");
        return FAIL;
    }
    Hex2int(&ptr, &m_iceId);
    //iceName
    ptr = pdest + CONTROL_FLAG_LEN;
    pdest = strstr(ptr,CONTROL_FLAG);        //是否有分割符
    if(NULL == pdest)
    {
        sysLoger(LWARNING, "RSP_TcpServer::Open: Lost the ICE Name part!");
        return FAIL;
    }
    memcpy(m_ICEName, ptr, pdest - ptr);
    //isSupportPrivateProtocol
    ptr = pdest + CONTROL_FLAG_LEN;
    pdest = strstr(ptr,CONTROL_FLAG);        //是否有分割符
    if(NULL == pdest)
    {
        sysLoger(LWARNING, "RSP_TcpServer::Open: Lost the isSupportPrivateProtocol part!");
        return FAIL;
    }
    Hex2int(&ptr, (T_WORD*)(&m_isSupportPrivateProtocol));
    m_isSupportPrivateProtocol = m_isSupportPrivateProtocol?TRUE:FALSE;
    //configfile
    ptr = pdest + CONTROL_FLAG_LEN;
    pdest = strstr(ptr,CONTROL_FLAG);        //是否有分割符
    if(NULL == pdest)
    {
        /*最后缺少分隔符，这里也认为有数据*/
        if(strlen(ptr) > 0)
        {
            pdest = ptr + strlen(ptr);
        }
        else
        {
            sysLoger(LWARNING, "RSP_TcpServer::Open: Lost the config file part!");
            return FAIL;
        }
    }
    char configfile[MAX_PATH] = "\0";
    if(pdest - ptr > MAX_PATH)
    {
        sysLoger(LWARNING, "RSP_TcpServer::Open: Invalid config file name!");
        return FAIL;
    }
    memcpy(configfile, ptr, (T_UWORD)(pdest - ptr));
    m_CfgFile = QString::fromLocal8Bit(configfile);

    //进行配置文件的解析
    m_CfgFile = QString::fromLocal8Bit(m_CfgFile.toAscii().data());
    //根据是否包含有路径符号来确定是否是本地配置文件还是远程
    if(!m_CfgFile.contains("\\"))
    {
        //配置文件保存在TS的LoadFile目录下
        QString cfgFileName;
        char path[MAX_PATH] = "\0";
        GetCurrentDirectory(MAX_PATH, path);
        cfgFileName = QString::fromLocal8Bit(path);
        cfgFileName += QString(IS_CFGFILE) + m_CfgFile;
        m_CfgFile = cfgFileName;
    }
    sysLoger(LDEBUG, "RSP_TcpServer::Open: Start to prase config file [%s].", m_CfgFile.toAscii().data());
    if(!parseConfig())
    {
        sysLoger(LWARNING, "RSP_TcpServer::Open: Parse ice config file failure!");
        return FAIL;
    }

    m_lambdaIce.SetServer(this);

    //根据端口号启动服务器，检查端口号是否被占用
    if (INVALID_SOCKET == m_serverSocket.m_sock)
    {
            if (!m_serverSocket.createServerSocket(0))
            {
                return FAIL;
            }
    }
    else
    {
        return FAIL;
    }

    m_port = m_serverSocket.GetLocalPort();
    //QString str = QString("监听端口号:%1").arg(m_port);
    //m_errLogger->write(1, QString("执行命令"), str);

    
    //启动线程，监听服务器
    this->start();  //自启动
    return m_port;
}

T_BOOL RSP_TcpServer::GetIceEndian()
{
    return m_iceEndian;
}

T_BOOL RSP_TcpServer::GetTargetEndian()
{
    return m_targetEndian;
}

/**
* @brief
*    关闭操作，关闭服务器
*/
T_VOID RSP_TcpServer::Close()
{
    m_sktMutex.lock();
    //关闭线程
    CloseThread();

    //唤醒RX线程，让其正常退出
    WakeUp();

    //如果子SOCKET没有关闭,关闭之
    //---------------------------------
    //2006.06.19 加锁，对SOCKET进行保护，避免关闭瞬间SOCKET为空指针造成异常
    m_clientMutex.lock();
    if(m_sClient != NULL)
    {
        m_sClient->Close();
    }
    m_clientMutex.unlock();
    m_sktMutex.unlock();

    //关闭服务器
    m_serverSocket.Close();

    //等待线程退出
    m_quitMutex.lock();
    while (m_rxExitReady == FALSE || m_localExitReady == FALSE || m_txExitReady == FALSE)
    {
        m_quitReady.wait(&m_quitMutex, 1000);    //等待5秒的超时
    }
    m_quitMutex.unlock();


    #ifdef IS_PACK_LOG
    m_logger.close();
    #endif
}

T_VOID RSP_TcpServer::HandleIDECmd(T_HWORD hwSrcAid,T_HWORD hwSrcSaid, T_CHAR* cpBuf,T_WORD wSize)
{
#if 0
    //检查参数输入合法性
    if(wSize < 4 || cpBuf[0] != 'r' || cpBuf[1] != 's' || cpBuf[2] != 'p' || cpBuf[3] != '-')
    {
        SendIDEResult("error:input unkown cmd!", strlen("error:input unkown cmd!"), hwSrcAid, hwSrcSaid);
        return;
    }
#endif
    //记录消息来源
    m_MtxMsgSrc.lock();
    m_CurCmdSource = TSAPI_DATA;
    m_CurSrcAid = hwSrcAid;
    m_CurSrcSaid = hwSrcSaid;
    m_MtxMsgSrc.unlock();
    
    switch(cpBuf[0])
    {
#if 0
    case 'l':
        {
            //日志选项设置，默认关闭
            if(wSize < 6 || cpBuf[0] != 'l' || cpBuf[1] != 'o' || cpBuf[2] != 'g' || cpBuf[3] != '-')
            {
                SendIDEResult("错误：未知的日志操作命令！", strlen("错误：未知的日志操作命令！"), hwSrcAid, hwSrcSaid);
                break;
            }
            
            if(cpBuf[5] == 'n')
            {
                sysLoger(LINFO, "HandleIDECmd:Open the log success!");
                SendIDEResult("日志打开成功！", strlen("日志打开成功！"), hwSrcAid, hwSrcSaid);
            }
            else if(cpBuf[5] == 'f')
            {
                sysLoger(LINFO, "HandleIDECmd:Close the log success!");
                SendIDEResult("日志关闭成功！", strlen("日志关闭成功！"), hwSrcAid, hwSrcSaid);
            }
            else
            {
                sysLoger(LINFO, "HandleIDECmd:Unknow command packet!");
                SendIDEResult("错误：未知的日志操作命令！", strlen("错误：未知的日志操作命令！"), hwSrcAid, hwSrcSaid);
            }
            break;
        }
    case 'm':
        {
            //内存优化选项算法设置，默认打开
            if(wSize < 6 || cpBuf[0] != 'm' || cpBuf[1] != 'o' || cpBuf[2] != 'a' || cpBuf[3] != '-')
            {
                SendIDEResult("错误：未知的内存优化设置命令！", strlen("错误：未知的内存优化设置命令！"), hwSrcAid, hwSrcSaid);
                break;
            }
            
            if(cpBuf[5] == 'n')
            {
                //打开内存优化算法
                int ret = SetMop(TRUE);
                if(SUCC == ret)
                {
                    SendIDEResult("内存优化算法打开成功！", strlen("内存优化算法打开成功！"), hwSrcAid, hwSrcSaid);
                }
                else
                {
                    SendIDEResult("内存优化算法打开失败！", strlen("内存优化算法打开失败！"), hwSrcAid, hwSrcSaid);
                }

            }
            else if(cpBuf[5] == 'f')
            {
                //关闭内存优化算法
                int ret = SetMop(FALSE);
                if(SUCC == ret)
                {
                    SendIDEResult("内存优化算法关闭成功！", strlen("内存优化算法关闭成功！"), hwSrcAid, hwSrcSaid);
                }
                else
                {
                    SendIDEResult("内存优化算法关闭失败！", strlen("内存优化算法关闭失败！"), hwSrcAid, hwSrcSaid);
                }
            }
            else
            {
                SendIDEResult("错误：未知的内存优化设置命令！", strlen("错误：未知的内存优化设置命令！"), hwSrcAid, hwSrcSaid);
            }

            break;
        }
#endif

    case 'i':
        {
            /*查询目标机的CPU IDCODE*/
            sysLoger(LINFO, "HandleIDECmd:Recieved Query target CPU ID Code command!");
            PushBackCmd(RSP_ArmConst::CMD_QUERY_TARGET_CPUID);
            PushBackCmd(RSP_ArmConst::CMD_ANALY_QUER_TARGET_CPUID);
            //装载命令参数
            QList<T_VOID*> *queryPara = new QList<T_VOID*>();
            QList<T_VOID*> *ReplyDest = new QList<T_VOID*>();
            T_HWORD *pSrcAid = new T_HWORD(hwSrcAid);
            T_HWORD *pSrcSAid = new T_HWORD(hwSrcSaid);
            ReplyDest->push_back(pSrcAid);
            ReplyDest->push_back(pSrcSAid);                
            m_atomParas.push_back(queryPara);
            m_atomParas.push_back(ReplyDest);
        }
        break;

    case 'j':
        {
            /*查询所有寄存器的值*/
            char buf[PACKET_DATA_SIZE] = "\0";
            char *pBuf = buf;
            sysLoger(LINFO, "HandleIDECmd:Recieved Query all Registers command!");
            for(int i = 0; i < RSP_ArmConst::ARM_REG_NUM; i++)
            {
                pBuf += int2hex(pBuf, 10, m_currentRegs[i]);
                memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
                pBuf += CONTROL_FLAG_LEN;
            }
            //发送
            SendIDEResult(buf, (T_WORD)(pBuf - buf), hwSrcAid, hwSrcSaid);
        }
        break;

    case 'J':
        {
            /*设置所有寄存器的值*/
            QString qsRegValues = QString::fromLocal8Bit(&cpBuf[1], wSize-1);
            QString qsSingleValue;
            T_WORD value = 0, idx = 0, controlCharCount = 0; 
            bool ok = false;
            sysLoger(LINFO, "HandleIDECmd:Recieved Set all Registers value command!");
            //判断数据包是否合法(即带有36个寄存器的值)
            for(int i = 0 ; i < qsRegValues.length(); i++)
            {
                if((idx = qsRegValues.indexOf(CONTROL_FLAG, idx+1))>0)
                    controlCharCount++;
                else
                    break;
            }
            if(controlCharCount < 35)
            {
                sysLoger(LINFO, "HandleIDECmd:set register value, data is invaid!");
                SendIDEResult(ERR_UNKONW_DATA, FAIL_ACK_LEN, hwSrcAid, hwSrcSaid);
                break;
            }
            char *pBuf = NULL;
            idx = 0;
            for(int i = 0; (i < RSP_ArmConst::ARM_REG_NUM)&&(idx < qsRegValues.length()); i++)
            {
                qsSingleValue = qsRegValues.section(CONTROL_FLAG, idx, idx);
                pBuf = qsSingleValue.toAscii().data();
                hex2int(&pBuf, &value);                
                sysLoger(LINFO, "HandleIDECmd: Set Register's [regno= %d] value; old value:%x, new value:%x",
                                                i, m_currentRegs[i], value);
                m_currentRegs[i] = value;
                idx++;
            }
            SendIDEResult(SUCC_ACK, SUCC_ACK_LEN, hwSrcAid, hwSrcSaid);
        }
        break;
        
    case 'Y':
            //收到simihost的注册命令
            handleSimiHostCommand(cpBuf, wSize, hwSrcAid, hwSrcSaid);            
        break;
        
    default:      //处理semihost的一些读写内存命令
        {
            T_CHAR *pData = cpBuf;
            T_WORD dataSize = wSize;
            if(MessageHandler(pData, dataSize) == IS_NEED_EXC_CMD)
            {
                ExcuteCommand(NULL, 0, TRUE);
            }
            
            break;
        }
    }
    return;
}
T_VOID RSP_TcpServer::SendIDEResult(T_CHAR *cpBuf, T_WORD wSize, T_HWORD hwSrcAid,T_HWORD hwSrcSaid)
{
    //申请分配一个新的缓冲包
    RSP_Packet *pack = RSP_PacketManager::GetInstance()->Alloc();
    T_CHAR *pBuf = pack->GetData();

    //组装协议缓冲，格式 Maddr,size,data
    memcpy(pBuf, cpBuf, wSize);
    pBuf += wSize;

    pack->SetSize(pBuf - pack->GetData());

    pack->SetDesAid(hwSrcAid);
    pack->SetDesSaid(hwSrcSaid);
    pack->SetSrcSaid(1);
    //组包完成，插入到队列中去
    Enqueue(pack);
}
/**
* @brief
*    回调操作，该方法将收到的ICE的数据进行解析，推动服务器的工作
* @param[in] hwDesSaid 服务器所对应的二级会话ID号
* @param[in] hwSrcAid ICE的ID号
* @param[in] hwSrcSaid ICE上会话的ID号
* @param[in] cpBuf 数据缓冲指针,服务器可以从中解析到所需的参数
* @param[in] wSize 数据缓冲的大小
* @return SUCC或者FAIL
*/
int RSP_TcpServer::Putpkt(T_HWORD hwDesSaid,T_HWORD hwSrcAid,T_HWORD hwSrcSaid,T_CHAR* cpBuf,T_WORD wSize)
{
    m_quitMutex.lock();
    m_txExitReady = FALSE;
    m_quitMutex.unlock();

#ifdef IS_PACK_LOG
    if(IsLog())
    {
        m_logger.write(RECV_PACK, cpBuf, wSize);
    }
#endif
    sysLoger(LDEBUG, "RSP_TcpServer::Putpkt: TX thread runing!");
    //--------------------------------------------------------------------------
    //2006.07.14 by guojc
    //增加接收IDE命令进行相应设置功能，通过判断desSaid为1，表示数据是发送给设置代理的
    //目前考虑提供的设置功能主要包括日志功能的打开关闭，内存优化功能的打开关闭
    if(hwDesSaid == 1)
    {        
        HandleIDECmd(hwSrcAid, hwSrcSaid, cpBuf, wSize);
        
        PUTPKT_RETURN(SUCC)
    }
    
    //-------------------------------------------------------------------------

    if(hwSrcAid != m_iceId)
    {
        //收到IDE的S05命令
        if(cpBuf[0] == 'S' && cpBuf[1] == '0' && cpBuf[2] == '5' && GetTargetStatus() == RSP_ArmConst::CORE_RUNNING)
        {
            //如果命令序列中还有,暂时不响应IDE的命令
            if(CmdSize() != 0)
            {
                PUTPKT_RETURN(SUCC)
            }
            sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S05 from IDE or other tool via target server!");
            //设置内存区域无效,清空内存
            RefreshMemory();

            //处于运行状态,强行停止目标机
            //装载命令序列
            SetTargetStatus(RSP_ArmConst::CORE_DEBUG_BKWTPT);  //将状态置为停止
            PushBackCmd(RSP_ArmConst::CMD_STOP_TRT);
            PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
            PushBackCmd(RSP_ArmConst::CMD_READ_REG);
            PushBackCmd(RSP_ArmConst::CMD_UPDATE_READ_REG);
            PushBackCmd(RSP_ArmConst::CMD_REPLY_S05);
            //装载参数,因为都不需要参数，所以只需要加入空链表即可
            //1. 停止目标机,无参数
            QList<T_VOID*> *stp = new QList<T_VOID*>();
            m_atomParas.push_back(stp);

            //2. 分析执行结果,无参数
            QList<T_VOID*> *rwm = new QList<T_VOID*>();
            m_atomParas.push_back(rwm);

            QList<T_VOID*> *readReg = new QList<T_VOID*>();
            QList<T_VOID*> *replyReg = new QList<T_VOID*>();
            QList<T_VOID*> *replyS05 = new QList<T_VOID*>();
            m_atomParas.push_back(readReg);
            m_atomParas.push_back(replyReg);
            m_atomParas.push_back(replyS05);
            ExcuteCommand(cpBuf, wSize, TRUE);
            
            PUTPKT_RETURN(SUCC)
        }
    }

    //FIXME:ICE在目标机掉电，上电后有时候还会发出S05命令，这样导致IS在读取寄存器的时候会收到PN0，导致IS崩溃
    //这个问题还没有解决
    //检查数据内容是否为信号
    if(cpBuf[0] == 'S' && cpBuf[1] == '0')
    {
        switch(cpBuf[2])
        {
        case '5'://目标机停止,应该装载命令序列：1. 读寄存器 2.更新寄存器 3.发送S05给GDB
            {
                if(CmdSize() != 0)
                {
                    sysLoger(LDEBUG, "RSP_TcpServer::Putpkt:Recieved S05 from ICE , but cmd queue have cmds recived!");
                    PUTPKT_RETURN(SUCC)
                }
                if(m_targetStatus != RSP_ArmConst::CORE_RUNNING)
                {
                    sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S05 from ICE in the invalid state!");
                    PUTPKT_RETURN(SUCC)
                }
                sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S05 from ICE !");
                //设置内存区域无效,清空内存
                RefreshMemory();
                //装载命令序列
                SetTargetStatus(RSP_ArmConst::CORE_DEBUG_BKWTPT);  //将状态置为停止
                PushBackCmd(RSP_ArmConst::CMD_READ_REG);
                PushBackCmd(RSP_ArmConst::CMD_UPDATE_READ_REG);
                QList<T_VOID*> *readReg = new QList<T_VOID*>();
                QList<T_VOID*> *replyReg = new QList<T_VOID*>();
                m_atomParas.push_back(readReg);
                m_atomParas.push_back(replyReg);
                
                //--------------------------------------------------------------------------------------
                //故障：2006.06.15 发现调试过程中插入到内存的断点没有在S05后写回去，怀疑是GDB没有发删除断点命令
                //当然也不排除是IS内部因素造成
                //修改方法：目标机停止后，IS立即将软件断点地址的实际内容立即恢复
                //恢复断点
                if(!m_bpMap.empty())
                {
                    QMapIterator<T_WORD, T_WORD> i(m_bpMap);
                    while (i.hasNext())
                    {
                        i.next();
                        T_WORD armAddr = i.key();
                        T_WORD value = i.value();

                        //装载删除软件断点指令
                        //存在该断点，从断点表里面取出该断点
                        T_WORD *pArmAddr = new T_WORD(armAddr);
                        T_WORD *pValue = new T_WORD(value);
                        T_WORD *pWidth = new T_WORD(32);
                        //装载命令序列 1.写内存 2. 分析结果 3. 回复OK
                        PushBackCmd(RSP_ArmConst::CMD_WRITE_INT_MEM);
                        PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);

                        //装载参数
                        //1. 写内存参数
                        QList<T_VOID*> *pWriteMem = new QList<T_VOID*>();
                        pWriteMem->push_back(pArmAddr);
                        pWriteMem->push_back(pValue);
                        pWriteMem->push_back(pWidth);
                        m_atomParas.push_back(pWriteMem);

                        //2. 分析结果没有参数
                        QList<T_VOID*> *pCheckWriteMem = new QList<T_VOID*>();
                        m_atomParas.push_back(pCheckWriteMem);
                    }

                    
                }

                //清空断点map
                m_bpMap.clear();
                /*add for simihost, todo: analy the pc rsgister, if the value equal the registered address, 
                then  send the SWI to simihost*/
                if(m_SimiRegisterFlag == TRUE)
                {
                    sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S05, push the CMD_ANALY_SIMIHOST_BREAK cmd!");
                    PushBackCmd(RSP_ArmConst::CMD_ANALY_SIMIHOST_BREAK);
                    QList<T_VOID*> *replySimihost = new QList<T_VOID*>();
                    m_atomParas.push_back(replySimihost);
                }

                //-------------------------------------------------------------------------------------------
                //装载参数,因为都不需要参数，所以只需要加入空链表即可
                PushBackCmd(RSP_ArmConst::CMD_REPLY_S05);
                QList<T_VOID*> *replyS05 = new QList<T_VOID*>();
                m_atomParas.push_back(replyS05);
                ExcuteCommand(cpBuf, wSize, TRUE);
                break;
            }
        case '6'://掉电事件,应该检查当前命令序列中是否还有值，有则提取出最后一条命令执行。并将目标机状态置为不可用
            {
                if(!m_atomCmds.isEmpty())
                {
                    ExcuteCommand(cpBuf, wSize, FALSE);
                }
                sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S06 from ICE !");
                SetTargetStatus(RSP_ArmConst::CORE_POWER_OFF);
                //如果支持私有扩展，则自动上报掉电事件
                if(m_isSupportPrivateProtocol)
                {
                    //方案部需求,IS向TC自动上报上下电事件,这里需要向SOCKET数据方向报
                    m_CurCmdSource = SOCKET_DATA;
                    PutData("Poff", strlen("Poff"));                
                }                
                break;
            }
        case '7'://上电事件,应该查询目标机状态，根据回复结果推动初始化事件            {
            {    //清空内存
                RefreshMemory();
                sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved S07 from ICE !");
                //如果支持私有扩展，则自动上报上电事件
                if(m_isSupportPrivateProtocol)
                {
                    //方案部需求,IS向TC自动上报上下电事件,这里需要向SOCKET数据方向报
                    m_CurCmdSource = SOCKET_DATA;
                    PutData("Pon", strlen("Pon"));
                }
                //查询状态
                InitTarget();
                QueryTarget();
                break;
            }

        default:
            //FIXME:记录错误信息
            PUTPKT_RETURN(SUCC)
        }
    }
    else
    if(cpBuf[0] == 'P')
    {
        //上电,初始化目标机,执行命令序列
        sysLoger(LINFO, "RSP_TcpServer::Putpkt:Recieved %s from ICE !", cpBuf);
        if(cpBuf[1] == 'N')
        {
            InitTarget();
            ExcuteCommand(cpBuf, wSize, TRUE);
        }
        else
        {
            SetTargetStatus(RSP_ArmConst::CORE_POWER_OFF);
        }

    }
    else
    {
        //是上一条命令的回复，推动下一条命令的执行
        ExcuteCommand(cpBuf, wSize, TRUE);
    }

    PUTPKT_RETURN(SUCC)
    sysLoger(LDEBUG, "RSP_TcpServer::Putpkt: TX thread exiting!");
}

/**
* @brief
*    回调操作，该方法阻塞在服务器维护的缓冲队列接收包，收到数据后\n
*    将数据拷贝到调用者提供的缓冲区后返回\n
* @param[in] hwpDesAid  ICE的通信ID号
* @param[in] hwpDesSaid ICE上具体会话的ID号
* @param[in] hwpSrcSaid Rsp服务器的二级ID号
* @param     cpBuf    调用者提供的数据缓冲
* @param[in] wSize    调用者提供的缓冲区的大小
* @return  实际要发送数据的大小
*/
int RSP_TcpServer::Getpkt(T_HWORD *hwpDesAid,T_HWORD *hwpDesSaid,T_HWORD *hwpSrcSaid,T_CHAR* cpBuf,T_WORD wSize)
{
    //从队列里取下一个包
    RSP_Packet *pack = NULL;
    sysLoger(LDEBUG, "RSP_TcpServer::Getpkt: RX thread starting!");
    pack = Dequeue();
    while(NULL == pack)
    {
        //队列为空，阻塞等待
        WaitUp();
        if(m_runFlag == FALSE)
        {
            m_quitMutex.lock();
            m_rxExitReady = TRUE;
            m_quitReady.wakeAll();
            m_quitMutex.unlock();
            return FAIL;
        }
        pack = Dequeue();
    }
    
#ifdef IS_PACK_LOG
    if(IsLog())
    {
        m_logger.write(SEND_PACK, pack->GetData(), pack->GetSize());
    }
#endif

    //如果数据大小大于所提供的缓冲区大小
    if(pack->GetSize() > wSize)
    {
        m_quitMutex.lock();
        m_rxExitReady = TRUE;
        m_quitReady.wakeAll();
        m_quitMutex.unlock();
        return FAIL;
    }

    //将数据拷贝到用户缓冲
    if(pack->GetDesAid() == 0)
    {
        *hwpDesAid = m_iceId;
        *hwpDesSaid = m_sessId;
        *hwpSrcSaid = 0;
    }
    else
    {
        *hwpDesAid = pack->GetDesAid();
        *hwpDesSaid = pack->GetDesSaid();
        *hwpSrcSaid = pack->GetSrcSaid();
    }
    memcpy(cpBuf, pack->GetData(), pack->GetSize());

    int returnSize = pack->GetSize();

    //释放缓冲包
    RSP_PacketManager::GetInstance()->Free(pack);
    sysLoger(LDEBUG, "RSP_TcpServer::Getpkt: RX thread exiting!");
    return returnSize;
}

/**
* @brief
*    设置目标机状态，加锁保护
* @param[in] wStatus 目标机状态
*/
T_VOID RSP_TcpServer::SetTargetStatus(T_WORD wStatus)
{
    m_statusLock.lock();
    sysLoger(LINFO, "RSP_TcpServer::SetTargetStatus:set target status to %d", wStatus);
    m_targetStatus = wStatus;
    m_statusLock.unlock();
}

/**
* @brief
*    得到目标机状态，加锁保护
* @param[in] wStatus 目标机状态
*/
T_WORD RSP_TcpServer::GetTargetStatus()
{
    T_WORD status = 0;
    m_statusLock.lock();
    status = m_targetStatus;
    m_statusLock.unlock();

    return status;
}

/**
* @brief
*    RX线程阻塞方法
*/
T_VOID RSP_TcpServer::WaitUp()
{
    m_packMutex.lock();
    m_queueNotFull.wait(&m_packMutex);    //等待5秒的超时
    m_packMutex.unlock();
}

/**
* @brief
*    服务器线程唤醒RX方法
*/
T_VOID RSP_TcpServer::WakeUp()
{
    m_packMutex.lock();
    m_queueNotFull.wakeAll();
    m_packMutex.unlock();
}

/**
* @brief
*    将缓冲包插入到服务器维护的缓冲队列中
* @param[in] tpPack    缓冲包
*/
T_VOID RSP_TcpServer::Enqueue(RSP_Packet* tpPack)
{
    m_packMutex.lock();
    m_packetQueue.append(tpPack);
    m_queueNotFull.wakeAll();
    m_packMutex.unlock();
}

/*
* @brief
*    从缓冲队列里面取出一个命令包\n
* @return  缓冲数据包
*/
RSP_Packet* RSP_TcpServer::Dequeue()
{
    RSP_Packet* pack = NULL;
    if (m_packetQueue.size() > 0)
    {
        m_packMutex.lock();
        pack = m_packetQueue.first ();
        m_packetQueue.pop_front();
        m_packMutex.unlock();
    }
    return pack;
}

/*
* @brief
*    关闭服务器线程方法
*/
T_VOID RSP_TcpServer::CloseThread()
{
    m_runFlag = FALSE;
}

/**
* @brief
*    服务器线程运行入口方法
*/
T_VOID RSP_TcpServer::run()
{
    //定义缓冲
    T_CHAR recvBuf[PACKET_DATA_SIZE];
    bool bLocked = TRUE;

    m_sktMutex.lock();

#ifdef IS_PACK_LOG
    QString strPort = QString("%1").arg(m_port);
    QString name = strPort.append("IS.log");
    m_logger.open(name.toAscii().data());
#endif

    m_runFlag = TRUE;

    sysLoger(LDEBUG, "RSP_TcpServer::run: ICE Server sarting...!");
    
    SetTargetStatus(RSP_ArmConst::CORE_POWER_OFF);
    if(LAMBDA_JTAG_ICE == m_targetType)
    {
        //删除原来的会话
        PushBackCmd(RSP_ArmConst::CMD_DELETE_SESSION);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_DELETE_SESSION);
        //装载参数
        QList<T_VOID*> *deleteSessionPara = new QList<T_VOID*>();
        QList<T_VOID*> *analydeleteSessionResult = new QList<T_VOID*>();
        m_atomParas.push_back(deleteSessionPara);
        m_atomParas.push_back(analydeleteSessionResult);
    }
    
    //创建会话，    
    PushBackCmd(RSP_ArmConst::CMD_CREATE_SESSION);
    PushBackCmd(RSP_ArmConst::CMD_ANALY_CREATE_SESSION);
    //装载参数
    QList<T_VOID*> *createSessionPara = new QList<T_VOID*>();
    QList<T_VOID*> *analySessionResult = new QList<T_VOID*>();
    int createSessionParaLen = m_IceBuf.length();
    createSessionPara->push_back(m_IceBuf.toAscii().data());
    createSessionPara->push_back((T_VOID*)(&createSessionParaLen));
    m_atomParas.push_back(createSessionPara);
    m_atomParas.push_back(analySessionResult);

    //查询ICE大小端
    PushBackCmd(RSP_ArmConst::CMD_QUERY_ICEENDIAN);
    PushBackCmd(RSP_ArmConst::CMD_ANALY_QUERY_ICEENDIAN);
    //PushBackCmd(RSP_ArmConst::CMD_UPDATE_ICEENDIAN);
    //装载参数
    QList<T_VOID*> *queryendian = new QList<T_VOID*>();
    QList<T_VOID*> *analyResult = new QList<T_VOID*>();
    m_atomParas.push_back(queryendian);
    m_atomParas.push_back(analyResult);

    //查询目标机状态
    PushBackCmd(RSP_ArmConst::CMD_QUERY_TRT);
    //装载参数
    QList<T_VOID*> *queryTargetPara = new QList<T_VOID*>();
    m_atomParas.push_back(queryTargetPara);
    
    sysLoger(LINFO, "RSP_TcpServer::run: Starting to delete session, create session, queryICE endian!");
    
    //执行首条命令，推动进程
    ExcuteCommand(NULL, 0, TRUE);
  
    while(m_runFlag)
    {
        //阻塞监听服务器
        if( bLocked != TRUE )
        {
            m_sktMutex.lock();
            bLocked = TRUE;
        }

        m_sClient = new WSocket;

        m_sktMutex.unlock();
        bLocked = FALSE;

        if (!m_serverSocket.acceptSocket(*m_sClient))
        {
            sysLoger(LINFO, "RSP_TcpServer::run: start to listen failure!");

            break;
        }
        else
        {
        
            sysLoger(LINFO, "RSP_TcpServer::run: Detected a client connected !");

            //重新连接执行上电初始化         
            this->usleep(100 * 1000);
            if(!m_isSupportPrivateProtocol)
            {                
                T_WORD count = 40;
                T_WORD targetStatus = GetTargetStatus();
                while(CmdSize() != 0 || RSP_ArmConst::CORE_POWER_OFF == targetStatus || RSP_ArmConst::CORE_RUNNING == targetStatus)
                {
                    this->usleep(500 * 1000);
                    if(!count--)
                        break;
                    targetStatus = GetTargetStatus();    
                }
            }
        }

    

        while(m_runFlag)
        {
            //连接到来后阻塞接收数据
            memset(recvBuf, 0, sizeof(recvBuf));
            int ret = GetData(recvBuf, PACKET_DATA_SIZE);

            //日志
            #ifdef IS_PACK_LOG
            if(TRUE == IsLog())
            {
                m_logger.write(RECV_PACK, recvBuf, ret);
            }            
            #endif

            if(FAIL == ret)
            {
                //网络连接断开，重新监听连接
                if(NULL != m_sClient)
                {
                    sysLoger(LINFO, "RSP_TcpServer::run: a client [GDB] linker disconnected!");
                    //加锁进行保护
                    DeleteSocket();
                }
                //删除目标端已存在的断点
                if(ClearTarget())
                {
                    ExcuteCommand(NULL, 0, TRUE);
                }
                break;
            }
            //记录消息来源
            m_MtxMsgSrc.lock();
            m_CurCmdSource = SOCKET_DATA;
            m_CurSrcAid = INVALID_AID;
            m_CurSrcSaid = INVALID_AID;
            //进行协议解析，装载命令序列
            if(MessageHandler(recvBuf, ret) != IS_NEED_EXC_CMD)
            {
                m_MtxMsgSrc.unlock();
                continue;
            }

            //执行首条命令，推动进程
            ExcuteCommand(NULL, 0, TRUE);
            m_MtxMsgSrc.unlock();
        }
    }

    m_quitMutex.lock();
    m_localExitReady = TRUE;
    m_quitReady.wakeAll();
    m_quitMutex.unlock();
    sysLoger(LDEBUG, "RSP_TcpServer::run: ICE Server exiting!");
}

//T_VOID RSP_TcpServer::SetLog(T_BOOL isLog)
//{
//    QMutexLocker locker(&m_logMutex);
//    m_isLog = isLog;
//}
//
//T_BOOL RSP_TcpServer::IsLog()
//{
//    QMutexLocker locker(&m_logMutex);
//    return m_isLog;
//}

int RSP_TcpServer::SetMop(T_BOOL isMop)
{
    QMutexLocker locker(&m_mopMutex);
    if(CmdSize() > 0)
    {
        return FAIL;
    }
    m_mopFlag = isMop;
    return SUCC;
}

T_BOOL RSP_TcpServer::GetMop()
{
    QMutexLocker locker(&m_mopMutex);
    return m_mopFlag;
}


T_VOID RSP_TcpServer::DeleteSocket()
{
    m_clientMutex.lock();
    delete m_sClient;
    m_sClient = NULL;
    m_clientMutex.unlock();
}
/*
* @brief
*    在GDB断开和IS的连接和检查目标机端是否有断点设置，如果有，就装载命令清空断点内容\n
*/
T_BOOL RSP_TcpServer::ClearTarget()
{
    //网络断开,强迫将目标机状态设置为停止状态
    if(GetTargetStatus() == RSP_ArmConst::CORE_POWER_OFF)
    {
        sysLoger(LINFO, "RSP_TcpServer::ClearTarget:target is power off, start to clear command queue!");
        ExcuteCommand(NULL, 0, FALSE);
        m_hardBp = FALSE;
        m_bpMap.clear();
        return FALSE;
    }
    SetTargetStatus(RSP_ArmConst::CORE_DEBUG_BKWTPT);

    //如果命令序列里面还有命令，表示ICE掉电或者网络连接断开，此时不需要再删除目标端的断点，只需要在本地进行清空即可
    if(CmdSize() > 0)
    {
        m_hardBp = FALSE;
        m_bpMap.clear();
        return FALSE;
    }
    sysLoger(LINFO, "RSP_TcpServer::ClearTarget: starting to excute the clear operating!");
    T_BOOL flag = FALSE;

    /*如果没有注册Simihost ，则在GDB退出时删除硬件断点--by qingxh*/
    if(!m_SimiRegisterFlag)
    {
        //装载命令序列 1.删除硬件断点 2. 删除软件断点 3. 设置目标机状态不确定
        if(m_hardBp)
        {
            PushBackCmd(RSP_ArmConst::CMD_REMOVE_HBP);
            PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);

            //1. 删除硬件断点
            T_WORD *pArmAddr = new T_WORD(0);
            T_WORD *pWidth = new T_WORD(4);
            QList<T_VOID*> *pRemoveHbp = new QList<T_VOID*>();
            pRemoveHbp->push_back(pArmAddr);
            pRemoveHbp->push_back(pWidth);
            m_atomParas.push_back(pRemoveHbp);

            //2.分析结果,没有参数
            QList<T_VOID*> *pCheckRemoveHbp = new QList<T_VOID*>();
            m_atomParas.push_back(pCheckRemoveHbp);

            flag = TRUE;
        }
    }

    if(!m_bpMap.empty())
    {
        QMapIterator<T_WORD, T_WORD> i(m_bpMap);
        while (i.hasNext())
        {
            i.next();
            T_WORD armAddr = i.key();
            T_WORD value = i.value();

            //装载删除软件断点指令
            //存在该断点，从断点表里面取出该断点
            T_WORD *pArmAddr = new T_WORD(armAddr);
            T_WORD *pValue = new T_WORD(value);
            T_WORD *pWidth = new T_WORD(32);
            //装载命令序列 1.写内存 2. 分析结果 3. 回复OK
            PushBackCmd(RSP_ArmConst::CMD_WRITE_INT_MEM);
            PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);

            //装载参数
            //1. 写内存参数
            QList<T_VOID*> *pWriteMem = new QList<T_VOID*>();
            pWriteMem->push_back(pArmAddr);
            pWriteMem->push_back(pValue);
            pWriteMem->push_back(pWidth);
            m_atomParas.push_back(pWriteMem);

            //2. 分析结果没有参数
            QList<T_VOID*> *pCheckWriteMem = new QList<T_VOID*>();
            m_atomParas.push_back(pCheckWriteMem);
        }

        flag = TRUE;
    }
    

    //清空断点map
    m_bpMap.clear();

    return flag;
}

/*
* @brief
*    查询目标机状态，通过ICE的回复推动状态流程执行，装载命令序列\n
* @return
*/
T_VOID RSP_TcpServer::QueryTarget()
{

    m_lambdaIce.QueryTarget();

}

/*
* @brief
*    初始化目标机操作，在目标机上电以后进行，装载命令序列\n
* @return
*/
T_VOID RSP_TcpServer::InitTarget()
{
    //命令序列包括 1.停止目标机 2. 解析执行结果 3. 读寄存器 4.更新寄存器 5.（写内存 ,分析执行结果， 延时）循环 6.设置目标机可运行状态
    sysLoger(LINFO, "RSP_TcpServer::InitTarget: starting to excute the init operating!");
    PushBackCmd(RSP_ArmConst::CMD_STOP_TRT);
    PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
    PushBackCmd(RSP_ArmConst::CMD_READ_REG);
    PushBackCmd(RSP_ArmConst::CMD_UPDATE_READ_REG);

    for(int i = 0; i < m_memCfg.size(); i++)
    {
        PushBackCmd(RSP_ArmConst::CMD_WRITE_INT_MEM);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
        PushBackCmd(RSP_ArmConst::CMD_DELAY);
    }

    //更新CP15
    T_WORD udateCp15Num = m_cpp.size();
    for(T_WORD j = 0; j < udateCp15Num; j++)
    {
        PushBackCmd(RSP_ArmConst::CMD_INIT_CP15);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);

    }
    //启用DCC
    if(m_DccEnable == TRUE)
    {
        PushBackCmd(RSP_ArmConst::CMD_OPEN_DCC_FUNCTION);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_OPEN_DCC);
    }
    PushBackCmd(RSP_ArmConst::CMD_SET_TRT_STATUS);

    //装载参数序列
    //1. 停止目标机，无参数
    QList<T_VOID*> *stopTargetPara = new QList<T_VOID*>();
    m_atomParas.push_back(stopTargetPara);
    //2. 解析执行结果，无参数
    QList<T_VOID*> *checkReply_stp = new QList<T_VOID*>();
    m_atomParas.push_back(checkReply_stp);
    //3. 读寄存器，无参数
    QList<T_VOID*> *readRegsPara = new QList<T_VOID*>();
    m_atomParas.push_back(readRegsPara);
    //4. 更新寄存器，无参数
    QList<T_VOID*> *updateRegsPara = new QList<T_VOID*>();
    m_atomParas.push_back(updateRegsPara);

    //5. 写内存（循环）， 参数
    for(int i = 0; i < m_memCfg.size(); i++)
    {
        //写内存参数
        QList<T_VOID*> *writePara = new QList<T_VOID*>();
        //地址，value
        pair<T_WORD, T_WORD> *memPair = m_memCfg.value(i);
        T_WORD *pAddr = new T_WORD(memPair->first);
        T_WORD *pValue = new T_WORD(memPair->second);
        T_WORD *pWidth = new T_WORD(m_widthCfg.value(i));
        T_WORD *delay = new T_WORD(m_delayCfg.value(i));
        writePara->push_back(pAddr);
        writePara->push_back(pValue);
        writePara->push_back(pWidth);
        writePara->push_back(delay);
        
        m_atomParas.push_back(writePara);

        //分析写内存结果,无参数
        QList<T_VOID*> *checkReply_wmem = new QList<T_VOID*>();
        m_atomParas.push_back(checkReply_wmem);

        //延时,参数timeout单位毫秒
        QList<T_VOID*> *delayPara = new QList<T_VOID*>();
        T_WORD *timeout = new T_WORD(m_delayCfg.value(i));
        delayPara->push_back(timeout);
        m_atomParas.push_back(delayPara);
    }

    //6. 更新CP15寄存器
    QList<T_WORD> keys;
    QMapIterator<T_WORD, T_WORD> i_keys(m_cpp);
    while (i_keys.hasNext()) {
        i_keys.next();
       keys.push_back(i_keys.key());
    }

    for(T_WORD j = 0; j < udateCp15Num; j++)
    {
        QList<T_VOID*> *initCp15Para = new QList<T_VOID*>();
        T_WORD *num = new T_WORD(keys.value(j));
        T_WORD *value = new T_WORD(m_cpp.value(keys.value(j)));
        
        initCp15Para->push_back(num);
        initCp15Para->push_back(value);
        m_atomParas.push_back(initCp15Para);

        //分析写CP15结果,无参数
        QList<T_VOID*> *checkReply_wmem = new QList<T_VOID*>();
        m_atomParas.push_back(checkReply_wmem);
    }
    if(m_DccEnable)
    {
        QList<T_VOID*> *dccPara = new QList<T_VOID*>();
        T_WORD *dccAddr = new T_WORD(m_DccAddr);
        QList<T_VOID*> *dccResult = new QList<T_VOID*>();
        dccPara->push_back(dccAddr);
        m_atomParas.push_back(dccPara);
        m_atomParas.push_back(dccResult);
    }
    //6. 更新目标机状态
    QList<T_VOID*> *updateStatusPara = new QList<T_VOID*>();
    m_atomParas.push_back(updateStatusPara);
    return;
}

/*
* @brief
*    将接收GDB的数据进行协议解析，装载命令序列\n
* @param[in] cpBuf    调用者提供的数据缓冲
* @param[in] wSize    调用者提供的缓冲区的大小
* @return
*/
int RSP_TcpServer::MessageHandler(T_CHAR *cpBuf, T_WORD wSize)
{
    QMutexLocker locker(&m_excuteLock);

    T_WORD iRet = IS_NEED_EXC_CMD;
    //检查输入参数合法性
    if(NULL == cpBuf)
    {
        return FAIL;
    }
    
    sysLoger(LINFO, "RSP_TcpServer::MessageHandler: Recieved the command:%c", cpBuf[0]);
    //私有扩展, 支持查询目标机的上下电状态
    if(cpBuf[0] == 'A')
    {
        if((m_targetStatus != RSP_ArmConst::CORE_POWER_OFF)
            && (m_targetStatus != RSP_ArmConst::CORE_INVALID))
        {
            //上电状态
            sysLoger(LDEBUG, "MessageHandler:Recieved Query target power state CMD, power on!");
            PutData("Pon", strlen("Pon"));
            return FAIL;
        }
        else
        {
            //下电状态
            sysLoger(LDEBUG, "MessageHandler:Recieved Query target power state CMD, power off!");
            PutData("Poff", strlen("Poff"));
            return FAIL;
        }
    }
    
    T_WORD cmdSize = CmdSize();
    if(cmdSize > 0)
    {
        sysLoger(LINFO, "MessageHandler:command queue is not empty, refuse the command:%c"\
            " the first command in command queue:%d", cpBuf[0], m_atomCmds.first());
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    
    //检查目标机状态合法性
    T_WORD targetStatus = GetTargetStatus();
    if(RSP_ArmConst::CORE_POWER_OFF == targetStatus || RSP_ArmConst::CORE_RUNNING == targetStatus)
    {
        //一些命令在目标机掉电状态或运行状态不支持
        sysLoger(LINFO, "MessageHandler:target is power off or running,refuse the command:%c", cpBuf[0]);
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    
    switch(cpBuf[0])
    {
    case 'P':
        {
            iRet = handleWriteSingleRegister(cpBuf, wSize);
            break;
        }
    case 'g':
         iRet = handleReadRegisters(cpBuf, wSize);
         break;
        
    case 'X':
        iRet = handleWriteMemory(cpBuf, wSize);
        break;
        
    case 'm':
        iRet = handleReadMemory(cpBuf, wSize);
        break;
        
    case 'c':
        iRet = handleContinueTarget(cpBuf, wSize);
        break;
        
    case 'Z':
        iRet = handleSetBreakPoint(cpBuf, wSize);
        break;
        
    case '?':
        PutData("S11", strlen("S11"));
        return FAIL;
        break;
        
    case 'z':
        iRet = handleRemoveBreakPoint(cpBuf, wSize);
        break;
        
    case 'R':
            /*目标机复位*/
            iRet = handleResetTarget(cpBuf, wSize);
        break;
        
    default:
        //不支持该命令
        PutData("", strlen(""));
        return FAIL;
    }

    return iRet;
}

/*
* @brief
*    从本地内存缓冲中获取需要的内存信息，如果没有，该方法会压入命令序列从目标机中读取相应的内存块\n
* @param[in] wAddr   内存地址
* @param[in] wSize   内存大小
* @param[in] cpBuf   调用者给定的缓冲区
* @param[in] type    调用者想获得的内存的格式 0为GDB格式 1为二进制格式
* @return  给缓冲区拷贝的内存区的大小，0为无本地缓存
*/
int RSP_TcpServer::GetLocalMemory(T_WORD wAddr, T_WORD wSize, T_CHAR *cpBuf, T_WORD type)
{
    //计算基地址,从映射表中查找本地缓冲是否存在该内存块
    T_WORD baseAddr = wAddr & RSP_Memory::MEMORY_BLOCK_MASK;
    T_WORD offset = wAddr - baseAddr;
    T_WORD leftSize = wSize;
    T_CHAR *pLocalBuf = cpBuf;


    //计算所需要内存块的数目
    T_WORD computeSize = (wSize + RSP_Memory::MEMORY_BLOCK_SIZE - 1 + offset) / RSP_Memory::MEMORY_BLOCK_SIZE;
    T_WORD count = 0;
/*    if(computeSize == 1)
    {
        count = 1;
    }
    else
    {
        count = computeSize + 1;
    }
*/
    count = computeSize;
    //检查本地是否有足够的内存块
    T_WORD validAddr = baseAddr;  //内存地址
    T_WORD i = count;
    while(i != 0)
    {
        if(!m_addr2mem.contains(validAddr))
        {
            break;
        }
        validAddr += RSP_Memory::MEMORY_BLOCK_SIZE;
        i--;
    }

    //如果本地有足够的内存缓冲,则将内存信息拷贝到缓冲区中
    if(i == 0)
    {
        T_WORD index = offset; //本地内存数组索引
        T_WORD localSize = 0;
        while(count)
        {
            localSize = wSize < RSP_Memory::MEMORY_BLOCK_SIZE ? wSize : RSP_Memory::MEMORY_BLOCK_SIZE;
            //
            if((wSize + index) < RSP_Memory::MEMORY_BLOCK_SIZE)
            {
                localSize = wSize;
            }
            else
            {
                localSize = RSP_Memory::MEMORY_BLOCK_SIZE - index;
            }

            //取出一块内存块
            RSP_Memory *pMem = m_addr2mem.value(baseAddr);
            for(T_WORD j = index; j < (localSize + index); j++)
            {
                if(type == 0)
                {//gdb格式内存
                    pLocalBuf += Int2Hex(pLocalBuf, static_cast<int>(*(pMem->GetValue() + j)), 2);
                }
                else
                {//本地缓存格式内存
                    *pLocalBuf = *(pMem->GetValue() + j);
                    pLocalBuf++;
                }
            }

            baseAddr += RSP_Memory::MEMORY_BLOCK_SIZE;
            //clear offset while count > 2
            index = 0;
            wSize -= localSize;
            count--;
        }

        return pLocalBuf - cpBuf;//返回发送数据的长度
    }
    else
    {
        //-------------------------------------------------------------------------------------------
        //2006.10.30 内存优化算法存在一定的问题，主要是因为当没有足够的缓冲块的时候，会调用refreshMemory方法
        //有可能会释放掉已经存在的，并且在此次调试命令所需要的内存块，因此造成无法返回给GDB足够的内存数据而失败
        //因此在这里改为在发送命令之前先检查是否有足够的内存块，如果没有，就释放后重新组织下发命令
        if(i > m_memMan.LeftPackets())
        {
            //如果需要申请的缓冲包个数大于可以分配的个数，则清零缓冲区
            this->RefreshMemory();
            //因为刷新，所以validAddr必须从基地址开始算起
            validAddr = baseAddr;
            i = count;
        }

        //从目标机读取相应的内存，地址为validAddr,大小为：
        T_WORD memSize = i * RSP_Memory::MEMORY_BLOCK_SIZE;

        //装载命令序列 1.读内存 2.更新内存区
        PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
        PushBackCmd(RSP_ArmConst::CMD_UPDATE_READ_MEM);

        //装载读内存参数
        QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
        T_WORD *pReadAddr = new T_WORD(validAddr);
        T_WORD *pReadSize = new T_WORD(memSize);
        readMemPara->push_back(pReadAddr);
        readMemPara->push_back(pReadSize);
        //装载更新内存参数
        QList<T_VOID*> *updateMemPara = new QList<T_VOID*>();
        T_WORD *pUpdateAddr = new T_WORD(validAddr);
        T_WORD *pUpdateSize = new T_WORD(memSize);
        updateMemPara->push_back(pUpdateAddr);
        updateMemPara->push_back(pUpdateSize);


        //装载参数序列
        m_atomParas.push_back(readMemPara);
        m_atomParas.push_back(updateMemPara);

        return SUCC;
    }


}

/*
* @brief
*    执行命令序列，推动服务器流程\n
* @param[in] cpBuf   参数值，可以为空
* @return  SUCC或FAIL
*/
int RSP_TcpServer::ExcuteCommand(T_CHAR *cpBuf, T_WORD wSize, T_BOOL bOpsFlag)
{
    QMutexLocker locker(&m_excuteLock);

    int result = 0;
    T_WORD nextCmd = 0;
    T_BOOL opsFlag = bOpsFlag;  //标识当前操作成功

    while(TRUE)
    {
        result = FALSE;

        //取出下一条命令,如果命令执行完毕，退出循环
        nextCmd = GetNextCmd();

        if(nextCmd == FAIL)
        {
            sysLoger(LINFO, "RSP_TcpServer::ExcuteCommand:CMD queue is empty!");
            return SUCC;
        }

        //取出下一组参数
        QList<T_VOID*> *para = NULL;

        para = GetNextPara();

        if(NULL == para)
        {
            sysLoger(LINFO, "RSP_TcpServer::ExcuteCommand:CMD parameter is NULL!");
            return FAIL;
        }
        sysLoger(LINFO, "RSP_TcpServer::ExcuteCommand:Recieved the CMD:%d", nextCmd);
        
        switch(nextCmd)
        {
        case RSP_ArmConst::CMD_QUERY_ICEENDIAN:
            result = CmdQueryIceEndian(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_CREATE_SESSION:
            result = CmdCreateSession(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_DELETE_SESSION:
            result = CmdDeleteSession(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_DELETE_SESSION:
            result = CmdAnalyDeleteSession(cpBuf, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_QUERY_ICEENDIAN:
            result = CmdAnalyQueryICEEndian(cpBuf, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_CREATE_SESSION:
            result = CmdAnalyCreateSession(cpBuf, opsFlag);
            break;
        case RSP_ArmConst::CMD_WRITE_MEM:
            result = CmdWriteIceMemory(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_READ_MEM:
            result = CmdReadIceMemory(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_WRITE_REG:
            result = CmdWriteIceRegisters(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_READ_REG:
            result = CmdReadIceRegisters(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_RUN_TRT:
            result = CmdRunTarget(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_STOP_TRT:
            result = CmdStopTarget(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_RESULT:
            result = CmdCheckReply(cpBuf, opsFlag);
            break;
        case RSP_ArmConst::CMD_SIMPLE_REPLY_GDB:
            result = CmdSimpleReply(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_UPDATE_READ_MEM:
            result = CmdUpdateMems(para, cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_INSERT_HBP:
            result = CmdInsertHardwareBp(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_UPDATE_READ_REG:
            result = CmdUpdateRegs(cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_WRITE_INT_MEM:
            result = CmdWriteIntMemory(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_SET_TRT_STATUS:
            result = CmdUpdateStatus(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_REPLY_READ_MEM:
            result = CmdReplyMems(para, cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_REMOVE_HBP:
            result = CmdRemoveHardwareBp(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_REPLY_S05:
            result = CmdReplyS05(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_INSERT_SBP:
            result = CmdInsertSoftwareBp(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_INSERT_WBP:
            result = CmdInsertWatchPoint(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_REMOVE_WBP:
            result = CmdRemoveWatchPoint(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_DELAY:
            result = CmdDelay(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_INSERT_SBP:
            result = CmdCheckSbp(para, cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_INIT_CP15:
            result = CmdWriteCp15Registers(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_GET_CP15:
            result = CmdReadCp15Registers(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_RESET_TARGET:
            result = CmdResetTarget(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_RESET_TARGET:
            result = CmdAnalyResetTarget(para, cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_QUERY_TRT:
            result = CmdQueryTarget(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_BP_READ_MEM:
            result = CmdAnalyBreakPointReadMem(para, cpBuf, wSize, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_SIMIHOST_BREAK:
            result = CmdAnalySimihostBreakPoint(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_OPEN_DCC_FUNCTION:
            result = CmdOpenDccFunction(para, opsFlag);
            break;
        case RSP_ArmConst::CMD_ANALY_OPEN_DCC:
            result = CmdAnalyOpenDccResult(para, cpBuf, wSize, opsFlag);
            break;
            
        default:
            break;
        }

        //释放资源
        if(NULL != para)
        {
            delete para;
            para = NULL;
        }

        //如果命令执行失败，那么清理命令序列和参数序列，并且执行最后一条命令
        if(result == FALSE)
        {
            sysLoger(LINFO, "RSP_TcpServer::ExcuteCommand:Excute the command %d failure!", nextCmd);
            opsFlag = FALSE;
        }

        if(opsFlag == FALSE)
        {
            continue;
        }

        //如果该条命令需要和ICE交互，那么退出循环等待ICE回复
        //否则，继续执行下一条命令
        if(nextCmd > RSP_ArmConst::CMD_NEED_ICE)
        {
            continue;
        }
        else
        {
            m_waitForIce = TRUE;
            break;
        }
    }

    return SUCC;
}

/*
* @brief
*    从命令序列里面取出下一条命令\n
* @return  命令序号，如果没有返回失败
*/
int RSP_TcpServer::GetNextCmd()
{
    //加锁保护
    QMutexLocker locker(&m_cmdMutex);

    if (m_atomCmds.isEmpty())
    {
        return FAIL;
    }

    return m_atomCmds.takeFirst();
}

/*
* @brief
*    装载一条命令到命令序列里面\n
* @para [in] wCmd 命令序号
* @return  命令序号，如果没有返回失败
*/
T_VOID RSP_TcpServer::PushBackCmd(T_WORD wCmd)
{
    //加锁保护
    QMutexLocker locker(&m_cmdMutex);

    m_atomCmds.push_back(wCmd);

}
/*
* @brief
*    查看命令序列的大小\n
* @return  命令序列的大小
*/
T_WORD RSP_TcpServer::CmdSize()
{
    //加锁保护
    QMutexLocker locker(&m_cmdMutex);
    return m_atomCmds.size();
}

/*
* @brief
*    从参数序列里面取出第一组参数\n
* @return  参数值，如果没有返回失败
*/
QList<T_VOID*> * RSP_TcpServer::GetNextPara()
{
    //加锁保护
    QMutexLocker locker(&m_cmdMutex);

    if (m_atomParas.isEmpty())
    {
        return NULL;
    }

    return m_atomParas.takeFirst();

}

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<命令序列方法实现>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


/*
* @brief
*    写内存,对应命令序列CMD_WRITE_MEM\n
* @param[in] tpArg  写内存所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdWriteIceMemory(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pSize = static_cast<T_WORD*>(tpArg->takeFirst());
    T_CHAR *pData = static_cast<T_CHAR*>(tpArg->takeFirst());

    //如果标记为FALSE，表明只需要清理资源，不需要执行操作
    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdWriteIceMemory:Addr:%x, Length:%d, Value:%x", 
                                    *pAddr, *pSize, *pData);
        m_lambdaIce.WriteMemory(*pAddr, pData, 0, *pSize);
    }
    //释放资源
    delete pAddr;
    delete pSize;
    delete pData;
    return TRUE;
}

/*
* @brief
*    写整型参数内存,对应命令序列CMD_WRITE_INT_MEM\n
* @param[in] tpArg  写内存所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdWriteIntMemory(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pValue = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pWidth = NULL;
    T_WORD *delay;
    if(tpArg->isEmpty())
    {
        pWidth = new T_WORD(32);
    }
    else
    {
        pWidth = static_cast<T_WORD*>(tpArg->takeFirst());    
    }
    if(tpArg->isEmpty())
    {
        delay = new T_WORD(0);
    }
    else
    {
        delay = static_cast<T_WORD*>(tpArg->takeFirst());
    }

    if(bFlag)
    {
        //根据目标机大小端转换buf
        T_CHAR buf[4];
        Int2Char(*pValue, buf, m_targetEndian);
        sysLoger(LINFO, "RSP_TcpServer::CmdWriteIntMemory:Addr:%x,Value:%x, delay:%d, width:%dbit", 
                                    *pAddr, *pValue, *delay, *pWidth);
        //m_lambdaIce.WriteMemoryWithDelay(*pAddr, buf, 0, 4, *delay);
        m_lambdaIce.WriteMemory(*pAddr, buf, 0, *pWidth/8);
    }

    //释放资源
    delete pAddr;
    delete pValue;
    delete delay;
    delete pWidth;
    return TRUE;

}

T_BOOL RSP_TcpServer::CmdCheckSbp(QList<T_VOID*> *tpArg,  T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        if((cpBuf[0] == 'E') && (wSize == FAIL_ACK_LEN))
        {
            return FALSE;
        }

        T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());


        int readValue = Char2Int(&cpBuf[4], m_targetEndian);
        sysLoger(LINFO, "RSP_TcpServer::CmdCheckSbp:Addr:%x,Value:%x.", 
                                    *pAddr, readValue);
        if(readValue != this->m_bpMem)
        {
            //2006.6.15，发现IS偶尔会出现打不上断点的问题，即写下去的内存内容和读上来的内存内容不匹配
            //因此引入插入断点重试机制，初始化重试次数m_retryTimes ＝ 5，5次重试失败后，再回复失败信息给GDB
            if(m_retryTimes > 0)
            {
                //重试次数减小一次
                m_retryTimes--;

                //将回复GDB命令序列取出
                GetNextCmd();

                QList<T_VOID*> *para = NULL;
                para = GetNextPara();
                if(NULL != para)
                {
                    delete para;
                    para = NULL;
                }

                //压入插断点命令序列
                PushBackCmd(RSP_ArmConst::CMD_WRITE_INT_MEM);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_INSERT_SBP);
                PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                //压入参数序列
                //1写内存，需要参数: 地址 值
                QList<T_VOID*> *writeMem = new QList<T_VOID*>();
                T_WORD *pWriteMemAddr = new T_WORD(*pAddr);
                T_WORD *pWriteMemValue = new T_WORD(m_bpMem);
                T_WORD *pWriteMemWidth = new T_WORD(32);
                writeMem->push_back(pWriteMemAddr);
                writeMem->push_back(pWriteMemValue);
                writeMem->push_back(pWriteMemWidth);
                m_atomParas.push_back(writeMem);

                //2.解析执行结果，不需要参数
                QList<T_VOID*> *analyWriteMem = new QList<T_VOID*>();
                m_atomParas.push_back(analyWriteMem);

                //3读内存
                QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
                T_WORD *pReadAddr = new T_WORD(*pAddr);
                T_WORD *pReadSize = new T_WORD(4);
                readMemPara->push_back(pReadAddr);
                readMemPara->push_back(pReadSize);
                m_atomParas.push_back(readMemPara);

                //4检查软件断点是否插入成功
                QList<T_VOID*> *cmpMemPara = new QList<T_VOID*>();
                T_WORD *pCheckAddr = new T_WORD(*pAddr);
                cmpMemPara->push_back(pCheckAddr);
                m_atomParas.push_back(cmpMemPara);

                //5. 回复GDB结果，不需要参数
                QList<T_VOID*> *replyGdb = new QList<T_VOID*>();
                m_atomParas.push_back(replyGdb);

                return TRUE;
            }

            //复原m_retryTimes
            m_retryTimes = 5;
        
            //从断点表里面取出该断点
            assert((m_bpMap.contains(*pAddr)));
            m_bpMap.take(*pAddr);
            return FALSE;
        }
    }

    return TRUE;
}

/*
* @brief
*    延时操作,对应命令序列CMD_DELAY\n
* @param[in] tpArg  延时所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdDelay(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pTimeout = static_cast<T_WORD*>(tpArg->takeFirst());

    this->wait(*pTimeout);

    //释放资源
    delete pTimeout;

    return TRUE;
}

/*
* @brief
*    清空本地内存缓冲,
*/
T_VOID RSP_TcpServer::RefreshMemory()
{
    T_WORD size = m_addr2mem.size();

    if(size == 0)
    {
        return;
    }

    QList<T_WORD> keys;

    QMapIterator<T_WORD, RSP_Memory*> i(m_addr2mem);
    while (i.hasNext()) {
        i.next();
       keys.push_back(i.key());
    }

    for(T_WORD j = 0; j < size; j++)
    {
        RSP_Memory *mem = m_addr2mem.take(keys.value(j));
        if(NULL != mem)
        {
            m_memMan.Free(mem);
        }
    }

}

/*
* @brief
*    读内存,对应命令序列CMD_READ_MEM\n
* @param[in] tpArg  读内存所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdReadIceMemory(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pSize = static_cast<T_WORD*>(tpArg->takeFirst());

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdReadIceMemory:Addr:%x,length:%d.", 
                                    *pAddr, *pSize);
        m_lambdaIce.ReadMemory(*pAddr, *pSize);
    }

    //释放资源
    delete pAddr;
    delete pSize;
    return TRUE;
}

/*
* @brief
*    读寄存器,对应命令序列CMD_READ_REG\n
* @param[in] tpArg  读寄存器可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdReadIceRegisters(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdReadIceRegisters:Read all registers!");
        m_lambdaIce.ReadAllRegisters();
    }

    //没有资源可以释放
    return TRUE;

}

/*
* @brief
*    写寄存器,对应命令序列CMD_WRITE_REG\n
* @param[in] tpArg  写寄存器可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdWriteIceRegisters(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdWriteIceRegisters:Write all registers!");
        m_lambdaIce.WriteAllRegisters(m_currentRegs, RSP_ArmConst::ARM_REG_NUM);
    }

    //没有资源可以释放
    return TRUE;

}

/*
* @brief 
*    写CP15寄存器,对应命令序列CMD_INIT_CP15\n
* @param[in] tpArg  写寄存器可能所需要的参数 
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdWriteCp15Registers(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pNum = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pValue = static_cast<T_WORD*>(tpArg->takeFirst()); 

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdWriteCp15Registers:Write Cp15 register, No:%d, value:%x!",
                                        *pNum, *pValue);
        m_lambdaIce.WriteCp15Reg(*pNum, *pValue);
    }

    //释放资源
    delete pNum;
    delete pValue;
    return TRUE;
}

/*
* @brief 
*    读CP15寄存器,对应命令序列CMD_GET_CP15\n
* @param[in] tpArg  读寄存器可能所需要的参数 
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdReadCp15Registers(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pNum = static_cast<T_WORD*>(tpArg->takeFirst());
    if(NULL == pNum)
    {
        sysLoger(LINFO, "CmdReadCp15Registers:The paramenter is null!");
        return FALSE;
    }
    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdReadCp15Registers:Read Cp15 register, No:%d!", *pNum);
        m_lambdaIce.ReadCp15Reg(*pNum);
    }

    //释放资源
    delete pNum;
    return TRUE;
}


/*
* @brief 
*    运行目标机,对应命令序列CMD_RUN_TRT\n
* @param[in] tpArg  运行目标机可能所需要的参数 
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdRunTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdRunTarget:Continue target!");
        m_lambdaIce.ContinueTarget();
        SetTargetStatus(RSP_ArmConst::CORE_RUNNING);
    }

    //没有资源可以释放
    return TRUE;
}

/*
* @brief
*    停止目标机,对应命令序列CMD_STOP_TRT\n
* @param[in] tpArg  停止目标机可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdStopTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdStopTarget:Stop target!");
        m_lambdaIce.StopTarget();
    }

    //没有资源可以释放
    return TRUE;
}


/*
* @brief
*    检查ICE回复是否成功,对应命令序列CMD_ANALY_RESULT\n
* @param[in] cpBuf  ICE回复的数据
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdCheckReply(T_CHAR *cpBuf, T_BOOL bFlag)
{
    if(bFlag)
    {
        assert((NULL !=cpBuf));
        sysLoger(LDEBUG, "RSP_TcpServer::CmdCheckReply:ICE reply is %s!", cpBuf);
        if(cpBuf[0] == 'O' && cpBuf[1] == 'K')
        {
            return TRUE;
        }
        else
        if(cpBuf[0] == 'E')
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*
* @brief
*    通过ICE回复更新本地寄存器缓冲,对应命令序列CMD_UPDATE_READ_REG\n
* @param[in] cpBuf  ICE回复的数据
* @param[in] wSize  ICE回复的数据的大小
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdUpdateRegs(T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    if(bFlag)
    {
        assert((NULL != cpBuf));

        //格式, 成功data  失败E
        if(('E' == cpBuf[0]) && (wSize == FAIL_ACK_LEN))
        {
            sysLoger(LINFO, "RSP_TcpServer::CmdUpdateRegs:Read Registers failure!answer pack:%s",
                                        cpBuf);
            return FALSE;
        }
        else
        {
            if(wSize < 3)
            {
                sysLoger(LINFO, "RSP_TcpServer::CmdUpdateRegs:Read Registers failure!The recieved pack"\
                                           " was invalid.");
                return FALSE;
            }

            T_CHAR *ptr = &cpBuf[2];
            //得到长度
            T_WORD length = 0;
            Hex2int(&ptr, &length);
            if(wSize < length)
            {
                sysLoger(LINFO, "RSP_TcpServer::CmdUpdateRegs:Invalid answer pack!");
                return FALSE;
            }
            //跳过逗号
            ptr++;
            //得到寄存器的个数
            T_WORD regNum = length / 4;
            //根据大小端将数据转换为本地缓存的寄存器值
            for(T_WORD i = 0; i < regNum; i++)
            {
                sysLoger(LDEBUG, "RSP_TcpServer::CmdUpdateRegs: Update Register [%d] value from :%x"\
                                                "  to :%x.", i, m_currentRegs[i], Char2Int(ptr, TRUE));
                m_currentRegs[i] = Char2Int(ptr, TRUE); 
                ptr += 4;
            }            
            return TRUE;
        }
    }



    return TRUE;

}

/*
* @brief
*    通过ICE回复更新本地内存缓冲,对应命令序列CMD_UPDATE_READ_MEM\n
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] cpBuf  ICE回复的数据
* @param[in] wSize  ICE回复的数据的大小
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdUpdateMems(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pSize = static_cast<T_WORD*>(tpArg->takeFirst());

    if(bFlag)
    {
        //更新内存
        assert((NULL != cpBuf));

        if(cpBuf[0] == 'E')
        {
            //m_errLogger->write(1, QString("ICE回复"), QString("读内存失败"));

            delete pAddr;
            delete pSize;
            return FALSE;
        }
        //FIXME:没有做长度校验

        //计算内存块的个数
        T_WORD blockNum = *pSize / RSP_Memory::MEMORY_BLOCK_SIZE;
        T_WORD addr = *pAddr;

        T_CHAR *ptr = &cpBuf[0];
        T_CHAR *pdest = strstr(ptr,CONTROL_FLAG);
        
        //----------------------------------------------------------------
        //2006.06.19  发现回复命令可能只有OK，后面的内存内容并没有得到回复
        //因此增加对回复内容是否完整的检查，避免空指针异常
        if(pdest == NULL)
        {
            //m_errLogger->write(1, QString("ICE回复"), QString("回复的读内存结果包不正确，只有OK"));
            delete pAddr;
            delete pSize;
            return FALSE;
        }
        //-----------------------------------------------------------------
        pdest += strlen(CONTROL_FLAG);

        ptr = pdest;

        while(blockNum)
        {
            //如果已经有该内存块，则不用更新
            if(m_addr2mem.contains(addr))
            {
                blockNum--;
                continue;
            }

            RSP_Memory *memory = m_memMan.Alloc();
            if(memory == NULL)
            {
                //这里是不会出现这种情况的，如果出现了该情况，说明程序有问题，报错，退出，释放所有内存
                RefreshMemory();
                delete pAddr;
                delete pSize;
                return FALSE;
            }

            memory->SetBaseAddr(addr);
            memcpy(memory->GetValue(), ptr, RSP_Memory::MEMORY_BLOCK_SIZE);

            ptr += RSP_Memory::MEMORY_BLOCK_SIZE;
            m_addr2mem.insert(addr, memory);
            blockNum--;
            addr += RSP_Memory::MEMORY_BLOCK_SIZE;
        }
    }

    //释放资源
    delete pAddr;
    delete pSize;

    return TRUE;
}

/*
* @brief
*    回复GDB读内存命令,对应命令序列CMD_REPLY_READ_MEM\n
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdReplyMems(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize,T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(tpArg->isEmpty())
    {
        return FALSE;
    }

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pSize = static_cast<T_WORD*>(tpArg->takeFirst());

    if(bFlag)
    {
        T_CHAR sendBuf[PACKET_DATA_SIZE] = "\0", *pBuf = NULL;
        int ret = 0;
        if(GetMop())
        {
            ret = GetLocalMemory(*pAddr, *pSize, sendBuf, 0);
            //因为这个命令之前一定会执行read memory，因此此处如果ret <= 0，肯定是代码有错误
            assert((ret > 0));
        }
        else
        {
            /*读内存错误*/
            if((cpBuf[0] == 'E') && ( wSize == FAIL_ACK_LEN))
            {
                delete pAddr;
                delete pSize;
                PutData("E03", sizeof("E03"));
                return FALSE;
            }
            /*回复格式OK/Exx len, value*/
            T_CHAR *ptr = &cpBuf[0];
            //ok
            ptr += strlen("OK");
            //len,
            T_CHAR *pdest = strstr(ptr,CONTROL_FLAG);
            if(pdest == NULL)
            {
                delete pAddr;
                delete pSize;
                PutData("E03", sizeof("E03"));
                return FALSE;
            }            
            T_WORD len = 0;
            hex2int(&ptr, &len);
            ptr = pdest + CONTROL_FLAG_LEN;
            pBuf = sendBuf;
            for(int i = 0; i < min(*pSize, len); i++)
            {
                pBuf += Int2Hex(pBuf, static_cast<int>(*(ptr+i)), 2);
            }
            ret = pBuf - sendBuf;
        }
        PutData(sendBuf, ret);
    }

    m_waitForIce = FALSE;

    //释放资源
    delete pAddr;
    delete pSize;

    return TRUE;
}

/*
* @brief
*    回复GDB简单命令,对应命令序列CMD_SIMPLE_REPLY_GDB\n
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdSimpleReply(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        PutData("OK", strlen("OK"));

    }
    else
    {
        PutData("E03", strlen("E03"));

    }

    m_waitForIce = FALSE;

    return TRUE;
}

/*
* @brief
*    回复GDB S05,对应命令序列CMD_REPLY_S05\n
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdReplyS05(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    m_CurCmdSource = SOCKET_DATA;
    if(bFlag)
    {
        if(m_NeedReplyS05 == TRUE)
        {
            PutData("S05", strlen("S05"));
        }

    }

    m_waitForIce = FALSE;

    return TRUE;

}


/*
* @brief
*    设置目标机状态可用,对应命令序列CMD_SET_TRT_STATUS\n
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdUpdateStatus(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        SetTargetStatus(RSP_ArmConst::CORE_DEBUG_BKWTPT);
    }
    else
    {
        SetTargetStatus(RSP_ArmConst::CORE_RUNNING);
    }

    //更新CPSR
    if(m_cpsr != -1)
    {
        m_currentRegs[RSP_ArmConst::JTAG_CPSR] = m_cpsr;
    }
    m_waitForIce = FALSE;

    return TRUE;
}

/*
* @brief 
*    根据ICE回复更新目标机状态，并推动流程发展CMD_UPDATE_TRT_STATUS\n
* @param[in] tpArg  更新目标机可能所需要的参数 
* @return  
*/
T_BOOL RSP_TcpServer::CmdQueryTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(bFlag)
    {
        QueryTarget();
    }

    return TRUE;
}


/*
* @brief
*    插入硬件断点,对应命令序列CMD_INSERT_HBP\n
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdInsertHardwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pLength = static_cast<T_WORD*>(tpArg->takeFirst());


    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdInsertHardwareBp:Insert hardware breakpoint, addr:%x, length:%d",
                                        *pAddr, *pLength);
        //m_lambdaIce.InsertBreakPoint(*pAddr, *pLength);
        m_lambdaIce.InsertBreakPointORWatchPoint(HARD_BP, *pAddr, *pLength);
    }

    //设置硬件断点已经设置
    m_hardBp = TRUE;

    //释放资源
    delete pAddr;
    delete pLength;

    return TRUE;
}

/*
* @brief
*    删除硬件断点,对应命令序列CMD_REMOVE_HBP\n
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdRemoveHardwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pLength = static_cast<T_WORD*>(tpArg->takeFirst());


    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdRemoveHardwareBp:Remove hardware breakpoint, addr:%x, length:%d",
                                        *pAddr, *pLength);
        m_lambdaIce.DeleteBreakPointORWatchPoint(HARD_BP, *pAddr, *pLength);
    }

    m_hardBp = FALSE;

    //释放资源
    delete pAddr;
    delete pLength;

    return TRUE;

}

/*
* @brief
*    插入硬件观察点,对应命令序列CMD_INSERT_WBP\n
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdInsertWatchPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pWidth = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pAccess = static_cast<T_WORD*>(tpArg->takeFirst());


    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdInsertWatchPoint:Insert watch breakpoint, addr:%x, length:%d, Access:%x",
                                        *pAddr, *pWidth, *pAccess);
        m_lambdaIce.InsertBreakPointORWatchPoint(*pAccess, *pAddr, *pWidth);
    }

    //设置硬件断点已经设置
    m_hardBp = TRUE;

    //释放资源
    delete pAddr;
    delete pWidth;
    delete pAccess;

    return TRUE;

}

/*
* @brief
*    删除硬件观察点,对应命令序列CMD_REMOVE_WBP\n
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdRemoveWatchPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pWidth = static_cast<T_WORD*>(tpArg->takeFirst());
    T_WORD *pAccess = static_cast<T_WORD*>(tpArg->takeFirst());


    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdRemoveWatchPoint:Remove watch breakpoint, addr:%x, length:%d, Access:%x",
                                        *pAddr, *pWidth, *pAccess);
        m_lambdaIce.DeleteBreakPointORWatchPoint(*pAccess, *pAddr, *pWidth);
    }

    m_hardBp = FALSE;

    //释放资源
    delete pAddr;
    delete pWidth;
    delete pAccess;

    return TRUE;

}

/*
* @brief
*    插入软件断点,对应命令序列CMD_INSERT_SBP\n
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdInsertSoftwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    T_WORD *pAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    T_CHAR buf[4];
    memset(buf,0,4);

    if(bFlag)
    {
        int ret = 0;
        if(GetMop())
        {
           ret = GetLocalMemory(*pAddr, 4, buf, 1);
            assert((ret > 0));
        }
        else
        {
            sysLoger(LDEBUG, "RSP_TcpServer::CmdInsertSoftwareBp:Insert breakpoint, push READ_MEM, ANALY_BP_READ_MEM command");
            PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
            PushBackCmd(RSP_ArmConst::CMD_ANALY_BP_READ_MEM);
            //装载读内存参数
            QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
            T_WORD *pReadAddr = new T_WORD(*pAddr);
            T_WORD *pReadSize = new T_WORD(4);//一个整数的大小
            readMemPara->push_back(pReadAddr);
            readMemPara->push_back(pReadSize);
            //装载分析读内存结果并插入断点的参数
            QList<T_VOID*> *analyPara = new QList<T_VOID*>();
            T_WORD *pBpAddr = new T_WORD(*pAddr);
            analyPara->push_back(pBpAddr);
            //装载参数列表
            m_atomParas.push_back(readMemPara);
            m_atomParas.push_back(analyPara);            
        }
    }

    //释放资源
    delete pAddr;
    return TRUE;
}


/*
* @brief
*    查询ICE的大小端
* @param[in] tpArg  更新内存缓冲可能所需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdQueryIceEndian(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdQueryIceEndian:Query ICE endian!");
        m_lambdaIce.QueryTargetEndian(m_iceId);
    }

    return TRUE;
}

/*
* @brief
*    创建ICE会话
* @param[in] tpArg  创建会话可能需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdCreateSession(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdCreateSession:Create session !%s", m_IceBuf.toAscii().data());
        m_lambdaIce.CreateSeesion(m_IceBuf.toAscii().data(), m_IceBuf.length()+1, m_iceId);
    }

    return TRUE;
}
/*
* @brief
*   删除ICE会话
* @param[in] tpArg 删除会话可能需要的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdDeleteSession(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdDeleteSession:Delete session !");
        m_lambdaIce.DeleteSeesion(m_iceId);
    }

    return TRUE;
}
/*
* @brief
*    分析查询ICE大小端的结果
* @param[in] cpBuf  查询结果信息
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdAnalyQueryICEEndian(char *cpBuf, T_BOOL bFlag)
{
    if(NULL == cpBuf)
    {
        return FALSE;
    }
    if(bFlag)
    {
        if(cpBuf[0] == 'l')
        {
            m_iceEndian = FALSE;
        }
        else
        {
            m_iceEndian = TRUE;
        }
    }
    m_waitForIce = FALSE;
    return TRUE;
}

/*
* @brief
*    分析创建会话的结果
* @param[in] cpBuf  创建会话结果信息
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdAnalyCreateSession(char *cpBuf, T_BOOL bFlag)
{
    if(NULL == cpBuf)
    {
        return FALSE;
    }
    if(bFlag)
    {
        if((cpBuf[0] == 'O') && (cpBuf[1] == 'K'))
        {
            //char *pBuf = &cpBuf[2];
            //Hex2int(&(pBuf), &m_sessId);
            m_sessId = 11;          //目前固定为11
        }
        else
        {
            m_sessId = -1;
        }
    }
    m_waitForIce = FALSE;
    return TRUE;
}

/*
* @brief
*    分析删除会话的结果
* @param[in] cpBuf  删除会话结果信息
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdAnalyDeleteSession(char *cpBuf, T_BOOL bFlag)
{
    if(NULL == cpBuf)
    {
        return FALSE;
    }
    if(bFlag)
    {
        if((cpBuf[0] == 'O') && (cpBuf[1] == 'K'))
        {
            //char *pBuf = &cpBuf[2];
            //Hex2int(&(pBuf), &m_sessId);
            m_sessId = -1;
        }
        else
        {
            m_sessId = -1;
        }
    }
    m_waitForIce = FALSE;
    return TRUE;
}

/*
* @brief
*    重启目标机
* @param[in] tpArg  重启目标机的参数(重启类型)
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdResetTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(bFlag)
    {
        if(tpArg->isEmpty())
        {
            return FALSE;
        }    
        T_UWORD *pType = static_cast<T_UWORD*>(tpArg->takeFirst());
        if(pType == NULL)
        {
            return FALSE;
        }
        sysLoger(LINFO, "RSP_TcpServer::CmdResetTarget: Reset target, type:%d", *pType);
        m_lambdaIce.ReSetTarget(*pType);
        delete pType;
    }

    return TRUE;
}

/*
* @brief
*    分析重启目标机结果
* @param[in] cpBuf  重启目标机的结果数据
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdAnalyResetTarget(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != cpBuf));
    assert((NULL != tpArg));
    
    if(tpArg->size() < 2)
    {
        return FALSE;
    }
    T_HWORD *hwSrcAid = static_cast<T_HWORD*>(tpArg->takeFirst());
    T_HWORD *hwSrcSaid = static_cast<T_HWORD*>(tpArg->takeFirst());
    if((hwSrcAid == NULL) || (hwSrcSaid == NULL))
    {
        if(hwSrcAid)
           delete hwSrcAid;
        if(hwSrcSaid)
           delete hwSrcAid;
        
        return FALSE;
    }
    if(bFlag)
    {
        if(strlen(cpBuf) < SUCC_ACK_LEN)
        {
            delete hwSrcAid;
            delete hwSrcSaid;
            return FALSE;
        }
        SendIDEResult(cpBuf, strlen(cpBuf), *hwSrcAid, *hwSrcSaid);
        
    }
    else
    {
        SendIDEResult("E03", strlen("E03"), *hwSrcAid, *hwSrcSaid);
    }
    m_waitForIce = FALSE;
    
    delete hwSrcAid;
    delete hwSrcSaid;
    return TRUE;
}

/*
* @brief
*    重启目标机
* @param[in] tpArg  重启目标机的参数(重启类型)
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdQueryTargetCpuIDCode(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdQueryTargetCpuIDCode: Query CPU ID CODE");
        m_lambdaIce.QueryCPUIDCode();
    }
    return TRUE;
}

/*
* @brief
*    重启目标机
* @param[in] tpArg  重启目标机的参数(重启类型)
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdAnalyQueryTargetCpuIDCode(QList<T_VOID*> *tpArg, 
                                                                                    T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != cpBuf));
    assert((NULL != tpArg));
    if(tpArg->size() < 2)
    {
        return FALSE;
    }
    T_HWORD *pSrcAid = static_cast<T_HWORD*>(tpArg->takeFirst());
    T_HWORD *pSrcSaid = static_cast<T_HWORD*>(tpArg->takeFirst());
    if((pSrcAid == NULL) || (pSrcSaid == NULL))
    {
        if(pSrcAid)
           delete pSrcAid;
        if(pSrcSaid)
           delete pSrcSaid;        
        return FALSE;
    }
    if(bFlag)
    {
        if(strlen(cpBuf) < min(SUCC_ACK_LEN, FAIL_ACK_LEN))
        {
            SendIDEResult(ERR_SYSTEM_ERROR, FAIL_ACK_LEN, *pSrcAid, *pSrcSaid);            
            delete pSrcAid;
            delete pSrcSaid;
            return FALSE;
        }
        SendIDEResult(cpBuf, wSize, *pSrcAid, *pSrcSaid);
    }
    else
    {
        SendIDEResult(ERR_SYSTEM_ERROR, FAIL_ACK_LEN, *pSrcAid, *pSrcSaid);
    }
    delete pSrcAid;
    delete pSrcSaid;
    return TRUE;
}

/*
* @brief
*    分析插入断点时读内存的结果，并插入断点
* @param[in] tpArg  插入断点读内存的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdAnalyBreakPointReadMem(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != cpBuf));
    assert((NULL != tpArg));
    if(tpArg->size() < 1)
    {
        return FALSE;
    }
    T_WORD *pAddr =  static_cast<T_WORD*>(tpArg->takeFirst());
    if(NULL == pAddr)
    {
        return FALSE;
    }
    sysLoger(LDEBUG, "CmdAnalyBreakPointReadMem:Recived read breakpoint mem:");
    if(bFlag)
    {        
        if((cpBuf[0] == 'E') && (wSize == FAIL_ACK_LEN))
        {
            sysLoger(LDEBUG,"CmdAnalyBreakPointReadMem: ICE read breakpoint memory failure!");
            delete pAddr;
            return FALSE;
        }
        /*回复格式OK/Exx len, value*/
        T_CHAR *ptr = &cpBuf[0];
        //ok/exx
        ptr += strlen("OK");
        
        //len,
        T_CHAR *pdest = strstr(ptr,CONTROL_FLAG);
        if(pdest == NULL)
        {
            sysLoger(LDEBUG,"CmdAnalyBreakPointReadMem: ICE read breakpoint memory , lack of len part!");
            delete pAddr;
            return FALSE;
        }
        T_WORD len = 0;
        hex2int(&ptr, &len);
        ptr = pdest + CONTROL_FLAG_LEN;
        //value
        T_CHAR buf[10] = "\0";
        memcpy(buf, ptr, min(4, len));
        //本地内存有效
        T_WORD value = Char2Int(buf, m_targetEndian);
        m_bpMap.insert(*pAddr, value);
        sysLoger(LINFO, "RSP_TcpServer::CmdInsertSoftwareBp:Insert soft breakpoint, addr:%x, value:%x",
                                        *pAddr, value);
    }
    delete pAddr;
    return TRUE;
}

/*
* @brief
*    分析PC寄存器的值，如果等于SIMIHOST注册的地址值，则上报SWI
* @param[in] tpArg  插入断点读内存的参数
* @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
* @return  成功或者失败
*/
T_BOOL RSP_TcpServer::CmdAnalySimihostBreakPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    
    m_NeedReplyS05 = TRUE;
    
    if(bFlag == TRUE)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdAnalySimihostBreakPoint: PC register value:%x, simihost registered value:%x!",
                        m_currentRegs[RSP_ArmConst::JTAG_R15], m_SimiPCAddr);
        if(m_currentRegs[RSP_ArmConst::JTAG_R15] == m_SimiPCAddr)
        {
            SendIDEResult("SWI", strlen("SWI"), m_SimiAid, m_SimiSaid);
            m_NeedReplyS05 = FALSE;
        }
    }

    return TRUE;
}

/**
 * @Funcname:  CmdOpenDccFunction
 * @brief        :  启用DCC功能
 * @para1      : [IN] tpArg DCC功能的地址
 * @para2      : [IN] bFlag  是否真正执行操作
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月18日 15:45:30
 *   
**/
T_BOOL RSP_TcpServer::CmdOpenDccFunction(QList < T_VOID * > * tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));
    if(tpArg->size() < 1)
    {
        return FALSE;
    }
    T_WORD *pDccAddr = static_cast<T_WORD*>(tpArg->takeFirst());
    if(NULL == pDccAddr)
    {
        return FALSE;
    }
    if(bFlag)
    {
        sysLoger(LINFO, "RSP_TcpServer::CmdOpenDccFunction:Open Dcc function, dcc starting address:%x", *pDccAddr);
        m_lambdaIce.OpenDccFunction(*pDccAddr);
    }
    delete pDccAddr;
    return TRUE;
}

/**
 * @Funcname:  CmdAnalyOpenDccResult
 * @brief        :  分析打开DCC功能的结果
 * @para1      : [IN]tpArg, 参数列表
 * @para2      : [IN]cpBuf, 收到ICE回复的结果
 * @para3      : [IN]wSize, 回复结果的长度
 * @para4      : [IN]bFlag, 是否真正执行操作
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月18日 16:04:18
 *   
**/
T_BOOL RSP_TcpServer::CmdAnalyOpenDccResult(QList < T_VOID * > * tpArg, T_CHAR * cpBuf, T_WORD wSize, T_BOOL bFlag)
{
    assert((NULL != cpBuf));
    assert((NULL != tpArg));

    if(bFlag)
    {
        if(cpBuf[0] == 'E' && wSize == FAIL_ACK_LEN)
        {
            sysLoger(LINFO, "RSP_TcpServer::CmdAnalyOpenDccResult: Open Dcc function failure!");
            return FALSE;
        }
        else if(strcmp(cpBuf, "OK") == 0)
        {
            sysLoger(LINFO, "RSP_TcpServer::CmdAnalyOpenDccResult: Open Dcc function success!");
            return TRUE;
        }
        sysLoger(LINFO, "RSP_TcpServer::CmdAnalyOpenDccResult: unknow data recived from ICE: %s!", cpBuf);
        return FALSE;
    }
    
    return TRUE;
}
/*
* @brief
*    根据处理器的当前状态设置寄存器\n
* @param[in] regNo   要写的寄存器号
* @param[in] value   寄存器的值
* @return  SUCC或FAIL
*/
T_BOOL RSP_TcpServer::WriteSingleRegister(T_WORD regNo, T_WORD value)
{

    sysLoger(LINFO, "RSP_TcpServer::WriteSingleRegister: Set the register [no:%d] as %x", regNo, value);

    //判断处理器模式
    if(((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f)  == RSP_ArmConst::ARM_USER_MODE)
        || ((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SYSTEM_MODE))
    {
        //用户或系统模式下
        if(waCurUsrRegs[regNo] == -1)
        {
            return TRUE;  //查表为-1表示当前ICE不支持该赋值，但可返回正确
        }
        m_currentRegs[waCurUsrRegs[regNo]] = value;
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_FIQ_MODE)
    {
        //FIQ模式下
        if(waCurFiqRegs[regNo] == -1)
        {
            return TRUE;  //查表为-1表示当前ICE不支持该赋值，但可返回正确
        }
        m_currentRegs[waCurFiqRegs[regNo]] = value;

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_IRQ_MODE)
    {
        //IRQ模式下
        if(waCurIrqRegs[regNo] == -1)
        {
            return TRUE;  //查表为-1表示当前ICE不支持该赋值，但可返回正确
        }
        m_currentRegs[waCurIrqRegs[regNo]] = value;

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SUPER_MODE)
    {
        //特权模式下
        if(waCurSvcRegs[regNo] == -1)
        {
            return TRUE;  //查表为-1表示当前ICE不支持该赋值，但可返回正确
        }
        m_currentRegs[waCurSvcRegs[regNo]] = value;

    }
    else
        if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_ABORT_MODE)
    {
        //ABT模式下
        if(waCurAbtRegs[regNo] == -1)
        {
            return TRUE;  //查表为-1表示当前ICE不支持该赋值，但可返回正确
        }
        m_currentRegs[waCurAbtRegs[regNo]] = value;

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_UNDEFINED_MODE)
    {
        //未定义模式下
        if(waCurUndRegs[regNo] == -1)
        {
            return TRUE;  //查表为-1表示当前ICE不支持该赋值，但可返回正确
        }
        m_currentRegs[waCurUndRegs[regNo]] = value;

    }
    else
    {
        //如果不符合以上模式
        return FALSE;
    }

    return TRUE;
}

/*
* @brief
*    根据处理器模式读取单个寄存器\n
* @param[in] wRegNo    寄存器号
* @return  成功则返回寄存器的值，失败则返回FAIL
*/
T_WORD RSP_TcpServer::ReadSingleRegister(T_WORD wRegNo)
{
        //判断处理器模式
    sysLoger(LINFO, "RSP_TcpServer::ReadSingleRegister: Read the register [no:%d] .", wRegNo);
    if(((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_USER_MODE)
        || ((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SYSTEM_MODE))
    {
        //用户或系统模式下
        if(waCurUsrRegs[wRegNo] == -1)
        {
            return 0;  //查表为-1表示当前ICE不支持该赋值，但可返回0
        }
        return m_currentRegs[waCurUsrRegs[wRegNo]];
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_FIQ_MODE)
    {
        //FIQ模式下
        if(waCurFiqRegs[wRegNo] == -1)
        {
            return 0;  //查表为-1表示当前ICE不支持该赋值，但可返回0
        }
        return m_currentRegs[waCurFiqRegs[wRegNo]];

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_IRQ_MODE)
    {
        //IRQ模式下
        if(waCurIrqRegs[wRegNo] == -1)
        {
            return 0;  //查表为-1表示当前ICE不支持该赋值，但可返回0
        }
        return m_currentRegs[waCurIrqRegs[wRegNo]];

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SUPER_MODE)
    {
        //特权模式下
        if(waCurSvcRegs[wRegNo] == -1)
        {
            return 0;  //查表为-1表示当前ICE不支持该赋值，但可返回0
        }
        return m_currentRegs[waCurSvcRegs[wRegNo]];

    }
    else
        if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_ABORT_MODE)
    {
        //ABT模式下
        if(waCurAbtRegs[wRegNo] == -1)
        {
            return 0;  //查表为-1表示当前ICE不支持该赋值，但可返回0
        }
        return m_currentRegs[waCurAbtRegs[wRegNo]];

    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_UNDEFINED_MODE)
    {
        //未定义模式下
        if(waCurUndRegs[wRegNo] == -1)
        {
            return 0;  //查表为-1表示当前ICE不支持该赋值，但可返回0
        }
        return m_currentRegs[waCurUndRegs[wRegNo]];

    }
    else
    {
        //如果不符合以上模式
        return 0;
    }
}

/*
* @brief
*    根据处理器模式读取所有寄存器\n
* @param[in] wRegs    寄存器数组
* @return  成功则返回SUCC，失败则返回FAIL
*/
T_BOOL RSP_TcpServer::ReadAllRegisters(T_WORD *wRegs)
{
            //判断处理器模式
    sysLoger(LINFO, "RSP_TcpServer::ReadAllRegisters:Enter the function!");
    if(((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_USER_MODE)
        || ((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SYSTEM_MODE))
    {
        //用户或系统模式下
        GetRegs(wRegs, waCurUsrRegs);
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_FIQ_MODE)
    {
        //FIQ模式下
        GetRegs(wRegs, waCurFiqRegs);
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_IRQ_MODE)
    {
        //IRQ模式下
        GetRegs(wRegs, waCurIrqRegs);
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_SUPER_MODE)
    {
        //特权模式下
        GetRegs(wRegs, waCurSvcRegs);
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_ABORT_MODE)
    {
        //ABT模式下
        GetRegs(wRegs, waCurAbtRegs);
    }
    else
    if((m_currentRegs[RSP_ArmConst::JTAG_CPSR] & 0x1f) == RSP_ArmConst::ARM_UNDEFINED_MODE)
    {
        //未定义模式下
        GetRegs(wRegs, waCurUndRegs);
    }
    else
    {
        //如果不符合以上模式
        return FALSE;
    }

    return TRUE;

}

/*
* @brief
*    根据输入的描述限定表取出对应的寄存器\n
* @param     waRegs    调用者提供的寄存器数组缓冲
* @param[in] wTable    调用者提供的寄存器映射表
* @return
*/
T_VOID RSP_TcpServer::GetRegs(T_WORD *waRegs, T_CONST T_WORD *waTable)
{
    for(int i = 0; i < RSP_ArmConst::RSP_REG_NUM; i++)
    {
        if( -1 == waTable[i])
        {
            waRegs[i] = 0;
            continue;
        }
        waRegs[i] = m_currentRegs[waTable[i]];
    }
}

/*
* @brief
*    将一个整型根据大小端转为CHAR数组\n
* @param[in] wReg    调用者提供的寄存器数值
* @param[in] cpBuf   调用者提供的缓冲
* @param[in] bIsBigEndian  调用者提供的大小端信息
* @return
*/
T_VOID RSP_TcpServer::Int2Char(T_WORD wReg, T_CHAR *cpBuf, T_BOOL bIsBigEndian)
{
    if(bIsBigEndian)
    {
        *cpBuf = ((wReg >> 24) & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 16) & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 8)  & 0xff);
        cpBuf++;
        *cpBuf = (wReg & 0xff);
        }
    else
    {
        *cpBuf = (wReg & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 8)  & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 16) & 0xff);
        cpBuf++;
        *cpBuf = ((wReg >> 24) & 0xff);
    }
}

/*
* @brief
*    将一个CHAR数组根据大小端转为整型\n
* @param[in] cpBuf   调用者提供的缓冲
* @param[in] bIsBigEndian  调用者提供的大小端信息
* @return 该整型
*/
T_WORD RSP_TcpServer::Char2Int(T_CHAR *cpBuf, T_BOOL bIsBigEndian)
{
    if(bIsBigEndian)
    {
        return    (*(cpBuf + 3) & 0xff)       |
                (*(cpBuf + 2) & 0xff) <<  8 |
                (*(cpBuf + 1) & 0xff) << 16 |
                (*cpBuf)       << 24;

    }
    else
    {
        return  (*(cpBuf + 3))        << 24 |
                  (*(cpBuf + 2) & 0xff) << 16 |
                  (*(cpBuf + 1) & 0xff) <<  8 |
                  (*cpBuf & 0xff);
    }
}


/*
* @brief
*    阻塞在服务器接收GDB的数据，收到数据后\n
*    将数据拷贝到调用者提供的缓冲区后返回\n
* @param     cpBuf    调用者提供的数据缓冲
* @param[in] wSize    调用者提供的缓冲区的大小
* @return  实际接收的数据的大小，网络断开返回-1
*/
int RSP_TcpServer::GetData(T_CHAR *cpBuf, T_WORD wSize)
{

    //阻塞接收GDB命令
    T_CHAR head;
    T_WORD offset = 0;

    while(TRUE)
    {
        //检查SOCKET合法性
        if (NULL == m_sClient || NULL == cpBuf)
        {
            sysLoger(LDEBUG, "RSP_TcpServer::GetData:m_sClient or cpBuf is bad pointer!FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            return FAIL;
        }


        offset = 0;
        int ret = m_sClient->Recv(&head, 1);

        //如果网络断开，退出循环
        if(ret < 0)
        {
            sysLoger(LDEBUG, "RSP_TcpServer::GetData:m_sClient->Recv return failure!FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            return FAIL;
        }

        //如果接收的头字符不是$，则继续接收
        if('$'!= head)
        {
            continue;
        }

        //接收数据直到#
        ret = m_sClient->Recv(&head, 1);
        while((offset < wSize) && (FAIL != ret))
        {
            if('#' == head)
            {
                break;
            }
            cpBuf[offset++] = head;
            ret = m_sClient->Recv(&head, 1);
        }

        if(ret < 0)
        {
            sysLoger(LDEBUG, "RSP_TcpServer::GetData:m_sClient->Recv return failure!FILE:%s, LINE:%d", __MYFILE__, __LINE__);
            return FAIL;
        }

        if(offset > wSize)
        {
            continue;
        }

        //应答，因为是TCP接收，故没有进行校验
        m_sClient->Send("+", strlen("+"));
        //sysLoger(LDEBUG, "RSP_TcpServer::GetData:Recieved data: %s", cpBuf);
        return offset;
    }
}

/*
* @brief
*    将回复数据发送给GDB\n
* @param     cpBuf    调用者要发送的数据缓冲
* @param[in] size    调用者要发送数据的大小
* @return  实际发送的数据的大小
*/
int RSP_TcpServer::PutData(T_CHAR *cpBuf, T_WORD wSize)
{

    T_WORD i = 0;
    T_UBYTE *sendBuf;
    T_UBYTE checkSum = 0;
    T_WORD    count = wSize;
    int result = FAIL;
    //根据消息来源，将操作执行结果按源来源发送回复
    if(m_CurCmdSource == TSAPI_DATA)
    {
        SendIDEResult(cpBuf, wSize, m_CurSrcAid, m_CurSrcSaid);
    }
    else if(m_CurCmdSource == SOCKET_DATA)
    {
        if(m_sClient == NULL || cpBuf == NULL)
        {
            return FAIL;
        }

        //日志
#ifdef IS_PACK_LOG
        if(TRUE == IsLog())
        {
            m_logger.write(SEND_PACK, cpBuf, wSize);
        }        
#endif

        //分配发送缓冲区
        sendBuf = new T_UBYTE[wSize+10];

        if(sendBuf == NULL)
        {
            return FAIL;
        }

        memset(sendBuf, 0, wSize + 10);
        //构造包头
        sendBuf[0] = '$';

        //求校验和
        while(count--)
        {
            checkSum += static_cast<int>(cpBuf[i]);
            sendBuf[i+1] = static_cast<T_UBYTE>(cpBuf[i]);
            i++;
        }
        //构造包尾
        sendBuf[wSize+1] = '#';
        sendBuf[wSize+2] = vcaHexchars[(checkSum >> 4) & 0xf];
        sendBuf[wSize+3] = vcaHexchars[checkSum & 0xf];
        //发送数据
        //--------------------------------------------------------------------------
        //2006.06.19 在发送数据前对SOCKET的合法性进行检验，避免SOCKET删除瞬间，发送线程仍在调用发送方法发送数据
        m_clientMutex.lock();
        if(m_sClient != NULL)
        {
            result = m_sClient->Send(reinterpret_cast<T_CHAR *>(sendBuf),wSize+4);
        }
        m_clientMutex.unlock();



        delete []sendBuf;
        return result;
    }
    else
    {
        sysLoger(LINFO, "RSP_TcpServer::PutData:Invalid msg source!s");
    }
    return result;
}

/*
* @brief
*    将给定的16进制字符串转换为整型,需要注意的是该操作会移动给定缓冲指针的地址\n
* @param[in]     cptr    16进制字符串
* @param[out] wIntValue 转换的INT数据
* @return  指针移动的字节数
*/
T_UWORD RSP_TcpServer::Hex2int(T_CHAR **cptr, T_WORD *wIntValue)
{
  int numChars = 0;
  int hexValue;

  *wIntValue = 0;

  while (**cptr)
    {
      hexValue = Hex2num(**cptr);
      if (hexValue < 0)
      break;

      *wIntValue = (*wIntValue << 4) | hexValue;
      numChars ++;

      (*cptr)++;
    }

  return (numChars);
}


/*
* @brief
*    将单个的16进制ASCII字符转换成对应的数字\n
* @param[in]     cptr   单个的16进制ASCII字符
* @return  返回对应的数字
*/
int RSP_TcpServer::Hex2num(T_UBYTE vubCh)
{
  if (vubCh >= 'a' && vubCh <= 'f')
    return vubCh-'a'+10;
  if (vubCh >= '0' && vubCh <= '9')
    return vubCh-'0';
  if (vubCh >= 'A' && vubCh <= 'F')
    return vubCh-'A'+10;
  return -1;
}


/**
 * @Funcname:  parseConfig
 * @brief        :  解析配置文件
 * @return      : true /false
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年3月6日 18:04:23
 *   
**/
bool RSP_TcpServer::parseConfig()
{
    QFile f(m_CfgFile);
    if(!f.open(QIODevice::ReadOnly))
    {
        sysLoger(LWARNING, "RSP_TcpServer::parseConfig: Open the config file [%s] failure!", 
                        m_CfgFile.toAscii().data());
        return false;
    }
    QByteArray block = f.readAll();
    
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8")) ;    
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextDecoder *decoder = codec->makeDecoder();
    QString buf = decoder->toUnicode(block);
    
    //QString buf = QString::fromLocal8Bit(block.data(),block.size());
    QXmlInputSource in;
    in.setData(buf);
    QXmlSimpleReader reader;
    // don't click on these!
    reader.setFeature("http://xml.org/sax/features/namespaces", false);
    reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
    reader.setFeature("http://trolltech.com/xml/features/report-whitespace"
        "-only-CharData", false);
    XmlHandler *hand = new XmlHandler();    //关联对象
    reader.setContentHandler(hand);
    reader.setErrorHandler(hand);

    reader.parse(in);    //读取文件内容
    reader.setContentHandler(0);
    reader.setErrorHandler(0);
    //delete hand;
    f.close();

    //保存解析结果
    sysLoger(LDEBUG, "RSP_TcpServer::parseConfig: Save the parse result!");
    m_targetEndian = hand->getTargetEndian();
    m_cpsr = hand->getCPSR();
    m_bpMem = hand->getBpMem();
    sysLoger(LDEBUG, "RSP_TcpServer::parseConfig: Endian:%d, CPSR:0x%x, breakPoint:0x%x.",
                                    m_targetEndian, m_cpsr, m_bpMem);
    
    QMap<T_WORD, T_WORD> mcpp = hand->getCppRegisterInfo();
    QMap<T_WORD, T_WORD>::iterator itrCpp = mcpp.begin();
    m_cpp.clear();
    while(itrCpp != mcpp.end())
    {
        sysLoger(LDEBUG, "RSP_TcpServer::parseConfig: cp15_regno:d, value:0x%x", 
                                        itrCpp.key(), itrCpp.value());
        m_cpp.insert(itrCpp.key(), itrCpp.value());
        itrCpp++;
    }

    QList<pair<T_WORD, T_WORD>*> listMem = hand->getMemeryConfigInfo();
    m_memCfg.clear();
    for(int i = 0; i < listMem.size(); i++)
    {
        sysLoger(LDEBUG, "RSP_TcpServer::parseConfig: Memory_addr:0x%x, value:0x%x", 
                                        listMem[i]->first, listMem[i]->second);
        m_memCfg.push_back(listMem[i]);
    }
    
    QList<T_WORD> listDelay = hand->getMemConfigInfoWithDelay();
    m_delayCfg.clear();
    for(int i = 0; i < listDelay.size(); i++)
    {
        sysLoger(LDEBUG, "RSP_TcpServer::parseConfig: MemoryDelay value:%d", 
                                        listDelay[i]);
        m_delayCfg.push_back(listDelay[i]);
    }

    QList<T_WORD> listWidth = hand->getMemWidthCfg();
    m_widthCfg.clear();
    for(int i = 0; i < listWidth.size(); i++)
    {
        sysLoger(LDEBUG, "RSP_TcpServer::parseConfig:MemoryWidth value:%d", listWidth[i]);
        m_widthCfg.push_back(listWidth[i]);
    }
    
    m_IceBuf = hand->getICEConfigBuffer();

    m_targetType = hand->getTargetType();
    m_CpuType = hand->getTargetCpuType();
    
    m_DccEnable = hand->getDccEnable();
    m_DccAddr = hand->getDccAddr();
    sysLoger(LDEBUG, "RSP_TcpServer::parseConfig:DccEnable:%d, DccAddress:%x", m_DccEnable, m_DccAddr);
    delete hand;
    
    return true;
}

T_WORD RSP_TcpServer::handleWriteSingleRegister(T_CHAR *cpBuf, T_WORD wSize)
{
    //写单个寄存器，不需要装载命令序列
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    T_WORD regNo = 0;
    T_WORD value = 0;
    T_CHAR* ptr = &cpBuf[1];
    if( Hex2int(&ptr, &regNo) && (*(ptr++) == '='))
    {
        if (Hex2int(&ptr, &value))
        {
            T_CHAR tmpBuf[4];
            Int2Char(value, tmpBuf, m_targetEndian);
            value = Char2Int(tmpBuf, TRUE);
            
            //060929 加入修改cpsr寄存器的限制 谭金伟
            if( regNo == 0x19 )
            {
                int mode = value & 0x1f;
                if( (mode != RSP_ArmConst::ARM_USER_MODE) 
                    && (mode != RSP_ArmConst::ARM_SYSTEM_MODE)
                    && (mode != RSP_ArmConst::ARM_FIQ_MODE)
                    && (mode != RSP_ArmConst::ARM_IRQ_MODE)
                    && (mode != RSP_ArmConst::ARM_SUPER_MODE)
                    && (mode != RSP_ArmConst::ARM_ABORT_MODE)
                    && (mode != RSP_ArmConst::ARM_UNDEFINED_MODE))
                {
                    PutData("E03", strlen("E03"));
                    return FAIL;
                }
            }

            if(WriteSingleRegister(regNo, value))
            {
                PutData("OK", strlen("OK"));
                return SUCC;
            }
        }
    }
    PutData("E03", strlen("E03"));
    return FAIL;

}
T_WORD RSP_TcpServer::handleReadRegisters(T_CHAR *cpBuf, T_WORD wSize)
{
    //从本地缓冲读取寄存器,不需要装载命令序列
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    T_CHAR sendBuf[PACKET_DATA_SIZE] = "\0";
    T_CHAR *pBuf = sendBuf;
    //首先判断是读单个寄存器还是读所有
    if(wSize > 1 && cpBuf[1] == 'g')
    {
        //读单个寄存器,命令格式gRegNo
        T_WORD regNo = 0;
        T_WORD value = 0;
        T_CHAR* ptr = &cpBuf[2];
        if( Hex2int(&ptr,&regNo))
        {
            value = ReadSingleRegister(regNo);
        }

        Int2Char(value, pBuf, TRUE);
        value = Char2Int(pBuf, m_targetEndian);
        pBuf += Int2Hex(pBuf, value, 8);
        sysLoger(LDEBUG, "RSP_TcpServer::handleReadRegisters:RegNo:%d, int value:%x.", regNo, value);
    }
    else
    {
        //读所有寄存器命令
        T_WORD regs[RSP_ArmConst::RSP_REG_NUM];
        T_WORD value = 0;
        T_WORD ret = 0;
        ReadAllRegisters(regs);

        //T_BOOL endian = !m_iceEndian;
        //组装缓冲,浮点寄存器24字节,一般寄存器为8字节,fps8字节,cpsr8字节
        for(T_WORD i = 0; i < RSP_ArmConst::RSP_REG_NUM; i++)
        {
            //大小端转换
            if(m_targetEndian == FALSE)
            {
                Int2Char(regs[i], pBuf, TRUE);
                value = Char2Int(pBuf, m_targetEndian);
            }
            else
            {
                value = regs[i];
            }
            if(i > 15 && i < 24)//范围内都是浮点寄存器
            {
                ret = Int2Hex(pBuf, value, 24);
            }
            else
            {
                ret = Int2Hex(pBuf, value, 8);
            }

            pBuf += ret;
            sysLoger(LDEBUG, "RSP_TcpServer::handleReadRegisters:RegNo:%d, int value:%x.", i, value);
        }
    }

    PutData(sendBuf, pBuf - sendBuf);
    
    return FAIL;
}

T_WORD RSP_TcpServer::handleWriteMemory(T_CHAR *cpBuf, T_WORD wSize)
{
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    
    T_WORD *addr = new T_WORD;
    T_WORD *pSize = new T_WORD;
    T_CHAR *ptr = &cpBuf[1];
    
    if(Hex2int(&ptr, addr) &&
            *(ptr++)==','         &&
            Hex2int(&ptr, pSize)  &&
            *(ptr++) == ':')
    {
        //如果地址和大小都为零表示检查是否支持二进制下载，应该直接回复OK
        if(*pSize == 0)
        {
            delete addr;
            delete pSize;
            PutData("OK", strlen("OK"));
            return FAIL;
        }

        //在写内存后，清空本地缓存的内存。避免上下保存的内容不一致
        RefreshMemory();
        //处理GDB 0x7d的数据问题
        if(*pSize < 0)
        {
            delete addr;
            delete pSize;
            PutData("E03", strlen("E03"));
            return FAIL;
        }
        //写二进制内存,装载3条命令 1.写内存 2.解析操作结果 3.回复GDB操作结果
        PushBackCmd(RSP_ArmConst::CMD_WRITE_MEM);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
        PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);
        
        T_CHAR *content = new T_CHAR[*pSize];
        T_CHAR *src = ptr;
        T_CHAR *des = content;
        T_WORD cnt = *pSize;
        while(cnt--)
        {
            if(*src == 0x7d)
            {
                src++;
                *des++ = *src++ ^ 0x20;
            }
            else
            {
                *des++ = *src++;
            }
        }

        //1 装载写内存参数
        QList<T_VOID*> *wma = new QList<T_VOID*>();
        wma->push_back(static_cast<T_VOID*>(addr));
        wma->push_back(static_cast<T_VOID*>(pSize));
        wma->push_back(static_cast<T_VOID*>(content));

        //2 装载分析结果参数,目前没有参数
        QList<T_VOID*> *rwm = new QList<T_VOID*>();

        //3 装载回复GDB参数,目前没有参数
        QList<T_VOID*> *replyToGdb = new QList<T_VOID*>();


        m_atomParas.push_back(wma);
        m_atomParas.push_back(rwm);
        m_atomParas.push_back(replyToGdb);
    }
    else
    {
        //参数解析失败，直接回复
        PutData("E03", strlen("E03"));
        return FAIL;
    }

    return IS_NEED_EXC_CMD;
}

T_WORD RSP_TcpServer::handleReadMemory(T_CHAR *cpBuf, T_WORD wSize)
{
    //读内存,首先检查本地缓冲的内存是否有效,如果有效,则将本地缓冲直接拷贝给GDB
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    T_CHAR sendBuf[PACKET_DATA_SIZE] = "\0";
    T_WORD addr = 0; //内存地址
    T_WORD size = 0; //内存长度
    T_CHAR *ptr = &cpBuf[1];
    T_CHAR *pSendBuf = &sendBuf[0];

    if(Hex2int(&ptr, &addr) && *(ptr++) == ',' && Hex2int(&ptr, &size))
    {
        if(size < 0 || size > 2048)
        {
            PutData("E03", strlen("E03"));
            return FAIL;
        }
        if(GetMop() == FALSE)
        {
            //装载命令序列 1.读内存 2.回复读结果
            PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
            PushBackCmd(RSP_ArmConst::CMD_REPLY_READ_MEM);

            //装载读内存参数
            QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
            T_WORD *pReadAddr = new T_WORD(addr);
            T_WORD *pReadSize = new T_WORD(size);
            readMemPara->push_back(pReadAddr);
            readMemPara->push_back(pReadSize);
            //装载回复GDB内存参数
            QList<T_VOID*> *replyMemPara = new QList<T_VOID*>();
            T_WORD *pReplyAddr = new T_WORD(addr);
            T_WORD *pReplySize = new T_WORD(size);
            replyMemPara->push_back(pReplyAddr);
            replyMemPara->push_back(pReplySize);

            //装载参数序列
            m_atomParas.push_back(readMemPara);
            m_atomParas.push_back(replyMemPara);
        }
        else
        {
            T_WORD ret = GetLocalMemory(addr, size, pSendBuf, 0);
            if(ret > 0)
            {//如果有数据，就发送，否则，返回
                PutData(pSendBuf, ret);
                return FAIL;
            }
            else
            {
                //装载回复GDB内存命令序列
                PushBackCmd(RSP_ArmConst::CMD_REPLY_READ_MEM);

                //装载回复GDB内存参数
                QList<T_VOID*> *replyMemPara = new QList<T_VOID*>();
                T_WORD *pReplyAddr = new T_WORD(addr);
                T_WORD *pReplySize = new T_WORD(size);
                replyMemPara->push_back(pReplyAddr);
                replyMemPara->push_back(pReplySize);

                m_atomParas.push_back(replyMemPara);
            }
        }
    }
    else
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    return IS_NEED_EXC_CMD;
}
T_WORD RSP_TcpServer::handleContinueTarget(T_CHAR *cpBuf, T_WORD wSize)
{
    //检查是否带有运行地址
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    
    if(wSize > 1)
    {
        T_WORD addr = 0;
        T_CHAR *ptr = &cpBuf[1];
        if(Hex2int(&ptr, &addr))
        {
            WriteSingleRegister(RSP_ArmConst::JTAG_R15, addr);
        }
        else
        {
            PutData("E03", strlen("E03"));
            return FAIL;
        }
    }

    //装载命令序列 1.写寄存器 2. 解析执行结果 3.连续运行命令 4.解析执行结果
    PushBackCmd(RSP_ArmConst::CMD_WRITE_REG);
    PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
    PushBackCmd(RSP_ArmConst::CMD_RUN_TRT);
    PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
    //装载参数序列,所有操作目前都没有参数
    QList<T_VOID*> *cWreg = new QList<T_VOID*>();
    QList<T_VOID*> *cAnaly_1 = new QList<T_VOID*>();
    QList<T_VOID*> *cRun = new QList<T_VOID*>();
    QList<T_VOID*> *cAnaly_2 = new QList<T_VOID*>();

    m_atomParas.push_back(cWreg);
    m_atomParas.push_back(cAnaly_1);
    m_atomParas.push_back(cRun);
    m_atomParas.push_back(cAnaly_2);

    return IS_NEED_EXC_CMD;
}

T_WORD RSP_TcpServer::handleSetBreakPoint(T_CHAR *cpBuf, T_WORD wSize)
{
    if(cpBuf == NULL)
    {
        sysLoger(LINFO, "RSP_TcpServer::handleSetBreakPoint:Argument cpBuf is bad pointer!");
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    //插入断点命令，首先检查断点类型
    T_WORD type = 0;  //类型
    T_WORD addr = 0;  //地址
    T_WORD length = 0; //宽度
    T_CHAR *ptr = &cpBuf[1];
    sysLoger(LDEBUG, "RSP_TcpServer::handleSetBreakPoint:enter the function!");
    if(Hex2int(&ptr, &type) && *(ptr++) == ','
        && Hex2int(&ptr, &addr) && *(ptr++) == ','
        && Hex2int(&ptr, &length))
    {
        switch(type)
        {
        case SOFT_BP:
            {
                //清空内存
                RefreshMemory();

                //软件断点
                //检查宽度，判断是否需要对齐
                if(RSP_ArmConst::RSP_SB_ARM_STATE == length)
                {
                    //是ARM态,地址对齐,二进制最后两位设为零
                    T_WORD armAddr = addr & 0xFFFFFFFC;

                    //检查软件断点是否存在
                    if(m_bpMap.contains(armAddr))
                    {
                        PutData("OK", strlen("OK"));
                        return FAIL;
                    }

                    //不存在,组建命令序列  1.读内存(由GetMem装载) 2.保存本地断点 3. 写内存 4. 解析执行结果 5 将内存读上来 6.比较是否真正写入成功，成功则回复GDB结果
                    T_CHAR memBuf[4];
                    bool bNeedReadMem = true;
                    if(GetMop())
                    {
                        T_WORD ret = GetLocalMemory(addr, 4, memBuf, 1);
                        if(ret > 0)
                            bNeedReadMem = false;
                    }
                    if(bNeedReadMem)
                    {//本地内存无效,读内存已经由GetLocalMemory装载，需要装载保存本地断点命令
                        //装载命令序列
                        /*PushBackCmd(RSP_ArmConst::CMD_INSERT_SBP);

                        //装载参数序列，需要参数： 断点地址
                        QList<T_VOID*> *insertSbp = new QList<T_VOID*>();
                        T_WORD *pArmAddr = new T_WORD(armAddr);
                        insertSbp->push_back(pArmAddr);

                        m_atomParas.push_back(insertSbp);*/
                        sysLoger(LDEBUG, "RSP_TcpServer::MessageHandler:Start to push inserting breakpoint cmds!");

                        PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
                        PushBackCmd(RSP_ArmConst::CMD_ANALY_BP_READ_MEM);
                        //装载读内存参数
                        QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
                        T_WORD *pReadAddr = new T_WORD(armAddr);
                        T_WORD *pReadSize = new T_WORD(4);//一个整数的大小
                        readMemPara->push_back(pReadAddr);
                        readMemPara->push_back(pReadSize);
                        //装载分析读内存结果并插入断点的参数
                        QList<T_VOID*> *analyPara = new QList<T_VOID*>();
                        T_WORD *pBpAddr = new T_WORD(armAddr);
                        analyPara->push_back(pBpAddr);
                        //装载参数列表
                        m_atomParas.push_back(readMemPara);
                        m_atomParas.push_back(analyPara);            

                    }
                    else
                    {
                        //本地内存有效
                        T_WORD value = Char2Int(memBuf, m_targetEndian);


                        m_bpMap.insert(armAddr, value);
                    }

                    //装载命令 3 4 5
                    PushBackCmd(RSP_ArmConst::CMD_WRITE_INT_MEM);
                    PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                    PushBackCmd(RSP_ArmConst::CMD_READ_MEM);
                    PushBackCmd(RSP_ArmConst::CMD_ANALY_INSERT_SBP);
                    PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                    //装载命令所需要的参数 //FIXME: 这里没有考虑写内存失败后的处理情况，以后补充
                    //3写内存，需要参数: 地址 值
                    QList<T_VOID*> *writeMem = new QList<T_VOID*>();
                    T_WORD *pWriteMemAddr = new T_WORD(armAddr);
                    T_WORD *pWriteMemValue = new T_WORD(m_bpMem);
                    T_WORD *pWriteMemWidth = new T_WORD(32);
                    writeMem->push_back(pWriteMemAddr);
                    writeMem->push_back(pWriteMemValue);
                    writeMem->push_back(pWriteMemWidth);
                    m_atomParas.push_back(writeMem);

                    //4.解析执行结果，不需要参数
                    QList<T_VOID*> *analyWriteMem = new QList<T_VOID*>();
                    m_atomParas.push_back(analyWriteMem);

                    //5读内存
                    QList<T_VOID*> *readMemPara = new QList<T_VOID*>();
                    T_WORD *pReadAddr = new T_WORD(armAddr);
                    T_WORD *pReadSize = new T_WORD(4);
                    readMemPara->push_back(pReadAddr);
                    readMemPara->push_back(pReadSize);
                    m_atomParas.push_back(readMemPara);

                    //检查软件断点是否插入成功
                    QList<T_VOID*> *cmpMemPara = new QList<T_VOID*>();
                    T_WORD *pCheckAddr = new T_WORD(armAddr);
                    cmpMemPara->push_back(pCheckAddr);
                    m_atomParas.push_back(cmpMemPara);

                    //5. 回复GDB结果，不需要参数
                    QList<T_VOID*> *replyGdb = new QList<T_VOID*>();
                    m_atomParas.push_back(replyGdb);

                }
                else
                {
                    //THUMB态，目前不支持
                }
                break;
            }
        case HARD_BP:
            {
                //硬件断点,
                //首先检查是否硬件断点已经设置
                if(m_hardBp)
                {
                    sysLoger(LINFO, "RSP_TcpServer::handleSetBreakPoint: already set the hardware "\
                                                "breakpoint, the system just permit only one hardware breakpoint!");
                    PutData("E03", strlen("E03"));
                    return FAIL;
                }

                //断点没有设置，装载命令序列 1. 插入硬件断点 2.分析结果 3. 回复操作结果
                PushBackCmd(RSP_ArmConst::CMD_INSERT_HBP);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                //装载参数
                //1. 插入硬件断点, 参数：地址，宽度
                QList<T_VOID*> *pInsertHbp = new QList<T_VOID*>();
                T_WORD *pBpAddr = new T_WORD(addr);
                T_WORD *pBpLength = new T_WORD(length);
                pInsertHbp->push_back(pBpAddr);
                pInsertHbp->push_back(pBpLength);
                m_atomParas.push_back(pInsertHbp);

                //2. 分析结果，不需要参数
                QList<T_VOID*> *pCheckInsertHbp = new QList<T_VOID*>();
                m_atomParas.push_back(pCheckInsertHbp);

                //3. 回复操作结果,不需要参数
                QList<T_VOID*> *pReplyInsertHbp = new QList<T_VOID*>();
                m_atomParas.push_back(pReplyInsertHbp);

                break;
            }
        case WRITEWATCH_BP:
            //写观察点
        case READWATCH_BP:
            //读观察点
        case ACCESS_BP:
            //访问观察点
            {
                if(m_hardBp)
                {
                    //PutData("E03", strlen("E03"));
                    PutData("OK", strlen("OK"));
                    return FAIL;
                }

                //--------------------------------------------------------
                //2006.06.21 GDB插入观察点会同时发送两条插入命令，第二条为有效命令
                //因此当收到GDB第一条插入命令将忽略掉，直接回复GDB OK
                //--------------------------------------------------------
                //2006.09.29 GDB插入观察点时发送的两条插入命令,有可能为第一条为有效
                //命令,的二条为0地址的命令,因此如果插入的地址为0地址,直接回复GDB OK
                //--------------------------------------------------------------------
                if(0x0 == addr)
                {
                    PutData("OK", strlen("OK"));
                    return FAIL;
                }

                //组装命令序列 1.插入观察点 2. 检查结果 3.回复GDB
                PushBackCmd(RSP_ArmConst::CMD_INSERT_WBP);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                //组装参数序列
                T_WORD *pAddr = new T_WORD(addr);
                T_WORD *pLength = new T_WORD(length);
                T_WORD *pAccess = new T_WORD(type);

                //1.插入观察点
                QList<T_VOID*> *pInsertWp = new QList<T_VOID*>();
                pInsertWp->push_back(pAddr);
                pInsertWp->push_back(pLength);
                pInsertWp->push_back(pAccess);
                m_atomParas.push_back(pInsertWp);

                //2. 分析结果，不需要参数
                QList<T_VOID*> *pCheckInsertHbp = new QList<T_VOID*>();
                m_atomParas.push_back(pCheckInsertHbp);

                //3. 回复操作结果,不需要参数
                QList<T_VOID*> *pReplyInsertHbp = new QList<T_VOID*>();
                m_atomParas.push_back(pReplyInsertHbp);

                break;
            }
        default:
            sysLoger(LINFO, "RSP_TcpServer::handleSetBreakPoint: Breakpoint type:%d invalid!", type);
            PutData("E03", strlen("E03"));
            return FAIL;
        }
    }
    else
    {
        sysLoger(LINFO, "RSP_TcpServer::handleSetBreakPoint: Parse the packet failure!");
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    sysLoger(LDEBUG, "RSP_TcpServer::handleSetBreakPoint:exit the function!");
    return IS_NEED_EXC_CMD;
}

T_WORD RSP_TcpServer::handleRemoveBreakPoint(T_CHAR *cpBuf, T_WORD wSize)
{
    if(cpBuf == NULL)
    {
        PutData("E03", strlen("E03"));
        return FAIL;
    }
    //删除断点命令，首先检查断点类型
    T_WORD type = 0;  //类型
    T_WORD addr = 0;  //地址
    T_WORD length = 0; //宽度
    T_CHAR *ptr = &cpBuf[1];

    if(Hex2int(&ptr, &type) && *(ptr++) == ','
        && Hex2int(&ptr, &addr) && *(ptr++) == ','
        && Hex2int(&ptr, &length))
    {
        switch(type)
        {
        case SOFT_BP:
            {//软件断点,首先地址对齐
            if(RSP_ArmConst::RSP_SB_ARM_STATE == length)
            {
                //清空内存
                RefreshMemory();
                PutData("OK", strlen("OK"));
                return FAIL;
            }
            else
            {
                //THUMB态，目前不支持，暂时为空
                PutData("E03", strlen("E03"));
                return FAIL;

            }

                break;
            }
        case HARD_BP:
            {

                //首先检查是否硬件断点已经设置
                if(!m_hardBp)
                {
                    sysLoger(LINFO, "RSP_TcpServer::handleRemoveBreakPoint: there is no hardware "\
                                                "breakpoint exited!");
                    PutData("E03", strlen("E03"));
                    return FAIL;
                }

                //组装命令序列 1.删除硬件断点 2.分析结果 3.回复GDB
                PushBackCmd(RSP_ArmConst::CMD_REMOVE_HBP);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                //组装参数序列
                //1. 删除硬件断点
                T_WORD *pArmAddr = new T_WORD(addr);
                T_WORD *pWidth = new T_WORD(length);
                QList<T_VOID*> *pRemoveHbp = new QList<T_VOID*>();
                pRemoveHbp->push_back(pArmAddr);
                pRemoveHbp->push_back(pWidth);
                m_atomParas.push_back(pRemoveHbp);

                //2.分析结果,没有参数
                QList<T_VOID*> *pCheckRemoveHbp = new QList<T_VOID*>();
                m_atomParas.push_back(pCheckRemoveHbp);

                //3.回复GDB
                QList<T_VOID*> *pReply = new QList<T_VOID*>();
                m_atomParas.push_back(pReply);

                break;
            }
        case WRITEWATCH_BP:
        case READWATCH_BP:
        case ACCESS_BP:
            {
                if(!m_hardBp)
                {
                    //PutData("E03", strlen("E03"));
                    PutData("OK", strlen("OK"));
                    return FAIL;
                }

                //--------------------------------------------------------
                //2006.06.21 GDB删除观察点会同时发送两条删除命令，第二条为有效命令
                //因此当收到GDB第一条删除命令将忽略掉，直接回复GDB OK
                //--------------------------------------------------------
                //2006.09.29 GDB删除观察点时发送的两条删除命令,有可能为第一条为有效
                //命令,的二条为0地址的命令,因此如果删除的地址为0地址,直接回复GDB OK

                if(0x0 == addr)
                {
                    PutData("OK", strlen("OK"));
                    return FAIL;
                }

                //---------------------------------------------------------

                //组装命令序列 1.删除观察点 2. 检查结果 3.回复GDB
                PushBackCmd(RSP_ArmConst::CMD_REMOVE_WBP);
                PushBackCmd(RSP_ArmConst::CMD_ANALY_RESULT);
                PushBackCmd(RSP_ArmConst::CMD_SIMPLE_REPLY_GDB);

                //组装参数序列
                T_WORD *pAddr = new T_WORD(addr);
                T_WORD *pLength = new T_WORD(length);
                T_WORD *pAccess = new T_WORD(type);

                //1.删除观察点
                QList<T_VOID*> *pRemoveWp = new QList<T_VOID*>();
                pRemoveWp->push_back(pAddr);
                pRemoveWp->push_back(pLength);
                pRemoveWp->push_back(pAccess);
                m_atomParas.push_back(pRemoveWp);

                //2. 分析结果，不需要参数
                QList<T_VOID*> *pCheckpRemoveWp = new QList<T_VOID*>();
                m_atomParas.push_back(pCheckpRemoveWp);

                //3. 回复操作结果,不需要参数
                QList<T_VOID*> *pReplypRemoveWp = new QList<T_VOID*>();
                m_atomParas.push_back(pReplypRemoveWp);

                break;
            }
        default:
            sysLoger(LINFO, "RSP_TcpServer::handleRemoveBreakPoint: Breakpoint type:%d invalid!", type);
            break;
        }
    }
    return IS_NEED_EXC_CMD;
}

T_WORD RSP_TcpServer::handleResetTarget(T_CHAR * cpBuf, T_WORD wSize)
{
    if(NULL == cpBuf)
    {
        sysLoger(LWARNING, "RSP_TcpServer::handleResetTarget:Argument cpBuf is bad pointer!");
        return FAIL;
    }
    if(wSize < 2)
    {
        sysLoger(LINFO, "handleResetTarget:Unknow Reset Target command, lost the reset type!");
        PutData(ERR_UNKONW_DATA, FAIL_ACK_LEN);
        return FAIL;
    }
    else
    {
        T_WORD *type = new T_WORD();     //复位类型
        char *pBuf = &cpBuf[1];
        hex2int(&pBuf, type);
        sysLoger(LINFO, "handleResetTarget:Recieved Reset Target command, reset type:%d!", *type);
        //装载复位目标机命令
        PushBackCmd(RSP_ArmConst::CMD_RESET_TARGET);
        PushBackCmd(RSP_ArmConst::CMD_ANALY_RESET_TARGET);
        //装载命令参数
        QList<T_VOID*> *resetType = new QList<T_VOID*>();
        QList<T_VOID*> *ReplyDest = new QList<T_VOID*>();
        *type = *type % 4;
        resetType->push_back(type);
        T_HWORD *pSrcAid = new T_HWORD(m_CurSrcAid);
        T_HWORD *pSrcSAid = new T_HWORD(m_CurSrcSaid);
        ReplyDest->push_back(pSrcAid);
        ReplyDest->push_back(pSrcSAid);                
        m_atomParas.push_back(resetType);
        m_atomParas.push_back(ReplyDest);
    }
    return IS_NEED_EXC_CMD;
}

T_WORD RSP_TcpServer::handleSimiHostCommand(T_CHAR * cpBuf, T_WORD wSize, T_HWORD hwSrcAid, T_HWORD hwSrcSaid)
{
    if(NULL == cpBuf)
    {
        sysLoger(LWARNING, "RSP_TcpServer::handleSimiHostCommand: Argument cpBuf is bad pointer!");
        return FAIL;
    }
    if(wSize < 2)
    {
        sysLoger(LWARNING, "RSP_TcpServer::handleSimiHostCommand: command size [%d] is invalid", wSize);
        return FAIL;
    }
    sysLoger(LINFO, "RSP_TcpServer::handleSimiHostCommand: simihost cmd:%c", cpBuf[1]);
    switch(cpBuf[1])
    {
        case 'R':
            //注册
            {
                if(wSize < 4)
                {
                    sysLoger(LWARNING, "RSP_TcpServer::handleSimiHostCommand: Invalid simihost register command!");
                    SendIDEResult(ERR_GDB_NORMAL, FAIL_ACK_LEN, hwSrcAid, hwSrcSaid);
                    return FAIL;
                }
                m_SimiSaid = hwSrcSaid;
                m_SimiAid = hwSrcAid;
                m_SimiRegisterFlag = TRUE;
                // format : YR, xxxxxxx(addr)
                T_CHAR *pBuf = &cpBuf[3];
                hex2int(&pBuf, &m_SimiPCAddr);
                sysLoger(LINFO, "RSP_TcpServer::handleSimiHostCommand: Recieved simihost's register CMD, pcAddr:%x!",
                                            m_SimiPCAddr);
                SendIDEResult(SUCC_ACK, SUCC_ACK_LEN, hwSrcAid, hwSrcSaid);
                
            }
            break;
            
        case 'r':
            //取消注册
            {
                m_SimiRegisterFlag = FALSE;
                m_SimiAid = INVALID_AID;
                m_SimiSaid = INVALID_AID;
                SendIDEResult(SUCC_ACK, SUCC_ACK_LEN, hwSrcAid, hwSrcSaid);
            }
            break;
            
        case 'A':
            //重试, 清空命令队列
            {
                ExcuteCommand(NULL, 0, FALSE);
                SendIDEResult(SUCC_ACK, SUCC_ACK_LEN, hwSrcAid, hwSrcSaid);
            }
            break;
            
        case 'B':
            //路由故障，通知GDB调试故障
            {
                m_MtxMsgSrc.lock();
                m_CurCmdSource = SOCKET_DATA;
                m_MtxMsgSrc.unlock();
                PutData("S05", strlen("S05"));
                SendIDEResult(SUCC_ACK, SUCC_ACK_LEN, hwSrcAid, hwSrcSaid);
            }
            break;
            
        default:
            sysLoger(LINFO, "RSP_TcpServer::handleSimiHostCommand: Unknow simihost command!");
            return FAIL;
    }
    return SUCC;
}


T_WORD RSP_TcpServer::getTargetType()
{
    return m_targetType;
}

void RSP_TcpServer::setTargetType(T_WORD type)
{
    m_targetType = type;
    return;
}

T_WORD RSP_TcpServer::getCpuType()
{
    return m_CpuType;
}

void RSP_TcpServer::setCputType(T_WORD type)
{
    m_CpuType = type;
    return;
}
#if 0
/*
* @brief
*    根据ICE回复更新目标机状态，并推动流程发展CMD_QUERY_TRT_STATUS\n
* @param[in] tpArg  更新目标机可能所需要的参数
* @return
*/

T_VOID RSP_TcpServer::CmdQueryTarget(T_CHAR *cpBuf, T_WORD wSize, QList<T_VOID*> *tpArg, T_BOOL bFlag)
{
    assert((NULL != tpArg));

    if(bFlag)
    {
        assert((NULL != cpBuf));

        if(cpBuf[0] == 'P' && cpBuf[1] == '0')
        {
            SetTargetStatus(cpBuf[3]);
        }

        //如果处于上电状态
        if(GetTargetStatus() == RSP_ArmConst::CORE_DEBUG_BKWTPT
            || GetTargetStatus() ==  RSP_ArmConst::CORE_RUNNING)
        {
            //装载初始化目标机命令序列
            InitTarget();

        }
    }

}
#endif

/*
* @brief
*    将一个整型根据宽度转为16进制字符串\n
* @param[in] cpBuf   调用者提供的缓冲
* @param[in] wNum    调用者提供的整型
* @param[in] wWidth  调用者提供的字符串宽度
* @return 宽度
*/
T_MODULE T_WORD Int2Hex(T_CHAR *cpBuf, T_WORD wNum, T_WORD wWidth)
{
  T_WORD i;

  //buf[width] = '\0';

  for (i = wWidth - 1; i >= 0; i--)
    {
      cpBuf[i] = "0123456789abcdef"[(wNum & 0xf)];
      wNum >>= 4;
    }

  return wWidth;
}

