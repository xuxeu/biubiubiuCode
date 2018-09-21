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


/* ���շ����쳣����ͨ�� */
#define TA_TASK_PDA_SESSION 5
/* ��ȡ���лỰ��Ϣ����*/
#define LIST_ALL_SESSION_CMD "LSS"
/* ����һ���Ự���� */
#define RECYCLE_SESSION_CMD "RCS"

#define ATTACH_KEYWORD ",attach,"
//OK;14,attach,p1,partition,continue;
#define BIND_SESSION_FMT "OK;%s,%s,%s,%s,%s;"
bool coreCmdDetachPartitionTaskCheckSubArgv(string& cmd){
	bool ret = true;
	vector<string> paraterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = paraterList.size();
	if(length != 1){
		std::cout << "�����ʽ����" << std::endl;
		ret =false;
	}
	return ret;
}
/*
	�������������ʽ:
	"OK;channelID,attachStatus,domainName;debugLevel;debugStatus;...$"
	����
	"OK;channelID,attachStatus;"
	
*/
static void getChannelIDFromMsg(string msg,string partitionName,string&  channelID){

	if( msg.size() > 0 && msg.find("$") != string::npos){
	
		string::size_type prePos = 0,nextPos = 0;
		
		bool endLoop = true;

		while(endLoop){
			
			string subStr = "";
			nextPos = msg.find_first_of(";",prePos);
			if( nextPos != string::npos){
				subStr = msg.substr(prePos,nextPos-prePos+1);
			}else{
				subStr = msg.substr(prePos);
				endLoop = false;
			}

			if( subStr != "OK;"){
				string::size_type findAttachPos = subStr.find(ATTACH_KEYWORD);
				string::size_type findNamePos ;
				string name ;
				if(findAttachPos != string::npos){
					findNamePos = subStr.find_first_of(",",findAttachPos+strlen(ATTACH_KEYWORD));
					if(findNamePos != string::npos){
						name = subStr.substr(findAttachPos+strlen(ATTACH_KEYWORD),findNamePos-(findAttachPos+strlen(ATTACH_KEYWORD)));
						if( partitionName == name){
							channelID = subStr.substr(0,findAttachPos);
						}
					}
				}
			
			}

			prePos = nextPos+1;
		
		}
	
	}
}



/*
		���2X�в���ָ���ķ����Ƿ����
		��ѯ���"LUD"
		�ظ�:OK;__kdomain__,normal,continue;$
*/
bool coreCmdProcess::checkPatitionIsExist(string patitionName){
	int ret = SUCC;
	bool bRet = true,loopGetMsgFromTs = true;
	
	char pBuf[MAX_MSG_SIZE] = "\0";
	string sendCmd;
	sendCmd = "LUD";
	string AllPartitionInfo;
	string matchPatition = "OK;";
	matchPatition += patitionName;
	matchPatition += ",";
	ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TASK_PDA_SESSION, 0, sendCmd.c_str(), (unsigned int)sendCmd.size());
	while(loopGetMsgFromTs){
		 
			memset(pBuf,0,MAX_MSG_SIZE);
			ret = recvDataFromTs(pBuf,MAX_MSG_SIZE);
			if( ret == STOP_CMD )
			{
				return false;
			}
			else if(ret < 0)
			{
				cout << "��������ʧ��!" << endl;
				return false;
			}
			
			if(strstr(pBuf,"$") != NULL){
				loopGetMsgFromTs = false;
			}
			AllPartitionInfo += pBuf;
		 }//end of while
	
	
	if(AllPartitionInfo.find(matchPatition) != string::npos){
		bRet = true;
	}else{
		bRet = false;
	}
	return bRet;

}
int coreCmdProcess::detachPartition(string cmd){
	
	int ret = SUCC;
	string partitionName ;
	string::size_type findPartitionPos = cmd.find(" ");

	string channelID = "";
	string AllPartitionInfo;
	char pBuf[MAX_MSG_SIZE] = "\0";
	string sendCmd;
	
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
		cout << "Ŀ���û�����÷��������Դ���" << endl;
		return FAIL;
	}
	//�������ƵĲ�ѯ,�Լ���ǰ�����Ƿ�󶨲���
	if(findPartitionPos != string::npos){
		
		partitionName = cmd.substr(findPartitionPos+1);
		
		//��ѯ��ǰ���������Ƿ����
		if(!checkPatitionIsExist(partitionName)){
			cout << "����"<<partitionName<<"������"<<endl;
			return FAIL;
		}
	
		memset(pBuf,0,MAX_MSG_SIZE);
		sendCmd = LIST_ALL_SESSION_CMD;
		strcpy_s(pBuf,MAX_MSG_SIZE,LIST_ALL_SESSION_CMD);
		
		//��ȡ���а󶨷�������Ϣ
		ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TASK_PDA_SESSION, 0, sendCmd.c_str(), (unsigned int) sendCmd.size());
		
		bool loopGetMsgFromTs = true;

		
		if(	ret < 0){
			std::cout << "��������ʧ��" << std::endl;
		}


		while(loopGetMsgFromTs){
		 
			memset(pBuf,0,MAX_MSG_SIZE);
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
			
			if(strstr(pBuf,"$") != NULL){
				loopGetMsgFromTs = false;
			}
			AllPartitionInfo += pBuf;

		 }//end of while
		
		//��ȡָ���������Ƶ�ͨ����
		getChannelIDFromMsg(AllPartitionInfo,partitionName,channelID);
		
		//���ҵ���Ҫ���ķ���ID�ţ���ֱ�ӷ���
		if(channelID != ""){
			
			string sendCmd = RECYCLE_SESSION_CMD;
			
			sendCmd +=":";
			
			sendCmd += channelID;
			
			memset(pBuf,0,MAX_MSG_SIZE);
			
			ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TASK_PDA_SESSION, 0, sendCmd.c_str(), (unsigned int)sendCmd.size());
			
			if(	ret < 0){
				std::cout << "��������ʧ��" << std::endl;
			}

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
				cout << "������" <<partitionName<<"�ɹ�" << endl;
				ret = SUCC;
			}else{
				cout << "������" <<partitionName<<"ʧ��" << endl;
				ret = FALSE;
			}
			
		}else{
			std::cout << "����" <<partitionName<<"û�а�"<<std::endl;
			ret = FALSE;
		}
	
	}else{
		ret = FALSE;
	}
	return SUCC;
}