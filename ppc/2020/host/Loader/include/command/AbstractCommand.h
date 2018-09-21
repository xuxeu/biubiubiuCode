/************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2007 CoreTek Systems Inc All Rights Reserved
************************************************************************/

/*
 * 修改历史：
 * 2010-8-18  彭元志，北京科银京成技术有限公司
 *                   创建该文件。
 */

/*
 * @file  AbstractCommand.h
 * @brief
 *       <li>功能：抽象的命令类</li>
 */
#ifndef _INCLUDE_ABSTRACTCOMMAND_H
#define _INCLUDE_ABSTRACTCOMMAND_H

/************************ 引用部分 ****************************/
#include "Loader.h"

/************************宏 定 义******************************/
namespace com_coretek_tools_loader_command
{
	class AbstractCommand
	{
		protected:
			
			/* 一般信息输出 */
			void NormalOutput(string outStr);

			/* 错误信息输出 */
			void ErrorMessageOutput(UINT32 errorNmber);

			/* Done信息输出 */
			void DoneMessageOutput(void);

			/* 进度信息输出 */
			void InformationProgressMessageOutput(string msg,UINT32 rate,UINT32 sendsize,UINT32 totalsize);

			/* 根据ErrprStr获取ErrprMum,以数字形式返回 */
			UINT32 GetErrorNumber(string str);
		public:
			
			/* 构造函数 */
			AbstractCommand(void){}

			/* 析构函数 */
			virtual ~AbstractCommand(void){}
	};
}

#endif
