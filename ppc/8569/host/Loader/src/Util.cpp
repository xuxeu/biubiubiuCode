/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/*
 * 修改历史：
 * 2010-8-18  彭元志，北京科银京成技术有限公司
 *                   创建该文件。
 */

/**
* @file  Util.cpp
* @brief
*       功能：
*       <li> 公共函数</li>
*/
/************************头 文 件******************************/
#include <iostream>
#include <windows.h>
#include <Tlhelp32.h>
#include "Util.h"
#include "loader.h"
#include "ErrorMessage.h"
#include "InformationProgressMessage.h"
#include "DoneMessage.h"

using namespace com_coretek_tools_loader_message;

/************************定义部分*****************************/
Util* Util::m_instance = NULL;

/************************实现部分******************************/
/**
 * @Funcname  : GetDefault
 * @brief	  : 获取实例
 * @para[IN]  : 无
 * @reture	  : 无
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
Util* Util::GetDefault()
{
	if (NULL == m_instance)
	{
		m_instance = new Util();
	}

	return m_instance;
}

/**
 * @Funcname  : LoadErrorFile
 * @brief	  : 加载错误定义文件
 * @para[IN]  : errorFileName：错误定义文件
 * @reture	  : TRUE:正确加载 FALDE:加载时出错
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
BOOL Util::LoadErrorFile(string errorFileName)
{
	return m_errorTable.LoadFile(errorFileName);
}

/**
 * @Funcname  : GetErrorNumber
 * @brief	  : 根揣ErrprStr获取ErrprMum,以数字形式返回
 * @para[IN]  : ErrorStr：错误字符串
 * @reture	  : 数字形式的错误号
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
UINT32 Util::GetErrorNumber(string str)
{
	return m_errorTable.GetErrorNumber(str);
}

/**
 * @Funcname  : InformationProgressMessageOutput
 * @brief	  : 进度信息输出
 * @para[IN]  : msg：信息 rate：下载进度 sendsize：当前下载大小 totalsize：下载文件总大小
 * @reture	  : 无
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
void Util::InformationProgressMessageOutput(string msg, UINT32 rate, UINT32 sendsize, UINT32 totalsize)
{
	InformationProgressMessage *progressmessage = new  InformationProgressMessage(msg, rate, sendsize, totalsize);
	NormalOutput(progressmessage->tostring());
	delete progressmessage;
	progressmessage = NULL;
}

/**
 * @Funcname  : ErrorMessageOutput
 * @brief	  : 错误信息输出
 * @para[IN]  : errorNumber：错误号
 * @reture	  : 无
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
void Util::ErrorMessageOutput(UINT32 errorNumber)
{
	ErrorMessage *errorMessage = new ErrorMessage(errorNumber);
	NormalOutput(errorMessage->tostring());
	delete errorMessage;
	errorMessage = NULL;
}

/**
 * @Funcname  : DoneMessageOutput
 * @brief	  : done信息输出
 * @para[IN]  : 无
 * @reture	  : 无
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
void Util::DoneMessageOutput(void)
{
	DoneMessage *doneMessage = new DoneMessage();
	NormalOutput(doneMessage->tostring());
	delete doneMessage;
	doneMessage = NULL;
}

/**
 * @Funcname  : NormalOutput
 * @brief	  : 一般信息输出
 * @para[IN]  : outStr：输出信息
 * @reture	  : 无
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
void Util::NormalOutput(string outStr)
{
	cout << outStr << endl;
} 

/**
 * @Funcname  : GetCurrentDirectory
 * @brief	  : 得到执行程序所在目录
 * @para[IN]  : 无
 * @reture	  : 执行程序路径
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
string Util::GetCurrentDirectory(void)
{
	if (m_exePath == "")
	{
		TCHAR Buffer[MAX_PATH];

	#ifdef _UNICODE
			wmemset(Buffer,0,MAX_PATH);
	#else
			memset(Buffer,0,MAX_PATH);
	#endif

		GetModuleFileName(NULL, Buffer, sizeof(Buffer));
	#ifdef _UNICODE
			*(wcsrchr(Buffer, '\\')) = '\0';
	#else
			*(strrchr(Buffer, '\\')) = '\0';
	#endif
			m_exePath = Buffer;
	}

	return m_exePath;
}

/**
 * @Funcname  : FindProcess
 * @brief	  : 查询进程是否启动
 * @para[IN]  : 无
 * @reture	  : processName：进程名
 * @Author	  : pengyz
 * @History: 1.  Created this function on 2010年11月24日 
 *	 
**/
BOOL Util::FindProcess(const char* processName)
{
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32) ;

    //创建快照句柄
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(INVALID_HANDLE_VALUE == hSnapshot)
	{
		return FALSE;
	}

    //先搜索系统中第一个进程的信息
    Process32First(hSnapshot, &pe);
    
    //下面对系统中的所有进程进行枚举
    do
    {
        //不区分大小写
        if (strnicmp(pe.szExeFile, processName, strlen(pe.szExeFile)) == 0)
        {
			//关闭快照句柄
			CloseHandle(hSnapshot); 

			return TRUE;
        }
    }while (Process32Next(hSnapshot, &pe));
    
    //关闭快照句柄
    CloseHandle(hSnapshot); 

	return FALSE;
}

/**
 *@brief
 *  大小端转换
 *@param[in] cnt:字节数
 *@param[in|out] ptr:要转换的字符串
 *@Return
 *   无
 */
void Util::big2little(int cnt, void *ptr)
{
    char    *ByteData;
    char    TempByte;
    
    /* Get data pointer */
    ByteData = ( char * )ptr;
    
    switch(cnt)
    {
        case 2:
        {
            TempByte = ByteData[0];
            ByteData[0] = ByteData[1];
            ByteData[1] = TempByte;
            break;
        }
        
        case 4:
        {
            TempByte = ByteData[0];
            ByteData[0] = ByteData[3];
            ByteData[3] = TempByte;
            TempByte = ByteData[1];
            ByteData[1] = ByteData[2];
            ByteData[2] = TempByte;
			break;
        }
        case 8:
        {
            TempByte = ByteData[0];
            ByteData[0] = ByteData[7];
            ByteData[7] = TempByte;
            TempByte = ByteData[1];
            ByteData[1] = ByteData[6];
            ByteData[6] = TempByte;
			TempByte = ByteData[2];
            ByteData[2] = ByteData[5];
            ByteData[5] = TempByte; 
			TempByte = ByteData[3];
            ByteData[3] = ByteData[4];
            ByteData[4] = TempByte; 
			break;
        }
		default:
		{
            TempByte = ByteData[0];
            ByteData[0] = ByteData[3];
            ByteData[3] = TempByte;
            TempByte = ByteData[1];
            ByteData[1] = ByteData[2];
            ByteData[2] = TempByte;
			break;
		}
    }
}
