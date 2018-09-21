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
/*获取分区信息*/
int coreCmdProcess::m_showPdInfo(string cmd)
{
    int len = 0;
    int ret = 0;
    char pbuf[MAX_MSG_SIZE] = {'\0'};
    int state = 0;

    UINT8 name[IMG_NAME_LENGTH] ; 
    int type; 
    UINT32 startAddr; 
    UINT32 size; 
    int lowPriority; 
    int highPriority;
    int partitionid;
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
	
    memset(name, 0, IMG_NAME_LENGTH);

    /* 发送查询任务信息命令包到shell agent */
    ret = sendDataToShellAgent( SHOW_PD_INFO_STR, sizeof(SHOW_PD_INFO_STR) );
    if(ret < 0)
    {
        return FAIL;
    }

    /* 打印标签 */

	printPdInfoLable();

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
        
        /* 解析分区信息 */
        ret = analysisParttitionInfo(pbuf, &partitionid, name, &type, &startAddr, &size, &lowPriority, &highPriority);
        if(ret < 0)
        {
            return FAIL;
        }
        
        /* 打印分区信息 */
        printSingleParttitionInfo(name, partitionid, type, startAddr, size, lowPriority, highPriority);
    }

    printf("\n");

    return SUCC;
}

bool coreCmdShowPdInfoCheckSubArgv(string & cmd){
	bool ret = true;
	
	if(cmd != SHOW_PD_INFO_STR ){
		std::cout <<  "命令格式错误" << std::endl;
		ret = false;
	}

	return ret;


}