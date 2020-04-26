
#include <vxWorks.h>
#include <vsbConfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <symLib.h>
#include <logLib.h>
#include <rtpLib.h>
#include <errnoLib.h>
#include <ioLib.h>
#include <private/objLibP.h>
#include <private/rtpLibP.h>
#include <private/taskLibP.h>
#include <private/rtpUtilLibP.h>
#include <tcf_config.h>
#include <framework/protocol.h>
#include <framework/json.h>
#include <framework/errors.h>
#include <framework/exceptions.h>
#include <services/sem.h>
#include <rtp.h>

#include <shlLibCommon.h>
#include <dllLib.h>
#include "taskLib.h"
#include "private/rtpLibP.h"
#include "private/rtpUtilLibP.h"
#include "rtpLibCommon.h"
#include <private/shlLibP.h>
#include "private/iosLibP.h"

#include <syscallLib.h>
#include <sysSymTbl.h>
#include <taskLib.h>
#include <objLib.h>
#include <hookLib.h>
#include <private/kernelBaseLibP.h>	/* MY_CTX_ID() */
#include <private/cplusLibP.h>
#include <private/funcBindP.h>
#include <private/syscallLibP.h>

#include <private/memEdrLibP.h>
#include <string.h>
#include <private/rtpLibP.h>
#include <symLib.h>
#include <sysSymTbl.h>
#include <taskHookLib.h>
#include <private/symLibP.h>
#define SYSCALL_NO_MONITOR 0
#define SYSCALL_MONITOR    1
LOCAL RTP_ID syscallRtpId;

extern FD_ENTRY * iosRtpFdTblEntryGet (RTP_ID, int);

extern DRV_ENTRY defaultDrv;

extern DRV_ENTRY fdInvalidDrv;

extern DRV_ENTRY *pIosSuspendedDevDrv;
static int flag1=0;

int GetRtpTaskIdList(int rtpId, int rtpTaskIdList[])
{
	WIND_TCB *tid;
	int nTask = 0;

	FOR_EACH_TASK_OF_RTP(rtpId, tid);
		if (nTask+1 <= MAX_RTP_TASKS)
			rtpTaskIdList[nTask++] = (int)tid;
	END_FOR_EACH_TASK_OF_RTP;

	return nTask;
}

LOCAL void RtpStatusStrGet
    (
    RTP_ID      rtpId,           /* RTP ID */
    char *      rtpStatusStr     /* string to return status */
    )
    {
    char *      statusStr = NULL;

    /* get the type of RTP */

    switch (RTP_STATE_GET (rtpId->status))
        {
        case RTP_STATE_CREATE:
            {
	    if (IS_RTP_STATUS_STOP (rtpId->status))
		{
                statusStr = "STATE_CREATE+S";
		}
	    else if (IS_RTP_STATUS_ELECTED_DELETER (rtpId->status))
		{
                statusStr = "STATE_CREATE+D";
		}
	    else
		{
                statusStr = "STATE_CREATE";
		}
            break;
            }
        case RTP_STATE_NORMAL:
            {
	    if (IS_RTP_STATUS_STOP (rtpId->status))
		{
                statusStr = "STATE_NORMAL+S";
		}
	    else if (IS_RTP_STATUS_ELECTED_DELETER (rtpId->status))
		{
                statusStr = "STATE_NORMAL+D";
		}
	    else
		{
                statusStr = "STATE_NORMAL";
		}
            break;
            }
        case RTP_STATE_DELETE:
            {
            if (IS_RTP_STATUS_STOP (rtpId->status))
                {
                statusStr = "STATE_DELETE+S";
                }
            else if (IS_RTP_STATUS_ELECTED_DELETER (rtpId->status))
                {
                statusStr = "STATE_DELETE+D";
                }
            else
                {
                statusStr = "STATE_DELETE";
                }
            break;
            }
        default:
             {
            statusStr = "STATE_UNKNOWN";
            break;
            }
        }

    sprintf (rtpStatusStr, "%s", statusStr);

    return;
    }



BOOL RtpSummaryShow
    (
    RTP_ID	rtpId,        /* RTP ID or NULL for all RTPs */
    int		lockNeeded,
    OutputStream *out
    )
#if 1
    {
    char buf[500];

    /* if rtpId is 0, print summary info of all RTPs in the system */

    if (rtpId == NULL)
        {
	    sprintf (buf,"%s", "no Rtp");
    	json_write_string(out,buf);
        }
    else
        {
        /* ensure access to RTP structure */

	if (lockNeeded & RTP_LOCK_IT)
	    {
            if (RTP_VERIFIED_LOCK (rtpId) == ERROR)
                {
                errno = S_rtpLib_ACCESS_DENIED;
                return FALSE;
                }
	    }
        sprintf(buf,"%s~%x",rtpId->pPathName, rtpId);
		json_write_string(out,buf);

        /* release lock to RTP structure */

	    if (lockNeeded & RTP_LOCK_IT)
            RTP_UNLOCK (rtpId);
        }

    return TRUE;
    }
#endif


