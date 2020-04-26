#include <string.h>
#include "cpushow.h"
#include <stdio.h>
#include <framework/protocol.h>
#include <framework/json.h>
#include <framework/errors.h>
#include <private/cplusLibP.h>
#include <private/dbgLibP.h>
#include <private/vxdbgLibP.h>
#include <private/vxdbgBpMsgQLibP.h>
#include <trcLib.h>
#include <shellConfigLib.h>
#include <shellDataLib.h>
#include <shellLib.h>
#include <private/shellInOutLibP.h>
#include <private/shellInternalLibP.h>
#include <private/shellLibP.h>
#include <private/shellMemLibP.h>
#include <vxWorks.h>
#include <cplusLib.h>
#include <qFifoGLib.h>
#include <qLib.h>
#include <symLib.h>
#include <sysSymTbl.h>
#include <vxLib.h>
#include <private/msgQLibP.h>
#include <private/msgQSmLibP.h>
#include <private/smObjLibP.h>
#include <private/taskLibP.h>
#include <private/usrLibP.h>
#include <private/taskWaitShowP.h>
/*******************eg for lock_forever**********************/
/* 任务        任务拥有的信号量                                    信号量       等待信号量的任务
 * ttask        tsem                   ssem        stask
 *  t_a          s_b                    s_d        t_a
 *  t_c          s_d                    s_b        t_c
 *   .            .                      .          .
 *   .            .                      .          .
 *   .            .                      .          .
 *   .            .                      .          .
 *   .            .                      .          .
 *   .            .                      .          .
 *
 *
 *             t_a → → s_b
 *             ↑         ↓
 *             ↑         ↓
 *             s_d ← ← t_c
 *
 *
 * */
IMPORT Q_HEAD	vxEventPendQ;

#define _WRS_FRAME_PRINT_STRING	"                   : "

static void DbgCallPrint(INSTR *callAdrs,int funcAdrs,int nargs,int *args,int taskId,BOOL isKernelAdrs);
static STATUS DbgStackTrace(int taskId);
#define	rtpFromTaskGet(taskId)	(taskTcb(taskId)->rtpId)
#define DBG_DEMANGLE_PRINT_LEN 256
IMPORT int    	trcDefaultArgs;
#define	TASK_WAIT_SM_OBJ_VERIFY	SM_OBJ_VERIFY

#define	TASK_WAIT_OBJ_VERIFY	OBJ_VERIFY

typedef struct
{
int ttask;//任务
int tsem;//任务拥有的信号量

}T_SEM;

typedef struct
{
int ssem;//信号量
int stask[20];//等待信号量的任务

}S_TASK;

typedef struct node
{
	int taskid;
	int semid;
	struct node *last;
	struct node *next;

}Node;

typedef struct
{
	int taskId;
	int semId;

}LOCKINFO;

Node *curr=NULL;
Node *head=NULL;
Node *lloc=NULL;
void check_task(int taskId,T_SEM *t_sem,S_TASK *s_task,int semid);
void check_sem(int taskId,T_SEM *t_sem,S_TASK *s_task,int taskid);
void lock_taskshow(LOCKINFO *idlist,OutputStream *out);
void get_sem_task(SEM_ID semId,T_SEM *t_sem,S_TASK *s_task);
T_SEM  t_sem[TASKID_MAX];/* “ 任务拥有信号量 ” 结构体数组*/
S_TASK s_task[TASKID_MAX];/* “ 等待信号量的任务 ” 结构体数组 */
int flag = 0;
TASK_DESC td;
char buf[3000];
char buftt[1500];
LOCKINFO final[30][35];//存放多个死锁
LOCKINFO idlist[35];//存放单个死锁


