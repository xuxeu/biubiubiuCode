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


/* 接收分区异常处理通道 */
#define TA_TASK_PDA_SESSION 5
/* 获取所有会话信息命令*/
#define LIST_ALL_SESSION_CMD "LSS"
/* 回收一个会话命令 */
#define RECYCLE_SESSION_CMD "RCS"

#define ATTACH_KEYWORD ",attach,"
//OK;14,attach,p1,partition,continue;
#define BIND_SESSION_FMT "OK;%s,%s,%s,%s,%s;"
bool coreCmdDetachPartitionTaskCheckSubArgv(string& cmd){
	bool ret = true;
	vector<string> paraterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = paraterList.size();
	if(length != 1){
		std::cout << "命令格式错误" << std::endl;
		ret =false;
	}
	return ret;
}
/*
	回收数据命令格式:
	"OK;channelID,attachStatus,domainName;debugLevel;debugStatus;...$"
	或者
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
		针对2X中查找指定的分区是否存在
		查询命令："LUD"
		回复:OK;__kdomain__,normal,continue;$
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
				cout << "接收数据失败!" << endl;
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
	
	//如果当前目标机的操作系统版本与当前名称不匹配，则直接不做任何处理
	if(m_osVersion != OS_2X_VERSION ){
		cout << "该命令只支持ACoreOS653操作系统"<< endl;
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
		cout << "目标机没有配置分区级调试代理" << endl;
		return FAIL;
	}
	//分区名称的查询,以及当前分区是否绑定操作
	if(findPartitionPos != string::npos){
		
		partitionName = cmd.substr(findPartitionPos+1);
		
		//查询当前分区名称是否存在
		if(!checkPatitionIsExist(partitionName)){
			cout << "分区"<<partitionName<<"不存在"<<endl;
			return FAIL;
		}
	
		memset(pBuf,0,MAX_MSG_SIZE);
		sendCmd = LIST_ALL_SESSION_CMD;
		strcpy_s(pBuf,MAX_MSG_SIZE,LIST_ALL_SESSION_CMD);
		
		//获取所有绑定分区的信息
		ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TASK_PDA_SESSION, 0, sendCmd.c_str(), (unsigned int) sendCmd.size());
		
		bool loopGetMsgFromTs = true;

		
		if(	ret < 0){
			std::cout << "发送数据失败" << std::endl;
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
				cout << "接收数据失败!" << endl;
				return FAIL;
			}
			
			if(strstr(pBuf,"$") != NULL){
				loopGetMsgFromTs = false;
			}
			AllPartitionInfo += pBuf;

		 }//end of while
		
		//获取指定分区名称的通道号
		getChannelIDFromMsg(AllPartitionInfo,partitionName,channelID);
		
		//查找到需要解绑的分区ID号，则直接发送
		if(channelID != ""){
			
			string sendCmd = RECYCLE_SESSION_CMD;
			
			sendCmd +=":";
			
			sendCmd += channelID;
			
			memset(pBuf,0,MAX_MSG_SIZE);
			
			ret = TClient_putpkt(tsLinkId, curTargetAID, TA_TASK_PDA_SESSION, 0, sendCmd.c_str(), (unsigned int)sendCmd.size());
			
			if(	ret < 0){
				std::cout << "发送数据失败" << std::endl;
			}

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
				cout << "解绑分区" <<partitionName<<"成功" << endl;
				ret = SUCC;
			}else{
				cout << "解绑分区" <<partitionName<<"失败" << endl;
				ret = FALSE;
			}
			
		}else{
			std::cout << "分区" <<partitionName<<"没有绑定"<<std::endl;
			ret = FALSE;
		}
	
	}else{
		ret = FALSE;
	}
	return SUCC;
}