void rtpshlShow(OutputStream *out,int rtpid)
{
	int count = 0;
	int idx = 0;
    RTP_SHL_NODE *pRtpShlNode;
    char shlbuf[400] = {0};
    char rtpbuf[600] = {0};
    static RTP_SHL_NODE *shlIdList[MAX_DSP_SHLS] = {0};
    RTP_ID rtpId=(RTP_ID)rtpid;
    char strstatus[20];
    RtpStatusStrGet(rtpId,strstatus);
    sprintf (rtpbuf,"%s~%x~%s~%x~%x~%d~text~%x~%d~data~%x~%d~bss~%x~%d",\
    		rtpId->pPathName, rtpId, strstatus,rtpId->entrAddr,\
    		rtpId->options,rtpId->taskCnt, rtpId->binaryInfo.textStart,\
    		rtpId->binaryInfo.textSize, rtpId->binaryInfo.dataStart, \
    		rtpId->binaryInfo.dataSize, rtpId->binaryInfo.bssStart,\
    		rtpId->binaryInfo.bssSize);
	json_write_string(out,rtpbuf);
	pRtpShlNode = (RTP_SHL_NODE *) DLL_FIRST(&((RTP_ID)rtpId)->rtpShlNodeList);\
	/* 通过rtpID获取rtp动态库信息列表 */
    while (pRtpShlNode != NULL)/* 将链表形式的动态库信息存入结构体数组 */
    {
    	shlIdList[count++] = pRtpShlNode;
    	pRtpShlNode = (RTP_SHL_NODE *) DLL_NEXT(pRtpShlNode);
	}
	if(count>0)
	{
	  json_write_char(out,',');
	}
    while (idx < count)
    {
    	bzero(shlbuf,sizeof(shlbuf));
		SHL_ID shlId=0;
		RTP_SHL_INFO  pRtpShlInfo;
		shlId=shlNodeToShlId(shlIdList[idx]->pShlNode);/* 获取动态库ID */
		shlRtpInfoGet (shlId,rtpId,&pRtpShlInfo);/* 获取动态库信息 */
		sprintf(shlbuf,"%s~%x~%x~%d~%x~%d",(&pRtpShlInfo)->fileName,shlId,\
				(&pRtpShlInfo)->dataBase,(&pRtpShlInfo)->dataSize,\
				(&pRtpShlInfo)->textBase,(&pRtpShlInfo)->textSize);
		if(idx!=0)//多条动态库信息间用“,”分开
		json_write_char(out,',');
		json_write_string(out,shlbuf);
		idx++;
    }


}

int sem_rtp_Show(SEM_ID semId,OutputStream *out,int *rtpTaskIdList)
{
	LOCAL char         *semTypeMsg[MAX_SEM_TYPE] = {
	    "BINARY", "MUTEX", "COUNTING", "OLD", "\0", "\0", "READ/WRITE",
	    "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0"
	};
	char sembuf[500];
	bzero(sembuf,sizeof(sembuf));
    char buf[100];
	char               *qMsg;
	char               *name;
	char IdName[100];
	int ix=0;
	int ixx=0;
    int numTasks=0;
    int *rtpTid=rtpTaskIdList;
	int taskIdList[MAX_RTP_TASKS];
	numTasks=semInfo(semId, taskIdList,20);/* 获取阻塞队列任务的ID */
	if (numTasks>0)
	{

		for (ix = 0; ix < numTasks; ix++)/* 信号量的每个ID */
		{
		 rtpTid=rtpTaskIdList;
		 while(0!=*rtpTid)           /* rtp的每个ID */
			{
			 if(*rtpTid==taskIdList[ix])
			 {
				SEM_ID SemId=(SEM_ID)semId;
				qMsg =((SemId->options & SEM_Q_MASK) == SEM_Q_FIFO) ? "FIFO" : "PRIORITY";
				sprintf(buf,"0x%x~", (int)SemId);/*信号量ID*/
				strcat(sembuf,buf);
				name = objNamePtrGet(SemId);
				sprintf(buf,"%s~",name == NULL ? "N/A" : name);/*信号量名称*/
				strcat(sembuf,buf);

				sprintf(buf,"%s~", semTypeMsg[SemId->semType]);/*信号量类型*/
				strcat(sembuf,buf);

				sprintf(buf,"%s~", qMsg);/*信号量属性*/
				strcat(sembuf,buf);

			   /*仅当为计数信号量时回复计数值，否则回复“0”*/
				switch (SemId->semType)
				{
				case SEM_TYPE_BINARY:
					strcat(sembuf,"0");
					break;

				case SEM_TYPE_COUNTING:

					sprintf(buf,"%u",SemId->semCount);
					strcat(sembuf,buf);
					break;

				case SEM_TYPE_MUTEX:
					strcat(sembuf,"0");
					break;

				case SEM_TYPE_RW:
					strcat(sembuf,"0");
					break;

				default:
					strcat(sembuf,"0");
					break;
				}
				if (numTasks > 0)	               /* 阻塞队列任务的ID */
				{
					memset(IdName,0,100);
					for (ixx = 0; ixx < min(numTasks, NELEMENTS(taskIdList)); ixx++)
					{
						if(ixx==0)
						{
							sprintf(buf,"~0x%x",taskIdList[ixx]);
							strcat(sembuf,buf);
							sprintf(buf,"~%s",(taskName(taskIdList[ixx])));//获取任务名
							strcat(IdName,buf);
						}
						else
						{
							sprintf(buf,",0x%x",taskIdList[ixx]);
							strcat(sembuf,buf);
							sprintf(buf,",%s",(taskName(taskIdList[ixx])));
							strcat(IdName,buf);
						}

					}
					strcat(sembuf,IdName);
				}
				else
				{
					strcat(sembuf,"~NULL~NULL");
				}
				if(flag1>0)/* 满足条件的信号量数大于1个时，用","分开 */
				{
					write_stream(out,',');
				}
				json_write_string(out,sembuf);
				flag1++;
			  return 2;
			}
			 rtpTid++;
		  }
		}

	}
	else
	{
		return 1;
	}
	return 1;

}

