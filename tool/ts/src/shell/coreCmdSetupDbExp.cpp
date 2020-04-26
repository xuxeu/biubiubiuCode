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
/* 设置接管单个调试异常*/
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
	
    ret = strcmp(pstr2,p);
    if(ret == 0)
    {
        for(i = 0;i<32;i++) 
        {
            memset(pbuf, 0, sizeof(pbuf));
            memset(pstr, 0, sizeof(pstr));
            sprintf(pstr,"%x",i);
            /* 组装接管调试异常命令包格式EXP:xxxx */
            len = sprintf(pbuf, "EXP:%s", pstr);
            if(NULL == pbuf)
            {
                return FAIL;
            }

            /* 发送数据包到目标机 */
            ret = TClient_putpkt(tsLinkId, curTargetAID, TA_SYS_DEBUGER, 0, pbuf, len);
            if( ret < 0 )
            {
                return ret;
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

            /* 回复的信息为错误代码,则直接打印接收信息 */
            if('E' == pbuf[0])
            {
                cout << "目标机回复的信息为错误\n" << endl;
                return FAIL;
            }
        }
    }
    else
    {
        memset(pbuf, 0, sizeof(pbuf));
        memset(pstr, 0, sizeof(pstr));
        
        /* 去掉命令字exp*/
        cmdtemp.erase( 0, strlen(SETUP_DEBUG_EXP_STR) );

        /* 查找第一个非空格的字符,并删除空格之前内容 */
        noblankpos = cmdtemp.find_first_not_of(BLANK_STR);
        if( (npos  == noblankpos) || (0  == noblankpos) )
        {
            cout << "输入的查询指定任务命令格式错误" << endl;
            return FAIL;
        }
        cmdtemp.erase(0, noblankpos);

        memset(pbuf, 0, sizeof(pbuf));

        /*将命令字符串中的数字由十进制转换为十六进制*/
        int ptemp = atoi(p);
        sprintf(pstr,"%x",ptemp);

        /* 组装接管调试异常命令包格式EXP:xxxx */
        len = sprintf(pbuf, "EXP:%s", pstr);

        /* 发送数据包到目标机 */
        ret = TClient_putpkt(tsLinkId, curTargetAID, TA_SYS_DEBUGER, 0, pbuf, len);
        if( ret < 0 )
        {
            return ret;
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

        /* 回复的信息为错误代码,则直接打印接收信息 */
        if('E' == pbuf[0])
        {
            cout << "目标机回复的信息错误\n" << endl;
            return FAIL;
        }
    }

    cout<< "完成" << endl;
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
		cout << "异常向量号无效" << endl;
	}
	return ret;
}
bool coreCmdSetupDebugExpCheckSubArgv(string& cmd){

	bool ret = true;
	vector<string> paraterList = getSubArgvFromCmd(cmd);
	vector<string>::size_type length = paraterList.size();
	if(length != 1){
		std::cout << "命令格式错误" << std::endl;
		ret =false;
	}else{
		ret = checkExcpNumer(paraterList[0]);
	}
	return ret;
}
