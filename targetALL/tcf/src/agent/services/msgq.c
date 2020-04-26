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
#include <services/msgq.h>



int msgqShow(MSG_Q_ID msgQId, int level,OutputStream *out)
{
	char buf[50];
    MSG_Q_INFO          info;
    int                 taskIdList[20];
    int                 taskDList[20];
    char               *msgPtrList[20];
    int                 msgLenList[20];
    WIND_TCB           *pTcb;
    STATUS              status;
    int                 ix;
    char               msgbuf[100];
    char               *pName;
    EVENTS_RSRC         msgQEvResource;

#ifdef _WRS_CONFIG_SMP
    WIND_TCB           *pTcbCurrent;
#else
    int                 lock;
#endif

#ifdef _WRS_CONFIG_SM_OBJ
    if (ID_IS_SHARED(msgQId))
    {
        if (msgQSmShowRtn == NULL)
        {
            errno = S_smObjLib_NOT_INITIALIZED;
            return (ERROR);
        }

        return ((*msgQSmShowRtn) (SM_OBJ_ID_TO_ADRS(msgQId), level));
    }
#endif

    bzero((char *)&info, sizeof(info));

    if (level >= 1)
    {

        info.taskIdList = taskIdList;
        info.taskIdListMax = NELEMENTS(taskIdList);

        info.msgPtrList = msgPtrList;
        info.msgLenList = msgLenList;
        info.msgListMax = NELEMENTS(msgPtrList);
    }

#ifdef _WRS_CONFIG_SMP

    _WRS_TASK_ID_CURRENT_GET(pTcbCurrent);

    if (msgQObjSafe(msgQId, FALSE, pTcbCurrent) != OK)
        return ERROR;
#else
    lock = KERNEL_INT_CPU_LOCK();
#endif

    if ((status = msgQInfoGet(msgQId, &info)) == ERROR)
    {
#ifdef _WRS_CONFIG_SMP
        msgQObjUnsafe(msgQId, FALSE, FALSE, FALSE, pTcbCurrent);
#else
        KERNEL_INT_CPU_UNLOCK(lock);
#endif
        sprintf(buf,"Invalid message queue id: %#x", (int)msgQId);
    	json_write_string(out,buf);
    	write_stream(out,'\n');
        return (ERROR);
    }

    if ((info.numTasks > 0) && (level >= 1))
    {
        for (ix = 0; ix < min(info.numTasks, NELEMENTS(taskIdList)); ix++)
        {
            pTcb = (WIND_TCB *)(taskIdList[ix]);
            if (pTcb->status & WIND_DELAY)
                taskDList[ix] = Q_KEY(&tickQHead, &pTcb->tickNode, 1);
            else
                taskDList[ix] = 0;
        }
    }

    msgQEvResource = msgQId->events;

#ifdef _WRS_CONFIG_SMP
    msgQObjUnsafe(msgQId, FALSE, FALSE, FALSE, pTcbCurrent);
#else
    KERNEL_INT_CPU_UNLOCK(lock);
#endif
    bzero(buf,sizeof(buf));
	bzero(msgbuf,sizeof(msgbuf));
	sprintf(buf,"0x%x~",(int)msgQId);/*消息队列ID*/
	strcat(msgbuf,buf);

	pName = objNamePtrGet(msgQId);/*消息队列名称*/
	sprintf(buf,"%s~",pName == NULL ? "N/A" : pName);
	strcat(msgbuf,buf);

if ((info.options & MSG_Q_TYPE_MASK) == MSG_Q_FIFO)/*消息队列属性*/
	{
	sprintf(buf,"%s~", "FIFO");
	strcat(msgbuf,buf);

	}
else
	{
	sprintf(buf,"%s~", "PRIORITY");
	strcat(msgbuf,buf);

	}

	sprintf(buf,"%d~", info.numMsgs);/*消息队列中当前消息数*/
	strcat(msgbuf,buf);


	sprintf(buf,"%d~", info.maxMsgs);/*消息队列最大消息数*/
	strcat(msgbuf,buf);


	sprintf(buf,"%d", info.maxMsgLength);/*单个消息最大长度*/
	strcat(msgbuf,buf);

	json_write_string(out,msgbuf);



return (TRUE);
}

