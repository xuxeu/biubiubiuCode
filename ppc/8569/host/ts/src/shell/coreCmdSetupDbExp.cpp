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
/* ���ýӹܵ��������쳣*/
int coreCmdProcess::m_setupDbExp(string cmd)
{
        int ret = 0, len = 0;
        string cmdtemp = cmd;
        string::size_type noblankpos = 0;
        char pbuf[MAX_MSG_SIZE] = {'\0'};
    char pstr[MAX_MSG_SIZE] = {'\0'};
    char pstr2[MAX_MSG_SIZE] = "exp all";
    const char *p = cmdtemp.c_str();
    int i ;
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
	
    ret = strcmp(pstr2,p);
    if(ret == 0)
    {
        for(i = 0;i<32;i++) 
        {
            memset(pbuf, 0, sizeof(pbuf));
            memset(pstr, 0, sizeof(pstr));
            sprintf(pstr,"%x",i);
            /* ��װ�ӹܵ����쳣�������ʽEXP:xxxx */
            len = sprintf(pbuf, "EXP:%s", pstr);
            if(NULL == pbuf)
            {
                return FAIL;
            }

            /* �������ݰ���Ŀ��� */
            ret = TClient_putpkt(tsLinkId, curTargetAID, TA_SYS_DEBUGER, 0, pbuf, len);
            if( ret < 0 )
            {
                return ret;
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
                cout << "Ŀ����ظ�����ϢΪ����\n" << endl;
                return FAIL;
            }
        }
    }
    else
    {
        memset(pbuf, 0, sizeof(pbuf));
        memset(pstr, 0, sizeof(pstr));
        
        /* ȥ��������exp*/
        cmdtemp.erase( 0, strlen(SETUP_DEBUG_EXP_STR) );

        /* ���ҵ�һ���ǿո���ַ�,��ɾ���ո�֮ǰ���� */
        noblankpos = cmdtemp.find_first_not_of(BLANK_STR);
        if( (npos  == noblankpos) || (0  == noblankpos) )
        {
            cout << "����Ĳ�ѯָ�����������ʽ����" << endl;
            return FAIL;
        }
        cmdtemp.erase(0, noblankpos);

        memset(pbuf, 0, sizeof(pbuf));

        /*�������ַ����е�������ʮ����ת��Ϊʮ������*/
        int ptemp = atoi(p);
        sprintf(pstr,"%x",ptemp);

        /* ��װ�ӹܵ����쳣�������ʽEXP:xxxx */
        len = sprintf(pbuf, "EXP:%s", pstr);

        /* �������ݰ���Ŀ��� */
        ret = TClient_putpkt(tsLinkId, curTargetAID, TA_SYS_DEBUGER, 0, pbuf, len);
        if( ret < 0 )
        {
            return ret;
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
            cout << "Ŀ����ظ�����Ϣ����\n" << endl;
            return FAIL;
        }
    }

    cout<< "���" << endl;
    return SUCC;
}
static bool checkExcpNumer(string excpNoStr){
	bool ret = false;
	unsigned int expNo;
	if(excpNoStr == "all"){
		ret = true;
	}else{
		ret = getUINT(excpNoStr,expNo,true);
		if( ret == true ){
			if( expNo >= 0 && expNo <= 31){
				ret = true;
			}else{
				ret = false;
			}
		}
	}
	if(!ret){
		cout << "�쳣��������Ч" << endl;
	}
	return ret;
}
bool coreCmdSetupDebugExpCheckSubArgv(string& cmd){

	bool ret = true;
	vector<string> paraterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = paraterList.size();
	if(length != 1){
		std::cout << "�����ʽ����" << std::endl;
		ret =false;
	}else{
		ret = checkExcpNumer(paraterList[0]);
	}
	return ret;
}
