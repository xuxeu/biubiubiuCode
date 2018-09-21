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
 * @file  Loader.h
 * @brief
 *       <li>定义使用到的公共宏</li>
 */
#ifndef _INCLUDE_LOADER_H_
#define _INCLUDE_LOADER_H_

/************************引用部分******************************/
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "basicTypes.h"
using namespace std;

/************************宏定义******************************/
#define ERRORFILE "ErrorDefine.cfg" //错误定义文件名

#define SDA_SAID  1    //系统级调试代理
#define PDA_LOADAID  12 //分区级下载代理

#ifndef TRUE
    #define TRUE        1
#endif
    
#ifndef FALSE
    #define FALSE        0
#endif
   
#define NULL_STRING  ""
#define OUT_BUFFFER_SIZE 2048
#define LOADER_ERROR_DEFINE_FILE_FAIL_INFO  "解析错误定义文件失败,检查文件是否存在相应目录,格式是否正确"

#define LOADER_INVALID_ID		        0xffffffff
#define LOADER_ERROR_OPENFILE	        "LOADER_ERROR_OPENFILE"
#define LOADER_ERROR_TIMEOUT	        "LOADER_ERROR_TIMEOUT"
#define LOADER_ERROR_CREATECOMMAND	    "LOADER_ERROR_CREATECOMMAND"
#define LOADER_ERROR_PARAMETER	        "LOADER_ERROR_PARAMETER"
#define LOADER_ERROR_OPENELF_FAIL		"LOADER_ERROR_OPENELF_FAIL"
#define LOADER_ERROR_BAD_ALLOC			"LOADER_ERROR_BAD_ALLOC"
#define LOADER_ERROR_LoadFile_FAIL		"LOADER_ERROR_LoadFile_FAIL"
#define LOADER_OK				        "LOADER_OK"
#define LOADER_ERROR_LOADOS_FAIL		"LOADER_ERROR_LOADOS_FAIL"
#define LOADER_SET_LOAD_DOMAINID_FAIL	"LOADER_SET_LOAD_DOMAINID_FAIL"
#define LOADER_REGISTER_IMAGE_FAIL		"LOADER_REGISTER_IMAGE_FAIL"
#define LOADER_ERROR_TARGETSERVER_NO_START		"LOADER_ERROR_TARGETSERVER_NO_START"

/*********************目标机部分错误宏定义****************/
#define TARGET_OK                       1
#define INVALID_PARAMETER  				"INVALID_PARAMETER"
#define TARGET_CREATE_TARGET_FAIL  	    "TARGET_CREATE__TARGET_FAIL"
#define TARGET_PUTPKT_FAIL        		"TARGET_PUTPKT_FAIL"
#define TARGET_GETPKT_FAIL        		"TARGET_GETPKT_FAIL"
#define TARGET_OUTTIME					"TARGET_OUTTIME"
#define TARGET_CONNECT_FAIL				"TARGET_CONNECT_FAIL"
#define TARGET_DISCONNECT_FAIL			"TARGET_DISCONNECT_FAIL"
#define TARGET_TCLIENT_OPEN_FAIL     	"TARGET_TCLIENT_OPEN_FAIL"
#define TARGET_TCLIENT_GETSERVERID_FAIL "TARGET_TCLIENT_GETSERVERID_FAIL"

/***********************加载命令宏定义**********************/
#define     COMMAND_LOAD_OS  			"LoadOS"
#define 	COMMAND_LOAD_AND_RUN_OS 	"LoadAndRunOS"
#define     TS_EXE                      "ts.exe"
#endif
