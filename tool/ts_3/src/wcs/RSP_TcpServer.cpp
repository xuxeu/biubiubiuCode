/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  RSP_TcpServer.cpp
 * @Version        :  1.0.0
 * @Brief           :  实现RSP协议收发和处理的TCP通信方式的服务器
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:24:26
 * @History        : 
 *   
 */

#include "Rsp_TcpServer.h"
#include "sysType.h"
#include "comfunc.h"
#include "eventHandler.h"
#include "txthread.h"
#include "wcs_interface.h"
/************************引用部分*****************************/

#pragma comment(lib, "WCS_LAMBDA_W55VA75")


/************************前向声明部分***********************/
RspTcpServer* RspTcpServer::m_pSelf = NULL;
QMutex RspTcpServer::m_mtxInit;

/************************定义部分*****************************/

/************************实现部分****************************/
#define ASSERT_DLLFUNC(a) {\
                                                    if(a == NULL)\
                                                        return FAIL;\
                                                 }

RspTcpServer::RspTcpServer()
{
    m_runFlag = false;
    m_pTxThread = NULL;
    m_pEventHandler = NULL;
    m_CpuType = 0;
    m_edian = 0;
    m_SimuloatorConnected = false;
    mapAddr2Value.clear();
    m_port = WCS_LIS_PORT;
    //m_wcsdll = NULL;
    m_dbgInfo = new(WCS_DBGINFO_T);
    if(m_dbgInfo != NULL)
    {
        m_dbgInfo->uCpuType = m_CpuType;
        m_dbgInfo->uEndian = m_edian;
        m_dbgInfo->pLogNotice = eventHander::wcsLogerNoticeCallback;
        m_dbgInfo->pModeSwitch = eventHander::wcsModeSwitchCallback;
    }
}

RspTcpServer::~RspTcpServer()
{
    if(m_pTxThread != NULL)
    {
        delete m_pTxThread;
        m_pTxThread = NULL;
    }
    if(m_pEventHandler != NULL)
    {
        delete m_pEventHandler;
        m_pEventHandler = NULL;
    }
    mapAddr2Value.clear();
    if(m_dbgInfo != NULL)
    {
        delete m_dbgInfo;
        m_dbgInfo = NULL;
    }
    /*if(m_wcsdll != NULL)
    {
        FreeLibrary(m_wcsdll);
    }*/
}

/**
 * @Funcname:  loadWCSLibraryProc
 * @brief        :  动态加载方式下加载华邦模拟器提供的所有接口
 * @para1      : null
 * @return      : SUCC returned if load all the func success, FAIL returned if failed
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月11日 16:57:16
 *   
**/
int RspTcpServer::loadWCSLibraryProc()
{
    return SUCC;    
    /*m_wcsdll = LoadLibraryA("WCS_LAMBDA_W55VA75.dll");
    if(m_wcsdll == NULL)
    {
        sysLoger(SLERROR, "RspTcpServer::loadWCSLibraryProc:Load Wcs library failure!");
        return FAIL;
    }
    m_wcsAbort = (pwcsAbort)GetProcAddress(m_wcsdll, "wcsAbort");
    ASSERT_DLLFUNC(m_wcsAbort);
    m_wcsClearBp = (pwcsClearBp)GetProcAddress(m_wcsdll, "wcsClearBp");
    ASSERT_DLLFUNC(m_wcsClearBp);
    m_wcsConnect = (pwcsConnect)GetProcAddress(m_wcsdll, "wcsConnect");
    ASSERT_DLLFUNC(m_wcsConnect);
    m_wcsDisConnect = (pwcsDisConnect)GetProcAddress(m_wcsdll, "wcsDisconnct");
    ASSERT_DLLFUNC(m_wcsDisConnect);
    m_wcsDownload = (pwcsDownload)GetProcAddress(m_wcsdll, "wcsDownload");
    ASSERT_DLLFUNC(m_wcsDownload);
    m_wcsGetCpuStatus = (pwcsGetCpuStatus)GetProcAddress(m_wcsdll, "wcsGetCpuStatus");
    ASSERT_DLLFUNC(m_wcsGetCpuStatus);
    m_wcsGetMem = (pwcsGetMem)GetProcAddress(m_wcsdll, "wcsGetMem");
    ASSERT_DLLFUNC(m_wcsGetMem);
    m_wcsGetProperty = (pwcsGetProperty)GetProcAddress(m_wcsdll, "wcsGetProperty");
    ASSERT_DLLFUNC(m_wcsGetProperty);
    m_wcsGetRegs= (pwcsGetRegs)GetProcAddress(m_wcsdll, "wcsGetRegs");
    ASSERT_DLLFUNC(m_wcsGetRegs);
    m_wcsGo= (pwcsGo)GetProcAddress(m_wcsdll, "wcsGo");
    ASSERT_DLLFUNC(m_wcsGo);
    m_wcsMiscFun = (pwcsMiscFun)GetProcAddress(m_wcsdll, "wcsMiscFun");
    ASSERT_DLLFUNC(m_wcsMiscFun);
    m_wcsReset = (pwcsReset)GetProcAddress(m_wcsdll, "wcsReset");
    ASSERT_DLLFUNC(m_wcsReset);
    m_wcsSetBp = (pwcsSetBp)GetProcAddress(m_wcsdll, "wcsSetBp");
    ASSERT_DLLFUNC(m_wcsSetBp);
    m_wcsSetMem = (pwcsSetMem)GetProcAddress(m_wcsdll, "wcsSetMem");
    ASSERT_DLLFUNC(m_wcsSetMem);
    m_wcsSetProperty = (pwcsSetProperty)GetProcAddress(m_wcsdll, "wcsSetProperty");
    ASSERT_DLLFUNC(m_wcsSetProperty);
    m_wcsSetRegs = (pwcsSetRegs)GetProcAddress(m_wcsdll, "wcsSetRegs");
    ASSERT_DLLFUNC(m_wcsSetRegs);
    */
    return SUCC;
}

