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
	//GDB英文错误信息
	string m_gdbEErrorInfo;
	//GDB中文错误信息
	string m_gdbCErrorInfo;
	//GDB英文错误信息中关键字体保存列表
	vector<string> m_errorKeywordList;
	//GDB英文错误信息中除开关键字体以外的符号名称保留列表
	void InitErrorList();
};
//GDB英文错误信息转换为中文错误信息
class TranslateGdbErrorInfo
{
public:
	//获取当前对象实例
	static TranslateGdbErrorInfo* getInstance();
	//当前唯一实例释放函数
	static  void destroy();
	//GDB中英文错误信息初始话
	void InitErrorInfo(GdbErrorDefine* errorAttr);
	~TranslateGdbErrorInfo();
	//GDB英文错误信息转换为中文错误信息
	void translateErrorInfo(T_CHAR* gdbInfo,int maxBufferSize);
protected:
	TranslateGdbErrorInfo();
private:
	//当前对象实例
	static TranslateGdbErrorInfo* __instance;
	
	//GDB中英文错误信息链表
	vector<GdbErrorDefine> m_gdbErrorInfo;
};


#endif