void get_lock(OutputStream *out)
{
	memset(t_sem,0,TASKID_MAX*sizeof(T_SEM));
	memset(s_task,0,TASKID_MAX*sizeof(S_TASK));
	memset(final,0,300*sizeof(int));

	LOCAL   int id[TASKID_MAX]={0};
	int num;
	int x=0;
	int lnum,onum,nnum,lnumm;

	taskIdListGet(id,TASKID_MAX);/* 获取所有任务ID（ID列表） */
	for(num=0;num<TASKID_MAX;num++)
	{
		if(id[num]!=0)
		{
			t_sem[num].ttask=id[num];/* 将所有任务ID存至 “ 任务拥有信号量 ” 结构体数组中 */
		}

	}


	DELTA_ID              objId = NULL;
	DL_NODE            *pNode;
	CLASS_ID            classId;
	DL_LIST             list;
	enum coreObjClassType classType=windSemClass;
	if ((classType >= coreNumObjClass) ||\
			((classId = objClassIdGet(classType)) == NULL))
	{
//		errnoSet(S_objLib_OBJ_ILLEGAL_CLASS_TYPE);
		printf("error\n");
	}

	_func_classListLock(classId);

	if ( FALSE)//objPublic
		list = classId->objPubList;
	else
		list = classId->objPrivList;

	DL_NODE * pNextNode;
	pNode = DLL_FIRST (&list);

	while (pNode != NULL)/* 所有信号量 */
	{

	objId = (DELTA_ID)((int)pNode - OFFSET (OBJ_CORE, classNode));
	pNextNode = DLL_NEXT (pNode);

	(* get_sem_task) (objId,t_sem,s_task); /* 通过semID获取并储存当前信号量的信息 */
	pNode = pNextNode;
	}
	_func_classListUnlock(classId);

	T_SEM *p=t_sem;
	while(((p->ttask)!=0))/* 对所有拥有信号量的任务进行检测 */
	{

		head=(Node *)malloc(sizeof(Node));/* 创建储存造成死锁的任务及其拥有的信号量的链表头 */
		head->last=NULL;
		head->next=NULL;
		head->taskid=0;/*表头任务ID为 0*/
		head->semid=p->tsem;/* 表头信号量ID为初始任务拥有的信号量 */
		curr=head;

		if((p->tsem)!=0)
		{
		check_task(p->ttask,t_sem,s_task,p->tsem);/* 当前任务若拥有信号量，则检测该信号量是否被其他任务等待 */
		}
		if(head->next!=NULL)
		{
			flag=1;
			memset(idlist,0,sizeof(idlist));
			int i=0;
			curr=head;
			for(i=0;i<35;i++)
			{
				if(curr->next->taskid!=0)//
				{
					idlist[i].taskId=curr->next->taskid;
					idlist[i].semId=curr->next->semid;
					curr=curr->next;
				}
				else
				{
					break;
				}
			}
			for(lnum=0;lnum<i;lnum++)
			{
				for(lnumm=(lnum+1);lnumm<i;lnumm++)
				{
					if(idlist[lnum].taskId>idlist[lnumm].taskId)/* 对形成死锁的任务排序 ，便于检查当前死锁是否已经存放*/
					{
						/* 交换信号量 */
						idlist[lnumm].semId=idlist[lnumm].semId+idlist[lnum].semId;
						idlist[lnum].semId=idlist[lnumm].semId-idlist[lnum].semId;
						idlist[lnumm].semId=idlist[lnumm].semId-idlist[lnum].semId;
						/* 交换任务ID */
						idlist[lnumm].taskId=idlist[lnumm].taskId+idlist[lnum].taskId;
						idlist[lnum].taskId=idlist[lnumm].taskId-idlist[lnum].taskId;
						idlist[lnumm].taskId=idlist[lnumm].taskId-idlist[lnum].taskId;
					}
				}
			}

			for(num=0;num<30;num++)
			{
				if(final[num][0].taskId!=0)
				{
					flag=0;
					for(onum=0;onum<i;onum++)
					{
						if(idlist[onum].taskId!=final[num][onum].taskId)//当前死锁的任务与已有死锁任务是否相同
						{
						flag=1;//当前死锁的任务与已有死锁任务只要有一个不相同，则需将当前死锁保存（flag置1）
						break;
						}
					}
				}
			}
			if(flag==1)
			{
				for(nnum=0;nnum<35;nnum++)
				{
					if(idlist[nnum].taskId!=0)
					{
						final[x][nnum].taskId=idlist[nnum].taskId;
						final[x][nnum].semId=idlist[nnum].semId;
					}
				}
				x++;
			}
		}
		p++;
	}
	if(final[0][0].taskId==0)
	{
		printf("no lock_forever!\n");
	}
	else
	{

		for(num=0;num<30;num++)
		{
			if(final[num][0].taskId!=0)
			{
				if(num!=0)
				write_stream(out,',');

				lock_taskshow(final[num],out);
			}
		}

	}
}

