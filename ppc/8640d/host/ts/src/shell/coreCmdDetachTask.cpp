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


#define TA_TDA_AGENT_NO 5
#define QUARY_TDA_NUMBER_CMD "SQ"
#define TDA_DEL_SESSTION_CMD "SD"
#define TDA_SESSTION_START_NO 10
#define TDA_SESSTION_END_NO 29
bool coreCmdDetachTaskCheckSubArgv(string & cmd){
	bool ret = true;
	if(cmd != DETACHTASK_STR ){
		std::cout <<  "�����ʽ����" << std::endl;
		ret = false;
	}
	return ret;

}

/*
	��ȡ��Ҫ����󶨵�����
	msg:OK;1;
*/
static void getTdaNumber(string msg,unsigned int& tdaNumber){
	
	if( msg.size() > 0 ){
		string::size_type okPos = msg.find_first_of("OK;");
		string::size_type lastPos = msg.find_last_of(";");
	
		if(okPos == 0 && (lastPos+1) == msg.size()){
			string strNo = msg.substr(strlen("OK;"),lastPos-strlen("OK;"));
			if(strNo.size() > 0){
				if(!getUINT(strNo,tdaNumber,true)){
					tdaNumber = 0;
				}
			}
		}
	}
}

/*
	���1X�����������TDA_ID�ֱ���10~29֮��
*/

int coreCmdProcess::detachTask(){
	
	int ret = SUCC;
	string sendCmd;
	char pBuf[MAX_MSG_SIZE] = "\0";
	unsigned int tdaNumber = 0;
	
	if( SUCC != queryTargetAgentOsVersion() )
	{
		return FAIL;
	}
	
	//��鵱ǰĿ���OS�뵱ǰ�����Ƿ�ƥ�䣬����ƥ����ֱ���˳���������
	if(m_osVersion != OS_1X_VERSION){
		cout << "������ֻ֧��ACoreOS����ϵͳ"<< endl;
		return FAIL;
	}
	//��ѯOS�Ƿ�����
	if( SUCC != queryTargetAgentOsStauts() )
	{
		return FAIL;
	}
	//��Ŀ�����ѯ���Դ����Ƿ�����
	sendCmd = "QA5";
	ret = TClient_putpkt(tsLinkId, curTargetAID, 0, 0, sendCmd.c_str(), (unsigned int)sendCmd.size());
	ret = recvDataFromTs(pBuf,MAX_MSG_SIZE);
	if( ret == STOP_CMD )
	{
		return FAIL;
	}
	else if(ret < 0)
	{
		cout << "��������ʧ��!" << endl;
		return FAIL;
	}
	if(pBuf[0] != 'O' || pBuf[1] != 'K'){
		cout << "Ŀ���û���������񼶵��Դ���" << endl;
		return FAIL;
	}
	//��ȡ���������
	sendCmd = QUARY_TDA_NUMBER_CMD;
	ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TDA_AGENT_NO, 0, sendCmd.c_str(),(unsigned int)sendCmd.size());

	ret = recvDataFromTs(pBuf,MAX_MSG_SIZE);
	
	if( ret == STOP_CMD )
	{
				return FAIL;
	}
	else if(ret < 0)
	{
			cout << "��������ʧ��!" << endl;
			return FAIL;
	}
	//�ӽ��������л�ȡ����ǰ�����������
	getTdaNumber(pBuf,tdaNumber);
	

	//��Ŀ��������������
	if(tdaNumber != 0 ){
		//���ð��������ʼID�ţ�10
		unsigned int tdaSesstionStartId = TDA_SESSTION_START_NO;
		
		//ѭ�����ͽ��ָ��ID�ŵ�����
		while(tdaNumber){
			
			memset(pBuf,0,MAX_MSG_SIZE);
			sprintf(pBuf,"%s:%x",TDA_DEL_SESSTION_CMD,tdaSesstionStartId);
			ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TDA_AGENT_NO, 0, pBuf,strlen(pBuf));
			ret = recvDataFromTs(pBuf,MAX_MSG_SIZE);
	
			if( ret == STOP_CMD )
			{
				return FAIL;
			}

			else if(ret < 0)
			{
				cout << "��������ʧ��!" << endl;
				return FAIL;
			}

			if(pBuf[0] == 'O' && pBuf[1] == 'K'){
				ret = SUCC;
			}else{
				ret = FAIL;
			}
			tdaNumber--;
			tdaSesstionStartId++;
			if(tdaSesstionStartId > TDA_SESSTION_END_NO){
				break;
			}
		}
	
	}else{
		cout << "û�����񱻽��" << endl;
		ret = FAIL;
	
	}
	if(ret == SUCC){
		cout << "�������ɹ�"<<endl;
	}
	return ret;
}
