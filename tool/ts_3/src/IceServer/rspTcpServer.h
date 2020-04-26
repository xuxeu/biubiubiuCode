/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: rspTcpServer.h,v $
  * Revision 1.8  2008/03/19 10:25:11  guojc
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
  * Revision 1.7  2007/06/21 11:37:29  guojc
  * 1. 修改了IS对simihost不删除硬件断点的处理
  * 2. 统一管理IS调试日志开关
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
  * Revision 1.7  2006/07/17 09:30:01  tanjw
  * 1.增加arm,x86 rsp日志打开和关闭功能
  * 2.增加arm,x86 rsp内存读取优化算法打开和关闭功能
  *
  * Revision 1.6  2006/07/14 09:19:30  tanjw
  * 1.修改release版问题
  * 2.加入ice升级功能
  *
  * Revision 1.5  2006/06/26 09:14:32  tanjw
  * 1.修改arm和x86内存读取出错bug
  * 2.iceserver去掉wsocket
  *
  * Revision 1.4  2006/06/22 06:37:08  tanjw
  * 1.访问观察点处理方式
  *
  * Revision 1.1.1.1  2006/04/19 06:29:13  guojc
  * no message
  *
  * Revision 1.2  2006/04/14 02:02:28  guojc
  * 增加了对CP15协处理器寄存器修改的支持
  *
  * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
  * TS代码整理
  *
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * 创建文件;定义文件的基本接口
  */

/**
* @file     rspTcpServer.h
* @brief     
*     <li>功能： 定义RSP_TcpServer服务器对象的类</li>
* @author     郭建川
* @date     2006-02-20
* <p>部门：系统部 
*/

#ifndef _RSP_TCP_SERVER_H
#define _RSP_TCP_SERVER_H

/************************************引用部分******************************************/
#include <vector>
#include <map>
#include <QList>
#include <QtCore>
//#include <QThread>
#include <QMutexLocker>
#include <QWaitCondition.h>
#include <QTextOStream>

#include "sysTypes.h"
#include "wsocket.h"
#include "memory.h"
#include "lambdaIce.h"
#include "packet.h"
#include "armConst.h"
#include "IceServerComm.h"
#include "common.h"
#include "common.h"
/************************************声明部分******************************************/
using namespace std;


/************************************定义部分******************************************/
/**
* @brief
*    RSP_TcpServer是ICE Server提供的支持解析标准RSP协议的TCP服务器，它将标准RSP协议解析后\n
*   专换成Lambda ICE提供的原子操作命令和ICE进行交互，达到调试目标机系统的目的。\n
* @author     郭建川
* @version 1.0
* @date     2006-02-20
* <p>部门：系统部 
*/