void rtpsemShow(OutputStream *out,int rtpId)
{
	DELTA_ID              objId = NULL;
	DL_NODE            *pNode;
	CLASS_ID            classId;
	DL_LIST             list;
	int	rtpTaskIdList[MAX_RTP_TASKS];
	memset(rtpTaskIdList,0,MAX_RTP_TASKS*(sizeof(int)));
	GetRtpTaskIdList(rtpId, rtpTaskIdList);/* 获取rtp下的所有任务ID */
	if(rtpTaskIdList[0]!=0)/* rtp下有任务时才进行信号量的获取 */
	{
		enum coreObjClassType classType=windSemClass;
		if ((classType >= coreNumObjClass) ||\
				((classId = objClassIdGet(classType)) == NULL))
		{
			errnoSet(S_objLib_OBJ_ILLEGAL_CLASS_TYPE);
		}

		_func_classListLock(classId);

		if ( FALSE)//objPublic
			list = classId->objPubList;
		else
			list = classId->objPrivList;

		DL_NODE * pNextNode;
		pNode = DLL_FIRST (&list);
		while (pNode != NULL)
		{
		objId = (DELTA_ID)((int)pNode - OFFSET (OBJ_CORE, classNode));
		pNextNode = DLL_NEXT (pNode);
		sem_rtp_Show(objId,out,rtpTaskIdList);

		pNode = pNextNode;
		}
		flag1=0;
		_func_classListUnlock(classId);
	}

}

int msgq_rtp_Show(MSG_Q_ID msgqId,OutputStream *out,int *rtpTaskIdList)
{
   char msgbuf[300];
   char buf[50];
   char *pName;
   MSG_Q_INFO pInfo;
   int                 taskIdList[20];/* 储存消息队列中处于等待状态的消息对应的任务ID的数组 */
   memset(taskIdList,0,20);
   char               *msgPtrList[20];
   int                 msgLenList[20];
   pInfo.taskIdList = taskIdList;
   pInfo.taskIdListMax = 20;
   pInfo.msgPtrList = msgPtrList;
   pInfo.msgLenList = msgLenList;
   pInfo.msgListMax = NELEMENTS(msgPtrList);
   msgQInfoGet(msgqId,&pInfo);
   int *msgTid=taskIdList;
   while((0!=*msgTid))              /* msgq的每个ID */
   {
	   int *rtpTid=rtpTaskIdList;
	   while(0!=*rtpTid)           /* rtp的每个ID */
		{
		 if(*rtpTid==*msgTid)
		 {
			   bzero(buf,sizeof(buf));
				bzero(msgbuf,sizeof(msgbuf));
				sprintf(buf,"0x%x~",(int)msgqId);/*消息队列ID*/
				strcat(msgbuf,buf);

				pName = objNamePtrGet(((MSG_Q_ID)msgqId));/*消息队列名称*/
				sprintf(buf,"%s~",pName == NULL ? "N/A" : pName);
				strcat(msgbuf,buf);

			if ((pInfo.options & MSG_Q_TYPE_MASK) == MSG_Q_FIFO)/*消息队列属性*/
				{
				sprintf(buf,"%s~", "FIFO");
				strcat(msgbuf,buf);

				}
			else
				{
				sprintf(buf,"%s~", "PRIORITY");
				strcat(msgbuf,buf);

				}

				sprintf(buf,"%d~", pInfo.numMsgs);/*消息队列中当前消息数*/
				strcat(msgbuf,buf);


				sprintf(buf,"%d~", pInfo.maxMsgs);/*消息队列最大消息数*/
				strcat(msgbuf,buf);


				sprintf(buf,"%d", pInfo.maxMsgLength);/*单个消息最大长度*/
				strcat(msgbuf,buf);
				json_write_string(out,msgbuf);
				return 2;
		 }
		 rtpTid++;
		}
	   msgTid++;
   }
	return 1;

}

