#include "consoleManager.h"
#include "CheckSubArgv.h"
int consoleManager::m_clearScreen()
{

    /* �����Ļ��ʾ */
    system("cls");

    return SUCC;
}

bool sysCmdClsCheckSubArgv(string&  cmd){
	bool ret = true;
	
	if(cmd != "cls"){
		std::cout <<"�����ʽ����" << std::endl;
		ret = false;
	}

	return ret;

}