void check_sem(int taskId,T_SEM *t_sem,S_TASK *s_task,int taskid)
{
	int flag2=0;
	T_SEM *p=t_sem;
	while(p->ttask)
	{
		if(p->ttask==taskid)
		{
			flag2=1;
			if(p->tsem)
			{
			curr->semid=p->tsem;
			check_task(taskId,t_sem,s_task,p->tsem);
			}
		}
		if(flag2==1)
		{
			break;
		}
		p++;
	}

}

void check_task(int taskId,T_SEM *t_sem,S_TASK *s_task,int semid)
{
	int flag=0;
	S_TASK *q=s_task;
	while(q->ssem)/* 遍历 “ 等待信号量的任务 ” 结构体数组*/
	{
		if(q->ssem==semid)/* 有任务等待该信号量 */
		{

		    flag=1;/* 遍历 “ 等待信号量的任务 ” 结构体数组 结束标志 */
			int *i=q->stask;
			while(*i)/* 遍历等待信号量的任务 */
			{

				lloc=(Node *)malloc(sizeof(Node));/*为该任务创建结构体*/
				lloc->last=curr;/* 加入储存造成死锁的任务及其拥有的信号量的链表 */
				lloc->next=NULL;
				lloc->taskid=*i;
				curr->next=lloc;
				curr=lloc;
				if(*i==taskId)/*该任务ID若等于初始任务ID，则形成死锁*/
				{
					curr->next=head;
					curr->semid=head->semid;

				}
				else  /* 若不等，则检测其拥有的信号量 */
				{
				check_sem(taskId,t_sem,s_task,*i);
				}
				if(curr->next==NULL)/* 若该任务不参与死锁，则从链表中删除其对应的结构体 */
				{
					lloc=curr;
					curr=curr->last;
					curr->next=NULL;
					free(lloc);
				}
				else/* 若该任务参与死锁，则退出 遍历等待信号量的任务 */
				{
					break;
				}
				i++;
			}

		}
		if(flag==1)/*信号量唯一，有信号量与被测信号量相等，结束 遍历 “ 等待信号量的任务 ” 结构体数组*/
		{
			break;
		}
		q++;
	}

}

void get_sem_task(SEM_ID semId,T_SEM *t_sem,S_TASK *s_task)
{
	if(semId->semType==SEM_TYPE_MUTEX)
	{

	int                 taskIdList[20]={0};
	int numTasks=0;
	int i,j;

	numTasks = semInfo(semId, &taskIdList[0], 20);

	for(i=0;i<TASKID_MAX;i++)
	{
		if(s_task[i].ssem==0)
		{
			s_task[i].ssem=(int)semId;
			if(numTasks>0)
			{
				for(j=0;j<numTasks;j++)
				{
					s_task[i].stask[j]=taskIdList[j];          //等待信号量的任务
				}
			}
			break;
		}
	}

	if(((int)(semId->state.owner))!=0)//持有该信号量的任务
	{
		for(i=0;i<TASKID_MAX;i++)
		{

			if(t_sem[i].ttask==(((int)(semId->state.owner))-1))
			{
				if(t_sem[i].tsem==0)
				{
				t_sem[i].tsem=(int)semId;
				break;
				}
				else      //当前结构体的任务已有相应的信号量，则在末尾添加当前任务及当前检测的信号量
				{
					T_SEM *y=t_sem;

					while(1)
					{

						if(y->ttask!=0)
						{

							y++;
						}
						else
						{
						y->ttask=(((int)(semId->state.owner))-1);
						y->tsem=(int)semId;
						break;

						}
					}
				}
			}
		}
	}




	}
}

