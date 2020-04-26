#include "consoleManager.h"
#include "CheckSubArgv.h"
/* 当前目标机连接的AID */
extern int tsLinkId ;
extern int curTargetAID;
/* 查询当前TS中的目标机连接信息 */
int consoleManager::m_listTargetInfo()
{
    int saCount = 0;
    int aidArray[256];
    ServerCfg serCfg;
    COMCONFIG comCfg1;
    COMCONFIG comCfg2;
    int curSaNmu = 0;
    int ret = 0;
    char taState[10];

    memset(aidArray, 0, sizeof(aidArray));

    //获取所有SA的AID
    saCount = getAllServerAid(aidArray, sizeof(aidArray));   
	if(saCount == -1){
		printf("查询所有目标机连接失败\n");
		return FAIL;
	}

    //获取每个SA的配置
    while( (curSaNmu < saCount) && (curSaNmu < sizeof(aidArray) - 1) && (0 != aidArray[curSaNmu]))
    {   
        memset(&serCfg, 0 ,sizeof(ServerCfg));
        memset(&comCfg1, 0 ,sizeof(COMCONFIG));
        memset(&comCfg2, 0 ,sizeof(COMCONFIG));
        serCfg.pComConfig = &comCfg1;
        comCfg1.next = &comCfg2;
        serCfg.aid = aidArray[curSaNmu++];

        //获取SA配置
        ret = TClient_queryServer(tsLinkId, &serCfg) ;
        if(TS_SUCCESS != ret)
        {
            printf("获取目标机配置失败, aid:%d\n", aidArray[curSaNmu - 1]);
            continue;
        }

        //判断连接方式
        if(DLL == serCfg.pComConfig->way)
        {
            continue;
        }
        else if(serCfg.extInfo[0]=='1')
        {
            continue;
        }

        //获取状态
        memset(taState, 0, sizeof(taState));
        switch(serCfg.state)
        {
        case SASTATE_INACTIVE:
            strncpy_s(taState, sizeof(taState), "Inactive", _TRUNCATE);
            break;

        case SASTATE_ACTIVE:
            strncpy_s(taState, sizeof(taState), "Active", _TRUNCATE);
            break;

        case SASTATE_DELETED:
            strncpy_s(taState, sizeof(taState), "Delete", _TRUNCATE);
            break;

        default:
            strncpy_s(taState, sizeof(taState), "Unknow", _TRUNCATE);
        }

        //打印目标机信息
        printf("%2d:%15s %2d %8s ",  
                    curSaNmu,
                    serCfg.name,
                    serCfg.aid,
                    taState);
        //第1通道配置
        switch(serCfg.pComConfig->way)
        {
        case UDP:
            printf("UDP %16s %6d %4d %4d",
                        serCfg.pComConfig->NET.ip,
                        serCfg.pComConfig->NET.port,
                        serCfg.pComConfig->NET.timeout,
                        serCfg.pComConfig->NET.retry);
            break;

        case UART:
            printf("UART%16s %6d %4d %4d",
                        serCfg.pComConfig->UART.channel,
                        serCfg.pComConfig->UART.baud,
                        serCfg.pComConfig->UART.timeout,
                        serCfg.pComConfig->UART.retry);
            break;

        case USB:
            printf("USB  %s  ", serCfg.pComConfig->USB.GUID);
            break;

        case DLL:
            break;
    
        default:
            printf("第1通道:未知通道类型");
        }

        
        /* 当前目标机连接 */
        if(curTargetAID == serCfg.aid)
        {
            printf(" *Using");
        }

        printf("\n");

    }

    if(0 == curSaNmu)
    {
        printf("当前没有创建任何目标机\n");
    }
    
    return SUCC;
    
}


bool sysCmdListCheckSubArgv(string& cmd){

	bool ret = true;
	if(cmd != "list"){
		
		std::cout << "命令格式错误" << std::endl;
		
		ret = false;
	
	}
	return ret;

}