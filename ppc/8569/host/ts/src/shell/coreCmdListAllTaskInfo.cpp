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


/* ��ȡ������Ϣ */

int coreCmdProcess::m_listAllTaskInfo(string cmd)
{
    int len = 0;
    int ret = 0;
    char pbuf[MAX_MSG_SIZE] = {'\0'};
    ACoreOs_task_information taskinfo;
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

	
    /* ���Ͳ�ѯ������Ϣ�������shell agent */
    ret = sendDataToShellAgent( LIST_ALL_TASK_STR, sizeof(LIST_ALL_TASK_STR) );
    if(ret < 0)
    {
        return FAIL;
    }

    /* ��ӡ��ǩ */
    printTaskInfoLable();
    
    while(1)
    {
        /* ���ն�ȡ��� */
        memset(pbuf, 0, sizeof(pbuf));
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

        if(0 == strcmp(pbuf, RECV_TASK_INFO_END_STR))
        {
            break;
        }

        /* ����������Ϣ */
        memset(&taskinfo, 0, sizeof(taskinfo));
        ret = analysisTaskInfo(pbuf, &taskinfo);
        if(ret < 0)
        {
            return FAIL;
        }

        /* ��ӡ������Ϣ */
        printSingleTaskInfo(taskinfo);

        /* ��ӡ����״̬ */
        state = taskinfo.state;
        printSingleTaskStatus(state);

        /* ����ָ����ʽ��ӡ������Ϣ */
        //cout << pbuf << endl;

    }

    printf("\n");

    return SUCC;
}


/*
	����������
*/
bool coreCmdListAllTaskCheckSubArgv(string & cmd){

	bool ret = true;
	if(cmd != LIST_ALL_TASK_STR ){
		std::cout <<  "�����ʽ����" << std::endl;
		ret = false;
	}

	return ret;
}