void lock_taskshow(LOCKINFO *idlist,OutputStream *out)
{
	char bufn[50];
	bzero(buf,sizeof(buf));
	LOCKINFO *id=idlist;
	int flag=0;
	while(id->taskId)
	{
		taskInfoGet(id->taskId,&td);
		if(flag>0)//多个信息标志
		{
			strcat(buf,"#");
		}
		sprintf(bufn,"%x~%x~%x",id->taskId,id->semId,td.td_rtpId);
		strcat(buf,bufn);
		DbgStackTrace(id->taskId);

		id++;
		flag++;
	}
	json_write_string(out,buf);

}

STATUS DbgStackTrace(int taskId)
{
    REG_SET regSet;
    BOOL    continueIt = FALSE;	/* flag to remember if resuming is necessary */
    RTP_ID  oldCtx = NULL;

    /* Make sure the task don't try to trace itself */

    if (taskId == 0 || taskId == taskIdSelf ())
	{
	return ERROR;
	}

    /* Make sure the task exists */

    if (taskIdVerify (taskId) != OK)
	{
	return ERROR;
	}

    /*
     * If the task is not already stopped or suspended, stop it while
     * we trace it
     */

    if ((!taskIsStopped (taskId)) && (!taskIsSuspended (taskId)))
	{
	if (taskStop (taskId) != OK)
	    {
	    return ERROR;
	    }

	continueIt = TRUE;              /* we want to resume it later */
	}

    /* Trace the stack */

    taskRegsGet (taskId, &regSet);

    /*
     * switch to the memory context of the task to be traced
     * since we will probe the RTP memory. If the task's
     * memory context may not be accessed, either because
     * the RTP is being deleted or the context is invalid,
     * the stack trace will fail with an ERROR.
     */

    if (_func_taskMemCtxSwitch != NULL)
        {
        if ((oldCtx = (RTP_ID) _func_taskMemCtxSwitch (
					rtpFromTaskGet (taskId), 0)) == NULL)
	    {
	    return ERROR;
	    }
	}

    trcStack (&regSet, (FUNCPTR) DbgCallPrint, taskId);

    /* restore the memory context */

    if (_func_taskMemCtxSwitch != NULL)
        {
	_func_taskMemCtxSwitch (oldCtx, 0);
	}

    if (continueIt)
	taskCont (taskId);		/* resume task if we suspended it */

    return OK;
}


