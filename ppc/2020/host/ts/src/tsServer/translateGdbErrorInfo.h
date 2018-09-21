#ifndef _TRANSLATE_GDB_ERRORINFO_H
#define _TRANSLATE_GDB_ERRORINFO_H
#include <vector>
#include <string>
#include <iostream>
#include "sysTypes.h"

using namespace std;

class GdbErrorDefine
{
public:
	GdbErrorDefine(string gdbEErrorInfo = "",string gdbCErrorInfo = "");
	bool isMatch(string info);
	void replaceErrorInfo(T_CHAR* gdbInfo,int maxBufferSize);
	string getGdbEErrorInfo() const;
private:
	//GDBӢ�Ĵ�����Ϣ
	string m_gdbEErrorInfo;
	//GDB���Ĵ�����Ϣ
	string m_gdbCErrorInfo;
	//GDBӢ�Ĵ�����Ϣ�йؼ����屣���б�
	vector<string> m_errorKeywordList;
	//GDBӢ�Ĵ�����Ϣ�г����ؼ���������ķ������Ʊ����б�
	void InitErrorList();
};
//GDBӢ�Ĵ�����Ϣת��Ϊ���Ĵ�����Ϣ
class TranslateGdbErrorInfo
{
public:
	//��ȡ��ǰ����ʵ��
	static TranslateGdbErrorInfo* getInstance();
	//��ǰΨһʵ���ͷź���
	static  void destroy();
	//GDB��Ӣ�Ĵ�����Ϣ��ʼ��
	void InitErrorInfo(GdbErrorDefine* errorAttr);
	~TranslateGdbErrorInfo();
	//GDBӢ�Ĵ�����Ϣת��Ϊ���Ĵ�����Ϣ
	void translateErrorInfo(T_CHAR* gdbInfo,int maxBufferSize);
protected:
	TranslateGdbErrorInfo();
private:
	//��ǰ����ʵ��
	static TranslateGdbErrorInfo* __instance;
	
	//GDB��Ӣ�Ĵ�����Ϣ����
	vector<GdbErrorDefine> m_gdbErrorInfo;
};


#endif