void rtpmsgqShow(OutputStream *out,int rtpId)
{
	int flag=0;
	int flag1=0;
	DELTA_ID              objId = NULL;
	DL_NODE            *pNode;
	CLASS_ID            classId;
	DL_LIST             list;
	int	rtpTaskIdList[MAX_RTP_TASKS];
	memset(rtpTaskIdList,0,MAX_RTP_TASKS*(sizeof(int)));
	GetRtpTaskIdList(rtpId, rtpTaskIdList);/* 获取rtp下的所有任务ID */
	if(rtpTaskIdList[0]!=0)/* rtp下有任务时才进行消息队列的获取 */
	{
		enum coreObjClassType classType=windMsgQClass;
		if ((classType >= coreNumObjClass) ||\
				((classId = objClassIdGet(classType)) == NULL))
		{
			errnoSet(S_objLib_OBJ_ILLEGAL_CLASS_TYPE);
		}

		_func_classListLock(classId);

		if ( FALSE)//objPublic
			list = classId->objPubList;
		else
			list = classId->objPrivList;

		DL_NODE * pNextNode;
		pNode = DLL_FIRST (&list);
		while (pNode != NULL)/* 系统所有消息队列 */
		{

		objId = (DELTA_ID)((int)pNode - OFFSET (OBJ_CORE, classNode));
		pNextNode = DLL_NEXT (pNode);

		flag=(msgq_rtp_Show(objId,out,rtpTaskIdList));
		if(flag==2)/* rtp下的任务有处于等待状态的消息 */
		{
			if(flag1!=0)/* 满足条件的消息队列数大于1个时，用","分开 */
			{
				write_stream(out,',');
			}
			flag1++;
		}
		pNode = pNextNode;
		}
		_func_classListUnlock(classId);
	}


}

STATUS RtpFdShow(OutputStream *out,RTP_ID rtpId)
{
	char buf[300];
    int fd;
    int size;
    char * name;
    char   fullFileName [MAX_FILENAME_LENGTH];

    if (rtpId == 0)
	{
	rtpId = taskIdCurrent->rtpId;
	}

    size = rtpId->fdTableSize;

    /* Add dump of defaultPath too! */

    for (fd = 0; fd < size; fd++)//循环获取
	{
	FD_ENTRY * pFdEntry = iosRtpFdTblEntryGet (rtpId, fd);//获取文件描述符

	if (pFdEntry != NULL)
	    {
	    name = objNamePtrGet (&pFdEntry->objCore);
	    if (name == NULL)
		strcpy (fullFileName, "(unknown)");
	    else if (pFdEntry->pDevHdr->pDrvEntry == pIosSuspendedDevDrv)
		{
		strcpy (fullFileName, "(dev suspended)");
		strcat (fullFileName, name);
		}
	    else if (pFdEntry->pDevHdr->pDrvEntry == &fdInvalidDrv ||
		     pFdEntry->pDrvEntry == &fdInvalidDrv)
		{
		strcpy (fullFileName, "(dev deleted)");
		strcat (fullFileName, name);
		}
	    else
		{
		strcpy (fullFileName, name);
		}

	    sprintf (buf,"%d~0x%x~%s", fd, (int) pFdEntry,fullFileName);
	    if(fd!=0)
	    {
	    	write_stream(out,',');
	    }
	    json_write_string(out,buf);
	    }
	}

    errno = 0; /* clear any EBADF errors */

    return (OK);
}



/********************系统调用hook函数，每当进行系统调用时，对系统调用进行统计********************/
LOCAL STATUS SysCallMonitorEntry
    (
    const SYSCALL_ENTRY_STATE * pState
    )
    {
    int             groupNum;
    int             routineNum;
    SYSCALL_RTN_TBL_ENTRY * pRtnTbl;
    int             i;
    RTP_ID          rtpId;

    rtpId = syscallRtpId;
	groupNum = (pState->scn & SYSCALL_GROUP_FIELD_MASK) >>
			   SYSCALL_ROUTINE_NUM_BITS;

	routineNum = pState->scn & SYSCALL_ROUTINE_FIELD_MASK;

	/*
	 * Validation. Check for
	 *   o null routine table pointer
	 *   o method number out of range
	 *   o null method pointer
	 */

	if (((pRtnTbl = syscallGroupTbl[groupNum].pRoutineTbl) == NULL) ||
		(routineNum >= syscallGroupTbl[groupNum].numRoutines) ||
		(pRtnTbl[routineNum].pMethod == NULL))
		{
		printf("invalid system call %X\n", pState->scn);
		return (OK);
		}
	int flag=0;
	for(i=1;i<=syscallinfo[0].callnum;i++)        /* 若已统计，计数+1 */
	{
		if(strcmp(pRtnTbl[routineNum].methodName,syscallinfo[i].syscallname)==0)
		{
			syscallinfo[i].callnum++;
			flag=1;
		}
	}
	if(flag!=1)                      /* 若未统计，追加至数组已统计数据之后 */
	{
		syscallinfo[0].callnum++;
		strcpy(syscallinfo[syscallinfo[0].callnum].syscallname,\
				pRtnTbl[routineNum].methodName);
	}

    return (OK);
    }


/************** 监控（关闭监控）系统调用 *************************/
void SysCallMonitor
   (
   int level,
   RTP_ID rtpId
   )
   {
   STATUS stat = OK;

   if (level == SYSCALL_MONITOR)
       {
	if (!hookFind (SysCallMonitorEntry,(void*)pSyscallEntryHookTbl,
		       syscallHookTblSize))	/* prevent double-monitoring */
	    {


	    stat |= syscallEntryHookAdd (SysCallMonitorEntry, TRUE);

	    if (stat != OK)	         /* undo if an error was encountered */
		{
		syscallEntryHookDelete (SysCallMonitorEntry);
		puts ("Error! syscallMonitor hook installation failed!\n"
		      "Ensure SYSCALL_HOOK_TBL_SIZE is nonzero & large enough\n");
		}
	    }

	syscallRtpId = rtpId;
       }

   if (level == SYSCALL_NO_MONITOR)
       {
	    syscallEntryHookDelete (SysCallMonitorEntry);
       }
  }


