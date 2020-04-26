#include <private/semSmLibP.h>
#include <vxWorks.h>
#include <vsbConfig.h>
#include <intLib.h>
#include <taskLib.h>
#include <errno.h>
#include <private/semLibP.h>
#include <private/kernelLibP.h>

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
#include <msgq.h>

#include <taskLib.h>
#include <private/taskLibP.h>
#include <spyext.h>
#include <private/taskLibP.h>
#include <cpusetCommon.h>
SysCallInfo syscallinfo[200];
static const char * SEM = "Sem";
TASK_DESC td;
char buf[100];
char sembuf[500];
#define TASKID_MAX 300
char bufstandby[50];
int Taskcpuindex=-1;
TASK_CPU_INFO cpuinfo[TASKID_MAX];
LOCAL char         *semTypeMsg[MAX_SEM_TYPE] = {
    "BINARY", "MUTEX", "COUNTING", "OLD", "\0", "\0", "READ/WRITE",
    "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0"
};
/********************** �������Ϣ���˸�ʽ������������ ******************************/
void TTCinfoToString(char *buf,TASK_CPU_INFO *tcpu,char*status,int *Taskcpuindex,TASK_DESC *td)
{

sprintf(buf,"%s~%s~%x~%x~%u~%f~%s~%d~%d~%d~%d",(td->td_rtpId)->pPathName,tcpu->tkName,tcpu->tkTid,tcpu->tkEntry,tcpu->tkPriority,tcpu->rate, \
		status,*Taskcpuindex, td->td_stackSize, td->td_stackCurrent, td->td_stackHigh);
//task: name id entry priority rate status cpu_index stack_size stack_current_size stack_high_history
}


/********************** ��ȡ�ȴ��ź������������� ******************************/
int SemInfo(SEM_ID semId, int idList[], int maxTasks)
{
    int                 numBlk;
    int                 ix;

#ifndef _WRS_CONFIG_SMP
    int                 lock;
#endif

    if (INT_RESTRICT() != OK)
        return (ERROR);

#ifdef _WRS_CONFIG_SM_OBJ
    if (ID_IS_SHARED(semId))
    {
        if (semSmInfoRtn == NULL)
        {
            errno = S_smObjLib_NOT_INITIALIZED;
            return (ERROR);
        }

        return ((*semSmInfoRtn) (SM_OBJ_ID_TO_ADRS(semId), idList, maxTasks));
    }
#endif

#ifndef _WRS_CONFIG_SMP
    lock = KERNEL_INT_CPU_LOCK();
#else
    OBJ_LOCK(semClassId, lock);
#endif

    if (OBJ_VERIFY(semId, semClassId) != OK)
    {
#ifndef _WRS_CONFIG_SMP
        KERNEL_INT_CPU_UNLOCK(lock);
#else
        OBJ_UNLOCK(semClassId, lock);
#endif
        return (ERROR);
    }

    numBlk = Q_INFO(&semId->qHead, idList, maxTasks);

    if (semId->semType == SEM_TYPE_RW)
    {
        if ((maxTasks - numBlk) > 0)
        {
            numBlk += Q_INFO(&SEMRW_RQHEAD(semId),
                             &idList[numBlk], (maxTasks - numBlk));
        }
    }

#ifndef _WRS_CONFIG_SMP
    KERNEL_INT_CPU_UNLOCK(lock);
#else
    OBJ_UNLOCK(semClassId, lock);
#endif

    if (idList != NULL)
        for (ix = 0; ix < numBlk; ix++)
            idList[ix] = (int)NODE_PTR_TO_TCB(idList[ix]);

    return (numBlk);
}


