#include "consoleManager.h"
#include "CheckSubArgv.h"
/* 当前目标机连接的AID */
extern int tsLinkId ;
extern int curTargetAID;
extern char curTargetName[];
/* 退出shell，不关闭TS */
int consoleManager::m_quit()
{

    runFlag = false;

    /* 关闭与ts的连接 */
    TClient_close(tsLinkId);

    tsLinkId = -1;

    return SUCC;
}

bool sysCmdQuitCheckSubArgv(string& cmd){
	bool ret = true;
	if(cmd != "quit"){
		std::cout <<  "命令格式错误" << std::endl;
		ret = false;
	}
	return ret;

}