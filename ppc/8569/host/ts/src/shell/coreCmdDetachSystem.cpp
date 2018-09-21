#include "coreCmdProcess.h"
#include "CheckSubArgv.h"
using namespace std;
/************************外部声明******************************/
/* 与ts建立的连接ID */
extern int tsLinkId;

/* 当前目标机连接的AID */
extern int curTargetAID;

/* 当前连接目标机的名字 */
extern int curTargetName;

#define TARGET_STATUS_QUERY_STRING      "QM"
#define QUERY_TARGET_STATUS_AGENT_2X          11
#define QUERY_TARGET_STATUS_AGENT_1X          1
#define DETACH_SYSTEM_2X_CMD "DTS"
#define DETACH_SYSTEM_1X_CMD ""
bool coreCmdDetachSymstemCheckSubArgv(string & cmd){
	bool ret = true;
	if(cmd != DETACHSYSTEM_STR){
		cout << "命令格式错误" << endl;
		ret = false;
	}
	return ret;
}
/*
		针对1X解绑系统调试
*/
int coreCmdProcess::detachSystem1x(string cmd){

	int ret = SUCC;
	
	
	return ret;
}
/*
		针对2X解绑系统调试
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
	//查询当前调试状态
	TClient_putpkt(tsLinkId, curTargetAID, 0, 0,"MD", strlen("MD"));  
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
	//获取到当前调试模式
	if(!strncmp(pBuf,"OK;",3)){
		if(strncmp(pBuf+3, "system", strlen("system"))){
			cout << "目标机不是系统级调试模式"<< endl;
			ret = FAIL;
		}else{
			ret = SUCC;
		}
	}else{
		ret = FAIL;
	}
	
	//当前调试模式为系统级模式则直接发送解绑系统调试
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
			cout << "接收数据失败!" << endl;
			return FAIL;
		}else if(pBuf[0] == 'O' && pBuf[1] =='K'){
			cout << "解绑系统成功" << endl;
			ret = SUCC;
		}else{
			cout << "解绑系统失败"<<endl;
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
		cout << "该命令只支持ACoreOS653操作系统"<< endl;
		return FAIL;
	}
}