/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [begin]*/
/*  Modifed brief:用单独写的线程类替换QT线程类*/
class RSP_TcpServer : public baseThread
/*Modifed by tangxp for BUG NO. 0002430 on 2008.1.4 [end]*/
{
public:
    /**
    * @brief 
    *    构造函数
    */   
    RSP_TcpServer();              

    /**
    * @brief 
    *    析构函数
    */   
    ~RSP_TcpServer();

    T_BOOL GetIceEndian();
    
    T_BOOL GetTargetEndian();
    /**
    * @brief 
    *    打开操作，进行协议解析，得到需要的参数信息
    * @param[in] id 对应服务器所绑定的AID号
    * @param[in] cpBuf 参数缓冲指针,服务器可以从中解析到所需的参数
    * @return 操作成功返回监听的端口号，失败返回FAIL
    */   
    int Open(T_WORD wId, T_CHAR *cpBuf);

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
    int Putpkt(T_HWORD hwDesSaid,T_HWORD hwSrcAid,T_HWORD hwSrcSaid,T_CHAR* cpBuf,T_WORD wSize);

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
    int Getpkt(T_HWORD *hwpDesAid,T_HWORD *hwpDesSaid,T_HWORD *hwpSrcSaid,T_CHAR* cpBuf,T_WORD wSize);

    /**
    * @brief 
    *    关闭操作，关闭服务器
    */
    T_VOID Close();
    /**
    * @brief 
    *    强制退出服务，目前仅用于在open操作失败时，
    *    因为open失败后线程一直无法退出，导致TS阻塞
    */
    T_VOID forceExit()
    {
        m_quitMutex.lock();
        m_rxExitReady = TRUE;
        m_localExitReady = TRUE;
        m_txExitReady = TRUE;
        m_quitMutex.unlock();
    }

    /**
    * @brief 
    *    线程运行执行入口方法
    */
    T_VOID run();
    
    /**
    * @brief 
    *    将缓冲包插入到服务器维护的缓冲队列中
    * @param[in] tpPack    缓冲包
    */
    T_VOID Enqueue(RSP_Packet* vtpPack);
    
        /*
    * @brief 
    *    将一个整型根据大小端转为CHAR数组\n
    * @param[in] vwReg    调用者提供的寄存器数值
    * @param[in] vcpBuf   调用者提供的缓冲
    * @param[in] vbIsBigEndian  调用者提供的大小端信息
    * @return  
    */
    T_VOID Int2Char(T_WORD vwReg, T_CHAR *vcpBuf, T_BOOL vbIsBigEndian);

    /*
    * @brief 
    *    将一个CHAR数组根据大小端转为整型\n
    * @param[in] vcpBuf   调用者提供的缓冲
    * @param[in] vbIsBigEndian  调用者提供的大小端信息
    * @return 该整型  
    */
    T_WORD Char2Int(T_CHAR *vcpBuf, T_BOOL vbIsBigEndian);
    
    /*
    * @brief 
    *    配置文件的解析
    * @return 成功返回TRUE，失败返回FALSE
    */
    bool parseConfig();
    T_WORD handleWriteSingleRegister(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleReadRegisters(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleWriteMemory(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleReadMemory(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleContinueTarget(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleSetBreakPoint(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleRemoveBreakPoint(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleResetTarget(T_CHAR *cpBuf, T_WORD wSize);
    T_WORD handleSimiHostCommand(T_CHAR *cpBuf, T_WORD wSize, T_HWORD hwSrcAid,T_HWORD hwSrcSaid);

/*Modified by qingxiaohai on 2007年4月6日 14:20:13 for TS3.2  [Begin]*/
/*  Modified brief:WCS simulator*/
    T_WORD getTargetType();
    void        setTargetType(T_WORD type);
    T_WORD getCpuType();
    void         setCputType(T_WORD type);
/*Modified for TS3.2 [End]*/
private:
    QMutex m_sktMutex;
    T_WORD m_port;   //服务器监听端口号
    WSocket m_serverSocket; //服务器SOCKET对象
    WSocket *m_sClient; //客户端对象指针
    QMutex m_clientMutex;                //对SOCKET进行锁定

    T_VBOOL m_runFlag;   //服务器线程运行标识
    T_VBOOL m_waitForIce;  //等待ICE
    QMutex m_excuteLock;    //执行命令序列状态加锁保护

    RSP_LambdaIce m_lambdaIce; //ICE的协议抽象接口
    
    T_WORD m_iceId;  //ICE的ID号
    T_WORD m_sessId; //对应ICE上的会话ID号

    T_WORD m_bpMem;  //软件断点地址内容
    QMap<T_WORD, T_WORD> m_bpMap; //软件断点列表
    T_WORD m_retryTimes;    //插入软件断点重试次数

    T_VBOOL m_hardBp;  //硬件断点是否已经被设置
    
    T_WORD m_cpsr;   //CPSR寄存器配置   
    T_WORD m_currentRegs[RSP_ArmConst::ARM_REG_NUM]; //当前寄存器值
    QMap<T_WORD, T_WORD> m_cpp;   //CP15寄存器组

    QList<pair<T_WORD, T_WORD>*> m_memCfg; //内存初始化配置 pair->first() addr pair->second() value;
    QList<T_WORD> m_delayCfg; //延迟配置
    QList<T_WORD> m_widthCfg; //宽度配置
    
    QMap<T_WORD, RSP_Memory*> m_addr2mem; //内存映射表

    T_MODULE T_CONST T_WORD ENDIAN_SMALL = 0;
    T_MODULE T_CONST T_WORD ENDIAN_BIG = 1;
    T_BOOL m_targetEndian;     //目标机的大小端
    T_BOOL m_iceEndian;        //ICE的大小端
    T_BOOL m_isSupportPrivateProtocol;         //是否支持私有扩展协议
    
    T_VWORD m_targetStatus; //目标机状态
    QMutex m_statusLock;    //目标机状态加锁保护

    QList<T_WORD> m_atomCmds;  //原子操作命令序列
    QList< QList<T_VOID*>* > m_atomParas; //原子操作命令的参数

    QList<RSP_Packet*> m_packetQueue;    //数据包缓冲队列
    QWaitCondition m_queueNotFull;    //条件变量,用来判断队列不为空
    QMutex m_packMutex;                //对数据包缓冲区进行锁定,

    QMutex m_cmdMutex;                //对命令缓冲区进行锁定

    QWaitCondition m_quitReady;  //条件变量，用来判断关闭方法是否可以正常返回
    QMutex m_quitMutex;    
    T_VBOOL m_rxExitReady;  //RX线程返回标志变量
    T_VBOOL m_localExitReady;  //本地线程退出标志变量
    T_VBOOL m_txExitReady;  //TX线程返回标志变量

    RSP_MemoryManager m_memMan;  //内存管理单元
    QMutex m_mopMutex;
    
    T_BOOL m_mopFlag;  //内存优化算法开启标志
    int SetMop(T_BOOL mop);
    T_BOOL GetMop();

    T_BOOL m_validForWp;  //可以打观察点标记

    T_BOOL m_validDeleWp; //可以删除观察点标记

    QString m_CfgFile;              //配置文件
    char m_ICEName[NAME_SIZE];            //ICE目标代理的名字
    QString m_IceBuf;                 //传给ICE的配置缓冲
    
    QMutex m_MtxMsgSrc;                 //当前消息来源的锁
    T_WORD m_CurCmdSource;             //当前执行的命令
    T_HWORD m_CurSrcAid;          //当前消息包的来源的代理
    T_HWORD m_CurSrcSaid;         //当前消息包的来源二级代理

    T_HWORD m_SimiAid;                            //simihost 的客户代理ID
    T_HWORD m_SimiSaid;                         //simihost 的二级代理
    T_BOOL    m_SimiRegisterFlag;           //simihost 注册标志
    T_WORD   m_SimiPCAddr;                  //simihost 中断向量地址
    T_BOOL    m_NeedReplyS05;              //simihost,收到s05是否需要向GDB发送

    T_BOOL      m_DccEnable;                    //DCC通道是否打开
    T_WORD      m_DccAddr;                      //DCC handler的地址
/*Modified by qingxiaohai on 2007年4月6日 14:20:13 for TS3.2  [Begin]*/
/*  Modified brief:WCS simulator*/
    T_WORD   m_targetType;          /*指示所适配的目标机类型是
                                                        ICE还是WCS Simulator, (0: ICE,   1: wcs simulator)*/
    T_WORD   m_CpuType;        /*模拟器模拟的CPU类型*/
/*Modified for TS3.2 [End]*/
#ifdef IS_PACK_LOG
    QMutex m_logMutex;
    T_BOOL m_isLog;  //是否需要数据包日志选项，默认为FALSE
    PackLog m_logger;

    T_VOID SetLog(T_BOOL isLog)
    {
        m_isLog = isLog;
    }
    T_BOOL IsLog()
    {
        return m_isLog;
    }
#endif

    //SystemLogger *m_errLogger;  //错误日志记录
    /*
    * @brief 
    *    阻塞在服务器接收GDB的数据，收到数据后\n
    *    将数据拷贝到调用者提供的缓冲区后返回\n
    * @param     pBuf    调用者提供的数据缓冲
    * @param[in] size    调用者提供的缓冲区的大小
    * @return  实际接收的数据的大小，网络断开返回-1
    */
    int GetData(T_CHAR *pBuf, T_WORD size);

    /*
    * @brief 
    *    将回复数据发送给GDB\n
    * @param     pBuf    调用者要发送的数据缓冲
    * @param[in] size    调用者要发送数据的大小
    * @return  实际发送的数据的大小
    */
    int PutData(T_CHAR *pBuf, T_WORD size);

    /*
    * @brief 
    *    将接收GDB的数据进行协议解析，装载命令序列\n
    * @param     pBuf    调用者提供的数据缓冲
    * @param[in] size    调用者提供的缓冲区的大小
    * @return  
    */
    int MessageHandler(T_CHAR *pBuf, T_WORD size);

    /*
    * @brief 
    *    根据处理器的当前状态设置寄存器\n
    * @param[in] regNo   要写的寄存器号 
    * @param[in] value   寄存器的值
    * @return  SUCC或FAIL
    */
    T_BOOL WriteSingleRegister(T_WORD regNo, T_WORD value);

    /*
    * @brief 
    *    根据处理器模式读取单个寄存器\n
    * @param[in] vwRegNo    寄存器号
    * @return  成功则返回寄存器的值，失败则返回0
    */
    T_WORD ReadSingleRegister(T_WORD vwRegNo);

    /*
    * @brief 
    *    根据处理器模式读取所有寄存器\n
    * @param[in] vwRegs    寄存器数组
    * @return  成功则返回SUCC，失败则返回FAIL
    */
    T_BOOL ReadAllRegisters(T_WORD *vwRegs);

    /*
    * @brief 
    *    根据输入的描述限定表取出对应的寄存器\n
    * @param     vwaRegs    调用者提供的寄存器数组缓冲
    * @param[in] vwTable    调用者提供的寄存器映射表
    * @return  
    */
    T_VOID GetRegs(T_WORD *vwaRegs, T_CONST T_WORD *vwaTable);



    /*
    * @brief 
    *    从命令序列里面取出第一条命令\n
    * @param[in] vcpBuf   参数值，可以为空
    * @return  命令序号，如果没有返回失败
    */
    int GetNextCmd();

    /*
    * @brief 
    *    查看命令序列的大小\n
    * @return  命令序列的大小
    */
    T_WORD CmdSize();

    /*
    * @brief 
    *    装载一条命令到命令序列里面\n
    * @para [in] wCmd 命令序号 
    * @return  命令序号，如果没有返回失败
    */
    T_VOID PushBackCmd(T_WORD wCmd);
    
    /*
    * @brief 
    *    从参数序列里面取出第一组参数\n
    * @return  参数值，如果没有返回失败
    */
    QList<T_VOID*> * GetNextPara();

    /**
    * @brief 
    *    设置目标机状态，加锁保护
    * @param[in] vwStatus 目标机状态
    */
    T_VOID SetTargetStatus(T_WORD vwStatus);

    /**
    * @brief 
    *    得到目标机状态，加锁保护
    * @param[in] vwStatus 目标机状态
    */
    T_WORD GetTargetStatus();

    /*
    * @brief 
    *    将给定的16进制字符串转换为整型,需要注意的是该操作会移动给定缓冲指针的地址\n
    * @param[in]     cptr    16进制字符串
    * @param[out] wIntValue 转换的INT数据
    * @return  指针移动的字节数
    */
    T_UWORD Hex2int(T_CHAR **vcptr, T_WORD *vwIntValue);

    /*
    * @brief 
    *    将单个的16进制ASCII字符转换成对应的数字\n
    * @param[in]     cptr   单个的16进制ASCII字符
    * @return  返回对应的数字
    */
    int Hex2num(T_UBYTE vubCh);

    /*
    * @brief 
    *    执行命令序列，推动服务器流程\n
    * @param[in] cpBuf   参数值，可以为空
    * @return  SUCC或FAIL
    */
    int ExcuteCommand(T_CHAR *cpBuf, T_WORD wSize, T_BOOL bOpsFlag);


    /*
    * @brief 
    *    从本地内存缓冲中获取需要的内存信息，如果没有，该方法会压入命令序列从目标机中读取相应的内存块\n
    * @param[in] wAddr   内存地址
    * @param[in] wSize   内存大小
    * @param[in] cpBuf   调用者给定的缓冲区
    * @param[in] type    调用者想获得的内存的格式 0为GDB格式 1为二进制格式
    * @return  给缓冲区拷贝的内存区的大小，0为无本地缓存
    */
    int GetLocalMemory(T_WORD wAddr, T_WORD wSize, T_CHAR *cpBuf, T_WORD type);

    /*
    * @brief 
    *    写内存,对应命令序列CMD_WRITE_MEM\n
    * @param[in] tpArg  写内存所需要的参数 
    * @return  成功或者失败
    */
    T_BOOL CmdWriteIceMemory(QList<T_VOID*> *vtpArg, T_BOOL vbFlag);

    /*
    * @brief 
    *    读内存\n
    * @param[in] tpArg  读内存所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdReadIceMemory(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    读寄存器,对应命令序列CMD_READ_REG\n
    * @param[in] tpArg  读寄存器可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdReadIceRegisters(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    写寄存器,对应命令序列CMD_WRITE_REG\n
    * @param[in] tpArg  写寄存器可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdWriteIceRegisters(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    运行目标机,对应命令序列CMD_RUN_TRT\n
    * @param[in] tpArg  运行目标机可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdRunTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    停止目标机,对应命令序列CMD_STOP_TRT\n
    * @param[in] tpArg  停止目标机可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdStopTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    检查ICE回复是否成功,对应命令序列CMD_ANALY_RESULT\n
    * @param[in] cpBuf  ICE回复的数据 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdCheckReply(T_CHAR *cpBuf, T_BOOL bFlag);

    /*
    * @brief 
    *    通过ICE回复更新本地寄存器缓冲,对应命令序列CMD_UPDATE_READ_REG\n
    * @param[in] cpBuf  ICE回复的数据 
    * @param[in] wSize  ICE回复的数据的大小 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdUpdateRegs(T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);

    /*
    * @brief 
    *    通过ICE回复更新本地内存缓冲,对应命令序列CMD_UPDATE_READ_MEM\n
    * @param[in] tpArg  更新内存缓冲可能所需要的参数 
    * @param[in] cpBuf  ICE回复的数据 
    * @param[in] wSize  ICE回复的数据的大小 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdUpdateMems(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);

    /*
    * @brief 
    *    回复GDB读内存命令,对应命令序列CMD_REPLY_READ_MEM\n
    * @param[in] tpArg  更新内存缓冲可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdReplyMems(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);

    /*
    * @brief 
    *    设置目标机状态可用,对应命令序列CMD_SET_TRT_STATUS\n
    * @param[in] tpArg  更新内存缓冲可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdUpdateStatus(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    根据ICE回复更新目标机状态，并推动流程发展CMD_UPDATE_TRT_STATUS\n
    * @param[in] tpArg  更新目标机可能所需要的参数 
    * @return  
    */
   T_BOOL CmdQueryTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    回复GDB简单命令,对应命令序列CMD_SIMPLE_REPLY_GDB\n
    * @param[in] tpArg  更新内存缓冲可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdSimpleReply(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    插入硬件断点,对应命令序列CMD_INSERT_HBP\n
    * @param[in] tpArg  更新内存缓冲可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdInsertHardwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    插入软件断点,对应命令序列CMD_INSERT_SBP\n
    * @param[in] tpArg  更新内存缓冲可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdInsertSoftwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    写整型参数内存,对应命令序列CMD_WRITE_INT_MEM\n
    * @param[in] tpArg  写内存所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdWriteIntMemory(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    删除硬件断点,对应命令序列CMD_REMOVE_HBP\n
    * @param[in] tpArg  更新内存缓冲可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdRemoveHardwareBp(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    回复GDB S05,对应命令序列CMD_REPLY_S05\n
    * @param[in] tpArg  更新内存缓冲可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdReplyS05(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    插入硬件观察点,对应命令序列CMD_INSERT_WBP\n
    * @param[in] tpArg  更新内存缓冲可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdInsertWatchPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag);


    /*
    * @brief 
    *    删除硬件观察点,对应命令序列CMD_REMOVE_WBP\n
    * @param[in] tpArg  更新内存缓冲可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdRemoveWatchPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag);


    T_BOOL CmdCheckSbp(QList<T_VOID*> *tpArg,  T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);


    /*
    * @brief 
    *    延时操作,对应命令序列CMD_DELAY\n
    * @param[in] tpArg  延时所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdDelay(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    写CP15寄存器,对应命令序列CMD_INIT_CP15\n
    * @param[in] tpArg  写寄存器可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdWriteCp15Registers(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    读CP15寄存器,对应命令序列CMD_INIT_CP15\n
    * @param[in] tpArg  写寄存器可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdReadCp15Registers(QList<T_VOID*> *tpArg, T_BOOL bFlag);
    
    /*
    * @brief 
    *    查询ICE的大小端
    * @param[in] tpArg  写寄存器可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdQueryIceEndian(QList<T_VOID*> *tpArg, T_BOOL bFlag);

    /*
    * @brief 
    *    创建ICE会话
    * @param[in] tpArg  写寄存器可能所需要的参数 
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdCreateSession(QList<T_VOID*> *tpArg, T_BOOL bFlag);
    /*
    * @brief 
    *    删除ICE会话
    * @param[in] tpArg  
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdDeleteSession(QList<T_VOID*> *tpArg, T_BOOL bFlag);
    /*
    * @brief 
    *    分析删除ICE会话结果
    * @param[in] tpArg  
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理 
    * @return  成功或者失败
    */
    T_BOOL CmdAnalyDeleteSession(T_CHAR *cpBuf, T_BOOL bFlag);
    /*
    * @brief
    *    分析查询ICE大小端的结果
    * @param[in] cpBuf  查询操作的结果
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
    * @return  成功或者失败
    */
    T_BOOL CmdAnalyQueryICEEndian(T_CHAR *cpBuf, T_BOOL bFlag);
    
    /*
    * @brief
    *    分析创建会话的结果
    * @param[in] cpBuf  创建会话操作的结果
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
    * @return  成功或者失败
    */
    T_BOOL CmdAnalyCreateSession(T_CHAR *cpBuf, T_BOOL bFlag);  

    /*
    * @brief
    *    重启目标机
    * @param[in] tpArg  重启目标机的参数(重启类型)
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
    * @return  成功或者失败
    */
    T_BOOL CmdResetTarget(QList<T_VOID*> *tpArg, T_BOOL bFlag);  
    
    /*
    * @brief
    *    重启目标机
    * @param[in] tpArg  重启目标机的参数(重启类型)
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
    * @return  成功或者失败
    */
    T_BOOL CmdAnalyResetTarget(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);  
    
    /*
    * @brief
    *    重启目标机
    * @param[in] tpArg  重启目标机的参数(重启类型)
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
    * @return  成功或者失败
    */
    T_BOOL CmdQueryTargetCpuIDCode(QList<T_VOID*> *tpArg, T_BOOL bFlag);  

    /*
    * @brief
    *    重启目标机
    * @param[in] tpArg  重启目标机的参数(重启类型)
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
    * @return  成功或者失败
    */
    T_BOOL CmdAnalyQueryTargetCpuIDCode(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);  
    /*
    * @brief
    *    分析插入断点时读内存的结果，并插入断点
    * @param[in] tpArg  插入断点读内存的参数
    * @param[in] bFlag  全局操作标记，如果为失败，则方法内部只做释放资源处理
    * @return  成功或者失败
    */
    T_BOOL CmdAnalyBreakPointReadMem(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);  

    /*
     * @brief   
     * 分析PC寄存器值是否等于SIMIHOST注册时登记的值，如果是则向其上报SWI
     */
    T_BOOL CmdAnalySimihostBreakPoint(QList<T_VOID*> *tpArg, T_BOOL bFlag);
    
    /*
     * @brief   
     * 启用DCC 功能
     */
    T_BOOL CmdOpenDccFunction(QList<T_VOID*> *tpArg, T_BOOL bFlag);
    /*
     * @brief   
     * 分析启用DCC  功能的结果
     */
    T_BOOL CmdAnalyOpenDccResult(QList<T_VOID*> *tpArg, T_CHAR *cpBuf, T_WORD wSize, T_BOOL bFlag);

    /*
    * @brief 
    *    查询目标机状态，通过ICE的回复推动状态流程执行，装载命令序列\n
    * @return  
    */
    T_VOID QueryTarget();

    /*
    * @brief 
    *    初始化目标机操作，在目标机上电以后进行，装载命令序列\n
    * @return  
    */
    T_VOID InitTarget();

    /*
    * @brief 
    *    清空本地内存缓冲,
    */
    T_VOID RefreshMemory();

    /*
    * @brief 
    *    在GDB断开和IS的连接和检查目标机端是否有断点设置，如果有，就装载命令清空断点内容\n
    */
    T_BOOL ClearTarget();

    /*
    * @brief 
    *    该方法用于阻塞RX线程\n
    */
    T_VOID WaitUp();

    /*
    * @brief 
    *    该方法用于唤醒RX线程\n
    */
    T_VOID WakeUp();

    /*
    * @brief 
    *    从缓冲队列里面取出一个命令包\n
    * @return  缓冲数据包
    */
    RSP_Packet* Dequeue();

    /*
    * @brief 
    *    关闭服务器线程方法
    */
    T_VOID CloseThread();

    T_VOID DeleteSocket();

    T_VOID HandleIDECmd(T_HWORD hwSrcAid,T_HWORD hwSrcSaid, T_CHAR* cpBuf,T_WORD wSize);
    T_VOID SendIDEResult(T_CHAR *cpBuf, T_WORD wSize, T_HWORD hwSrcAid,T_HWORD hwSrcSaid);
};

#endif