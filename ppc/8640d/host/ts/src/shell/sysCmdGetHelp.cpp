#include "consoleManager.h"
#include "CheckSubArgv.h"
extern char* coreosHelpTable[];
extern char* CHelpTable[];
int consoleManager::m_getHelp()
{
    int i = 0;

    cout << "\n*****帮助信息*****\n" << endl;
	
    if(COREOS_INTERPRETER == currentInterpreter )
    {
        for(i=0;coreosHelpTable[i];i++)
        {
            cout << coreosHelpTable[i] << endl;
        }
    }
    else
    {
        for(i=0;CHelpTable[i];i++)
        {
            cout << CHelpTable[i] << endl;
        }
    }

    return SUCC;
}


bool sysCmdHelpCheckSubArgv(string&  cmd){

	bool ret = true;

	if(cmd != "help"){
		std::cout << "命令格式错误" << std::endl;
		ret = false;
	}
	
	return ret;

}