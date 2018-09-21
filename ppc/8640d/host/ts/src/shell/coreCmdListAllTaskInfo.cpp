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


/* 获取任务信息 */

int coreCmdProcess::m_listAllTaskInfo(string cmd)
{
    int len = 0;
    int ret = 0;
    char pbuf[MAX_MSG_SIZE] = {'\0'};
    ACoreOs_task_information taskinfo;
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

	
    /* 发送查询任务信息命令包到shell agent */
    ret = sendDataToShellAgent( LIST_ALL_TASK_STR, sizeof(LIST_ALL_TASK_STR) );
    if(ret < 0)
    {
        return FAIL;
    }

    /* 打印标签 */
    printTaskInfoLable();
    
    while(1)
    {
        /* 接收读取结果 */
        memset(pbuf, 0, sizeof(pbuf));
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

        if(0 == strcmp(pbuf, RECV_TASK_INFO_END_STR))
        {
            break;
        }

        /* 解析任务信息 */
        memset(&taskinfo, 0, sizeof(taskinfo));
        ret = analysisTaskInfo(pbuf, &taskinfo);
        if(ret < 0)
        {
            return FAIL;
        }

        /* 打印任务信息 */
        printSingleTaskInfo(taskinfo);

        /* 打印任务状态 */
        state = taskinfo.state;
        printSingleTaskStatus(state);

        /* 按照指定格式打印任务信息 */
        //cout << pbuf << endl;

    }

    printf("\n");

    return SUCC;
}


/*
	命令参数检查
*/
bool coreCmdListAllTaskCheckSubArgv(string & cmd){

	bool ret = true;
	if(cmd != LIST_ALL_TASK_STR ){
		std::cout <<  "命令格式错误" << std::endl;
		ret = false;
	}

	return ret;
}