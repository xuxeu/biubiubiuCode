// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once

#ifndef _STDAFX_H
#define _STDAFX_H
#define WIN32_LEAN_AND_MEAN     // �� Windows ͷ���ų�����ʹ�õ�����

/************************ͷ�ļ�********************************/
// Windows ͷ�ļ�:
#include <windows.h>

// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�
#include "../../include/wsocket.h"
#include "MutexLock.h"
#include "gdbServer.h"
#include "common.h"
//
/*��������ֵ����*/
/************************�궨��********************************/
/************************���Ͷ���******************************/
#ifndef SUCC
#define SUCC (0)
#endif

#ifndef FAIL
#define FAIL (-1)
#endif
/************************���Ͷ���******************************/
#endif