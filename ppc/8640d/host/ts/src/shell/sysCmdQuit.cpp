#include "consoleManager.h"
#include "CheckSubArgv.h"
/* ��ǰĿ������ӵ�AID */
extern int tsLinkId ;
extern int curTargetAID;
extern char curTargetName[];
/* �˳�shell�����ر�TS */
int consoleManager::m_quit()
{

    runFlag = false;

    /* �ر���ts������ */
    TClient_close(tsLinkId);

    tsLinkId = -1;

    return SUCC;
}

bool sysCmdQuitCheckSubArgv(string& cmd){
	bool ret = true;
	if(cmd != "quit"){
		std::cout <<  "�����ʽ����" << std::endl;
		ret = false;
	}
	return ret;

}