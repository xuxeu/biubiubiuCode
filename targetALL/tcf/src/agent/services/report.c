#include <tcf_config.h>
#include <framework/json.h>
#include <framework/errors.h>
#include <framework/exceptions.h>
#include <delta.h>
#include <spyext.h>
#include <taskLib.h>
#include <private/taskLibP.h>
#include <rtp.h>
#include <stdio.h>
#include <cpushow.h>
static const char * REPORT = "Report";
SysCallInfo callinfo[200];
int iNum=0;
char buf[100];
char bufinp[20];
char bufstandby[50];
extern int cpuInfoByTaskId( int tskId,TASK_CPU_INFO * info);
extern STATUS taskCpuIndexGet (int tid, int *pCpuIndex);

static void command_Rtptask_get(char * token, Channel * c)
{
	int Taskcpuindex=-1;

	TASK_CPU_INFO cpuinfo[TASKID_MAX];
	TASK_DESC td;
	bzero(buf,sizeof(buf));
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);

	json_read_string(&c->inp,bufinp,sizeof(bufinp));
	if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	int rtpId;
	sscanf(bufinp,"%x",&rtpId);     /* 将读取到的 16进制字符串转换为10进制  */
    int	rtpTaskIdList[MAX_RTP_TASKS];
	memset(rtpTaskIdList,0,MAX_RTP_TASKS*(sizeof(int)));
	write_stream(&c->out, '[');
	GetRtpTaskIdList(rtpId, rtpTaskIdList);/* 通过rtpID获取rtp的所有任务ID */

	for(iNum=0;iNum<MAX_RTP_TASKS;iNum++)
	{


		if(rtpTaskIdList[iNum]!=0)
		{

			if(iNum!=0)
			write_stream(&c->out,',');
			bzero(buf,sizeof(buf));
			cpuInfoByTaskId(rtpTaskIdList[iNum],&cpuinfo[iNum]);/* 通过ID获取任务的CPU使用率 */

			taskStatusString (rtpTaskIdList[iNum], bufstandby);/* 通过ID获取任务的状态 */

			taskCpuIndexGet(rtpTaskIdList[iNum],&Taskcpuindex);/* 通过ID获取任务所属的CPU的编号 */

			taskInfoGet(rtpTaskIdList[iNum],&td);              /* 通过ID获取任务信息 */

			TTCinfoToString(buf,&cpuinfo[iNum],bufstandby,&Taskcpuindex,&td);/* 按特定的格式将获取的信息输入到buf */

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

static void command_Rtpsyscall_monitor(char * token, Channel * c)
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
	sscanf(bufinp,"%x",&rtpId);     /* 将读取到的 16进制字符串转换为10进制  */
	level=atoi(l);
	write_stream(&c->out, '[');
	RSysCallMonitor(level,(RTP_ID)rtpId);
	/*level==1;开启系统调用监听函数
	 *level==0;关闭监听函数，统计系统调用次数
	 */
	if(level==0)
	{
		int i;
		for(i=1;i<=callinfo[0].callnum;i++)
		{
			if(i!=1)
			{
				write_stream(&c->out,',');
			}
			sprintf(bufsys,"%s~%d",callinfo[i].syscallname,callinfo[i].callnum);
			json_write_string(&c->out,bufsys);
		}
	}
	memset(callinfo,0,sizeof(callinfo));
	write_stream(&c->out,']');
	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}


void ini_report_service(Protocol * proto,TCFBroadcastGroup * bcg)
{

    add_command_handler(proto, REPORT, "RtpSysCallShow", command_Rtpsyscall_monitor);
    add_command_handler(proto, REPORT, "RtpTaskShow", command_Rtptask_get);
}
