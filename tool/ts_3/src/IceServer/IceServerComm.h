/***************************************************************************
 *   
 *                       ���������������޹�˾        ��Ȩ����
 *              CopyRight (C)     2000-2006   Coretek System Inc.   All Rights Reserved
 *
***************************************************************************/



/**
 * @filename      :  IceServerComm.h
 * @Version        :  1.0.0
 * @Brief           :  ����һЩ��������
 *   
 * @Author        :  qingxiaohai
 * @CreatedDate:  2007��3��6�� 14:17:10
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

//�����ļ�����ڶ�̬���·��
const char IS_CFGFILE[] = "\\LoadFile\\";

//ICE�еĻỰ�����������
#define ICE_SESS_MAN_ID     10

//�����
#define FAIL_ACK_LEN                3
#define SUCC_ACK_LEN                2
#define SUCC_ACK                       "OK"
#define ERR_UNKONW_DATA     "E04"
#define ERR_SYSTEM_ERROR     "E01"
#define ERR_GDB_NORMAL         "E03"



/*RSPЭ���еĳ���������*/
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

/*����ϵ�����*/
#define     SOFT_BP                                         0
#define     HARD_BP                                         1
#define     WRITEWATCH_BP                           2
#define     READWATCH_BP                             3
#define     ACCESS_BP                                     4


#define     SOCKET_DATA                         1
#define     TSAPI_DATA                            2

#define     INVALID_AID                         0xFFFF

/*������Ϣ����ʱ�Ƿ�ѹ�������������������*/
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


/************************���ò���*****************************/

/************************ǰ����������***********************/

/************************���岿��*****************************/

/************************ʵ�ֲ���****************************/

#endif /*_ICE_SERVER_COMM_H*/