LOCAL STATUS RSysCallMonitorEntry
    (
    const SYSCALL_ENTRY_STATE * pState
    )
    {
    int             groupNum;
    int             routineNum;
    SYSCALL_RTN_TBL_ENTRY * pRtnTbl;
    int             i;
    RTP_ID          rtpId;

    rtpId = syscallRtpId;
	groupNum = (pState->scn & SYSCALL_GROUP_FIELD_MASK) >>
			   SYSCALL_ROUTINE_NUM_BITS;

	routineNum = pState->scn & SYSCALL_ROUTINE_FIELD_MASK;

	/*
	 * Validation. Check for
	 *   o null routine table pointer
	 *   o method number out of range
	 *   o null method pointer
	 */

	if (((pRtnTbl = syscallGroupTbl[groupNum].pRoutineTbl) == NULL) ||
		(routineNum >= syscallGroupTbl[groupNum].numRoutines) ||
		(pRtnTbl[routineNum].pMethod == NULL))
		{
		printf("invalid system call %X\n", pState->scn);
		return (OK);
		}
	int flag=0;
	for(i=1;i<=callinfo[0].callnum;i++)        /* 若已统计，计数+1 */
	{
		if(strcmp(pRtnTbl[routineNum].methodName,callinfo[i].syscallname)==0)
		{
			callinfo[i].callnum++;
			flag=1;
		}
	}
	if(flag!=1)                      /* 若未统计，追加至数组已统计数据之后 */
	{
		callinfo[0].callnum++;
		strcpy(callinfo[callinfo[0].callnum].syscallname,\
				pRtnTbl[routineNum].methodName);
	}

    return (OK);
    }


void RSysCallMonitor
   (
   int level,
   RTP_ID rtpId
   )
   {
   STATUS stat = OK;

   if (level == SYSCALL_MONITOR)
       {
	if (!hookFind (RSysCallMonitorEntry,(void*)pSyscallEntryHookTbl,
		       syscallHookTblSize))	/* prevent double-monitoring */
	    {


	    stat |= syscallEntryHookAdd (RSysCallMonitorEntry, TRUE);

	    if (stat != OK)	         /* undo if an error was encountered */
		{
		syscallEntryHookDelete (RSysCallMonitorEntry);
		puts ("Error! syscallMonitor hook installation failed!\n"
		      "Ensure SYSCALL_HOOK_TBL_SIZE is nonzero & large enough\n");
		}
	    }

	syscallRtpId = rtpId;
       }

   if (level == SYSCALL_NO_MONITOR)
       {
	    syscallEntryHookDelete (RSysCallMonitorEntry);
       }
  }



/************** rtp 内存使用 *************************/
#ifndef MQ_INFO_TIMEOUT
#define MQ_INFO_TIMEOUT		(sysClkRateGet())
#endif
/* definitions */
#ifndef _WRS_CONFIG_RTP
#define _WRS_CONFIG_RTP 1
#endif

#define MEM_EDR_TNAME_LENGTH	16
#define POOL_MAX_TASKS		256


#ifndef MQ_OPEN_OPTIONS
#define MQ_OPEN_OPTIONS		(0)
#endif

/* locals */

/* local variables */

typedef struct mem_edr_tnode
    {
    struct mem_edr_tnode * next;	/* list node link */
    int    		taskId;		/* task ID */
    int			taskUid;	/* task user ID */
    RTP_ID		rtpId;		/* parent RTP ID */
    char		name[MEM_EDR_TNAME_LENGTH];
    } MEM_EDR_TNODE;

extern MEM_EDR_TNODE* memEdrTaskList;

LOCAL char blkMdNames[][8] =
    {
    "inval",			/* invalid */
    "globl",			/* MD_VAR */
    "alloc",			/* MD_ALLOC */
    "qfree",			/* MD_FREE */
    "local"			/* MD_STACK */
    };


LOCAL const char * noInstrErrMsg =
    "Cannot communicate with the RTP. Make sure the user application is built "
    "with memEdrLib support enabled and the process is started with the "
    "MEDR_SHOW_ENABLE environment variable set to TRUE";
LOCAL char * MemEdrTaskNameGet
    (
    int      taskId,		/* ID of task to look up */
    RTP_ID * pRtpId		/* placeholder for RTP ID */
    )
    {
    MEM_EDR_TNODE * pTask=memEdrTaskList;
    static char errName[]= "N/A";
    char * name;
//    printf("11\n");
//    taskDelay(120);
    /* user tasks without user TCB return ERROR for taskIdCurrent() */
//    pTask = (MEM_EDR_TNODE *)MemEdrTaskListget();
//    pTask = (MEM_EDR_TNODE *)0x111111111;
    if (taskId == ERROR)
	return (errName);

    /* if task still exists, taskName will work */
//    printf("22\n");
//    taskDelay(120);
    if ((name = taskName (taskId)) != NULL)
	{
	*pRtpId = ((WIND_TCB *) taskId)->rtpId;
	return (name);
	}

    /* otherwise try registered names */
//    printf("33\n");
//    taskDelay(120);
    while (pTask != NULL)
	{
	if (pTask->taskId == taskId)
	    {
	    *pRtpId = pTask->rtpId;
	    return (pTask->name);
	    }
	pTask = pTask->next;
	}

    /* could not get a task name */

    return (errName);
    }


