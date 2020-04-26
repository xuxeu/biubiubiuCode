/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2010   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @file           :  tsConfig.h
 * @Version        :  1.0.0
 * @brief          :  *   <li> 功能</li>进行一些tsServer的常量或宏的配置
                                 <li>设计思想</li> 
                                 <p> n     注意事项：n</p>
 *   
 * @author        :  zhangxu
 * @create date:  2008年9月23日 16:02:35
 * @History        : 
 *   
 */
 
#ifndef _TSS_CONFIG_H
#define _TSS_CONFIG_H

#define TS_IP           "127.0.0.1"
#define TS_PORT       8000

#define MAX_PATH_LEN                    260
#define MAX_EXPRESSION_LEN          300
#define MAX_IPV4_IP_LEN			   16
#define MAX_NAME_LEN			   100
#define MAX_MESSAGE_LEN             2048
#define MAX_SYSMBOL_FILE_NUM     10
#define MAX_TARGET_NUM               100
#define MAX_MESSAGE_NUM            20

#define TSS_GDB_RW_BUF_LEN          2048
#define TSS_CONTROL_FLAG                       ";"
#define TSS_CONTROL_FLAG_LEN                 1

#define TSS_TIME_WAIT_FOR_CONNECT  500  //ms

#define TSMAP_AID           1
#define SA_REPORT_TYPE                     0

#define DEFAULT_WAIT_TIME             2

#define TARGET_STATUS_QUERY_STRING      "QM"
#define TARGET_MODE_QUERY_STRING      "MV"
#define TARGET_CREATE_TDA_STRING          "SC"
#define TARGET_DELETE_TDA_STRING          "SD"
#define TARGET_CREATE_ATTACH_TASK_STRING    "AC"
#define TARGET_DETACH_DELETE_TASK_STRING    "AD"
#define TARGET_TASK_CONTINUE    "c"
#define TARGET_TASK_STOP	"!"


#define QUERY_TARGET_STATUS_AGENT_1X          1

#define QUERY_TARGET_STATUS_AGENT_2X          11


#define TARGET_SAID_SMA          5

#endif