/********************** show sem ******************************/
int SemDetailShow(SEM_ID semId,OutputStream *out)
{
    int                 taskIdList[20];
    int                 taskDList[20];
    int                 numTasks=0;
    WIND_TCB           *pTcb;
    char               *qMsg;
    int                 ix;
    char IdName[100];
    EVENTS_RSRC         semEvResource;
    char               *name;
	bzero(sembuf,sizeof(sembuf));
    if ((numTasks = SemInfo(semId, &taskIdList[0], 20)) == ERROR)
    {
        printf("Invalid semaphore id: %#x", (int)semId);
        return (FALSE);
    }
    if (numTasks > 0)
    {
        for (ix = 0; ix < min(numTasks, NELEMENTS(taskIdList)); ix++)
        {
            pTcb = (WIND_TCB *)(taskIdList[ix]);
            if (pTcb->status & WIND_DELAY)
                taskDList[ix] = Q_KEY(&tickQHead, &pTcb->tickNode, 1);
            else
                taskDList[ix] = 0;
        }
    }

    pTcb = SEM_OWNER(semId);
    semEvResource = semId->events;
    qMsg =((semId->options & SEM_Q_MASK) == SEM_Q_FIFO) ? "FIFO" : "PRIORITY";
    sprintf(buf,"0x%x~", (int)semId);/*�ź���ID*/
    strcat(sembuf,buf);

    name = objNamePtrGet(semId);
    sprintf(buf,"%s~",name == NULL ? "N/A" : name);/*�ź�������*/
    strcat(sembuf,buf);

    sprintf(buf,"%s~", semTypeMsg[semId->semType]);/*�ź�������*/
    strcat(sembuf,buf);

    sprintf(buf,"%s~", qMsg);/*�ź�������*/
    strcat(sembuf,buf);

   /*����Ϊ�����ź���ʱ�ظ�����ֵ������ظ���0��*/
    switch (semId->semType)
    {
    case SEM_TYPE_BINARY:
    	strcat(sembuf,"0");
        break;

    case SEM_TYPE_COUNTING:

    	sprintf(buf,"%u",semId->semCount);
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

                             /* �������������ID */
        if (numTasks > 0)
        {
        	memset(IdName,0,100);
            for (ix = 0; ix < min(numTasks, NELEMENTS(taskIdList)); ix++)
			{
            	if(ix==0)
				{
					sprintf(buf,"~0x%x",taskIdList[ix]);
					strcat(sembuf,buf);
					sprintf(buf,"~%s",(taskName(taskIdList[ix])));//��ȡ�����������棬����һ����Ϣ���á�~���ֿ�
					strcat(IdName,buf);
				}
				else
				{
					sprintf(buf,",0x%x",taskIdList[ix]);
					strcat(sembuf,buf);
					sprintf(buf,",%s",(taskName(taskIdList[ix])));//������������á������ֿ�
					strcat(IdName,buf);
				}

			}
            strcat(sembuf,IdName);//��������������Ϣ����ظ�buf
        }
        else
		{
			strcat(sembuf,"~NULL");
		}
    json_write_string(out,sembuf);

    return (TRUE);
}



/********************** ��ȡ��Ϣ���е���Ϣ ******************************/
static void command_msgq_get(char * token, Channel * c) {
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	write_stream(&c->out, '[');

	DELTA_ID              objId = NULL;
	DL_NODE            *pNode;
	CLASS_ID            classId;
	DL_LIST             list;
	int level=1;
	/*
	   level=0:��ʾ�ź����ĸ�Ҫ��Ϣ��
	   level=1:��ʾ�ź�������ϸ��Ϣ��
	   level=2:ֻ��ʾ�����ź�����
	   level=3:ֻ��ʾ��ֵ�ź�����
	   level=4:ֻ��ʾ�����ź�����
	   level=5:ֻ��ʾ����������������ź�����
	   level=6:ֻ��ʾ������������Ķ�ֵ�ź�����
	   level=7:ֻ��ʾ��ӵ���ߵĻ����ź�����
	   level=8:ֻ��ʾ��������������ź���(numTask>0)��
	*/

	enum coreObjClassType classType=windMsgQClass;
	if ((classType >= coreNumObjClass) ||\
			((classId = objClassIdGet(classType)) == NULL))
	{
		errnoSet(S_objLib_OBJ_ILLEGAL_CLASS_TYPE);
	}

	_func_classListLock(classId);

	if (FALSE)//objPublic
		list = classId->objPubList;
	else
		list = classId->objPrivList;

	DL_NODE * pNextNode;
	pNode = DLL_FIRST (&list);
	OutputStream *out=&c->out;
	while (pNode != NULL)
	{
	objId = (DELTA_ID)((int)pNode - OFFSET (OBJ_CORE, classNode));
	pNextNode = DLL_NEXT (pNode);

	if ((* msgqShow) (objId, level,out) == FALSE)
			break;
	pNode = pNextNode;
	if(pNode != NULL)
	write_stream(&c->out, ',');
	}
	_func_classListUnlock(classId);
	write_stream(&c->out, ']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);

}
/*************************��ȡϵͳ�������ź���**************************/
static void command_sem_get(char * token, Channel * c) {
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	write_stream(&c->out, '[');


	DELTA_ID              objId = NULL;
	DL_NODE            *pNode;
	CLASS_ID            classId;
	DL_LIST             list;
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
	OutputStream *out=&c->out;
	while (pNode != NULL)
	{

	objId = (DELTA_ID)((int)pNode - OFFSET (OBJ_CORE, classNode));
	pNextNode = DLL_NEXT (pNode);

	if ((* SemDetailShow) (objId,out) == FALSE)
			break;
	pNode = pNextNode;
	if(pNode != NULL)
	write_stream(&c->out, ',');
	}
	_func_classListUnlock(classId);
	write_stream(&c->out, ']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);

}


/********************** ��ȡ���е�rtp����Ϣ ******************************/
void command_rtp_get(char * token, Channel * c)
{
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	write_stream(&c->out, '[');

	DELTA_ID              objId = NULL;
	DL_NODE            *pNode;
	CLASS_ID            classId;
	DL_LIST             list;
	int level=1;
	/*
	   level=0:��ʾ�ź����ĸ�Ҫ��Ϣ��
	   level=1:��ʾ�ź�������ϸ��Ϣ��
	   level=2:ֻ��ʾ�����ź�����
	   level=3:ֻ��ʾ��ֵ�ź�����
	   level=4:ֻ��ʾ�����ź�����
	   level=5:ֻ��ʾ����������������ź�����
	   level=6:ֻ��ʾ������������Ķ�ֵ�ź�����
	   level=7:ֻ��ʾ��ӵ���ߵĻ����ź�����
	   level=8:ֻ��ʾ��������������ź���(numTask>0)��
	*/
	enum coreObjClassType classType=windRtpClass;
	if ((classType >= coreNumObjClass) ||\
			((classId = objClassIdGet(classType)) == NULL))/* ͨ�����ͻ�ȡ�������Ϣ */
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
	OutputStream *out=&c->out;
	while (pNode != NULL)
	{
	objId = (DELTA_ID)((int)pNode - OFFSET (OBJ_CORE, classNode));
	pNextNode = DLL_NEXT (pNode);

	if ((* RtpSummaryShow) (objId, level,out) == FALSE)
			break;
	pNode = pNextNode;
	if(pNode != NULL)
	write_stream(&c->out, ',');
	}
	_func_classListUnlock(classId);
	write_stream(&c->out, ']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);

}


/********************** ��ȡϵͳ�����е�������Ϣ ******************************/
static void command_alltasks_get(char * token, Channel * c)
{


	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	write_stream(&c->out, '[');

	LOCAL   int id[TASKID_MAX]={0};
	int num;
    struct in_addr iaddr;
    char name[32];
    int ip=0;

    gethostname(name,30);/* ��ȡ������ */
    ip=hostGetByName(name);/* ͨ����������ȡIP */
    bzero(buf,sizeof(buf));
    iaddr.s_addr=ip;
    sprintf(buf,"%s",inet_ntoa(iaddr));/* ��IPת��Ϊ�ַ�����д��buf */
    json_write_string(&c->out,buf);

	taskIdListGet(id,TASKID_MAX);/* ��ȡ��������ID��ID�б� */

	for(num=0;num<TASKID_MAX;num++)
	{
	  if(id[num]!=0)
	  {
		write_stream(&c->out, ',');
	    bzero(buf,sizeof(buf));
	    cpuInfoByTaskId(id[num],&cpuinfo[num]);/* ͨ��ID��ȡ�����CPUʹ���� */
	    taskStatusString (id[num], bufstandby);/* ͨ��ID��ȡ�����״̬ */
	    taskCpuIndexGet(id[num],&Taskcpuindex);/* ͨ��ID��ȡ����������CPU�ı�� */
	    taskInfoGet(id[num],&td);              /* ͨ��ID��ȡ������Ϣ */
	    TTCinfoToString(buf,&cpuinfo[num],bufstandby,&Taskcpuindex,&td);/* ���ض��ĸ�ʽ����ȡ����Ϣ���뵽buf */
	    json_write_string(&c->out,buf);
	  }
	  else
	  {
	    break;
	  }

	}
    write_stream(&c->out, ']');

    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}


/********************** ��ȡrtp������������Ϣ ******************************/
static void command_rtptask_get(char * token, Channel * c)
{
	bzero(buf,sizeof(buf));
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	char bufinp[20];
	json_read_string(&c->inp,bufinp,sizeof(bufinp));
	if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	int rtpId;
	sscanf(bufinp,"%x",&rtpId);     /* ����ȡ���� 16�����ַ���ת��Ϊ10����  */
    int	rtpTaskIdList[MAX_RTP_TASKS];
	memset(rtpTaskIdList,0,MAX_RTP_TASKS*(sizeof(int)));
	write_stream(&c->out, '[');
	GetRtpTaskIdList(rtpId, rtpTaskIdList);/* ͨ��rtpID��ȡrtp����������ID */
	int i=0;
	for(i=0;i<MAX_RTP_TASKS;i++)
	{
		if(rtpTaskIdList[i]!=0)
		{
			if(i!=0)
			write_stream(&c->out,',');

			bzero(buf,sizeof(buf));
			cpuInfoByTaskId(rtpTaskIdList[i],&cpuinfo[i]);/* ͨ��ID��ȡ�����CPUʹ���� */
			taskStatusString (rtpTaskIdList[i], bufstandby);/* ͨ��ID��ȡ�����״̬ */
			taskCpuIndexGet(rtpTaskIdList[i],&Taskcpuindex);/* ͨ��ID��ȡ����������CPU�ı�� */
			taskInfoGet(rtpTaskIdList[i],&td);              /* ͨ��ID��ȡ������Ϣ */
			TTCinfoToString(buf,&cpuinfo[i],bufstandby,&Taskcpuindex,&td);/* ���ض��ĸ�ʽ����ȡ����Ϣ���뵽buf */
			json_write_string(&c->out,buf);
		}
		else
		{
			break;
		}
	}
	write_stream(&c->out,']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);

}

/********************** ��ȡrtp�Ķ�̬����Ϣ ******************************/
static void command_shl_get(char * token, Channel * c)
{

	bzero(buf,sizeof(buf));
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	char bufinp[20];
	json_read_string(&c->inp,bufinp,sizeof(bufinp));
	if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	int rtpId;
	sscanf(bufinp,"%x",&rtpId);     /* ����ȡ���� 16�����ַ���ת��Ϊ10����  */
	write_stream(&c->out, '[');
	rtpshlShow(&c->out,rtpId);
	write_stream(&c->out,']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}


/********************** ��ȡrtp���ź�����Ϣ ******************************/
static void command_rtpsem_get(char * token, Channel * c)
{
	bzero(buf,sizeof(buf));
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	char bufinp[20];
	json_read_string(&c->inp,bufinp,sizeof(bufinp));
	if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	int rtpId;
	sscanf(bufinp,"%x",&rtpId);     /* ����ȡ���� 16�����ַ���ת��Ϊ10����  */
	write_stream(&c->out, '[');
	rtpsemShow(&c->out,rtpId);
	write_stream(&c->out,']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}


/********************** ��ȡrtp����Ϣ������Ϣ ******************************/
static void command_rtpmsgq_get(char * token, Channel * c)
{
	bzero(buf,sizeof(buf));
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	char bufinp[20];
	json_read_string(&c->inp,bufinp,sizeof(bufinp));
	if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	int rtpId;
	sscanf(bufinp,"%x",&rtpId);     /* ����ȡ���� 16�����ַ���ת��Ϊ10����  */
	write_stream(&c->out, '[');
	rtpmsgqShow(&c->out,rtpId);
	write_stream(&c->out,']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}


/********************** ��ȡrtp�򿪵��ļ�����������Ϣ ******************************/
static void command_rtpfd_get(char * token, Channel * c)
{
	bzero(buf,sizeof(buf));
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	char bufinp[20];
	json_read_string(&c->inp,bufinp,sizeof(bufinp));
	if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	int rtpId;
	sscanf(bufinp,"%x",&rtpId);     /* ����ȡ���� 16�����ַ���ת��Ϊ10����  */
	write_stream(&c->out, '[');
	RtpFdShow(&c->out,(RTP_ID)rtpId);
	write_stream(&c->out,']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}

/********************** ��ȡrtp��ϵͳ������Ϣ ******************************/
static void command_rtpsyscall_monitor(char * token, Channel * c)
{   char bufsys[50];
	bzero(bufsys,sizeof(bufsys));
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	char bufinp[20];
	json_read_string(&c->inp,bufinp,sizeof(bufinp));
	if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	int rtpId;
	int level;
	/*
	 * level==1; ��� ��ϵͳ���á�
	 * level==0���رնԡ�ϵͳ���á��ļ�أ���ȡ��ϵͳ���á�����Ϣ
	 */
	char l[2];
	char *p=bufinp;
	while(p)
	{
		if(*p==',')
		{
			*p='\0';
			p++;
			l[0]=*p;
			break;
		}
		p++;
	}
	sscanf(bufinp,"%x",&rtpId);     /* ����ȡ���� 16�����ַ���ת��Ϊ10����  */
	level=atoi(l);
	write_stream(&c->out, '[');
	SysCallMonitor(level,(RTP_ID)rtpId);
	if(level==0)
	{
		int i;
		for(i=1;i<=syscallinfo[0].callnum;i++)
		{
			if(i!=1)
			{
				write_stream(&c->out,',');
			}
			sprintf(bufsys,"%s~%d",syscallinfo[i].syscallname,syscallinfo[i].callnum);
			json_write_string(&c->out,bufsys);
		}
	}
	memset(syscallinfo,0,sizeof(syscallinfo));
	write_stream(&c->out,']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}


/********************** ��ȡrtp���ڴ�ʹ����Ϣ ******************************/
static void command_rtpmem_get(char * token, Channel * c)
{
	bzero(buf,sizeof(buf));
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	char bufinp[20];
	json_read_string(&c->inp,bufinp,sizeof(bufinp));
	if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	int rtpId;
	sscanf(bufinp,"%x",&rtpId);     /* ����ȡ���� 16�����ַ���ת��Ϊ10����  */
	write_stream(&c->out, '[');
	rtpEdrMemShow(&c->out,rtpId);
	write_stream(&c->out,']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}

/********************** ��ȡ�ں˵��ڴ�ʹ����Ϣ ******************************/
static void command_kernelmem_get(char * token, Channel * c)
{
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);
	write_stream(&c->out, '[');
	MemEdrPartShow(&c->out,0);
	write_stream(&c->out,']');

	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}
void ini_getsem_service(Protocol * proto,TCFBroadcastGroup * bcg)
{
	add_command_handler(proto, SEM, "SemShow", command_sem_get);
	add_command_handler(proto, SEM, "MsgQShow", command_msgq_get);
	add_command_handler(proto, SEM, "RtpShow", command_rtp_get);
    add_command_handler(proto, SEM, "AllTasks", command_alltasks_get);
    add_command_handler(proto, SEM, "RtpTaskShow", command_rtptask_get);
    add_command_handler(proto, SEM, "RtpShlShow", command_shl_get);
    add_command_handler(proto, SEM, "RtpSemShow", command_rtpsem_get);
    add_command_handler(proto, SEM, "RtpMsgQShow", command_rtpmsgq_get);
    add_command_handler(proto, SEM, "RtpFdShow", command_rtpfd_get);
    add_command_handler(proto, SEM, "RtpSysCallShow", command_rtpsyscall_monitor);
    add_command_handler(proto, SEM, "MemEdrRtpBlockShow", command_rtpmem_get);
    add_command_handler(proto, SEM, "MemEdrBlockShow", command_kernelmem_get);

}

