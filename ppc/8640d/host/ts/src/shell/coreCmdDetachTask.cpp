#include "coreCmdProcess.h"
#include "CheckSubArgv.h"
using namespace std;

static const basic_string <char>::size_type npos = -1;

/************************外部声明******************************/
/* 与ts建立的连接ID */
extern int tsLinkId;

/* 当前目标机连接的AID */
extern int curTargetAID;

/* 当前连接目标机的名字 */
extern int curTargetName;


#define TA_TDA_AGENT_NO 5
#define QUARY_TDA_NUMBER_CMD "SQ"
#define TDA_DEL_SESSTION_CMD "SD"
#define TDA_SESSTION_START_NO 10
#define TDA_SESSTION_END_NO 29
bool coreCmdDetachTaskCheckSubArgv(string & cmd){
	bool ret = true;
	if(cmd != DETACHTASK_STR ){
		std::cout <<  "命令格式错误" << std::endl;
		ret = false;
	}
	return ret;

}

/*
	获取需要解除绑定的数量
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
	针对1X解绑所有任务TDA_ID分别在10~29之间
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
	
	//检查当前目标机OS与当前命令是否匹配，若不匹配则直接退出本次命令
	if(m_osVersion != OS_1X_VERSION){
		cout << "该命令只支持ACoreOS操作系统"<< endl;
		return FAIL;
	}
	//查询OS是否启动
	if( SUCC != queryTargetAgentOsStauts() )
	{
		return FAIL;
	}
	//向目标机查询调试代理是否启动
	sendCmd = "QA5";
	ret = TClient_putpkt(tsLinkId, curTargetAID, 0, 0, sendCmd.c_str(), (unsigned int)sendCmd.size());
	ret = recvDataFromTs(pBuf,MAX_MSG_SIZE);
	if( ret == STOP_CMD )
	{
		return FAIL;
	}
	else if(ret < 0)
	{
		cout << "接收数据失败!" << endl;
		return FAIL;
	}
	if(pBuf[0] != 'O' || pBuf[1] != 'K'){
		cout << "目标机没有配置任务级调试代理" << endl;
		return FAIL;
	}
	//获取任务绑定数量
	sendCmd = QUARY_TDA_NUMBER_CMD;
	ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TDA_AGENT_NO, 0, sendCmd.c_str(),(unsigned int)sendCmd.size());

	ret = recvDataFromTs(pBuf,MAX_MSG_SIZE);
	
	if( ret == STOP_CMD )
	{
				return FAIL;
	}
	else if(ret < 0)
	{
			cout << "接收数据失败!" << endl;
			return FAIL;
	}
	//从接收命令中获取到当前绑定任务的数量
	getTdaNumber(pBuf,tdaNumber);
	

	//向目标机发出解绑命令
	if(tdaNumber != 0 ){
		//设置绑定任务的起始ID号：10
		unsigned int tdaSesstionStartId = TDA_SESSTION_START_NO;
		
		//循环发送解除指定ID号的任务
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
				cout << "接收数据失败!" << endl;
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
		cout << "没有任务被解绑" << endl;
		ret = FAIL;
	
	}
	if(ret == SUCC){
		cout << "解绑任务成功"<<endl;
	}
	return ret;
}