LOCAL char * MemEdrSymNameGet
    (
    void * pInstr,			/* symbol address to look up */
    char * pBuf,			/* buffer to store symbol name */
    int    maxLen,			/* length of buffer */
    RTP_ID rtpId			/* which RTP's symbol table to search */
    )
    {
    SYMBOL_ID	symId;
    void *	val;
    char *	name;
    SYMTAB_ID	symTabId;

    /* get symbol table */

    if (rtpId == kernelId)
	symTabId = sysSymTbl;
    else
	{
	CLASS_ID rtpClassId;

	/* validate RTP; this has to be done without pulling in RTP support */

	if (((rtpClassId = objClassIdGet (windRtpClass)) == NULL) ||
	    (OBJ_VERIFY (rtpId, rtpClassId) != OK))
	    {
	    /* no such RTP, try the kernel */

	    rtpId = kernelId;
	    symTabId = sysSymTbl;
	    }
	else
	    symTabId = (SYMTAB_ID) rtpId->symTabId;
	}

    /* get symbol name */

    if ((symTabId != (SYMTAB_ID) 0) &&
	(symFindSymbol (symTabId, NULL, (void *)pInstr,
			SYM_MASK_NONE, SYM_MASK_NONE, &symId) == OK) &&
	(symNameGet (symId, &name) == OK) &&
	(symValueGet (symId, &val) == OK) &&
	(val == (void *)pInstr))
	{
	name = cplusDemangle (name, pBuf, maxLen);

	return (name);
	}
    else
	{
	/* for user tasks try both kernel symbol table (for tInitTask) */

	if (rtpId != kernelId)
	    return (MemEdrSymNameGet (pInstr, pBuf, maxLen, kernelId));
	else
	    return (NULL);
	}
    }

void MemEdrBlockInfoPrint
    (
	char*membuf,
    MEM_EDR_BLK_INFO *	pInfo,		/* block */
    int			level,		/* detail level */
    RTP_ID		rtpId		/* RTP */
    )
    {
    MEM_EDR_BLK * pBlk = &pInfo->blk;
    char buf[200];
    if ((MODE_GET(pBlk) == MD_STACK) || (MODE_GET(pBlk) == MD_VAR))
	{
	/* global or local variable */

	sprintf (buf,"0x%x~%s~%d~N/A~N/A~N/A~N/A||",
		(UINT) pBlk->adrNode.key, blkMdNames[MODE_GET(pBlk)],
		pBlk->blkSize);
	strcat(membuf,buf);
	}
    else
	{
	/* allocated block; print info, if possible use task name */

	int  taskId = ERROR;
	char * name;

#ifdef _WRS_CONFIG_RTP
	if (rtpId == kernelId)
	    {
	    taskId = pBlk->inf.gbl.taskId;
	    }
	else if (_func_objHandleToObjId != NULL)
	    {
	    taskId = (int) _func_objHandleToObjId (pBlk->inf.gbl.taskId, rtpId,
					     FALSE);
	    }
#else
        taskId = pBlk->inf.gbl.taskId;
#endif /* _WRS_CONFIG_RTP */

	name = MemEdrTaskNameGet (taskId, &rtpId);
	sprintf (buf,"0x%x~%s~%d~0x%x~0x%x~%s", (UINT) pBlk->adrNode.key,
		blkMdNames[MODE_GET(pBlk)], pBlk->blkSize,
		pBlk->inf.gbl.partId, taskId, name);
	strcat(membuf,buf);
	/* print extended information */

	if (level > 0)
	    {
	    int i;
	    MEM_EDR_EXTD * pExtd = &pInfo->xtd;
	    char symName [32];

	    /* check if trace info is available */

	    if (pExtd->trace[0] == NULL)
		{
		sprintf (buf," N/A||");
		strcat(membuf,buf);
		return;
		}

	    /* print top function address on same line */

	    if ((name = MemEdrSymNameGet (pExtd->trace[0], symName,
					  sizeof (symName), rtpId)) != NULL)
		{
	    	sprintf (buf,"~%s()", name);
	    	strcat(membuf,buf);
		}
	    else
	    {
			sprintf(buf,"~0x%x()",pExtd->trace[0]);
			strcat(membuf,buf);

	    }
	    /* print rest of trace on new lines */

	    for (i = 1; i < MEM_TRACE_LEN; i++)
	 	{
		/* check if more trace info is available */

		if (pExtd->trace[i] == NULL)
		{
				break;
	 	}
		if ((name = MemEdrSymNameGet (pExtd->trace[i], symName,
					      sizeof (symName), rtpId)) != NULL)
		    {
		   // sprintf (buf,"\t\t\t\t\t\t\t%s()", name);
			 sprintf (buf,"$%s()", name);
		    strcat(membuf,buf);
		    }
		else
		    {
		    //sprintf (buf,"\t\t\t\t\t\t\t0x%08x()", pExtd->trace[i]);
			sprintf (buf,"$0x%x()", pExtd->trace[i]);
		    strcat(membuf,buf);
		    }
		}

	    /* if trace buffer is filled, it may have truncated trace info */

	    if (i == MEM_TRACE_LEN)
		//sprintf (buf,"\t\t\t\t\t\t\t... trace truncated");
	    sprintf (buf,"$... trace truncated");
		strcat(membuf,buf);
	    sprintf(buf,"||");
		strcat(membuf,buf);
	    }
	else
	    {
	    /* do not print trace */

	    sprintf (buf," N/A||");
		strcat(membuf,buf);
	    }
	}

    return;
    }