int RspTcpServer::run()
{
    m_runFlag = true;
    
    SystemLog::getInstance()->openLogFile("wcsSysLog.log");
    SystemLog::getInstance()->setLogLevel(SLDEBUG);
    
    /*if(loadWCSLibraryProc() != SUCC )
    {
        sysLoger(SLERROR, "RspTcpServer::run:load library or functions failed!");
        m_runFlag = false;
        return FAIL;
    }*/
    
    while(m_runFlag)
    {
        WSocket srvSocket;
        if(!m_listenTcpDevice.createServerSocket(m_port))
        {
            sysLoger(SLWARNING, "RspTcpServer::run: Create server socket object failed!");
            return FAIL;
        }
        if(!m_listenTcpDevice.acceptSocket(srvSocket))
        {
            break;
        }
        else
        {
            m_clientTcpDevice.setTcpSocket(srvSocket.m_sock);
            sysLoger(SLINFO, "RspTcpServer::run: Detected a client connecting");
        }
        tcpDevice *clientTcpDevice = new tcpDevice();
        if(clientTcpDevice == NULL)
        {
            sysLoger(SLWARNING, "RspTcpServer::run: alloc tcp device fro TxThread obejct failed!");
            break;
        }
        clientTcpDevice->setTcpSocket(srvSocket.m_sock);

        TxThread::getInstance()->setDevice(clientTcpDevice);
        /*start the eventHandler*/
        if(NULL == m_pEventHandler)
        {
            m_pEventHandler = new eventHander();
            if(NULL == m_pEventHandler)
            {
                sysLoger(SLWARNING, "RspTcpServer::run: alloc the event handler object failure!");
                m_runFlag = false;
                break;
            }
            m_pEventHandler->start();
        }
        
        while(m_runFlag)
        {
            Packet*pack = new Packet();
            if(NULL == pack)
            {
                sysLoger(SLWARNING, "RspTcpServer::run: allocate packet failure!");
                continue;
            }

            /*recieve the pack and process */
            if(TxThread::getInstance()->recvPack(pack) < 0)
            {
                sysLoger(SLINFO, "RspTcpServer::run:Client disconnect!");
                m_runFlag = false;
                break;
            }

            msgProcess(pack);
        }
    }
    
    m_runFlag = false;
    if(m_pTxThread != NULL)
        m_pTxThread->stop();
    if(m_pEventHandler != NULL)
        m_pEventHandler->stop();
    
    return SUCC;
}


