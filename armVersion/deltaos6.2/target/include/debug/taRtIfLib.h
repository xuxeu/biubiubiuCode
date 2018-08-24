/***************************************************************************
 *                �����������ɼ������޹�˾ ��Ȩ����
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *�޸���ʷ:
 *2013-08-28         ��Ԫ־�������������ɼ������޹�˾
 *                          �������ļ���
 */

/*
 * @file: taRtIfLib.h
 * @brief:
 *             <li>����ʱ�ӿ�</li>
 */
#ifndef TARTIFLIB_H_
#define TARTIFLIB_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/************************ͷ �� ��******************************/
#include "ta.h"

/************************���Ͷ���*****************************/

/* �����Ĳ����ӿ� */
typedef struct
{
	STATUS (*suspend)(int sid);
    STATUS (*resume)(int sid);
    ulong_t (*attach)(TA_CTX_TYPE ctxType, ulong_t objId, UINT32 *pSid);
    ulong_t (*detach)(UINT32 sid);
    STATUS (*contextGet)(ulong_t ctxId, T_DB_ExceptionContext *context);
    STATUS (*contextSet)(ulong_t ctxId, T_DB_ExceptionContext *context);
    STATUS (*idVerify)(ulong_t ctxId);
} TA_CTX_IF;

/* ����ʱ�ӿ� */
typedef struct
{
    /* �ڴ�����ӿ�  */
    ulong_t	(*memWrite)(ulong_t ctxId, ulong_t dest,const UINT8 *src, UINT32 nBytes, UINT32 width);
    ulong_t	(*memRead)(ulong_t ctxId, ulong_t readAddr, UINT8 *outBuf,UINT32 nBytes,UINT32 width);

    /* �ڴ����ӿ�  */
    void* (*memalloc)(UINT32 nBytes);
    void  (*memfree)(void *pData);

    /* ����HOOK�����ӿ� */
    STATUS (*createHookAdd)(FUNCPTR createHook);
    STATUS (*deleteHookAdd)(FUNCPTR deleteHook);

    /* �ڴ��������л�����  */
    ulong_t (*memCtxSwitch)(ulong_t newCtx, int options);

    /* ҳ��С */
    int	pageSize;

    /* �����Ĳ����ӿ� */
    TA_CTX_IF taCtxIf[TA_CTX_TYPE_NUM];
} TA_RT_IF;

/************************�� �� ��******************************/

#define	TA_RT_CTX(contextType)	taRtIf.taCtxIf[(contextType)]

/* ����ʱ�����Ĳ����ӿ� */
#define TA_RT_CTX_ID_VERIFY(contextType, ctxId)		\
	    ((TA_RT_CTX(contextType)).idVerify == NULL ?\
	     TA_ERR_NO_RT_PROC :					    \
		(TA_RT_CTX(contextType)).idVerify((ctxId)))

#define TA_RT_CTX_ATTACH(contextType, objId, sid)	\
	    ((TA_RT_CTX(contextType)).attach == NULL ?  \
	     TA_ERR_NO_RT_PROC :					    \
		(TA_RT_CTX(contextType)).attach((contextType),(objId),(sid)))

#define TA_RT_CTX_DETACH(contextType, sid)	        \
	    ((TA_RT_CTX(contextType)).detach == NULL ?  \
	     TA_ERR_NO_RT_PROC :					    \
		(TA_RT_CTX(contextType)).detach((sid)))

#define TA_RT_CTX_SUSPEND(contextType, ctxId)	    \
	    ((TA_RT_CTX(contextType)).suspend == NULL ? \
	     TA_ERR_NO_RT_PROC :					    \
		(TA_RT_CTX(contextType)).suspend((ctxId)))

#define TA_RT_CTX_REGSSET(contextType, ctxId, pRegs) \
	    ((TA_RT_CTX(contextType)).regsSet == NULL ?  \
	     TA_ERR_NO_RT_PROC :					     \
		(TA_RT_CTX(contextType)).regsSet((ctxId),(pRegs)))

#define TA_RT_CTX_REGSGET(contextType, ctxId, pRegs) \
	    ((TA_RT_CTX(contextType)).regsGet == NULL ?  \
	     TA_ERR_NO_RT_PROC :					     \
		(TA_RT_CTX(contextType)).regsGet((ctxId),(pRegs)))

#define TA_RT_CTX_RESUME(contextType, ctxId)	    \
	    ((TA_RT_CTX(contextType)).resume == NULL ?  \
	     TA_ERR_NO_RT_PROC :					    \
		(TA_RT_CTX(contextType)).resume((ctxId)))

/* ����ʱ�ӿ� */
#define	TA_RT_MEM_ALLOC(type, size)					\
		 (taRtIf.memalloc == NULL ?					\
		 (type *)NULL :                             \
		 (type *)taRtIf.memalloc((UINT32)((size) * sizeof(type))))

#define	TA_RT_MEM_FREE(addr)					    \
		(taRtIf.memfree == NULL ?					\
	     TA_ERR_NO_RT_PROC:						    \
	     taRtIf.memfree((void*)((addr))))

#define	TA_RT_MEM_SWITCH(memCtx,options)			\
		(taRtIf.memCtxSwitch == NULL ?				\
	     TA_ERR_NO_RT_PROC:						    \
	     taRtIf.memCtxSwitch((memCtx),(options)))

#define	TA_RT_MEM_CACHE_UPDATE(addr, nBytes)		\
		(taRtIf.cacheTextUpdate == NULL ?			\
		TA_ERR_NO_RT_PROC :					        \
		taRtIf.cacheTextUpdate ((addr), (nBytes)))

#define	TA_RT_DELETE_HOOK_ADD(hook)		            \
		(taRtIf.deleteHookAdd == NULL ?			    \
		TA_ERR_NO_RT_PROC :					        \
		taRtIf.deleteHookAdd ((hook)))

#define	TA_RT_CREATE_HOOK_ADD(hook)		            \
		(taRtIf.createHookAdd == NULL ?			    \
		TA_ERR_NO_RT_PROC :					        \
		taRtIf.createHookAdd ((hook)))

#define	TA_RT_MEM_READ(sid, readAddr, outBuf, nBytes, width)\
		(taRtIf.memRead == NULL ?					        \
		TA_ERR_NO_RT_PROC :					                \
		taRtIf.memRead((sid),(readAddr),(outBuf),(nBytes),(width)))

#define	TA_RT_MEM_WRITE(sid, dest, src, nBytes, width)		\
		(taRtIf.memWrite == NULL ?					        \
		TA_ERR_NO_RT_PROC :					                \
		taRtIf.memWrite((sid), (dest), (src), (nBytes), (width)))

#define TA_RT_GET_PAGE_SIZE() (taRtIf.pageSize)

/************************�ӿ�����*****************************/

extern TA_RT_IF taRtIf;

#ifdef __cplusplus
}
#endif

#endif /* TARTIFLIB_H_ */
