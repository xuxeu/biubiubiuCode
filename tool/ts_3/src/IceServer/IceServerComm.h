/***************************************************************************
 *   
 *                       北京科银技术有限公司        版权所有
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  IceServerComm.h
 * @Version        :  1.0.0
 * @Brief           :  定义一些公共数据
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007年3月6日 14:17:10
 * @History        : 
 *   
 */
#ifndef _ICE_SERVER_COMM_H
#define _ICE_SERVER_COMM_H


#define HEX_NUM     16
#define DEC_NUM     10
#define OCT_NUM     8
#define BIN_NUM      2

//const char CONTROL_FLAG[] = ",";
const char ICE_SEPARATE_CHAR[] = ",";

const char COLON_CHAR[] = ":";
#define COLON_CHAR_LEN     strlen(COLON_CHAR)
#define CONTROL_FLAG_LEN    strlen(CONTROL_FLAG)

//配置文件相对于动态库的路径
const char IS_CFGFILE[] = "\\LoadFile\\";

//ICE中的会话管理二级代理
#define ICE_SESS_MAN_ID     10

//错误号
#define FAIL_ACK_LEN                3
#define SUCC_ACK_LEN                2
#define SUCC_ACK                       "OK"
#define ERR_UNKONW_DATA     "E04"
#define ERR_SYSTEM_ERROR     "E01"
#define ERR_GDB_NORMAL         "E03"



/*RSP协议中的常用命令字*/
#define     RSP_CMD_LEN                              1
#define     RSP_QUERY_TARGET_STATE      "q"
#define     RSP_SET_ALL_REGISTER            "G"
#define     RSP_GET_ALL_REGISTER            "g"
#define     RSP_READ_MEMORY                    "x"
#define     RSP_WRITE_MEMORY                  "X"
#define     RSP_STOP_TARGET                      "!"
#define     RSP_RUN_TARGET                        "c"
#define     RSP_RESET_TARGET                     "R"
#define     RSP_SET_BREAKPOINT                "Z"
#define     RSP_DELETE_BREAKPOINT          "z"
#define     RSP_GET_TARGET_CPUCODE      "i"
#define     RSP_SET_CPP_REGISTER            "D"
#define     RSP_GET_CPP_REGISTER            "d"
#define     LAMBDA_ICE_START_DCC            "A"

/*定义断点类型*/
#define     SOFT_BP                                         0
#define     HARD_BP                                         1
#define     WRITEWATCH_BP                           2
#define     READWATCH_BP                             3
#define     ACCESS_BP                                     4


#define     SOCKET_DATA                         1
#define     TSAPI_DATA                            2

#define     INVALID_AID                         0xFFFF

/*定义消息处理时是否压入了命令至命令队列中*/
#define     IS_NEED_EXC_CMD                 0x10
#define     NAME_SIZE                          100
/*define the target type */
#define LAMBDAICE_SIMULATOR_TARGET         1
#define LAMBDA_JTAG_ICE                                  0

#ifdef assert
#undef assert
#define assert(bflag) {\
                                    if(!bflag)\
                                        return FALSE;\
                                    }
#else
#define assert(bflag) {\
                                    if(!bflag)\
                                        return FALSE;\
                                    }
#endif


/************************引用部分*****************************/

/************************前向声明部分***********************/

/************************定义部分*****************************/

/************************实现部分****************************/

#endif /*_ICE_SERVER_COMM_H*/
