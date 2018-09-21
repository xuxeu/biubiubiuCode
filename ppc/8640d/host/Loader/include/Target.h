/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *修改历史:
       2010-9-15 15:11:08        彭元志      北京科银京成技术有限公司
       修改及方案:创建本文件
 */

/*
 * @file  Target.h
 * @brief
 *       <li>定义和目标机服务器之间的交互操作</li>
 */
#ifndef _INCLUDE_TARGET_H_
#define _INCLUDE_TARGET_H_

/************************引用部分******************************/
#include "Loader.h"
#include "TClientAPI.h"
#include "Util.h"

/************************类定义******************************/
class Target
{
	private:
		
		/* IP地址 */
		string m_tsIP;
		
		/* 目标机服务器名称 */
		string m_tgtName ;
		
		/* 目标机服务端口号 */
		UINT32 m_tsPort;
		
		/* 连接句柄 */
		UINT16 m_tsFd;

		/* 目标机ID */
		UINT16 m_desAid;

		/* 当前要操作的代理号 */
		UINT16 m_currentDesSaid;
	public:
		
		/* 构造函数 */
		Target(string tsip, string tgtname, UINT32 tsport);

		/* 析构函数 */
		virtual ~Target(void){}

		/* 发送命令并等指定回复 */
		UINT32 ExecCommand(const char *cmd, UINT32 cmdCount, UINT32 timeout);

		/* 发送命令并等回复,将回复放到指定的buffer中 */
		UINT32 ExecCommand(const char *cmd, UINT32 cmdCount, char *result, UINT32 resultCount, UINT32 timeout);

		/* 发送命令不等回复 */
		UINT32 ExecCommand(const char *cmd, UINT32 cmdCount);

		/* 连接到指定的目标机服务器 */
		UINT32 Connect(void);

		/* 断开连接 */
		UINT32 DisConnect(void);

		/* 设置二级代理号 */
		void SetDesSAid(UINT16 desSAid);

		/* 接受数据 */
		UINT32 ReceiveData(char *result, UINT32 resultCount, UINT32 timeout, INT32 *receSize);
	protected:
		
		/* 根揣ErrprStr获取ErrprMum,以数字形式返回 */
		UINT32 GetErrorNumber(string str);
};
#endif
