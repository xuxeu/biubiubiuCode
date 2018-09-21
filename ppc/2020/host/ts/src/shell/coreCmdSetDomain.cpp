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


/* 设置内存访问的分区 */
int coreCmdProcess::m_setDomain(string cmd)
{
int ret = 0, len = 0;
    string cmdtemp = cmd;
    string::size_type noblankpos = 0;
    char pbuf[MAX_MSG_SIZE] = {'\0'};
    char* pstr = NULL;
    int state = 0;

	//查询OS版本
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
	//查询代理是否启动
	if( SUCC != queryTargetShellAgent() )
	{
		return FAIL;
	}
	
    /* 去掉命令字setD*/
    cmdtemp.erase( 0, strlen(SET_USING_DOMAIN_STR) );

    /* 查找第一个非空格的字符,并删除空格之前内容 */
    noblankpos = cmdtemp.find_first_not_of(BLANK_STR);
	if( (string::npos  == noblankpos) || (0  == noblankpos) )
    {
        cout << "输入的查询指定任务命令格式错误" << endl;
        return FAIL;
    }
    cmdtemp.erase(0, noblankpos);

    memset(pbuf, 0, sizeof(pbuf));

    /* 组装查询指定任务命令包格式ti:xxxx */
    if(strcmp(cmdtemp.c_str() ,"coreOS") == 0)
    {
        strcpy(pbuf,"setD:__kdomain__");
        len = strlen(pbuf);
    }
    else 
    {
        len = sprintf(pbuf, "setD:%s", cmdtemp.c_str());
    }
	
    /* 发送查询指定任务信息命令包到shell agent */
    ret = sendDataToShellAgent(pbuf, len);
    if(ret < 0)
    {
        return FAIL;
    }

    memset(pbuf, 0, sizeof(pbuf));

    /* 接收读取结果 */
    ret = recvDataFromTs( pbuf, sizeof(pbuf) );
    if( ret == STOP_CMD )
    {
        return FAIL;
    }
    else if(ret < 0)
    {
        cout << "接收数据失败" << endl;
        return FAIL;
    }

    /* 回复的信息为错误代码,则直接打印操作失败 */
    if('E' == pbuf[0])
    {
        cout << "分区" <<cmdtemp<<"不存在"<<endl;
        return FAIL;
    }

    cout<< "完成" << endl;
    return SUCC;
}


bool coreCmdSetDomainCheckSubArgv(string & cmd){

	bool ret = true;
	vector<string> paraterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = paraterList.size();
	if(length != 1){
		std::cout << "命令格式错误" << std::endl;
		ret =false;
	}

	return ret;
}