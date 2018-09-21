/*
* 更改历史：
* 2004-12-28 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  tsConfig.h
* @brief
*       功能：
*       <li>TS二级代理的配置对象</li>
*/


#ifndef TSCONFIG_INCLUDE
#define TSCONFIG_INCLUDE

/************************头文件********************************/
#include "SubAgentBase.h"
#include "Manager.h"
/************************宏定义********************************/
/************************类型定义******************************/

TMutex Manager::mutexInstance ;
ControlSubAgent controlSubAgent;    //定义了一个控制的二级代理

/************************接口声明******************************/

#endif