int RspTcpServer::msgProcess(Packet * pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::msgProcess: argument pack is bad pointer!");
        return FAIL;
    }
    
    /*get the message src aid*/
    m_CurOpsSrcAid = pack->getSrc_aid();
    char *pData = pack->getData();
    int packSize = pack->getSize();
    int iRet = 0;
    sysLoger(SLINFO, "RspTcpServer::msgProcess: Recieved command char :%c", pData[0]);
    if((packSize == strlen(GET_ENDIAN)) && (strcmp(pData, GET_ENDIAN) == 0))
    {
        //回得大小端
        sendReply("little", strlen("little"));
        return SUCC;
    }
    switch(pData[0])
    {
        case CRT_SESSION:
            iRet = createSessionProc(pack);
            break;
            
        case RSP_WRITE_MEMORY:
            iRet = writeMemoryProc(pack);
            break;
            
        case RSP_READ_MEMORY:
            iRet = readMemoryProc(pack);
            break;
            
        case RSP_SET_ALL_REGISTER:
            iRet = writeAllRegistersProc(pack);
            break;
            
        case RSP_GET_ALL_REGISTER:
            iRet = readAllRegistersProc(pack);
            break;
            
        case RSP_SET_BREAKPOINT:
            iRet = setBreakPointProc(pack);
            break;
            
        case RSP_DELETE_BREAKPOINT:
            iRet = delBreakPointProc(pack);
            break;
            
        case RSP_RUN_TARGET:
            iRet = continueTargetProc(pack);
            break;

        case RSP_STOP_TARGET:
            iRet = stopTargetProc(pack);
            break;

        case RSP_STEP_TARGET:
            iRet = stepTargetProc(pack);
            break;

        case RSP_QUERY_TARGET_STATE:
            iRet = queryTargetCPUStatusProc(pack);
            break;
            
        default:
            sysLoger(SLWARNING, "RspTcpServer::msgProcess: recived unkonw command!");
            iRet = FAIL;
            break;
    }
    return iRet;
}
/**
 * @Funcname:  createSessionProc
 * @brief        :  处理创建会话, 连接模拟器
 * @para1      : RSP_Packet *pack 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月11日 16:57:16
 *   
**/
int RspTcpServer::createSessionProc(Packet *pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::createSessionProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    /*Recored the ice server aid*/
    m_IceSrvId = pack->getSrc_aid();
    
    char *pBuf = pack->getData() + RSP_CMD_LEN;
    char *pStr = NULL;
    pStr = strstr(pBuf, "-cputype=");
    if(NULL == pStr)
    {
        sysLoger(SLWARNING, "RspTcpServer::createSessionProc: Lost cputype config part!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    pBuf = pStr + strlen("-cputype=");
    hex2int(&pBuf, &m_CpuType);

    m_edian = BIG_EDIAN;
    pStr = strstr(pBuf, "-endian=big");
    if(NULL == pStr)
    {
        pStr = strstr(pBuf, "-endian=little");
        if(NULL == pStr)
        {
            sysLoger(SLWARNING, "RspTcpServer::createSessionProc: Lost cputype config part!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
        m_edian = LITTLE_EDIAN;
    }
    
    try
    {
        if(m_SimuloatorConnected)
        {
            wcsDisConnect();
        }

        if(wcsConnect(m_dbgInfo) == WCS_RET_OK)
        {
            m_SimuloatorConnected = true;
            
            sendReply(SUCC_ACK, SUCC_ACK_LEN);
            return SUCC;
        }
        else
        {
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::createSessionProc: Exception encountered when called wcsConnect()");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
    }
    return FAIL;
}

/**
 * @Funcname:  writeMemoryProc
 * @brief        :  写内存操作
 * @para1      : RSP_Packet * pack  写内存包
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月11日 16:59:32
 *   
**/
int RspTcpServer::writeMemoryProc(Packet * pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::writeMemoryProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }

    /*write memory packet format: Xaddr,size:data*/
    char *pBuf = pack->getData() + RSP_CMD_LEN;
    char *pStr = NULL;

    int addr, size, value;
    //address
    pStr = strstr(pBuf, ",");
    if(pStr == NULL)
    {
        sysLoger(SLWARNING, "RspTcpServer::writeMemoryProc: Command format error, lost address part!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    hex2int(&pBuf, &addr);
    pBuf = pStr + strlen(",");
    //size
    pStr = strstr(pBuf, ":");
    if(pStr == NULL)
    {
        sysLoger(SLWARNING, "RspTcpServer::writeMemoryProc: Command format error, lost size part!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    hex2int(&pBuf, &size);
    pBuf = pStr + strlen(":");


    WCS_ADDR_T wcsAddr;
    wcsAddr.uAddr = addr;
    try
    {
        if(wcsSetMem(&wcsAddr, (const UINT8*)pBuf, size) == WCS_RET_OK)
        {
            sysLoger(SLDEBUG, "RspTcpServer::writeMemoryProc: addr:0x%x, size:%d", addr, size);
            sendReply(SUCC_ACK, SUCC_ACK_LEN);
            return SUCC;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::writeMemoryProc: Exception encounted when called wcsSetMem()");
    }
    sendReply(FAIL_ACK, FAIL_ACK_LEN);
    return FAIL;
}

/**
 * @Funcname:  readMemoryProc
 * @brief        :  处理读内存
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月11日 17:45:44
 *   
**/
int RspTcpServer::readMemoryProc(Packet *pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::readMemoryProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    /*Format: maddr,size*/
    char *pBuf = pack->getData() + RSP_CMD_LEN;
    char *pStr = NULL;
    int addr, size;
    pStr = strstr(pBuf, ",");
    if(pStr == NULL)
    {
        sysLoger(SLINFO, "RspTcpServer::readMemoryProc: command packet format error!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }

    hex2int(&pBuf, &addr);
    hex2int(&(++pStr), &size);

    UINT8 *pData = (UINT8*)malloc(size+10);
    if(NULL == pData)
    {
        sysLoger(SLINFO, "RspTcpServer::readMemoryProc: alloc the buffer for store memory failure!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    memset(pData, 0, size + 10);
    WCS_ADDR_T wcsAddr;
    wcsAddr.uAddr = addr;
    pack->clearData();
    char *pReply = pack->getData();
    try
    {
        if(wcsGetMem(&wcsAddr, pData, size) == WCS_RET_OK)
        {
            sysLoger(SLDEBUG, "RspTcpServer::readMemoryProc: addr:0x%x, size:%d..", addr, size);
            /*reply format: OK len, xxxx*/
            memcpy(pReply, SUCC_ACK, SUCC_ACK_LEN);
            pReply += SUCC_ACK_LEN;
            pReply += int2hex(pReply, 10, size);
            memcpy(pReply, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pReply += CONTROL_FLAG_LEN;
            
            memcpy(pReply, pData, size);
            pReply += size;
            pack->setSize(pReply - pack->getData());
            
            sendReply(pack->getData(), pack->getSize());
            free(pData);
            pData = NULL;
            return SUCC;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::readMemoryProc: Exception encounted when called wcsGetMem()");
    }
    sendReply(FAIL_ACK, FAIL_ACK_LEN);
    if(pData != NULL)
    {
        free(pData);
    }
    return FAIL;
}

/**
 * @Funcname:  readAllRegistersProc
 * @brief        :  读取所有寄存器的值
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月11日 20:04:24
 *   
**/
int RspTcpServer::readAllRegistersProc(Packet *pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::readAllRegistersProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    /*Format: g*/
    UINT32 regNo[ARM_REG_NUM];
    UINT32 regValue[ARM_REG_NUM];
    for(int i = 0; i < ARM_REG_NUM; i++)
    {
        if(jtagIDtoRealID[i] <= WCS_SPSR )
        {
            regNo[i] = WCS_MAKE_REGID(WCS_CURR_MODE, jtagIDtoRealID[i]);
        }
        else if((jtagIDtoRealID[i] >= WCS_SPSR_FIQ) && (jtagIDtoRealID[i] <= WCS_R14_FIQ))
        {
            regNo[i] = WCS_MAKE_REGID(WCS_FIQ_MODE, jtagIDtoRealID[i]);
        }
        else if((jtagIDtoRealID[i] >= WCS_SPSR_IRQ) && (jtagIDtoRealID[i] <= WCS_R14_IRQ))
        {
            regNo[i] = WCS_MAKE_REGID(WCS_IRQ_MODE, jtagIDtoRealID[i]);
        }
        else if((jtagIDtoRealID[i] >= WCS_SPSR_SVC) && (jtagIDtoRealID[i] <= WCS_R14_SVC))
        {
            regNo[i] = WCS_MAKE_REGID(WCS_SVC_MODE, jtagIDtoRealID[i]);
        }
        else if((jtagIDtoRealID[i] >= WCS_SPSR_ABT) && (jtagIDtoRealID[i] <= WCS_R14_ABT))
        {
            regNo[i] = WCS_MAKE_REGID(WCS_ABT_MODE, jtagIDtoRealID[i]);
        }
        else if((jtagIDtoRealID[i] >= WCS_SPSR_UND) && (jtagIDtoRealID[i] <= WCS_R14_UND))
        {
            regNo[i] = WCS_MAKE_REGID(WCS_UND_MODE, jtagIDtoRealID[i]);
        }
        regValue[i] = 0;
    }
    try
    {
        if(wcsGetRegs(ARM_REG_NUM, regNo, regValue) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::readAllRegistersProc: Get all arm regNo failure!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::readMemoryProc: Exception encounted when called wcsGetMem()");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }

    char replyBuf[(ARM_REG_NUM + 2)*4];
    char *pBuf = replyBuf;
    memset(pBuf, 0, sizeof(replyBuf));
    memcpy(pBuf, SUCC_ACK, SUCC_ACK_LEN);
    pBuf += SUCC_ACK_LEN;
    pBuf += int2hex(pBuf, 10, ARM_REG_NUM*4);
    memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
    pBuf += CONTROL_FLAG_LEN;
    for(int i = 0 ; i < ARM_REG_NUM; i++)
    {
        Int2Char(regValue[i], pBuf, TRUE);
        pBuf += sizeof(UINT32);
    }

    sendReply(replyBuf, pBuf - replyBuf);
    return SUCC;
}

/**
 * @Funcname:  writeAllRegistersProc
 * @brief        :  写寄存器的值
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月12日 17:23:44
 *   
**/
int RspTcpServer::writeAllRegistersProc(Packet * pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::writeAllRegistersProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }

    
    /*iceServer 下发的格式:Gxxxx*/
    char *pBuf = pack->getData() + RSP_CMD_LEN;
    UINT32 num = 0;
    UINT32 regNo[ARM_REG_NUM], regValue[ARM_REG_NUM];
    UINT32 tmpRegNo[ARM_REG_NUM], tmpRegValue[ARM_REG_NUM];
    UINT32 cpsrNo, cpsrValue, currentSpsrValue;
    memset(regValue, 0, ARM_REG_NUM);
    memset(regNo, 0, ARM_REG_NUM);
    num = (pack->getSize() - RSP_CMD_LEN)/sizeof(UINT32);
    
    cpsrNo = WCS_MAKE_REGID(WCS_CURR_MODE, WCS_CPSR);
    wcsGetRegs(1, &cpsrNo, &cpsrValue);
    printf("\nCurrent Register's mode :%d", cpsrValue & 0x1f);
    
    for(int i = 0; i < num; i++)
    {
        regValue[i] = Char2Int(pBuf, TRUE);
        pBuf += sizeof(UINT32);
    }

    //cpsr
    tmpRegValue[WCS_CPSR] = regValue[JTAG_CPSR];
    //spsr
    switch(getCurrentRegMode(regValue[JTAG_CPSR]))
    {
        case ARM_ABORT_MODE:
            currentSpsrValue = regValue[JTAG_ABT_SPSR];
            break;
            
        case ARM_FIQ_MODE:
            currentSpsrValue = regValue[JTAG_FIQ_SPSR];
            break;
            
        case ARM_IRQ_MODE:
            currentSpsrValue = regValue[JTAG_IRQ_SPSR];
            break;
            
        case ARM_SUPER_MODE:
            currentSpsrValue = regValue[JTAG_SVC_SPSR];
            break;

        case ARM_SYSTEM_MODE:
            currentSpsrValue = regValue[JTAG_SVC_SPSR];
            break;

        case ARM_UNDEFINED_MODE:
            currentSpsrValue = regValue[JTAG_UND_SPSR];
            break;

        default:
            currentSpsrValue = regValue[JTAG_UND_SPSR];
            break;
        
    }
    tmpRegValue[WCS_SPSR] = currentSpsrValue;
    tmpRegValue[WCS_SPSR_FIQ] = regValue[JTAG_FIQ_SPSR];
    tmpRegValue[WCS_SPSR_IRQ] = regValue[JTAG_IRQ_SPSR];
    tmpRegValue[WCS_SPSR_SVC] = regValue[JTAG_SVC_SPSR];
    tmpRegValue[WCS_SPSR_ABT] = regValue[JTAG_ABT_SPSR];
    tmpRegValue[WCS_SPSR_UND] = regValue[JTAG_UND_SPSR];
    //USer mode Registers
    for(int i = 0; i <= WCS_R15; i++)
    {        
        tmpRegValue[i] = regValue[i];
    }
    for(int i = 0; i <= WCS_SPSR; i ++)
    {
        tmpRegNo[i] = WCS_MAKE_REGID(WCS_CURR_MODE, i);
    }

    //FIQ mode registers
    for(int i = WCS_R8_FIQ, j = JTAG_FIQ_R8; i <= WCS_R14_FIQ, i <= JTAG_FIQ_R14; i++, j++)
    {
        tmpRegValue[i] = regValue[j];
    }
    for(int i = WCS_SPSR_FIQ; i <= WCS_R14_FIQ; i ++)
    {
        tmpRegNo[i] = WCS_MAKE_REGID(WCS_FIQ_MODE, i);
    }
    //IRQ mode registers
    for(int i = WCS_R13_IRQ, j = JTAG_IRQ_R13; i <= WCS_R14_IRQ, i <= JTAG_IRQ_R14; i++, j++)
    {
        tmpRegValue[i] = regValue[j];
    }
    for(int i = WCS_SPSR_IRQ; i <= WCS_R14_IRQ; i ++)
    {
        tmpRegNo[i] = WCS_MAKE_REGID(WCS_IRQ_MODE, i);
    }
    //SVC mode registers
    for(int i = WCS_R13_SVC, j = JTAG_SVC_R13; i <= WCS_R14_SVC, i <= JTAG_SVC_R14; i++, j++)
    {
        tmpRegValue[i] = regValue[j];
    }
    for(int i = WCS_SPSR_SVC; i <= WCS_R14_SVC; i ++)
    {
        tmpRegNo[i] = WCS_MAKE_REGID(WCS_SVC_MODE, i);
    }
    //ABT mode registers
    for(int i = WCS_R13_ABT, j = JTAG_ABT_R13; i <= WCS_R14_ABT, i <= JTAG_ABT_R14; i++, j++)
    {
        tmpRegValue[i] = regValue[j];
    }
    for(int i = WCS_SPSR_ABT; i <= WCS_R14_ABT; i ++)
    {
        tmpRegNo[i] = WCS_MAKE_REGID(WCS_ABT_MODE, i);
    }
    //UND mode registers
    for(int i = WCS_R13_UND, j = JTAG_UND_R13; i <= WCS_R14_UND, i <= JTAG_UND_R14; i++, j++)
    {
        tmpRegValue[i] = regValue[j];
    }
    for(int i = WCS_SPSR_UND; i <= WCS_R14_UND; i ++)
    {
        tmpRegNo[i] = WCS_MAKE_REGID(WCS_UND_MODE, i);
    }
    try
    {
        
        //current mode
        if(wcsSetRegs(WCS_SPSR+1, tmpRegNo, tmpRegValue) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::writeAllRegistersProc: Set Current mode arm registers failed!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
        //FIQ mode
        if(wcsSetRegs(WCS_R14_FIQ - WCS_SPSR_FIQ + 1, &tmpRegNo[WCS_SPSR_FIQ], &tmpRegValue[WCS_SPSR_FIQ]) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::writeAllRegistersProc: Set FIQ mode arm registers failed!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
        //IRQ MODE
        if(wcsSetRegs(WCS_R14_IRQ - WCS_SPSR_IRQ + 1, &tmpRegNo[WCS_SPSR_IRQ], &tmpRegValue[WCS_SPSR_IRQ]) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::writeAllRegistersProc: Set IRQ mode arm registers failed!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
        //SVC/SYS mode
        if(wcsSetRegs(WCS_R14_SVC - WCS_SPSR_SVC + 1, &tmpRegNo[WCS_SPSR_SVC], &tmpRegValue[WCS_SPSR_SVC]) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::writeAllRegistersProc: Set SVC mode arm registers failed!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
        //ABT mode
        if(wcsSetRegs(WCS_R14_ABT - WCS_SPSR_ABT + 1, &tmpRegNo[WCS_SPSR_ABT], &tmpRegValue[WCS_SPSR_ABT]) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::writeAllRegistersProc: Set ABT mode arm registers failed!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
        //UND mode
        if(wcsSetRegs(WCS_R14_UND - WCS_SPSR_UND + 1, &tmpRegNo[WCS_SPSR_UND], &tmpRegValue[WCS_SPSR_UND]) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::writeAllRegistersProc: Set UND mode arm registers failed!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::writeAllRegistersProc: Exception encounted when called wcsSetRegs()");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }    
    sendReply(SUCC_ACK, SUCC_ACK_LEN);
    return SUCC;
}

/**
 * @Funcname:  setBreakPointProc
 * @brief        :  设置断点
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月12日 17:25:06
 *   
**/
int RspTcpServer::setBreakPointProc(Packet * pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::setBreakPointProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    /*format: Z?,addr,len*/
    char *pBuf = pack->getData() + RSP_CMD_LEN;
    int type, addr, len;
    WCS_BKPT_T wcsBp;
    WCS_OPCODE_T wcsOpCode;
    
    pBuf +=hex2int(&pBuf, &type);
    pBuf += CONTROL_FLAG_LEN;
    pBuf +=hex2int(&pBuf, &addr);
    pBuf += CONTROL_FLAG_LEN;
    pBuf +=hex2int(&pBuf, &len);

    if((type == SOFT_BP) && (mapAddr2Value.contains(addr)))
    {
        sysLoger(SLINFO, "RspTcpServer::setBreakPointProc: Already set breakpoint at addr:%x, "\
                                                        "send ok to client!", addr);
        sendReply(SUCC_ACK, SUCC_ACK_LEN);
        return SUCC;
    }
    wcsBp.uAddr = addr;
    wcsBp.uType = (WCS_BKPT_TYPE_T)type;
    wcsBp.uLength = len;
    try
    {
        if(wcsSetBp(&wcsBp, &wcsOpCode) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::setBreakPointProc:set breakpoint at Addr:%x failure!", addr);
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::setBreakPointProc: Exception encounted when called wcsSetBp()");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }    
    sysLoger(SLINFO, "RspTcpServer::setBreakPointProc:set breakpoint at Addr:%x succcess!", addr);
    mapAddr2Value.insert(addr, wcsOpCode);
    sendReply(SUCC_ACK, SUCC_ACK_LEN);
    return SUCC;
    
}

/**
 * @Funcname:  delBreakPointProc
 * @brief        :  删除断点
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月12日 19:59:05
 *   
**/
int RspTcpServer::delBreakPointProc(Packet * pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::delBreakPointProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    /*format: z?,addr,len*/
    char *pBuf = pack->getData() + RSP_CMD_LEN;
    int type, addr, len;
    WCS_BKPT_T wcsBp;
    WCS_OPCODE_T wcsOpCode;
    
    pBuf +=hex2int(&pBuf, &type);
    pBuf += CONTROL_FLAG_LEN;
    pBuf +=hex2int(&pBuf, &addr);
    pBuf += CONTROL_FLAG_LEN;
    pBuf +=hex2int(&pBuf, &len);

    wcsBp.uAddr = addr;
    wcsBp.uType = (WCS_BKPT_TYPE_T)type;
    wcsBp.uLength = len;
    wcsOpCode = getValueFromAddr(addr);
    if((type == SOFT_BP) && (!mapAddr2Value.contains(addr)))
    {
        sysLoger(SLWARNING, "RspTcpServer::delBreakPointProc: There was no breakpoint at addr:%x!", addr);
        sendReply(SUCC_ACK, SUCC_ACK_LEN);
        return SUCC;
    }
    try
    {
        if(wcsClearBp(&wcsBp, wcsOpCode) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::delBreakPointProc:delete breakpoint at Addr:%x failure!", addr);
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::delBreakPointProc: Exception encounted when called wcsClearBp()");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }    
    sysLoger(SLINFO, "RspTcpServer::delBreakPointProc:delete breakpoint at Addr:%x success!", addr);
    mapAddr2Value.remove(addr);
    sendReply(SUCC_ACK, SUCC_ACK_LEN);
    return SUCC;
}

/**
 * @Funcname:  stopTargetProc
 * @brief        :  停止目标机处理
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月13日 10:03:48
 *   
**/
int RspTcpServer::stopTargetProc(Packet * pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::stopTargetProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    try
    {
        if(wcsAbort(0) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::stopTargetProc:Stop target failure!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::stopTargetProc: Exception encounted when called wcsAbort()");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }    
    sysLoger(SLINFO, "RspTcpServer::stopTargetProc:Stop target successfully!");
    sendReply(SUCC_ACK, SUCC_ACK_LEN);
    return SUCC;
}

/**
 * @Funcname:  continueTargetProc
 * @brief        :  运行目标机
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月13日 10:11:36
 *   
**/
int RspTcpServer::continueTargetProc(Packet * pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::continueTargetProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    try
    {
        /*TODO: 这里先回OK是因为在调用了wcsGo后模拟器会触发
        模式切换事件, 如果这时线程上下文发生了切换，那么S05事件
        会先于C命令的OK回给IS，导致IS 丢弃S05消息, GDB无法停下来
        */
        sendReply(SUCC_ACK, SUCC_ACK_LEN);
        if(wcsGo(0, NULL) == WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::continueTargetProc:Run the target successfully!");
            return SUCC;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::continuetargetProc: Exception encounted when called wcsGo()");
    }    
    sysLoger(SLINFO, "RspTcpServer::continuetargetProc:Run the target failed!");
    return FAIL;
}

/**
 * @Funcname:  stepTargetProc
 * @brief        :  单步目标机
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月13日 10:26:05
 *   
**/
int RspTcpServer::stepTargetProc(Packet * pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::stepTargetProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    try
    {
        if(wcsStepOne(0) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::stepTargetProc:Step  the target failure!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::stepTargetProc: Exception encounted when called wcsStepOne()");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }    
    sysLoger(SLINFO, "RspTcpServer::stepTargetProc:Step  the target successfully!");
    sendReply(SUCC_ACK, SUCC_ACK_LEN);
    return SUCC;
}

/**
 * @Funcname:  resetTargetProc
 * @brief        :  目标机复位
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月13日 10:34:06
 *   
**/
int RspTcpServer::resetTargetProc(Packet * pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::resetTargetProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    try
    {
        if(wcsReset(NULL) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::resetTargetProc:Reset  the target failure!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::resetTargetProc: Exception encounted when called wcsReset()");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }    
    sysLoger(SLINFO, "RspTcpServer::resetTargetProc:Reset  the target successfully!");
    sendReply(SUCC_ACK, SUCC_ACK_LEN);
    return SUCC;
}

/**
 * @Funcname:  queryTargetCPUStatusProc
 * @brief        :  查询目标机CPU状态
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月13日 10:36:03
 *   
**/
int RspTcpServer::queryTargetCPUStatusProc(Packet * pack)
{
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::queryTargetCPUStatusProc: argument pack is bad pointer!");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }
    WCS_CPUSTAT_T   wcsCpuStatus;
    try
    {
        if(wcsGetCpuStatus(&wcsCpuStatus) != WCS_RET_OK)
        {
            sysLoger(SLINFO, "RspTcpServer::queryTargetCPUStatusProc:Query  the target failure!");
            sendReply(FAIL_ACK, FAIL_ACK_LEN);
            return FAIL;
        }
    }
    catch(...)
    {
        sysLoger(SLWARNING, "RspTcpServer::queryTargetCPUStatusProc: Exception encounted when called wcsGetCpuStatus()");
        sendReply(FAIL_ACK, FAIL_ACK_LEN);
        return FAIL;
    }    
    /*convert the wcs cpu status to JTAG status*/
    UINT32  status;
    sysLoger(SLINFO, "RspTcpServer::queryTargetCPUStatusProc: wcscpuStatus.uMode:%d, wcsCpuStatus.uSigNum:%d",
        wcsCpuStatus.uMode, wcsCpuStatus.uSigNum);
    switch((wcsCpuStatus.uMode << 4) |wcsCpuStatus.uSigNum)
    {
        case (WCS_DEBUG_MODE << 4) | WCS_SIGHUP_SIGNAL:
            status = JTAG_CPU_DEBUG_HANGUP;
            break;
            
        case (WCS_DEBUG_MODE << 4) | WCS_SIGINT_SIGNAL:
            status = JTAG_CPU_BKP_HANGUP;
            break;
            
        case (WCS_DEBUG_MODE << 4) | WCS_SIGILL_SIGNAL:
        case (WCS_DEBUG_MODE << 4) | WCS_SIGTRAP_SIGNAL:
            status = JTAG_CPU_BKP_EXCEPTION;
            break;
            
        case (WCS_NORMAL_MODE << 4) | WCS_SIGNON_SIGNAL:
            status = JTAG_CPU_NORMAL;
            break;
            
        default:
            status = JTAG_CPU_UNKONW_STATUS;
            break;
    }
    char replyBuf[10] = "\0";
    replyBuf[0] = 'P';
    replyBuf[1] = 'N';
    sprintf(&replyBuf[2], "%x", status);
    sysLoger(SLINFO, "RspTcpServer::queryTargetCPUStatusProc:Query  the target successfully!");
    sendReply(replyBuf, strlen(replyBuf));
    return SUCC;
}

/**
 * @Funcname:  sendReply
 * @brief        :  发送回复消息给发送线程队列
 * @para1      : char * buf , 要发送的数据
 * @para2      : len，待发数据的字节长度
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月13日 14:48:32
 *   
**/
int RspTcpServer::sendReply(const char * buf, UINT32 len)
{
    if(NULL == buf)
    {
        sysLoger(SLWARNING, "RspTcpServer::sendReply:Argument buf is bad pointer!");
        return FAIL;
    }
    Packet *pack = PacketManager::getInstance()->alloc();
    if(NULL == pack)
    {
        sysLoger(SLWARNING, "RspTcpServer::sendReply:Alloc packet for repling failure!");
        return FAIL;
    }
    char *pBuf = pack->getData();
    memcpy(pBuf, buf, len);
    pBuf += len;
    pack->setSize(len);
    pack->setDes_aid(m_CurOpsSrcAid);
    
    //TxThread::getInstance()->pushPack(pack);
    TxThread::getInstance()->sendPack(pack);
    return SUCC;
}

/**
 * @Funcname:  getValueFromAddr
 * @brief        :  获取断点地址处的操作码
 * @para1      : addr: 断点地址
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月18日 11:00:22
 *   
**/
UINT32 RspTcpServer::getValueFromAddr(UINT32 addr)
{
    QMap<UINT32, UINT32>::iterator itrAddr = mapAddr2Value.find(addr);
    if(itrAddr != mapAddr2Value.end())
    {
        return itrAddr.value();
    }
    return 0;
}

/*bool RspTcpServer::getWcsRegValueSequ(wcsRegNo, JtagRegValue, num)
{
    for(int i = 0; i < num; i++)
    {
        if(wcsRegNo == jtagIDtoRealID[i])
        {
            return true;
        }
    }
    return false;
}*/
/***************** reserved ****************************/


/**
 * @Funcname:  downLoadImageFileProc
 * @brief        :  下载目标文件
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月13日 11:57:09
 *   
**/
int RspTcpServer::downLoadImageFileProc(Packet * pack)
{
    return SUCC;
}

/**
 * @Funcname:  setTargetPropetyProc
 * @brief        :  设置目标机属性
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月13日 13:50:18
 *   
**/
int RspTcpServer::setTargetPropetyProc(Packet * pack)
{
    return SUCC;
}

/**
 * @Funcname:  getTargetProperty
 * @brief        :  获取目标机属性
 * @para1      : 
 * @return      : 
 * @Author     :  qingxiaohai
 *  
 * @History: 1.  Created this function on 2007年4月13日 13:51:12
 *   
**/
int RspTcpServer::getTargetPropetyProc()
{
    return SUCC;
}


int main()
{
    RspTcpServer::getInstance()->run();
    return 0;
}
