
#include <memLib.h>
#include <tcf_config.h>
#include <framework/json.h>
#include <framework/errors.h>
#include <framework/exceptions.h>
#include <spyLib.h>
#include <framework/protocol.h>
#include <cpusetCommon.h>
#include <vxWorks.h>
#include <vsbConfig.h>
#include <string.h>
#include <sysSymTbl.h>
#include <sysLib.h>
#include <taskHookLib.h>
#include <logLib.h>
#include <stdio.h>
#include <intLib.h>
#include <private/cplusLibP.h>
#include <private/windLibP.h>
#include <private/kernelLibP.h>
#include <private/kernelLockLibP.h>
#include <spyext.h>
#include "cpushow.h"
#include <delta.h>
#include <taskLib.h>
#include <hostLib.h>
#include <framework/events.h>
#include <private/taskLibP.h>
#include <rtpLibCommon.h>

void CinfoToString(CPU_INFO *cpuinfo,char *buf)
{

	sprintf(buf,"%d_%f",cpuinfo->cpuid,cpuinfo->rate);

}

static const char * CPUSHOW = "Cpu";
static TCFBroadcastGroup * broadcast_group = NULL;
char buf[10];
int taskcpuindex=-1;

void taskcpunum(void *out)
{

	write_stringz(out, "E");
	write_stringz(out, "Cpu");
	write_stringz(out, "changed");
	write_stream(out, '[');
	char name[32];
	int ip=0;
	int bufnum=0;
	float memStat=0.0;
	char buf[50];
	char membuf[60]={0};
	char bufcpu[60];
	MEM_PART_STATS	partStats;
	struct in_addr iaddr;
	gethostname(name,30);/* ��ȡ������ */
	ip=hostGetByName(name);/* ͨ����������ȡIP */
	bzero(buf,sizeof(buf));
	iaddr.s_addr=ip;
	sprintf(buf,"%s",inet_ntoa(iaddr));/* ��IPת��Ϊ�ַ�����д��buf */
	json_write_string(out,buf);
	int cpu_num=vxCpuConfiguredGet();
	int a=cpu_num;
	CPU_INFO ker[cpu_num];
	unsigned int cpuset=0;
	while(a--)
	{
	  CPUSET_SET(cpuset,a);
	}
	getCpuInfoByIdlist(cpuset,ker);/*��ȡ�����ں�CPU��ʹ�����*/
	for(a=0;a<cpu_num;a++)
	{
	  write_stream(out, ',');
	  ker[a].rate+=0.5; /* �������� */
	  CinfoToString(&ker[a],buf);

	  bzero(bufcpu,sizeof(bufcpu));
	  for(bufnum=0;bufnum<100;bufnum++)/* ȡ�� */
	  {

		  if(buf[bufnum]!='.')
		  {
			  bufcpu[bufnum]=buf[bufnum];
		  }
		  else
		  {
			  break;
		  }
	  }
	 json_write_string(out,bufcpu);
	}
	write_stream(out, ',');
	memPartInfoGet (memSysPartId, &partStats);/* ��ȡϵͳ�ڴ�ʹ����� */
	memStat=((float)partStats.numBytesAlloc)/(((float)partStats.numBytesFree)+ \
		   ((float)partStats.numBytesAlloc))*100;/* ����ٷֱ� */
	memStat+=0.5;/* �������� */
	sprintf(membuf,"%s%f","Mem_",memStat);
	bzero(bufcpu,sizeof(bufcpu));
	for(bufnum=0;bufnum<100;bufnum++)/* ȡ�� */
	{

	  if(membuf[bufnum]!='.')
	  {
		  bufcpu[bufnum]=membuf[bufnum];
	  }
	  else
	  {
		  break;
	  }
	}
	json_write_string(out,bufcpu);
	write_stream(out, ']');
	write_stream(out, 0);
	write_stream(out, MARKER_EOM);
	post_event_with_delay(taskcpunum, out, 1000000);//ѭ��ִ�С�taskcpunum��
}


/* "R"�ظ�CPU������"E"�ظ������ں�CPU��ʹ���� */
static void command_cpunum(char * token, Channel * c)
{

	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
	bzero(buf,sizeof(buf));
    int cpu_num=vxCpuConfiguredGet();/* ��ȡ�ں˵�����Ŀ */
    sprintf(buf,"%d",cpu_num);
    json_write_string(&c->out,buf);
    post_event(taskcpunum,&broadcast_group->out);
    /*ͨ���ں���post_event(taskcpunum,...)�е��ú���post_event_with_delay(taskcpunum,...)ʵ��ѭ���ϱ���Ϣ*/
	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}
static void command_cpustop(char * token, Channel * c)
{

	if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);
	write_stringz(&c->out, "R");
	write_stringz(&c->out, token);
	write_errno(&c->out, 0);
	int stop=cancel_event(taskcpunum,&broadcast_group->out,1);
	if(stop==1)
	{
	json_write_string(&c->out,"stop success");
	}
	else
	{
	json_write_string(&c->out,"stop fail");
	}
	write_stream(&c->out, 0);
	write_stream(&c->out, MARKER_EOM);
}



void ini_cpushow_service(Protocol * proto,TCFBroadcastGroup * bcg)
{
	usrUtilSpyExtInit();   /* ��ɸó�ʼ������ѯ����CPUʹ������صĺ�����������ʹ�� */
	broadcast_group = bcg;
    add_command_handler(proto, CPUSHOW, "CpuStop", command_cpustop);
    add_command_handler(proto, CPUSHOW, "CpuNum", command_cpunum);

}
