/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  wcsStub.cpp
 * @Version        :  1.0.0
 * @Brief           :  华邦模拟器接口的桩函数
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年4月17日 10:09:26
 * @History        : 
 *   
 */

#include "wcs_interface.h"


WCS_RETCODE_T    wcsConnect
(
    const WCS_DBGINFO_T    *pDbgInfo
)
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsDisConnect()
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsGetRegs
(
    UINT32                uNum,
    const UINT32        *puRegIdList,    // puRegIdList[uNum]
    WCS_REGVAL_T        *puContent        // puContent[uNum]
)
{
    for(int i = 0; i < uNum; i++)
    {
        *puContent = 0xabcd;
        puContent += sizeof(UINT32);
    }
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsSetRegs
(
    UINT32                uNum,
    const UINT32        *puRegIdList,    // puRegIdList[uNum]
    const WCS_REGVAL_T    *puContent        // puContent[uNum]
)
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsDownload
(
    WCS_ADDR_T            *pAddr,
    const UINT8            *puImage,
    UINT32                uByteLen,       //FIXME: differences between uByteLen and uTotalLen
    UINT32                uTotalLen,
    UINT32                uFlag            // reserved
)
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsGetMem
(
    WCS_ADDR_T            *pAddr,
    UINT8                *puBuffer,
    UINT32                uByteLen
)
{
    return WCS_RET_OK;
}


WCS_RETCODE_T    wcsSetMem
(
    WCS_ADDR_T            *pAddr,
    const UINT8            *puBuffer,
    UINT32                uByteLen
)
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsAbort
(
    UINT32                uThreadId        // reserved
)
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsGetCpuStatus
(
    WCS_CPUSTAT_T        *pCpuStatus
)
{
    pCpuStatus->uMode = WCS_DEBUG_MODE;
    pCpuStatus->uSigNum = WCS_SIGHUP_SIGNAL;
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsGo
(
    UINT32                uThreadId,        // reserved
    WCS_ADDR_T            *pTillAddr
)
{
    return WCS_RET_OK;
}


WCS_RETCODE_T    wcsReset
(
    WCS_ADDR_T            *pHaltAddr        // reserved
)
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsStepOne
(
    UINT32                uStepCount        // reserved
)
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsClearBp
(
    const WCS_BKPT_T    *pBkpt,
    WCS_OPCODE_T        uSavedOpCode
)
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsSetBp
(
    const WCS_BKPT_T    *pBkpt,
    WCS_OPCODE_T        *puSavedOpCode
)
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsGetProperty
(
    UINT32                uProp,
    UINT32                *puValue
)
{
    return WCS_RET_OK;
}

WCS_RETCODE_T    wcsSetProperty
(
    UINT32                uProp,
    UINT32                uValue
)
{
    return WCS_RET_OK;
}


WCS_RETCODE_T    wcsMiscFun
(
    char                *psFunName,
    void                *pFunArg,
    void                *pFunResult
)
{
    return WCS_RET_OK;
}


