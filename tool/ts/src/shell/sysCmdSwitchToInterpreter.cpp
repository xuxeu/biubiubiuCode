#include "consoleManager.h"
#include "CheckSubArgv.h"
/* 切换到核心操作系统解析器 */
int consoleManager::m_switchToCoreOSInterpreter()
{
    currentInterpreter = COREOS_INTERPRETER;
    return SUCC;
}

/* 切换到C解析器 */
int consoleManager::m_switchToExpInterpreter()
{
    currentInterpreter = C_INTERPRETER;
    return SUCC;
}

bool sysCmdCCheckSubArgv(string&  cmd){
	bool ret = true;
	
	if(cmd != "C"){
		
		std::cout <<  "命令格式错误" << std::endl;
		ret = false;
	
	}

	return ret;

}

bool sysCmdCoreosCheckSubArgv(string&  cmd){
	
	
	bool ret = true;
	
	if(cmd != "coreos"){
		std::cout << "命令格式错误" << std::endl;
		ret = false;
	}

	return ret;

}