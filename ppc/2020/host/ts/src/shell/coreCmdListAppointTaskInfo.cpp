#include "coreCmdProcess.h"
#include "CheckSubArgv.h"
using namespace std;
static const basic_string <char>::size_type npos = -1;

/************************�ⲿ����******************************/
/* ��ts����������ID */
extern int tsLinkId;

/* ��ǰĿ������ӵ�AID */
extern int curTargetAID;

/* ��ǰ����Ŀ��������� */
extern int curTargetName;


/* ��ȡָ��������Ϣ */
int coreCmdProcess::m_listAppointTaskInfo(string cmd)
{
    int ret = 0, len = 0;
    string cmdtemp = cmd;
    string::size_type noblankpos = 0;
    char pbuf[MAX_MSG_SIZE] = {'\0'};
    ACoreOs_task_information taskinfo;
    char* pstr = NULL;
    int state = 0;
	//��ѯOS�汾
	if( SUCC != queryTargetAgentOsVersion() )
	{
		return FAIL;
	}
	//�����ǰĿ����Ĳ���ϵͳ�汾�뵱ǰ���Ʋ�ƥ�䣬��ֱ�Ӳ����κδ���
	if(m_osVersion != OS_2X_VERSION ){
		cout << "������ֻ֧��ACoreOS653����ϵͳ"<< endl;
		return FAIL;
	}
	//��ѯOS�Ƿ�����
	if( SUCC != queryTargetAgentOsStauts() )
	{
		return FAIL;
	}
	//��ѯ�����Ƿ�����
	if( SUCC != queryTargetShellAgent() )
	{
		return FAIL;
	}
	
    /* ȥ��������ti */
    cmdtemp.erase( 0, strlen(LIST_APPOINT_TASK_STR) );

    /* ���ҵ�һ���ǿո���ַ�,��ɾ���ո�֮ǰ���� */
    noblankpos = cmdtemp.find_first_not_of(BLANK_STR);
    if( (npos  == noblankpos) || (0  == noblankpos) )
    {
        cout << "����Ĳ�ѯָ�����������ʽ����" << endl;
        return FAIL;
    }
    cmdtemp.erase(0, noblankpos);

    memset(pbuf, 0, sizeof(pbuf));

    /* ��װ��ѯָ�������������ʽti:xxxx */
    len = sprintf(pbuf, "ti:%s", cmdtemp.c_str());

    /* ���Ͳ�ѯָ��������Ϣ�������shell agent */
    ret = sendDataToShellAgent(pbuf, len);
    if(ret < 0)
    {
        return FAIL;
    }

    memset(pbuf, 0, sizeof(pbuf));

    /* ���ն�ȡ��� */
    ret = recvDataFromTs( pbuf, sizeof(pbuf) );
    if( ret == STOP_CMD )
    {
        return FAIL;
    }
    else if(ret < 0)
    {
        cout << "��������ʧ��" << endl;
        return FAIL;
    }

    /* �ظ�����ϢΪ�������,��ֱ�Ӵ�ӡ������Ϣ */
    if('E' == pbuf[0])
    {
        cout << "����" <<cmdtemp<<"������" << endl;
        return FAIL;
    }

    pstr = pbuf;

    memset(&taskinfo, 0, sizeof(taskinfo));

    /* ����������Ϣ */
    ret = analysisTaskInfo(pstr, &taskinfo);
    if(ret < 0)
    {
        return FAIL;
    }

    /* ��ӡ��ǩ */
    printTaskInfoLable();

    /* ��ӡ������Ϣ */
    printSingleTaskInfo(taskinfo);

    /* ��ӡ����״̬ */
    state = taskinfo.state;
    printSingleTaskStatus(state);

    printf("\n");

    printf("status detail:");

    printSingleTaskDetailStatus(state);

    printf("    status value:0x%x\n", state);

    printf("\n");

    /* ��ӡ����ջ��Ϣ */
    printSingleTaskStackInfo(taskinfo);

    /* ��ӡ������Ϣ */
    printSingleTaskOtherInfo(taskinfo);

    //cout << pbuf << endl;

    return SUCC;
}

bool coreCmdListAppointTaskCheckSubArgv(string & cmd){
	bool ret = true;
	vector<string> paraterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = paraterList.size();
	if(length != 1){
		std::cout << "�����ʽ����" << std::endl;
		ret =false;
	}
	
	return ret;

}