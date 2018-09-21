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

/* ����ָ������  */
int coreCmdProcess::m_suspendTask(string cmd)
{
    int ret = 0, len = 0;
    string cmdtemp = cmd;
    string::size_type noblankpos = 0;
    char pbuf[MAX_MSG_SIZE] = {'\0'};
	
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
	
    /* ȥ��������ts */
    cmdtemp.erase( 0, strlen(STOP_TASK_STR) );

    /* ���ҵ�һ���ǿո���ַ�,��ɾ���ո�֮ǰ���� */
    noblankpos = cmdtemp.find_first_not_of(BLANK_STR);
	if( (npos  == noblankpos) || (0  == noblankpos) )
    {
        cout << "����Ĺ���ָ�������������ʽ����" << endl;
        return FAIL;
    }
    cmdtemp.erase(0, noblankpos);

    memset(pbuf, 0, sizeof(pbuf));

    /* ��װ��ѯָ�������������ʽts:xxxx */
    len = sprintf(pbuf, "ts:%s", cmdtemp.c_str());

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

    /* ����ָ����ʽ��ӡִ�н�� */
    if( 0 == strcmp(pbuf, "OK") )
    {
        printf("���\n");
        return SUCC;
    }
    else
    {
        printf("����%s������\n",cmdtemp.c_str());
        return FAIL;
    }

}

bool coreCmdStopTaskCheckSubArgv(string & cmd){

	bool ret = true;
	vector<string> paraterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = paraterList.size();
	if(length != 1){
		std::cout << "�����ʽ����" << std::endl;
		ret =false;
	}

	return ret;
}