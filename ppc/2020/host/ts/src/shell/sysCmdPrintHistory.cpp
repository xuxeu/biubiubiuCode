
#include "consoleManager.h"
#include "CheckSubArgv.h"
/* 显示历史记录 */
int consoleManager::m_printHistory(string cmd)
{
    string cmdtmp;
	deque<string>::size_type i = 0;
    int currcmdindex = 0;

    busyCmdListMtx.lock();

    if(m_totalExecuteCmdNum > m_storeCmdHistoryNum)
    {
        currcmdindex = m_totalExecuteCmdNum - m_storeCmdHistoryNum + 1;
    }
    else
    {
        currcmdindex = 1;
    }

    /* 逆序输出记录的命令  */
    for(i = cmdDeque.size(); i > 0; i--)
    {
        cmdtmp = cmdDeque.at(i-1);

        printf("%4d    %s\n", currcmdindex,  cmdtmp.c_str());
        
        currcmdindex++;
    }

    busyCmdListMtx.unlock();

    return SUCC;
}

bool sysCmdHCheckSubArgv(string&  cmd){

	bool ret = true;
	if(cmd != "h"){
		std::cout <<  "命令格式错误" << std::endl;
		ret = false;
	}

	return ret;
}