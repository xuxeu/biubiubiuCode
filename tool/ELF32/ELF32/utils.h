/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *�޸���ʷ:
       2010-9-16 11:23:17        amwyga      �����������ɼ������޹�˾
       �޸ļ�����:�������ļ�
 */

/**
 *@file   Utils.h
 *@brief
       <li>�궨��</li>
 *@Note
       
 */

#ifndef _INCLUDE_UTILS_H_2010916_AMWYGA
#define _INCLUDE_UTILS_H_2010916_AMWYGA
/************************ͷ �� ��******************************/
//#include "os.h"
#include "stdafx.h"

/************************��    ��******************************/
/************************�� �� ��******************************/
/************************���Ͷ���******************************/

	//����IP
	#define LOCALIP ("127.0.0.1")

	#define VERIFYREADSIZE 500

	//TA��ʽ����ڴ���ֽ�����󳤶ȣ�����������ǰ�����������ַ�/*2008-04-09����*/
    #define MEMFILL_SIZE 512
	//ת��intΪ�ַ���ʱ�õ���ʱ���鳤��
	#define TEMPSTR_LENGTH 40

	//ÿ�ι̻��ĳ���(ʮ����),TRA���֧��1024(����ǰ�����������ַ�)
	#define BURNLENGTH 1000

	//����/��������ʱ�Ļ�������С,12k bytes
	#define BUFFERSIZE (12288)
	//TA��ʽ������ڴ��С
	#define TAMEM_FILL_SIZE 512
	//���Դ���
	#define RETRYCOUNT 10

	#define MAX_PIPEBUFFER (1000)

	//ʮ������������󳤶�
	#define MAX_8HEX (4)
	#define MAX_16HEX (6)
	#define MAX_32HEX (10)
	#define MAX_64HEX (18)

	#define IP_BUF_LEN              16       //IP��ַ

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
