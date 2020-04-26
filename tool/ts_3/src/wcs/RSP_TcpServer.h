/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  RSP_TcpServer.h
 * @Version        :  1.0.0
 * @Brief           :  实现RSP协议收发和处理的TCP通信方式的服务器
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月9日 15:23:00
 * @History        : 
 *   
 */

#ifndef _RSP_TCP_SERVER_H_
#define _RSP_TCP_SERVER_H_

#include "wsocket.h"
#include "qthread.h"
#include "qmap.h"
#include "qstring.h"
#include "qmutex.h"
#include "qsemaphore.h"
#include "qwaitCondition.h"
#include "sysType.h"
#include "comfunc.h"
#include "tcpdevice.h"
#include "log.h"
#include "eventhandler.h"
#include "txthread.h"



typedef WCS_RETCODE_T (*pwcsConnect)(const WCS_DBGINFO_T	*pDbgInfo);
typedef WCS_RETCODE_T (*pwcsDisConnect)();
typedef WCS_RETCODE_T (*pwcsGetRegs)(UINT32 uNum, const UINT32 *puRegIdList, WCS_REGVAL_T *puContent);
typedef WCS_RETCODE_T (*pwcsSetRegs)(UINT32 uNum, const UINT32 *puRegIdList, const WCS_REGVAL_T *puContent);
typedef WCS_RETCODE_T (*pwcsDownload)(WCS_ADDR_T *pAddr, 
                                                                        const UINT8 *puImage,
                                                                    	UINT32 uByteLen,
                                                                    	UINT32 uTotalLen,
                                                                    	UINT32 uFlag);
typedef WCS_RETCODE_T (*pwcsGetMem)(WCS_ADDR_T			*pAddr,
                                                                        	UINT8				*puBuffer,
                                                                        	UINT32				uByteLen
                                                                               );

typedef WCS_RETCODE_T (*pwcsSetMem)(WCS_ADDR_T			*pAddr,
                                                                        	const UINT8			*puBuffer,
                                                                        	UINT32				uByteLen
                                                                                );


typedef WCS_RETCODE_T (*pwcsAbort)(UINT32 uThreadId);

typedef WCS_RETCODE_T (*pwcsGetCpuStatus)(WCS_CPUSTAT_T *pCpuStatus);
typedef WCS_RETCODE_T (*pwcsGo)(	UINT32 uThreadId,		// reserved
                                                                WCS_ADDR_T *pTillAddr);

typedef WCS_RETCODE_T (*pwcsReset)(WCS_ADDR_T *pHaltAddr);
typedef WCS_RETCODE_T (*pwcsStepOne)(UINT32 uStepCount);
typedef WCS_RETCODE_T (*pwcsClearBp)(const WCS_BKPT_T	*pBkpt,
	                                                               WCS_OPCODE_T	 uSavedOpCode);
typedef WCS_RETCODE_T (*pwcsSetBp)(const WCS_BKPT_T	*pBkpt,
                                                                      WCS_OPCODE_T	*puSavedOpCode);
typedef WCS_RETCODE_T (*pwcsGetProperty)(
                                                                            	UINT32				uProp,
                                                                            	UINT32				*puValue);
typedef WCS_RETCODE_T (*pwcsSetProperty)(
                                                                            	UINT32				uProp,
                                                                            	UINT32				uValue);
typedef WCS_RETCODE_T (*pwcsMiscFun)(
                                                                	char				*psFunName,
                                                                	void				*pFunArg,
                                                                	void				*pFunResult);

class RspTcpServer
{
    private:
        tcpDevice    m_listenTcpDevice;
        tcpDevice    m_clientTcpDevice;
        bool            m_runFlag;
        static QMutex m_mtxInit;
        static RspTcpServer* m_pSelf;
        TxThread*  m_pTxThread;
        eventHander* m_pEventHandler;
        int                 m_port;
        int                 m_CpuType;
        int                 m_edian;                                         /*false: little,   true: big*/
        bool              m_SimuloatorConnected;               /*indicate if connected to wcsSimulator*/
        QMap<UINT32, UINT32> mapAddr2Value;           /*used to save the breakpoint opcode*/

        WCS_DBGINFO_T *m_dbgInfo;                            /*wcs simulator connect argument*/
        //HMODULE        m_wcsdll;
        /*wcs dll's function*/
        /*pwcsConnect             m_wcsConnect;
        pwcsDisConnect      m_wcsDisConnect;
        pwcsGetRegs             m_wcsGetRegs;
        pwcsSetRegs             m_wcsSetRegs;
        pwcsDownload            m_wcsDownload;
        pwcsGetMem              m_wcsGetMem;
        pwcsSetMem              m_wcsSetMem;
        pwcsAbort                   m_wcsAbort;
        pwcsGetCpuStatus      m_wcsGetCpuStatus;
        pwcsGo                        m_wcsGo;
        pwcsReset                   m_wcsReset;
        pwcsStepOne               m_wcsStepOne;
        pwcsClearBp                 m_wcsClearBp;
        pwcsSetBp                   m_wcsSetBp;
        pwcsGetProperty         m_wcsGetProperty;
        pwcsSetProperty          m_wcsSetProperty;
        pwcsMiscFun                 m_wcsMiscFun;*/
        
    public:
    
        short m_IceSrvId;
        short m_CurOpsSrcAid;
        
        RspTcpServer();
        ~RspTcpServer();
        static RspTcpServer* getInstance()
        {
            if(NULL == m_pSelf)
            {
                m_mtxInit.lock();
                if(NULL == m_pSelf)
                {
                    m_pSelf = new RspTcpServer();
                }
                m_mtxInit.unlock();
            }
            return m_pSelf;
        }
        int run();
        void close();

        int msgProcess(Packet *pack);

        int createSessionProc(Packet *pack);
        
        int readMemoryProc(Packet *pack);
        int writeMemoryProc(Packet *pack);
        int readAllRegistersProc(Packet *pack);
        int writeAllRegistersProc(Packet *pack);
        int setBreakPointProc(Packet *pack);
        int delBreakPointProc(Packet *pack);
        int stopTargetProc(Packet *pack);
        int continueTargetProc(Packet *pack);
        int stepTargetProc(Packet *pack);
        int resetTargetProc(Packet *pack);
        int queryTargetCPUStatusProc(Packet *pack);
        int downLoadImageFileProc(Packet *pack);

        int setTargetPropetyProc(Packet *pack);
        int getTargetPropetyProc();

        int sendReply(const char* buf, UINT32 len);

        void addMapAddr2Value(UINT32 addr, UINT32 value);
        UINT32 getValueFromAddr(UINT32 addr);

        //UINT32 getWcsRegValueSequ(UINT32 wcsRegNo, UINT32 *JtagRegValue, UINT32 num);
        
        eventHander* getEventHandler()
        {
            return m_pEventHandler;
        }
        TxThread* getTxThread()
        {
            return m_pTxThread;
        }

        int loadWCSLibraryProc();
};


#endif /*_RSP_TCP_SERVER_H_*/

