#include "consoleManager.h"
#include "coreCmdProcess.h"
#include "CheckSubArgv.h"
/*
	�޸���ʷ��ȥ��CTA����
	    {CREATE_TARGET_CONNECT,         "cta",         sysCmdCtaCheckSubArgv},
*/
/************************ϵͳ��������б�******************************/

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
/****************************��������б�********************************************/
char* coreosHelpTable[] = 
{
    "����    ����        ˵��",
    "ata     [Ŀ������� | IP��ַ [�˿ں�] | ���ں� [������]] ��ָ����Ŀ�������",
    "list    [��]        ���Բ�ѯ��ǰϵͳ�д��ڵ�Ŀ�������",
    "C       [��]        �л���C������",
    "quit    [��]        �˳�shell",
    "help    [��]        �鿴����",
    "cls     [��]        ����",
    "h       [��]        �鿴��ʷ����",
    "d       [��ʼ��ַ[,��ȡ����[,��ȡ��Ԫ���]]]      ��ȡ�ڴ���Ϣ",
	"m       [��ʼ��ַ[,д��Ԫ���]]     д�ڴ���Ϣ",
	":       [������ | coreOS]  �����ڴ���ʵķ���",
    "i       [��]        ��ȡ����������Ϣ",
    "ti      [��������]  ��ȡָ��������Ϣ",
    "ts      [��������]  ����ָ������",
    "tr      [��������]  ���ָ������",
    "addSymbol  [���ű��ļ�����·��]    ��ӷ��ű��ļ�",
    "subSymbol  [���ű��ļ�����·��]    ɾ�����ű��ļ�",
    "exp        [�쳣������ | all]  �ӹܵ����쳣,exp allΪ�ӹ������쳣",
    "pdShow     [��]  ��ȡ������Ϣ",
    "taskIdFigure     [��������]  ͨ���������ƻ�ȡ����ID",
	"detachPartition  [��������]  ǿ�ƽ��ָ������",
	"detachTask       [��]  ǿ�ƽ����������",
	"detachSystem     [��]  ǿ�ƽ��ϵͳ",
	NULL
};



char* CHelpTable[] = 
{
    "����    ����        ˵��",
    "ata     [Ŀ������� | IP��ַ [�˿ں�] | ���ں� [������]]  ��ָ����Ŀ�������",
    "list    [��]        ���Բ�ѯ��ǰϵͳ�д��ڵ�Ŀ�������",
    "coreos  [��]        �л������Ĳ���ϵͳ������",
    "quit    [��]        �˳�shell",
    "help    [��]        �鿴����",
    "cls     [��]        ����",
    "h       [��]        �鿴��ʷ����",
	":       [������ | coreOS]       �����ڴ���ʵķ���",
    "l       [���ʽ[,����]]         �鿴�����",
    "addSymbol     [���ű��ļ�����·��]        ��ӷ��ű��ļ�",
    "subSymbol     [���ű��ļ�����·��]        ɾ�����ű��ļ�",
	NULL
};



/************************coreCmd��������б�******************************/

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
