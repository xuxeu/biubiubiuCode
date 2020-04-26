/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *修改历史:
       2010-9-16 11:23:17        amwyga      北京科银京成技术有限公司
       修改及方案:创建本文件
 */

/**
 *@file   Utils.h
 *@brief
       <li>宏定义</li>
 *@Note
       
 */

#ifndef _INCLUDE_UTILS_H_2010916_AMWYGA
#define _INCLUDE_UTILS_H_2010916_AMWYGA
/************************头 文 件******************************/
//#include "os.h"
#include "stdafx.h"

/************************引    用******************************/
/************************宏 定 义******************************/
/************************类型定义******************************/

	//本机IP
	#define LOCALIP ("127.0.0.1")

	#define VERIFYREADSIZE 500

	//TA方式填充内存的字节流最大长度，不包括命令前面的命令及长度字符/*2008-04-09增加*/
    #define MEMFILL_SIZE 512
	//转换int为字符串时用的临时数组长度
	#define TEMPSTR_LENGTH 40

	//每次固化的长度(十进制),TRA最大支持1024(包括前面的命令及长度字符)
	#define BURNLENGTH 1000

	//接收/发送数据时的缓冲区大小,12k bytes
	#define BUFFERSIZE (12288)
	//TA方式下填充内存大小
	#define TAMEM_FILL_SIZE 512
	//重试次数
	#define RETRYCOUNT 10

	#define MAX_PIPEBUFFER (1000)

	//十六进制数的最大长度
	#define MAX_8HEX (4)
	#define MAX_16HEX (6)
	#define MAX_32HEX (10)
	#define MAX_64HEX (18)

	#define IP_BUF_LEN              16       //IP地址

	#ifdef _USE_VS2005
		#define ITOA(_Value,_Dest,_SIZE,_Radix) _itoa_s((_Value),(_Dest),(_SIZE),(_Radix))

		#define UITOA(_Value,_Dest,_SIZE,_Radix) _ultoa_s((_Value),(_Dest),(_SIZE),(_Radix))
		
		#define I64TOA(_Value,_Dest,_SIZE,_Radix) _i64toa_s((_Value),(_Dest),(_SIZE),(_Radix))	

		#define UI64TOA(_Value,_Dest,_SIZE,_Radix) _ui64toa_s((_Value),(_Dest),(_SIZE),(_Radix))		

		#define STRLWR(_Str,_SIZE) _strlwr_s((_Str),(_SIZE))

		#define STRUPR(_Str,_SIZE) _strupr_s((_Str),(_SIZE))

		#define FOPEN(_FP,_FILENAME,_MODE) fopen_s(&(_FP),(_FILENAME),(_MODE))

		#define SPRINTF(_BUFFER,_SIZE,_FORMAT,_VALUE) sprintf_s((_BUFFER),(_SIZE),(_FORMAT),(_VALUE))
		
		#define SPRINTF2(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2) sprintf_s((_BUFFER),(_SIZE),(_FORMAT),(_VALUE1),(_VALUE2))

		#define SPRINTF3(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2,_VALUE3) sprintf_s((_BUFFER),(_SIZE),(_FORMAT),(_VALUE1),(_VALUE2),(_VALUE3))

		#define SPRINTF4(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2,_VALUE3,_VALUE4) sprintf_s((_BUFFER),(_SIZE),(_FORMAT),(_VALUE1),(_VALUE2),(_VALUE3),(_VALUE4))

		#define SPRINTF5(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2,_VALUE3,_VALUE4,_VALUE5) sprintf_s((_BUFFER),(_SIZE),(_FORMAT),(_VALUE1),(_VALUE2),(_VALUE3),(_VALUE4),(_VALUE5))

		#define SPRINTF6(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2,_VALUE3,_VALUE4,_VALUE5,_VALUE6) sprintf_s((_BUFFER),(_SIZE),(_FORMAT),(_VALUE1),(_VALUE2),(_VALUE3),(_VALUE4),(_VALUE5),(_VALUE6))

		#define SPRINTF7(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2,_VALUE3,_VALUE4,_VALUE5,_VALUE6,_VALUE7) sprintf_s((_BUFFER),(_SIZE),(_FORMAT),(_VALUE1),(_VALUE2),(_VALUE3),(_VALUE4),(_VALUE5),(_VALUE6),(_VALUE7))

		#define STRTOK(_StrToken,_StrDelimit,_Context) strtok_s((_StrToken),(_StrDelimit),(_Context))
	    
		#define TCSLWR(_Str,_Len) _tcslwr_s(_Str,_Len)
	#else
		#define ITOA(_Value,_Dest,_SIZE,_Radix) itoa((_Value),(_Dest),(_Radix))

		#define UITOA(_Value,_Dest,_SIZE,_Radix) _ultoa((_Value),(_Dest),(_Radix))

		#define I64TOA(_Value,_Dest,_SIZE,_Radix) _i64toa((_Value),(_Dest),(_Radix))

		#define UI64TOA(_Value,_Dest,_SIZE,_Radix) _ui64toa((_Value),(_Dest),(_Radix))		
	
		#define STRLWR(_Str,_SIZE) _strlwr(_Str)

		#define STRUPR(_Str,_SIZE) _strupr(_Str)

		#define FOPEN(_FP,_FILENAME,_MODE) _FP=fopen((_FILENAME),(_MODE))

		#define SPRINTF(_BUFFER,_SIZE,_FORMAT,_VALUE) sprintf((_BUFFER),(_FORMAT),(_VALUE))

		#define SPRINTF2(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2) sprintf((_BUFFER),(_FORMAT),(_VALUE1),(_VALUE2))

		#define SPRINTF3(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2,_VALUE3) sprintf((_BUFFER),(_FORMAT),(_VALUE1),(_VALUE2),(_VALUE3))

		#define SPRINTF4(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2,_VALUE3,_VALUE4) sprintf((_BUFFER),(_FORMAT),(_VALUE1),(_VALUE2),(_VALUE3),(_VALUE4))

		#define SPRINTF5(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2,_VALUE3,_VALUE4,_VALUE5) sprintf((_BUFFER),(_FORMAT),(_VALUE1),(_VALUE2),(_VALUE3),(_VALUE4),(_VALUE5))

		#define SPRINTF6(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2,_VALUE3,_VALUE4,_VALUE5,_VALUE6) sprintf((_BUFFER),(_FORMAT),(_VALUE1),(_VALUE2),(_VALUE3),(_VALUE4),(_VALUE5),(_VALUE6))

		#define SPRINTF7(_BUFFER,_SIZE,_FORMAT,_VALUE1,_VALUE2,_VALUE3,_VALUE4,_VALUE5,_VALUE6,_VALUE7) sprintf((_BUFFER),(_FORMAT),(_VALUE1),(_VALUE2),(_VALUE3),(_VALUE4),(_VALUE5),(_VALUE6),(_VALUE7))

		#define STRTOK(_StrToken,_StrDelimit,_Context) strtok((_StrToken),(_StrDelimit))

		#define TCSLWR(_Str,_Len) _tcslwr(_Str)
	#endif

#endif
