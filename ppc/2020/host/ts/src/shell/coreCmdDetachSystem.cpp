#include "coreCmdProcess.h"
#include "CheckSubArgv.h"
using namespace std;
/************************�ⲿ����******************************/
/* ��ts����������ID */
extern int tsLinkId;

/* ��ǰĿ������ӵ�AID */
extern int curTargetAID;

/* ��ǰ����Ŀ��������� */
extern int curTargetName;

#define TARGET_STATUS_QUERY_STRING      "QM"
#define QUERY_TARGET_STATUS_AGENT_2X          11
#define QUERY_TARGET_STATUS_AGENT_1X          1
#define DETACH_SYSTEM_2X_CMD "DTS"
#define DETACH_SYSTEM_1X_CMD ""
bool coreCmdDetachSymstemCheckSubArgv(string & cmd){
	bool ret = true;
	if(cmd != DETACHSYSTEM_STR){
		cout << "�����ʽ����" << endl;
		ret = false;
	}
	return ret;
}
/*
		���1X���ϵͳ����
*/
int coreCmdProcess::detachSystem1x(string cmd){

	int ret = SUCC;
	
	
	return ret;
}
/*
		���2X���ϵͳ����
		OK;task;continue;
*/
int coreCmdProcess::detachSystem2x(string cmd){

	int ret = SUCC;
	char pBuf[MAX_MSG_SIZE] = "\0";
	int targetSaid  =  0;
	string sendCmd;
	if( SUCC != queryTargetAgentOsVersion() )
	{
		return FAIL;
	}
	//��ѯ��ǰ����״̬
	TClient_putpkt(tsLinkId, curTargetAID, 0, 0,"MD", strlen("MD"));  
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
	//��ȡ����ǰ����ģʽ
	if(!strncmp(pBuf,"OK;",3)){
		if(strncmp(pBuf+3, "system", strlen("system"))){
			cout << "Ŀ�������ϵͳ������ģʽ"<< endl;
			ret = FAIL;
		}else{
			ret = SUCC;
		}
	}else{
		ret = FAIL;
	}
	
	//��ǰ����ģʽΪϵͳ��ģʽ��ֱ�ӷ��ͽ��ϵͳ����
	if(ret == SUCC){
		sendCmd = DETACH_SYSTEM_2X_CMD;
		ret = TClient_putpkt(tsLinkId, curTargetAID, 1, 0, sendCmd.c_str(), (unsigned int)sendCmd.size());
		ret = recvDataFromTs(pBuf,MAX_MSG_SIZE);

		if( ret == STOP_CMD )
		{
			ret = FAIL;
		}
		else if(ret < 0)
		{
			cout << "��������ʧ��!" << endl;
			return FAIL;
		}else if(pBuf[0] == 'O' && pBuf[1] =='K'){
			cout << "���ϵͳ�ɹ�" << endl;
			ret = SUCC;
		}else{
			cout << "���ϵͳʧ��"<<endl;
			ret = FAIL;
		}
	}
	return ret;
}
int coreCmdProcess::detachSystem(string cmd){
	
	if( SUCC != queryTargetAgentOsVersion() )
	{
		return FAIL;
	}
	if(m_osVersion == OS_2X_VERSION){
		return detachSystem2x(cmd);
	}else{
		cout << "������ֻ֧��ACoreOS653����ϵͳ"<< endl;
		return FAIL;
	}
}