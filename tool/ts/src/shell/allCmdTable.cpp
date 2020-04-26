#include "consoleManager.h"
#include "coreCmdProcess.h"
#include "CheckSubArgv.h"
/*
	修改历史：去掉CTA命令
	    {CREATE_TARGET_CONNECT,         "cta",         sysCmdCtaCheckSubArgv},
*/
/************************系统命令参数列表******************************/

sysCmdTableStruct sysCmdTable[] = 
{
    {ACTIVE_TARGET_CONNECT,         "ata",         sysCmdAtaCheckSubArgv},
    {LIST_TARGET_CONNECT,           "list",        sysCmdListCheckSubArgv},
    {SWITCH_TO_EXP_LABLE,           "C",           sysCmdCCheckSubArgv},
    {SWITCH_TO_COREOS_LABLE,        "coreos",      sysCmdCoreosCheckSubArgv},
    {QUIT_LABLE,                    "quit",        sysCmdQuitCheckSubArgv},
    {HELP_LABLE,                    "help",        sysCmdHelpCheckSubArgv},
    {CLEAR_SCREEN_LABLE,            "cls",         sysCmdClsCheckSubArgv},
    {PRINT_HISTROY,                 "h",           sysCmdHCheckSubArgv},
    {ADD_SYMBOL,                    "addSymbol",   sysCmdAddSymbolCheckSubArgv},
    {SUB_SYMBOL,                    "subSymbol",   sysCmdSubSymbolCheckSubArgv},
	{ERROR_ENUM,NULL,NULL}
};
/****************************命令帮助列表********************************************/
char* coreosHelpTable[] = 
{
    "命令    参数        说明",
    "ata     [目标机名称 | IP地址 [端口号] | 串口号 [波特率]] 绑定指定的目标机连接",
    "list    [无]        可以查询当前系统中存在的目标机连接",
    "C       [无]        切换到C解析器",
    "quit    [无]        退出shell",
    "help    [无]        查看帮助",
    "cls     [无]        清屏",
    "h       [无]        查看历史操作",
    "d       [起始地址[,读取长度[,读取单元宽度]]]      读取内存信息",
	"m       [起始地址[,写单元宽度]]     写内存信息",
	":       [分区名 | coreOS]  设置内存访问的分区",
    "i       [无]        获取所有任务信息",
    "ti      [任务名称]  获取指定任务信息",
    "ts      [任务名称]  挂起指定任务",
    "tr      [任务名称]  解挂指定任务",
    "addSymbol  [符号表文件绝对路径]    添加符号表文件",
    "subSymbol  [符号表文件绝对路径]    删除符号表文件",
    "exp        [异常向量号 | all]  接管调试异常,exp all为接管所有异常",
    "pdShow     [无]  获取分区信息",
    "taskIdFigure     [任务名称]  通过任务名称获取任务ID",
	"detachPartition  [分区名称]  强制解绑指定分区",
	"detachTask       [无]  强制解绑所有任务",
	"detachSystem     [无]  强制解绑系统",
	NULL
};



char* CHelpTable[] = 
{
    "命令    参数        说明",
    "ata     [目标机名称 | IP地址 [端口号] | 串口号 [波特率]]  绑定指定的目标机连接",
    "list    [无]        可以查询当前系统中存在的目标机连接",
    "coreos  [无]        切换到核心操作系统解析器",
    "quit    [无]        退出shell",
    "help    [无]        查看帮助",
    "cls     [无]        清屏",
    "h       [无]        查看历史操作",
	":       [分区名 | coreOS]       设置内存访问的分区",
    "l       [表达式[,行数]]         查看反汇编",
    "addSymbol     [符号表文件绝对路径]        添加符号表文件",
    "subSymbol     [符号表文件绝对路径]        删除符号表文件",
	NULL
};



/************************coreCmd命令参数列表******************************/

coreCmdTableStruct coreCmdTable[] = 
{
    {READ_MEMORY,             READ_MEMORY_STR,coreCmdReadMemoryCheckSubArgv},
    {WRITE_MEMORY,           WRITE_MEMORY_STR,coreCmdWriteMemoryCheckSubArgv},
    {SET_USING_DOMAIN,     SET_USING_DOMAIN_STR,coreCmdSetDomainCheckSubArgv},
    {LIST_ALL_TASK,            LIST_ALL_TASK_STR,coreCmdListAllTaskCheckSubArgv},
    {LIST_APPOINT_TASK,     LIST_APPOINT_TASK_STR,coreCmdListAppointTaskCheckSubArgv},
    {STOP_TASK,                  STOP_TASK_STR,coreCmdStopTaskCheckSubArgv},
    {RESUME_TASK,              RESUME_TASK_STR,coreCmdResumeTaskCheckSubArgv},
    {GET_TASK_ID,               GET_TASK_ID_STR,coreCmdGetTaskIdCheckSubArgv},
    {SHOW_PD_INFO,           SHOW_PD_INFO_STR,coreCmdShowPdInfoCheckSubArgv},
	{SETUP_DEBUG_EXP,	SETUP_DEBUG_EXP_STR,coreCmdSetupDebugExpCheckSubArgv},
	{DETACHTASK,DETACHTASK_STR,coreCmdDetachTaskCheckSubArgv},
	{DETACHPARTITION,DETACHPARTITION_STR,coreCmdDetachPartitionTaskCheckSubArgv},
	{DETACHSYSTEM,DETACHSYSTEM_STR,coreCmdDetachSymstemCheckSubArgv},
	{END_FLAGS_ENUM,NULL,NULL}
};