void DbgCallPrint
    (
    INSTR *	callAdrs,	/* address from which function was called */
    int		funcAdrs,	/* address of function called */
    int		nargs,		/* number of arguments in function call */
    int *	args,		/* pointer to function args */
    int		taskId,		/* task's ID */
    BOOL	isKernelAdrs	/* TRUE if Kernel addresses */
    )
{
    SYM_TYPE	symType;
    char *	nameToPrint;
    char *	name;
    char bufn[200];//单个信息缓存数组
    int		val;
    char	demangled [DBG_DEMANGLE_PRINT_LEN + 1];
    int		ix;
    BOOL	doingDefault = FALSE;
    SYMTAB_ID	symTabId = (SYMTAB_ID)0;

    /* Print call address and name of calling function plus offset */

    sprintf (bufn,"~%x", callAdrs);	/* print address from which called */
    strcat(buf,bufn);
    if (!isKernelAdrs)
	symTabId = shellInternalSymTblFromTaskGet (taskId, callAdrs);
    else
	symTabId = sysSymTbl;	/* kernel symbol table for a kernel task */

    name = NULL;

    if (symTabId != (SYMTAB_ID)0
	&& symByValueAndTypeFind (symTabId, (UINT)callAdrs,
				  &name, &val, &symType,
				  SYM_MASK_NONE, SYM_MASK_NONE) == OK)
	{
	nameToPrint = cplusDemangle (name, demangled, sizeof (demangled));
	sprintf (bufn,"~%s~+%x ：", nameToPrint, (int)callAdrs - val);
	strcat(buf,bufn);
	} else
    {
	sprintf (bufn,"%s",_WRS_FRAME_PRINT_STRING);
	strcat(buf,bufn);
    }


    free (name);
    name = NULL;

    /* Print function address/name */

    if (!isKernelAdrs)
	symTabId = shellInternalSymTblFromTaskGet (taskId, (INSTR *)funcAdrs);
    else
	symTabId = sysSymTbl;	/* kernel symbol table for a kernel task */

    if (symTabId != (SYMTAB_ID)0
	&& symByValueAndTypeFind (symTabId, (UINT)funcAdrs,
				  &name, &val, &symType,
				  SYM_MASK_NONE, SYM_MASK_NONE) == OK
	&& val == funcAdrs)
	{
	nameToPrint = cplusDemangle (name, demangled, sizeof (demangled));
	sprintf (bufn,"~%s(", nameToPrint);		/* print function name */
	strcat(buf,bufn);
	}
    else
    {
    sprintf (bufn,"~%x(", funcAdrs);	/* print function address */
    strcat(buf,bufn);
    }
    free (name);
    name = NULL;

    /* If no args are specified, print out default number (see doc at top) */

    if (nargs == 0 && trcDefaultArgs != 0)
	{
	doingDefault = TRUE;
	nargs = trcDefaultArgs;
	}

    if (!isKernelAdrs)
	symTabId = shellInternalSymTblFromTaskGet (taskId, 0);
    else
	symTabId = sysSymTbl;	/* kernel symbol table for a kernel task */

    /* Print args */

    for (ix = 0; ix < nargs; ix++)
	{
	if (ix > 0)
	{
	sprintf (bufn,",");
	strcat(buf,bufn);
	}

	if (args[ix] == 0)
	{
	sprintf (bufn,"0");
	strcat(buf,bufn);
	}
	else if (symTabId != (SYMTAB_ID)0
		 && symByValueAndTypeFind (symTabId, (UINT)args[ix],
					   &name, &val, &symType,
					   SYM_MASK_NONE, SYM_MASK_NONE) == OK
		 && val == args[ix])
	    {
	    nameToPrint = cplusDemangle (name, demangled,
					 sizeof (DBG_DEMANGLE_PRINT_LEN));
	    sprintf (bufn,"&%s", nameToPrint);	/* print argument name */
	    strcat(buf,bufn);
	    }
	else
	    {
		sprintf (bufn,"%x", args[ix]);
		strcat(buf,bufn);
	    }

	free (name);
	name = NULL;
	}

    sprintf (bufn,")");
    strcat(buf,bufn);
}



static const char * semTypeStrGet
    (
    SEM_TYPE semType	/* semaphore type */
    )
    {
    switch (semType)
	{
	case SEM_TYPE_BINARY:		return "SEM_B";
	case SEM_TYPE_MUTEX:		return "SEM_M";
	case SEM_TYPE_COUNTING:		return "SEM_C";
	case SEM_TYPE_OLD:		return "SEM_O";
	case SEM_TYPE_SM_BINARY:	return "SEM_SB";
	case SEM_TYPE_SM_COUNTING:	return "SEM_SC";
	case SEM_TYPE_RW:		return "SEM_RW";
	default:			return NULL;
	}

    return NULL;
    }

