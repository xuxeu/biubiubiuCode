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


/* �����������ƻ�ȡ����ID  */
int coreCmdProcess::m_getTaskID(string cmd)
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
	
    /* ȥ��������taskIdFigure */
    cmdtemp.erase( 0, strlen(GET_TASK_ID_STR) );

    /* ���ҵ�һ���ǿո���ַ�,��ɾ���ո�֮ǰ���� */
    noblankpos = cmdtemp.find_first_not_of(BLANK_STR);
    if( (npos  == noblankpos) || (0  == noblankpos) )
    {
        cout << "����Ļ�ȡָ��������ID�����ʽ����" << endl;
        return FAIL;
    }
    cmdtemp.erase(0, noblankpos);

    memset(pbuf, 0, sizeof(pbuf));

    /* ��װ��ѯָ�������������ʽgetid:xxxx */
    len = sprintf(pbuf, "getid:%s", cmdtemp.c_str());

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
        cout << "��������ʧ��!" << endl;
        return FAIL;
    }

    /* ����ָ����ʽ��ӡ������Ϣ */
    if(strncmp(pbuf, "OK,", strlen("OK,")) == 0)
    {
		char* pName =  pbuf + strlen("OK,");
		char* pIdStart = strstr(pbuf,":");
		if(pIdStart != NULL){
			*pIdStart = '\0';
			printf("%s:0x%x\n",pName,atoi(pIdStart+1));
		}else{
			 printf("��ȡ�����ʽ����\n");
			 return FAIL;
		}
		
        return SUCC;
    }
    
    cout << "����"<<cmdtemp<<"������" << endl;
    return FAIL;
}

bool coreCmdGetTaskIdCheckSubArgv(string & cmd){

	bool ret = true;
	vector<string> paraterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = paraterList.size();
	if(length != 1){
		std::cout << "�����ʽ����" << std::endl;
		ret =false;
	}

	return ret;
}