STATUS MemEdrRtpBlockShow
    (
	int bufcount,
    OutputStream *out,
    RTP_ID  rtpId,			/* RTP id */
    int     partId,			/* partition ID selector */
    void *  addr,			/* address selector */
    int     taskId,			/* task ID selector */
    UINT    type,			/* block type selector */
    UINT    level,			/* detail level */
    BOOL    continuous			/* print in continuous mode */
    )
    {
    MSG_Q_ID		msgQCmd;
    MSG_Q_ID		msgQInf;
    MEM_EDR_BLK_MSG	msg;
    MEM_EDR_SHOW_CMD	cmdMsg;
    char		qName[20];
    int			count;
    int ix=0;
    UINT		pid;
    STATUS		status = OK;
	char membuf[bufcount*260];
	bzero(membuf,260*bufcount);
	strcat(membuf,"@");
    /*
     * validate rtpId; there is no guaranty that the RTP will stay valid
     * for the duration of the call, but we must filter out at least calls
     * that invalid RTP to begin with.
     */

    if (RTP_ID_VERIFY(rtpId) !=  OK)
	{
	//printf ("invalid RTP\n");
	json_write_string(out,"invalid RTP");

	return (ERROR);
	}

    /* look up user task ID */

    if (taskId != 0)
	{
	if ((taskId = memEdrTaskUidGet (taskId)) == -1)
	    {
	  //  printf ("invalid task ID\n");
		json_write_string(out,"invalid task ID");
		return (0);
	    }
	}

    pid = rtpId->rtpHandleId; /* XXX - use Get() */

    sprintf (qName, MEDR_CMD_Q_FMT, pid);
    msgQCmd = msgQOpen (qName, 1, 0, MSG_Q_FIFO, MQ_OPEN_OPTIONS, NULL);

    if (msgQCmd == NULL)
	{
	//printf (noInstrErrMsg);
	json_write_string(out,noInstrErrMsg);
	return (ERROR);
	}

    sprintf (qName, MEDR_INF_Q_FMT, pid);
    msgQInf = msgQOpen (qName, 1, 0, MSG_Q_FIFO, MQ_OPEN_OPTIONS, NULL);

    if (msgQInf == NULL)
	{
	msgQClose (msgQCmd);
//	printf (noInstrErrMsg);
	json_write_string(out,noInstrErrMsg);
	return (ERROR);
	}

   // memEdrBlockHdrPrint();

    /* create command */

    cmdMsg.cmd = MEDR_BLOCK_INFO_CMD;
    cmdMsg.filt.level  = level;
    cmdMsg.filt.partId = partId;
    cmdMsg.filt.addr   = addr;
    cmdMsg.filt.taskId = taskId;
    cmdMsg.filt.type   = type;
    cmdMsg.filt.first  = 0;
    cmdMsg.filt.last   = MEM_EDR_SHOW_PRINT_CNT - 1;
    cmdMsg.filt.count  = 0;

    while (TRUE)
	{
	/* send command */

	msgQSend (msgQCmd, (char *) &cmdMsg, sizeof (cmdMsg),
		  NO_WAIT, MSG_PRI_NORMAL);

	/* receive info */

	if (msgQReceive (msgQInf, (char *) &msg, sizeof (msg),
			 MQ_INFO_TIMEOUT) == ERROR)
	    {
	    status = ERROR;
	    break;
	    }

	if (msg.status == ERROR)
	    {
	    status = ERROR;
	    break;
	    }

	/* print info */

	count = msg.count;

	for (ix = 0; ix < count - cmdMsg.filt.first; ix++)
	    MemEdrBlockInfoPrint (membuf,&msg.blkInfo[ix], level, rtpId);

	/* are there any more? */

	if (count < cmdMsg.filt.first + MEM_EDR_SHOW_PRINT_CNT)
	    break;

	/* ask user to continue or not */

	if (!continuous)
	    if (memEdrUserStopGet (cmdMsg.filt.first == 0))
		break;

	/* continue printing from the last */

	cmdMsg.filt.first = count;
	cmdMsg.filt.last  = count + MEM_EDR_SHOW_PRINT_CNT - 1;
	cmdMsg.filt.count = 0;
	}

    msgQClose (msgQCmd);
    msgQClose (msgQInf);

    if (status == ERROR)
	//printf ("error obtaining block info\n");
	json_write_string(out,"error obtaining block info");
    else
	json_write_string(out,membuf);
    return (status);

    }