static STATUS taskWaitSmObjGet
    (
    WIND_TCB *		pTcb,
    int *		pObjId,
    const char **	ppObjTypeStr
    )
    {
    int	address;

    if (vxMemProbe ((char *)pTcb->pPendQ + OFFSET(Q_FIFO_G_HEAD, pFifoQ),
		    VX_READ, sizeof (SM_SEM_ID), (char *)&address) == OK)
	{
	SM_MSG_Q_ID smObjId;
	UINT32	    verify;

	/* Pending on send */

	smObjId = (SM_MSG_Q_ID)(address - OFFSET(SM_SEMAPHORE, smPendQ)
				- OFFSET(SM_MSG_Q, freeQSem));

	if (vxMemProbe ((char *)smObjId + OFFSET(SM_MSG_Q, verify),
			VX_READ, sizeof (UINT32), (char *)&verify) == OK
	    && TASK_WAIT_SM_OBJ_VERIFY(smObjId) == OK)
	    {
	    *pObjId = verify | 1;
	    *ppObjTypeStr = "MSG_Q_S(S)";
	    return OK;
	    }

	/* Pending on receive */

	smObjId = (SM_MSG_Q_ID)(address - OFFSET(SM_SEMAPHORE, smPendQ)
				- OFFSET(SM_MSG_Q, msgQSem));

	if (vxMemProbe ((char *)smObjId + OFFSET(SM_MSG_Q, verify),
			VX_READ, sizeof (UINT32), (char *)&verify) == OK
	    && TASK_WAIT_SM_OBJ_VERIFY(smObjId) == OK)
	    {
	    *pObjId = verify | 1;
	    *ppObjTypeStr = "MSG_Q_S(R)";
	    return OK;
	    }
	}

    /* Test if task is pended on a shared semaphore */

    if (vxMemProbe ((char *)pTcb->pPendQ + OFFSET(Q_FIFO_G_HEAD, pFifoQ),
		    VX_READ, sizeof (SM_SEM_ID), (char *)&address) == OK)
	{
	SM_SEM_ID   smObjId;
	UINT32	    verify;
	SEM_TYPE    semType;

	smObjId = (SM_SEM_ID)(address - OFFSET(SM_SEMAPHORE, smPendQ));

	if (vxMemProbe ((char *)smObjId + OFFSET(SM_SEMAPHORE, verify),
			VX_READ, sizeof (UINT32), (char *)&verify) == OK
	    && vxMemProbe ((char *)smObjId + OFFSET(SM_SEMAPHORE, objType),
			   VX_READ, sizeof (UINT32), (char *)&semType) == OK
	    && TASK_WAIT_SM_OBJ_VERIFY(smObjId) == OK)
	    {
	    *pObjId = verify | 1;
	    *ppObjTypeStr = semTypeStrGet (semType);
	    return OK;
	    }
	}

    return ERROR;
    }



void pend_task(int taskId,char *buf)
{
	BOOL		objNameSet = FALSE;
	char		objNameStr[10 + 1];
	char		objTypeStr[10 + 1];
	char		bufsf[60];
	int			objId;
	const char *	pObjNameStr = objNameStr;
	const char *	pObjTypeStr = objTypeStr;
	WIND_TCB *	pTcb;
	SEMAPHORE *	pSem;
	MSG_Q *		pMsgR;
	MSG_Q *		pMsgS;

	/* Task is pending. Get the object the task is pending on */

	pTcb = taskTcb (taskId);

	pSem = (SEMAPHORE *)((char *)pTcb->pPendQ
				- OFFSET(SEMAPHORE, qHead));
	pMsgR = (MSG_Q *)((char *)pTcb->pPendQ - OFFSET(MSG_Q_HEAD, pendQ)
				- OFFSET(MSG_Q, msgQ));
	pMsgS = (MSG_Q *)((char *)pTcb->pPendQ - OFFSET(MSG_Q_HEAD, pendQ)
				- OFFSET(MSG_Q, freeQ));

	/*
	 * Check if a task is pending on a:
	 * - semaphore
	 * - message queue (receive)
	 * - message queue (send)
	 * - other objects
	 */

	if (TASK_WAIT_OBJ_VERIFY (pSem, semClassId) == OK)
	    {
	    /* Pending on a semaphore */

	    objId = (int)pSem;
	    pObjTypeStr = semTypeStrGet (pSem->semType);
	    if (pObjTypeStr == NULL)
		{
		sprintf (objTypeStr, "0x%8x", (int)pSem->semType);
		pObjTypeStr = (const char *)objTypeStr;
		}
	    sprintf(bufsf,"%s~%x",pObjTypeStr,objId);//////////////////////////////////////////////
	    strcat(buf,bufsf);
	    }
	else if (TASK_WAIT_OBJ_VERIFY (pMsgR, msgQClassId) == OK)
	    {
	    /* Pending on a message queue (receive) */

	    objId = (int)pMsgR;
	    pObjTypeStr = "MSG_Q(R)";
	    sprintf(bufsf,"%s~%x",pObjTypeStr,objId);//////////////////////////////////////////////
	    strcat(buf,bufsf);
	    }
	else if (TASK_WAIT_OBJ_VERIFY (pMsgS, msgQClassId) == OK)
	    {
	    /* Pending on a message queue (send) */

	    objId = (int)pMsgS;
	    pObjTypeStr = "MSG_Q(S)";
	    sprintf(bufsf,"%s~%x",pObjTypeStr,objId);//////////////////////////////////////////////
	    strcat(buf,bufsf);
	    }
	else if (pTcb->pPendQ == &vxEventPendQ)
	    {
	    /* Pending on the the global VxWorks event queue */

	    objId = 0;
	    pObjTypeStr = "VX_EVENT";
	    pObjNameStr = "";
	    objNameSet = TRUE;
	    sprintf(bufsf,"%s~%x",pObjTypeStr,objId);//////////////////////////////////////////////
	    strcat(buf,bufsf);
	    }
	else if (taskWaitSmObjGet (pTcb, &objId, &pObjTypeStr) == OK)
	    {
	    pObjNameStr = "N/A";
	    objNameSet = TRUE;
	    sprintf(bufsf,"%s~%x",pObjTypeStr,objId);//////////////////////////////////////////////
	    strcat(buf,bufsf);
	    }
	else
	    {
	    /* Unknown object */

	    objId = 0;
	    pObjTypeStr = "N/A";
	    pObjNameStr = "";
	    objNameSet = TRUE;
	    sprintf(bufsf,"%s~%x",pObjTypeStr,objId);//////////////////////////////////////////////
	    strcat(buf,bufsf);

	}

}


