#ifndef _CHECKSUBARGV_H
#define _CHECKSUBARGV_

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif
/*
	过滤多余空格和TAB间隔
*/
void skipInvalidChFormCmd(string & cmd);

/*
	从命令列表中获取二级参数列表
	返回值：vector<string> 二级参数命令列表
*/

vector<string> getSubArgvFromCmd(string cmd,string findStr=" ");

/*
	检查地址空间是否溢出
	返回值：
		true:表示溢出
		false:表示没有溢出
*/
bool checkAddrIsOverflow(unsigned int  baseAddr,unsigned int unitCount,unsigned int unitSize,bool isShowErrorFlags = true);


/*
	获取无符号整形
	参数;uiGetNoStr:无符号整形字符串
		  uiGetNo:待转换的无符号整形
		  zeroIsValid:字符0是否有效
    返回值:
		true:转换成功
		false:转换失败
*/
bool getUINT(string uiGetNoStr,unsigned int& uiGetNo, bool zeroIsValid = false, int *errNum = NULL);


/*
	C解析器模式下检查是否包含中文字符
*/
bool checkChineseCh(string str);
/************************系统命令检查函数前向声明***************************************/

/*ata参数检查*/
bool sysCmdAtaCheckSubArgv(string & cmd);

/*cta参数检查*/
bool sysCmdCtaCheckSubArgv(string & cmd);

/*查询命令参数检查*/
bool  sysCmdListCheckSubArgv(string & cmd);

/*切换到C解析器命令参数检查*/
bool sysCmdCCheckSubArgv(string & cmd);

/*切换到核心操作系统解析器命令参数检查*/
bool sysCmdCoreosCheckSubArgv(string & cmd);

/*退出命令参数检查*/
bool sysCmdQuitCheckSubArgv(string & cmd);

/*帮助命令参数检查*/
bool sysCmdHelpCheckSubArgv(string & cmd);

/*清屏命令参数检查*/
bool sysCmdClsCheckSubArgv(string & cmd);

/*显示历史记录参数检查*/
bool sysCmdHCheckSubArgv(string & cmd);

/*添加符号表命令参数检查*/
bool sysCmdAddSymbolCheckSubArgv(string & cmd);

/*删除符号表命令参数检查*/
bool sysCmdSubSymbolCheckSubArgv(string & cmd);

/**************************coreCmd命令参数检查前向声明************************************************************/

/*内存读命令参数检查*/
bool coreCmdReadMemoryCheckSubArgv(string & cmd);

/*内存写命令参数检查*/
bool coreCmdWriteMemoryCheckSubArgv(string & cmd);

/*内存访问域命令参数检查*/
bool coreCmdSetDomainCheckSubArgv(string & cmd);

/*获取所有任务信息参数检查*/
bool coreCmdListAllTaskCheckSubArgv(string & cmd);

/*获取指定任务信息参数检查*/
bool coreCmdListAppointTaskCheckSubArgv(string & cmd);

/*挂起指定任务的命令参数检查*/
bool coreCmdStopTaskCheckSubArgv(string & cmd);

/*解挂指定任务的命令参数检查*/
bool coreCmdResumeTaskCheckSubArgv(string & cmd);

/*根据任务名称获取指定ID的命令参数检查*/
bool coreCmdGetTaskIdCheckSubArgv(string & cmd);

/*获取分区信息命令的参数检查*/
bool coreCmdShowPdInfoCheckSubArgv(string & cmd);

/*接管调试异常命令的参数检查*/
bool coreCmdSetupDebugExpCheckSubArgv(string & cmd);

/*解绑所有任务命令参数的检查*/
bool coreCmdDetachTaskCheckSubArgv(string & cmd);
/*解绑指定分区命令参数的检查*/
bool coreCmdDetachPartitionTaskCheckSubArgv(string& cmd);

/*解绑指定系统级别命令参数的检查*/
bool coreCmdDetachSymstemCheckSubArgv(string& cmd);

/**************************************C解析器中命令参数检查**************************************************************/
/*反汇编查看命令检查*/
bool expressionCmdDisasCheck(string & cmd);


#endif