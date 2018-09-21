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
/*��ȡ������Ϣ*/
int coreCmdProcess::m_showPdInfo(string cmd)
{
    int len = 0;
    int ret = 0;
    char pbuf[MAX_MSG_SIZE] = {'\0'};
    int state = 0;

    UINT8 name[IMG_NAME_LENGTH] ; 
    int type; 
    UINT32 startAddr; 
    UINT32 size; 
    int lowPriority; 
    int highPriority;
    int partitionid;
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
	
    memset(name, 0, IMG_NAME_LENGTH);

    /* ���Ͳ�ѯ������Ϣ�������shell agent */
    ret = sendDataToShellAgent( SHOW_PD_INFO_STR, sizeof(SHOW_PD_INFO_STR) );
    if(ret < 0)
    {
        return FAIL;
    }

    /* ��ӡ��ǩ */

	printPdInfoLable();

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
        ret = analysisParttitionInfo(pbuf, &partitionid, name, &type, &startAddr, &size, &lowPriority, &highPriority);
        if(ret < 0)
        {
            return FAIL;
        }
        
        /* ��ӡ������Ϣ */
        printSingleParttitionInfo(name, partitionid, type, startAddr, size, lowPriority, highPriority);
    }

    printf("\n");

    return SUCC;
}

bool coreCmdShowPdInfoCheckSubArgv(string & cmd){
	bool ret = true;
	
	if(cmd != SHOW_PD_INFO_STR ){
		std::cout <<  "�����ʽ����" << std::endl;
		ret = false;
	}

	return ret;


}