void pend_tasks(OutputStream *out)
{
	char bufp[100];
	int flag=0;
	LOCAL   int id[TASKID_MAX]={0};
	int num;
	taskIdListGet(id,TASKID_MAX);/* 获取所有任务ID（ID列表） */
	for(num=0;num<TASKID_MAX;num++)
	{
		if(id[num]!=0)
		{
			if (taskIsPended (id[num]))
			{
			bzero(bufp,sizeof(bufp));
			TASK_DESC td;
			taskInfoGet(id[num],&td);
			sprintf(bufp,"%s~%x~",td.td_name,id[num]);//回复任务名、任务ID
			pend_task(id[num],bufp);//获取任务等待对象的信息
			if(flag!=0)//多条信息用“,”分开
			write_stream(out,',');
			json_write_string(out,bufp);
			flag++;
			}
		}
		else
		{
			break;
		}
	}
}


void DBGCallPrint
    (
    INSTR *	callAdrs,	/* address from which function was called */
    int		funcAdrs,	/* address of function called */
    int		nargs,		/* number of arguments in function call */
    int *	args,		/* pointer to function args */
    int		taskId,		/* task's ID */
    BOOL	isKernelAdrs	/* TRUE if Kernel addresses */
    )
{
    SYM_TYPE	symType;
    char *	nameToPrint;
    char *	name;
    char bufn[200];
    int		val;
    char	demangled [DBG_DEMANGLE_PRINT_LEN + 1];
    int		ix;
    BOOL	doingDefault = FALSE;
    SYMTAB_ID	symTabId = (SYMTAB_ID)0;

    /* Print call address and name of calling function plus offset */

    sprintf (bufn,"%x", callAdrs);	/* print address from which called */
    strcat(buftt,bufn);
    if (!isKernelAdrs)
	symTabId = shellInternalSymTblFromTaskGet (taskId, callAdrs);
    else
	symTabId = sysSymTbl;	/* kernel symbol table for a kernel task */

    name = NULL;
    if (symTabId != (SYMTAB_ID)0
	&& symByValueAndTypeFind (symTabId, (UINT)callAdrs,
				  &name, &val, &symType,
				  SYM_MASK_NONE, SYM_MASK_NONE) == OK)
	{
	nameToPrint = cplusDemangle (name, demangled, sizeof (demangled));
	sprintf (bufn,"~%s~+%x : ", nameToPrint, (int)callAdrs - val);
	strcat(buftt,bufn);
	}
    else
    {
	sprintf (bufn,"%s",_WRS_FRAME_PRINT_STRING);
	strcat(buftt,bufn);
    }
    free (name);
    name = NULL;

    /* Print function address/name */

    if (!isKernelAdrs)
	symTabId = shellInternalSymTblFromTaskGet (taskId, (INSTR *)funcAdrs);
    else
	symTabId = sysSymTbl;	/* kernel symbol table for a kernel task */

    if (symTabId != (SYMTAB_ID)0
	&& symByValueAndTypeFind (symTabId, (UINT)funcAdrs,
				  &name, &val, &symType,
				  SYM_MASK_NONE, SYM_MASK_NONE) == OK
	&& val == funcAdrs)
	{
	nameToPrint = cplusDemangle (name, demangled, sizeof (demangled));
	sprintf (bufn,"~%s(", nameToPrint);		/* print function name */
	strcat(buftt,bufn);
	}
    else
    {
    sprintf (bufn,"~%x(", funcAdrs);	/* print function address */
    strcat(buftt,bufn);
    }
    free (name);
    name = NULL;

    /* If no args are specified, print out default number (see doc at top) */

    if (nargs == 0 && trcDefaultArgs != 0)
	{
	doingDefault = TRUE;
	nargs = trcDefaultArgs;
	strcat(buftt,"[");
	}

    if (!isKernelAdrs)
	symTabId = shellInternalSymTblFromTaskGet (taskId, 0);
    else
	symTabId = sysSymTbl;	/* kernel symbol table for a kernel task */

    /* Print args */

    for (ix = 0; ix < nargs; ix++)
	{
	if (ix > 0)
	{
	sprintf (bufn,",");
	strcat(buftt,bufn);
	}

	if (args[ix] == 0)
	{
	sprintf (bufn,"0");
	strcat(buftt,bufn);
	}
	else if (symTabId != (SYMTAB_ID)0
		 && symByValueAndTypeFind (symTabId, (UINT)args[ix],
					   &name, &val, &symType,
					   SYM_MASK_NONE, SYM_MASK_NONE) == OK
		 && val == args[ix])
	    {
	    nameToPrint = cplusDemangle (name, demangled,
					 sizeof (DBG_DEMANGLE_PRINT_LEN));
	    sprintf (bufn,"&%s", nameToPrint);	/* print argument name */
	    strcat(buftt,bufn);
	    }
	else
	    {
		sprintf (bufn,"%x", args[ix]);
		strcat(buftt,bufn);
	    }
	free (name);
	name = NULL;
	}

    if (doingDefault)
    strcat(buftt,"]");
    sprintf (bufn,")||");
    strcat(buftt,bufn);
}