int rtpEdrMemShow(OutputStream *out,RTP_ID rtpId)
{
    MSG_Q_ID		msgQCmd;
    MSG_Q_ID		msgQInf;
    MEM_EDR_PART_MSG	msg;
    MEM_EDR_SHOW_CMD	cmdMsg;
    MEM_EDR_PART_INFO *	pInfo = &msg.partInfo;
    char		qName[16];
    UINT		pid;
    STATUS		status = OK;
    int bufcount=0;
    /*
     * validate rtpId; there is no guaranty that the RTP will stay valid
     * for the duration of the call, but we must filter out at least calls
     * that invalid RTP to begin with.
     */

    if (RTP_ID_VERIFY (rtpId) !=  OK)
	{
	//printf ("invalid RTP\n");
    	json_write_string(out,"invalid RTP");
	return (ERROR);
	}

    pid = rtpId->rtpHandleId; /* XXX - use Get() */


    sprintf (qName, MEDR_CMD_Q_FMT, pid);


    msgQCmd = msgQOpen (qName, 1, 0, MSG_Q_FIFO, MQ_OPEN_OPTIONS, NULL);

    if (msgQCmd == NULL)
	{
//	printf (noInstrErrMsg);
	json_write_string(out,noInstrErrMsg);
	return (ERROR);
	}

    sprintf (qName, MEDR_INF_Q_FMT, pid);


    msgQInf = msgQOpen (qName, 1, 0, MSG_Q_FIFO, MQ_OPEN_OPTIONS, NULL);

    if (msgQInf == NULL)
	{
	msgQClose (msgQCmd);
//	printf (noInstrErrMsg);
	json_write_string(out,noInstrErrMsg);
	return (ERROR);
	}

    /* create command message */

    cmdMsg.cmd    = MEDR_PART_INFO_CMD;
    cmdMsg.pPart  = NULL;
    cmdMsg.partId = 0;

    /* print header */

  //  memEdrPartHdrPrint();

    do
	{
	/* send command */

	msgQSend (msgQCmd, (char *) &cmdMsg, sizeof (cmdMsg),
		  NO_WAIT, MSG_PRI_NORMAL);

	/* receive info */

	if (msgQReceive (msgQInf, (char *) &msg, sizeof (msg),
			 MQ_INFO_TIMEOUT) == ERROR)
	    {
	    status = ERROR;
	    break;
	    }

	if (msg.status == ERROR)
	    {
	    status = ERROR;
	    break;
	    }

	///* print info */
	bufcount+=pInfo->allocBlocks+pInfo->fqBlocks;
	//memEdrPartInfoPrint (pInfo);

	cmdMsg.pPart = pInfo->next;

	} while (cmdMsg.pPart != NULL);

#ifdef OM_DELETE_ON_LAST_CLOSE
    msgQClose (msgQCmd);
    msgQClose (msgQInf);
#endif

	MemEdrRtpBlockShow(bufcount,out,rtpId,0,0,0,0,1,1);
	return 0;
}


STATUS MemEdrBlockShow
    (
    OutputStream *out,
    int	bufcount,
    int     partId,			/* partition ID selector */
    void *  addr,			/* address selector */
    int     taskId,			/* task ID selector */
    UINT    type,			/* block type selector */
    UINT    level,			/* detail level */
    BOOL    continuous			/* print in continuous mode */
    )
    {
    MEM_EDR_FILTER   filter;
    MEM_EDR_BLK_INFO blkInfo[MEM_EDR_SHOW_PRINT_CNT];

    /* search using the other criateria */

    filter.level  = level;
    filter.partId = partId;
    filter.addr   = addr;
    filter.taskId = taskId;
    filter.type   = type;
    filter.first  = 0;
    filter.last   = MEM_EDR_SHOW_PRINT_CNT - 1;
    filter.count  = 0;
	printf("count :%d\n",bufcount);
    char bufmem[bufcount*260];
    bzero(bufmem,260*bufcount);
	while (TRUE)
	{
	int  count;
	int  ix;

	/* get block info for a set */


	if (memEdrBlockInfoGet (&filter, blkInfo) == ERROR)
	    {
	    printf ("\n");
	    return (ERROR);
	    }

	count = filter.count;
	/* print info */

	for (ix = 0; ix < count - filter.first; ix++)
		MemEdrBlockInfoPrint (bufmem,&blkInfo[ix], level, kernelId);

	/* are there any more? */

	if (count < filter.first + MEM_EDR_SHOW_PRINT_CNT)
	    break;

	/* ask user to continue or not */

	if (!continuous)
	    if (memEdrUserStopGet (filter.first == 0))
		break;

	/* continue printing from the last */

	filter.first = count;
	filter.last  = count + MEM_EDR_SHOW_PRINT_CNT - 1;
	filter.count = 0;
	}
	json_write_string(out,bufmem);
    return (OK);
    }

STATUS MemEdrPartShow
    (
    OutputStream *out,
    PART_ID		partId			/* partition ID */
    )
    {
    MEM_EDR_PART_INFO	partInfo;
    int bufcount=0;

    /* NULL means start with first on list */

    partInfo.next = NULL;

    /* iterate until no more partitions to list */

    do
	{
	if (memEdrPartInfoGet (partInfo.next, partId, &partInfo) == ERROR)
	    {
	    printf ("error getting partition info\n");
	    break;
	    }

	/* display info */

	bufcount+=(partInfo.allocBlocks+partInfo.fqBlocks);

	} while (partInfo.next != NULL);
    MemEdrBlockShow(out,bufcount,0,0,0,0,1,1);
    return (OK);
    }
