
#include "consoleManager.h"
#include "CheckSubArgv.h"
/* ��ʾ��ʷ��¼ */
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

    /* ���������¼������  */
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
		std::cout <<  "�����ʽ����" << std::endl;
		ret = false;
	}

	return ret;
}