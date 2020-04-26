/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
* �޸���ʷ��
* 2004-12-6 ��Ԫ־  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  Loader.cpp
* @brief
*       ���ܣ�
*       <li>���ع��������򣬴��󡢽��ȵ������Ϣͨ��MI��ϢЭ���ϱ���IDE</li>
*/

/************************���ò���******************************/
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

/************************���岿��******************************/

/************************ʵ�ֲ���******************************/

/************************ģ�麯��******************************/

/* �ͷŲ�����Դ */
static void ReleaseResource(vector<Parameter*> &param);

/* �������� */
static Command* CreateCommand(string type,vector<Parameter*> &param);

/************************ʵ�ֲ���*********************************/

/**
 * @brief
    ����ڳ���
 * @param[in]  argc:��������
 * @param[in]  argv:�����ַ�������
 * @Return
     ���Ƿ���0
 */
int main(int argc, char* argv[])
{	
	string commandType = NULL_STRING;     /* �������� */
	vector<Parameter*> vctorParam;	      /* ����vctor */	
	string errorDefineFile = NULL_STRING; /* �������ļ� */
	INT32 argCount = 0;					  /* �������� */
	Command *command = NULL;              /* ������ */  
	UINT32 status = 0;                    /* ִ��״̬ */
	BOOL isOK = FALSE;                    /* �����������ļ���־ */  
	Target *target = NULL;                /* Ŀ��� */

	/*��¼���ݵĲ���*/
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

	/* ��ô����ļ�����Ŀ¼ */
	errorDefineFile = Util::GetDefault()->GetCurrentDirectory();
	errorDefineFile += "\\";
	errorDefineFile += ERRORFILE;

	/* ���������ļ� */
	isOK = Util::GetDefault()->LoadErrorFile(errorDefineFile);
	if (!isOK)
	{
		Util::GetDefault()->NormalOutput(LOADER_ERROR_DEFINE_FILE_FAIL_INFO);
		return 0;
	}

	/* ���������� */
	if (argc < 2)
	{		
		status = Util::GetDefault()->GetErrorNumber(LOADER_ERROR_PARAMETER);
		Util::GetDefault()->ErrorMessageOutput(status);
		return 0;
	}

	/* ��ȡ�������� */
	commandType = argv[1];

	/* ������������ */
	for(argCount = 2; argCount < argc; argCount += 2)
	{
		Parameter *parameter = new Parameter(argv[argCount], argv[argCount + 1]);
		vctorParam.push_back(parameter);
	}

	/* �������� */
	command = CreateCommand(commandType, vctorParam);
	if(NULL != command)
	{	
		/* Ŀ���������δ���� */
		if(!Util::GetDefault()->FindProcess(TS_EXE))
		{
			status = Util::GetDefault()->GetErrorNumber(LOADER_ERROR_TARGETSERVER_NO_START);
			Util::GetDefault()->ErrorMessageOutput(status);
			return 0;
		}

		/* ����Ŀ��� */
		target = command->CreateTarget();		

		/* ����Ŀ��� */
		status = target->Connect();    
		if(TARGET_OK != status)
		{
			Util::GetDefault()->ErrorMessageOutput(status);
			return 0;
		}
		
		if((commandType == COMMAND_LOAD_OS) || (commandType == COMMAND_LOAD_AND_RUN_OS))
		{
			/* ����OSӳ�� */
			command->Exec(SDA_SAID);
		}
		else
		{
			/* ����APPӳ�� */
			command->Exec(PDA_LOADAID);
		}
		
		/* �Ͽ�Ŀ������� */
		target->DisConnect();   

		delete command;
	}
	
	/* �ظ�^^done */
	Util::GetDefault()->DoneMessageOutput();

	/* �ͷŲ�����Դ */
	ReleaseResource(vctorParam);

	return 0;
}

/**
 * @Funcname  : ReleaseResource
 * @brief	  : �ͷŲ����������Դ
 * @para[IN]  : vector<Parameter*>����
 * @reture	  : ��
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
 *	 
**/
static void ReleaseResource(vector<Parameter*> &parameter)
{
	UINT32 i = 0;
	
	/* �ͷŲ����������Դ */
	for(i = 0; i < (UINT32)parameter.size(); i++)
	{
		delete parameter[i];
		parameter[i] = NULL;
	}
}

/**
 * @Funcname  : CreateCommand
 * @brief	  : �����������
 * @para[IN]  : commandtype:�������� vector<Parameter*>:����
 * @reture	  : �ɹ�����������󣬷��򷵻�NULL
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010��11��24�� 
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