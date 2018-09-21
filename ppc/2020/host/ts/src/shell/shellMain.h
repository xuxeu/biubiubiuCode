/*
* 更改历史：
* 2004-12-6  董洪涛  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 唐兴培  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  shellMain.h
* @brief
*       功能：
*       <li>shell主程序相关宏定义</li>
*/

#ifndef _SHELLMAIN_H
#define _SHELLMAIN_H


/************************头文件********************************/

/************************宏定义********************************/
#ifdef __cplusplus
extern "C" {
#endif

/* 默认等待收包时间,单位秒 */
#define DEFAULT_WAIT_TIME (10)


#define SUCC (0)
#define FAIL (-1)

/*用ctrl+c终止操作的返回值*/
#define STOP_CMD (-2)


#define PROMPT_SYMBOL ("****************************************")

#define BLANK_STR        (" ")

#define MAX_MSG_SIZE (1024)

#define MAX_NAME_MSG_SIZE (49)

#ifdef __cplusplus
}
#endif
/************************类型定义******************************/

/************************接口声明******************************/

#endif