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
	���˶���ո��TAB���
*/
void skipInvalidChFormCmd(string & cmd);

/*
	�������б��л�ȡ���������б�
	����ֵ��vector<string> �������������б�
*/

vector<string> getSubArgvFromCmd(string cmd,string findStr=" ");

/*
	����ַ�ռ��Ƿ����
	����ֵ��
		true:��ʾ���
		false:��ʾû�����
*/
bool checkAddrIsOverflow(unsigned int  baseAddr,unsigned int unitCount,unsigned int unitSize,bool isShowErrorFlags = true);


/*
	��ȡ�޷�������
	����;uiGetNoStr:�޷��������ַ���
		  uiGetNo:��ת�����޷�������
		  zeroIsValid:�ַ�0�Ƿ���Ч
    ����ֵ:
		true:ת���ɹ�
		false:ת��ʧ��
*/
bool getUINT(string uiGetNoStr,unsigned int& uiGetNo, bool zeroIsValid = false, int *errNum = NULL);


/*
	C������ģʽ�¼���Ƿ���������ַ�
*/
bool checkChineseCh(string str);
/************************ϵͳ�����麯��ǰ������***************************************/

/*ata�������*/
bool sysCmdAtaCheckSubArgv(string & cmd);

/*cta�������*/
bool sysCmdCtaCheckSubArgv(string & cmd);

/*��ѯ����������*/
bool  sysCmdListCheckSubArgv(string & cmd);

/*�л���C����������������*/
bool sysCmdCCheckSubArgv(string & cmd);

/*�л������Ĳ���ϵͳ����������������*/
bool sysCmdCoreosCheckSubArgv(string & cmd);

/*�˳�����������*/
bool sysCmdQuitCheckSubArgv(string & cmd);

/*��������������*/
bool sysCmdHelpCheckSubArgv(string & cmd);

/*��������������*/
bool sysCmdClsCheckSubArgv(string & cmd);

/*��ʾ��ʷ��¼�������*/
bool sysCmdHCheckSubArgv(string & cmd);

/*��ӷ��ű�����������*/
bool sysCmdAddSymbolCheckSubArgv(string & cmd);

/*ɾ�����ű�����������*/
bool sysCmdSubSymbolCheckSubArgv(string & cmd);

/**************************coreCmd����������ǰ������************************************************************/

/*�ڴ������������*/
bool coreCmdReadMemoryCheckSubArgv(string & cmd);

/*�ڴ�д����������*/
bool coreCmdWriteMemoryCheckSubArgv(string & cmd);

/*�ڴ����������������*/
bool coreCmdSetDomainCheckSubArgv(string & cmd);

/*��ȡ����������Ϣ�������*/
bool coreCmdListAllTaskCheckSubArgv(string & cmd);

/*��ȡָ��������Ϣ�������*/
bool coreCmdListAppointTaskCheckSubArgv(string & cmd);

/*����ָ�����������������*/
bool coreCmdStopTaskCheckSubArgv(string & cmd);

/*���ָ�����������������*/
bool coreCmdResumeTaskCheckSubArgv(string & cmd);

/*�����������ƻ�ȡָ��ID������������*/
bool coreCmdGetTaskIdCheckSubArgv(string & cmd);

/*��ȡ������Ϣ����Ĳ������*/
bool coreCmdShowPdInfoCheckSubArgv(string & cmd);

/*�ӹܵ����쳣����Ĳ������*/
bool coreCmdSetupDebugExpCheckSubArgv(string & cmd);

/*�������������������ļ��*/
bool coreCmdDetachTaskCheckSubArgv(string & cmd);
/*���ָ��������������ļ��*/
bool coreCmdDetachPartitionTaskCheckSubArgv(string& cmd);

/*���ָ��ϵͳ������������ļ��*/
bool coreCmdDetachSymstemCheckSubArgv(string& cmd);

/**************************************C������������������**************************************************************/
/*�����鿴������*/
bool expressionCmdDisasCheck(string & cmd);


#endif