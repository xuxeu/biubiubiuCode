#include "consoleManager.h"
#include "CheckSubArgv.h"
/* �л������Ĳ���ϵͳ������ */
int consoleManager::m_switchToCoreOSInterpreter()
{
    currentInterpreter = COREOS_INTERPRETER;
    return SUCC;
}

/* �л���C������ */
int consoleManager::m_switchToExpInterpreter()
{
    currentInterpreter = C_INTERPRETER;
    return SUCC;
}

bool sysCmdCCheckSubArgv(string&  cmd){
	bool ret = true;
	
	if(cmd != "C"){
		
		std::cout <<  "�����ʽ����" << std::endl;
		ret = false;
	
	}

	return ret;

}

bool sysCmdCoreosCheckSubArgv(string&  cmd){
	
	
	bool ret = true;
	
	if(cmd != "coreos"){
		std::cout << "�����ʽ����" << std::endl;
		ret = false;
	}

	return ret;

}