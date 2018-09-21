/************************************************************************
*				�����������ɼ������޹�˾ ��Ȩ����
*    Copyright (C) 2000-2007 CoreTek Systems Inc. All Rights Reserved.
***********************************************************************/

/**
 * @file 	utils.c
 * @brief	���ú�����
 * @author 	����F
 * @date 	2007��12��17��
 * @History
 */
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "types.h"
#include "Utils.h"

signed short big2little_short(signed short big)
{
    signed short result,temp0,temp1;

    temp0 = (((big)&(0xff00))>>8);
    temp1 = (((big)&(0x00ff))<<8);

    result = temp0|temp1;

    return result;
}

unsigned int big2little_endian(unsigned int big)
{
    unsigned int result,temp0,temp1,temp2,temp3;

    temp0 = ((big)&(0xff000000))>>24;
    temp1 = ((big)&(0x00ff0000))>>8;
    temp2 = ((big)&(0x0000ff00))<<8;
    temp3 = ((big)&(0x000000ff))<<24;

    result = temp0|temp1|temp2|temp3;

    return result;
}


