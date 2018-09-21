#include "consoleManager.h"
#include "CheckSubArgv.h"
/* 当前目标机连接的AID */
extern int tsLinkId ;
extern int curTargetAID;
extern char curTargetName [] ;
/* 创建并激活目标机连接*/
int consoleManager::m_creatAndActiveTargetServer()
{
   
    int aid = -1;
    int ret = 0;
    ServerCfg serCfg;
    COMCONFIG com1Cfg;
        
    memset(&serCfg, 0, sizeof(ServerCfg));
    memset(&com1Cfg, 0, sizeof(COMCONFIG));
    serCfg.pComConfig = &com1Cfg;
    com1Cfg.next = NULL;
    
    /* 提示输入参数*/
    if(inputSerCfg(&serCfg) != SUCC)
    {
        return FAIL;
    }

    //创建serverAgent
    aid = TClient_createServer(tsLinkId, &serCfg);
    if(aid < 0)
    {
        printf("\n创建目标机连接失败!\n");
        return aid;
    }

    //激活serverAgent
    ret = TClient_activeServer(tsLinkId, aid);
    if(ret < 0)
    {
        printf("\n创建目标机连接失败!\n");

        //删除SA
        TClient_deleteServer(tsLinkId, aid);
        return ret;
    }

    printf("\n创建目标机连接成功!\n");

    /* 设置为当前目标机*/
    curTargetAID = aid;
    strcpy(curTargetName ,serCfg.name);

    return aid;
}


bool sysCmdCtaCheckSubArgv(string&  cmd){
	bool ret = true;
	
	if(cmd != "cta"){
		std::cout <<"cta:命令格式错误,当前命令无参数，请重新输入"<<std::endl;
		ret = false;
	}

	return ret;

}