STATUS DBGStackTrace(int taskId)
{
    REG_SET regSet;
    BOOL    continueIt = FALSE;	/* flag to remember if resuming is necessary */
    RTP_ID  oldCtx = NULL;

    /* Make sure the task don't try to trace itself */

    if (taskId == 0 || taskId == taskIdSelf ())
	{
	return ERROR;
	}

    /* Make sure the task exists */

    if (taskIdVerify (taskId) != OK)
	{
	return ERROR;
	}

    /*
     * If the task is not already stopped or suspended, stop it while
     * we trace it
     */

    if ((!taskIsStopped (taskId)) && (!taskIsSuspended (taskId)))
	{
	if (taskStop (taskId) != OK)
	    {
	    return ERROR;
	    }

	continueIt = TRUE;              /* we want to resume it later */
	}

    /* Trace the stack */

    taskRegsGet (taskId, &regSet);

    /*
     * switch to the memory context of the task to be traced
     * since we will probe the RTP memory. If the task's
     * memory context may not be accessed, either because
     * the RTP is being deleted or the context is invalid,
     * the stack trace will fail with an ERROR.
     */

    if (_func_taskMemCtxSwitch != NULL)
        {
        if ((oldCtx = (RTP_ID) _func_taskMemCtxSwitch (
					rtpFromTaskGet (taskId), 0)) == NULL)
	    {
	    return ERROR;
	    }
	}

    trcStack (&regSet, (FUNCPTR) DBGCallPrint, taskId);

    /* restore the memory context */

    if (_func_taskMemCtxSwitch != NULL)
        {
	_func_taskMemCtxSwitch (oldCtx, 0);
	}

    if (continueIt)
	taskCont (taskId);		/* resume task if we suspended it */

    return OK;
}



void tasktrace(OutputStream *out,int taskId)
{
	bzero(buftt,sizeof(buftt));
	DBGStackTrace(taskId);
	json_write_string(out,buftt);
}
