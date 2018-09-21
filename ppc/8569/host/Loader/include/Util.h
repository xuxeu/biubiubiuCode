/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * 修改历史：
 * 2010-8-18  彭元志，北京科银京成技术有限公司
 *                   创建该文件。
 */

/*
 * @file  Util.h
 * @brief
 *       <li>公共函数</li>
 */
#ifndef _INCLUDE_UTIL_H_
#define _INCLUDE_UTIL_H_

/************************引用部分******************************/
#include "Message.h"
#include "ErrorTableDefine.h"
#include <wchar.h>

using namespace com_coretek_tools_loader_error;

/************************类定义******************************/
class Util
{
	private:
		string m_exePath;

		/* 单实例 */
		static Util *m_instance;

		/* 错误定义表 */
		ErrorTableDefine m_errorTable;

		Util(void):m_exePath(NULL_STRING){}
	public:
		~Util(void){};

		static Util *GetDefault();

		/* 加载错误定义文件 */
		BOOL LoadErrorFile(string errorFileName);

		/* 根揣ErrprStr获取ErrprMum,以数字形式返回 */
		UINT32 GetErrorNumber(string str);

		/* 一般信息输出 */
		void NormalOutput(string outStr);

		/* 错误信息输出 */
		void ErrorMessageOutput(UINT32 errorNumber);

		/* done信息输出 */
		void DoneMessageOutput(void);

		/* 进度信息输出 */
		void InformationProgressMessageOutput(string msg, UINT32 rate, UINT32 sendsize, UINT32 totalsize);

		/* 得到程序所在目录 */
		string GetCurrentDirectory(void);

		/* 查询进程是否启动 */
		BOOL Util::FindProcess(const char* processName);

		/*大小端转换*/
		void big2little(INT32 cnt, void *ptr);


};

#endif
