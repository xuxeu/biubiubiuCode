#include "consoleManager.h"
#include "CheckSubArgv.h"
int consoleManager::m_clearScreen()
{

    /* 清除屏幕显示 */
    system("cls");

    return SUCC;
}

bool sysCmdClsCheckSubArgv(string&  cmd){
	bool ret = true;
	
	if(cmd != "cls"){
		std::cout <<"命令格式错误" << std::endl;
		ret = false;
	}

	return ret;

}