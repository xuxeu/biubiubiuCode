/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
* 修改历史：
* 2004-12-6 彭元志  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  Loader.cpp
* @brief
*       功能：
*       <li>加载工具主程序，错误、进度等相关信息通过MI消息协议上报给IDE</li>
*/

/************************引用部分******************************/
#include "Loader.h"
#include "Parameter.h"
#include "Message.h"
#include "WarnningProgressMessage.h"
#include "InformationProgressMessage.h"
#include "Util.h"
#include "LoadOSCommand.h"
#include "LoadAndRunOSCommand.h"
#include "Target.h"

using namespace com_coretek_tools_loader_command;
using namespace com_coretek_tools_loader_message;

/************************定义部分******************************/

/************************实现部分******************************/

/************************模块函数******************************/

/* 释放参数资源 */
static void ReleaseResource(vector<Parameter*> &param);

/* 创建命令 */
static Command* CreateCommand(string type,vector<Parameter*> &param);

/************************实现部分*********************************/

/**
 * @brief
    主入口程序
 * @param[in]  argc:参数个数
 * @param[in]  argv:参数字符串数组
 * @Return
     总是返回0
 */
int main(int argc, char* argv[])
{	
	string commandType = NULL_STRING;     /* 命令类型 */
	vector<Parameter*> vctorParam;	      /* 参数vctor */	
	string errorDefineFile = NULL_STRING; /* 错误定义文件 */
	INT32 argCount = 0;					  /* 参数个数 */
	Command *command = NULL;              /* 命令类 */  
	UINT32 status = 0;                    /* 执行状态 */
	BOOL isOK = FALSE;                    /* 解析错误定义文件标志 */  
	Target *target = NULL;                /* 目标机 */

	/*记录传递的参数*/
	FILE *fp = fopen("loader_para.txt", "a+");
    if(NULL != fp)
    {
        for(int i = 0; i < argc; ++i)
        {
            fprintf(fp, "%s ", argv[i]);
        }
        fprintf(fp, "\n");
        fclose(fp);
    }

	/* 获得错误文件所在目录 */
	errorDefineFile = Util::GetDefault()->GetCurrentDirectory();
	errorDefineFile += "\\";
	errorDefineFile += ERRORFILE;

	/* 解析错误文件 */
	isOK = Util::GetDefault()->LoadErrorFile(errorDefineFile);
	if (!isOK)
	{
		Util::GetDefault()->NormalOutput(LOADER_ERROR_DEFINE_FILE_FAIL_INFO);
		return 0;
	}

	/* 检查参数个数 */
	if (argc < 2)
	{		
		status = Util::GetDefault()->GetErrorNumber(LOADER_ERROR_PARAMETER);
		Util::GetDefault()->ErrorMessageOutput(status);
		return 0;
	}

	/* 获取命令类型 */
	commandType = argv[1];

	/* 解析各个参数 */
	for(argCount = 2; argCount < argc; argCount += 2)
	{
		Parameter *parameter = new Parameter(argv[argCount], argv[argCount + 1]);
		vctorParam.push_back(parameter);
	}

	/* 创建命令 */
	command = CreateCommand(commandType, vctorParam);
	if(NULL != command)
	{	
		/* 目标机服务器未启动 */
		if(!Util::GetDefault()->FindProcess(TS_EXE))
		{
			status = Util::GetDefault()->GetErrorNumber(LOADER_ERROR_TARGETSERVER_NO_START);
			Util::GetDefault()->ErrorMessageOutput(status);
			return 0;
		}

		/* 创建目标机 */
		target = command->CreateTarget();		

		/* 连接目标机 */
		status = target->Connect();    
		if(TARGET_OK != status)
		{
			Util::GetDefault()->ErrorMessageOutput(status);
			return 0;
		}
		
		if((commandType == COMMAND_LOAD_OS) || (commandType == COMMAND_LOAD_AND_RUN_OS))
		{
			/* 加载OS映像 */
			command->Exec(SDA_SAID);
		}
		else
		{
			/* 加载APP映像 */
			command->Exec(PDA_LOADAID);
		}
		
		/* 断开目标机连接 */
		target->DisConnect();   

		delete command;
	}
	
	/* 回复^^done */
	Util::GetDefault()->DoneMessageOutput();

	/* 释放参数资源 */
	ReleaseResource(vctorParam);

	return 0;
}

/**
 * @Funcname  : ReleaseResource
 * @brief	  : 释放参数分配的资源
 * @para[IN]  : vector<Parameter*>参数
 * @reture	  : 无
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
static void ReleaseResource(vector<Parameter*> &parameter)
{
	UINT32 i = 0;
	
	/* 释放参数分配的资源 */
	for(i = 0; i < (UINT32)parameter.size(); i++)
	{
		delete parameter[i];
		parameter[i] = NULL;
	}
}

/**
 * @Funcname  : CreateCommand
 * @brief	  : 创建命令对象
 * @para[IN]  : commandtype:命令类型 vector<Parameter*>:参数
 * @reture	  : 成功返回命令对象，否则返回NULL
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
static Command* CreateCommand(string commandtype, vector<Parameter*> &parameter)
{	
	Command *command = NULL;

	if(commandtype == COMMAND_LOAD_OS)
	{
		command = new LoadOSCommand(parameter);
	}
	else if(commandtype == COMMAND_LOAD_AND_RUN_OS)
	{
		command = new LoadAndRunOSCommand(parameter);
	}

	return command;
}