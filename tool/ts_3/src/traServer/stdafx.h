// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// TODO: 在此处引用程序要求的附加头文件
#include "../../include/wsocket.h"
#include "MutexLock.h"
#include "gdbServer.h"
#include "common.h"
//
/*函数返回值定义*/
#ifndef SUCC
#define SUCC (0)
#endif

#ifndef FAIL
#define FAIL (-1)
#endif