/***************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2013 CoreTek Systems Inc. All Rights Reserved.
***************************************************************************/

/*
 *修改历史:
 *2013-05-26         彭元志，北京科银京成技术有限公司
 *                           创建该文件。
 */

/**
 * @file  rseConfig.h
 * @brief
 *       功能：
 *       <li>RES相关ID定义</li>
 */
#ifndef RSE_CONFIG_H
#define RSE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/************************头 文 件******************************/
#include <vxWorks.h>

/************************宏 定 义******************************/

/* 包结束字符串 */
#define RSE_END_STRING '$'

/* 名字最大长度 */
#define RSE_MAX_NAME_LEN  31

/* 路径最大长度 */
#define RSE_MAX_PATH_LENGTH 255

/* 错误码长度 */
#define RSE_ERRCODE_LEN  10

/* 属性、操作，对象id为零 */
#define RSE_NULL_OPER  0
#define RSE_NULL_ATTR  0
#define RSE_NULL_OBJECTID  0

/* 参数起始偏移 */
#define RSE_ARG_START_OFFSET 4

/* 结果字符串长度 */
#define RSE_RESULTCODE_LEN   4

/* 据统计任务最大信息长度为100 */
#define RSE_TASK_MAX_INFO_LEN  150

/* 据统计模块最大信息长度为638 */
#define RSE_MODULE_MAX_INFO_LEN  650

/* 据统计RTP最大信息长度为126 */
#define RSE_RTP_MAX_INFO_LEN  150

/* 据统计RTP共享库最大信息长度为310 */
#define RSE_RTP_SHL_MAX_INFO_LEN 350

#define l2n(l,c)\
do\
{\
    *((c)++) = (UINT8)(((l)>>24L)&0xff);\
    *((c)++) = (UINT8)(((l)>>16L)&0xff);\
    *((c)++) = (UINT8)(((l)>> 8L)&0xff);\
    *((c)++) = (UINT8)(((l))&0xff);\
} while(0)

#define s2n(s,c)\
do\
{\
    c[0] = (UINT8)(((s)>> 8)&0xff);\
    c[1] = (UINT8)((s)&0xff);\
    c += 2;\
} while(0)

#define n2s(c,s)\
do\
{\
    s = (((UINT16)((UINT8)c[0]))<< 8)|\
    (((UINT16)((UINT8)c[1])));\
} while(0)

#define n2l(c,l)\
do\
{\
    l = ((((UINT32)((UINT8)c[0])) << 24) & 0xff000000)|\
    ((((UINT32)((UINT8)c[1]))<< 16)& 0x00ff0000)|\
    ((((UINT32)((UINT8)c[2]))<< 8) & 0x0000ff00)|\
    (((UINT32)((UINT8)c[3])) & 0x000000ff);\
} while(0)

/* 转换short到网络字节序，并放入包中 */
#define RSE_PUTPKT_SHORT(value,dest)\
do\
{\
    s2n(value,dest);\
} while(0)

/* 转换long到网络字节序，并放入包中 */
#define RSE_PUTPKT_LONG(value,dest)\
do\
{\
   l2n(value,dest);\
} while(0)

/* 把BOOL值写入包中 */
#define RSE_PUTPKT_BOOL(value, dest, printlen)\
do\
{\
    if(0 == value)\
    {\
        printlen = sprintf(dest,"%s","FALSE");\
    }\
    else\
    {\
        printlen = sprintf(dest,"%s","TRUE");\
    }\
    dest += (printlen+1);\
} while(0)

/* 取得类型id */
#define RSE_GET_TYPE(inbuf,typeId)\
do\
{\
     n2s(inbuf,typeId);\
} while(0)

/* 判定写入数据是否超过包长度 */
#define RSE_CHECK_OVER_PKTSIZE(ptr,outbuf,curlen)\
if(((ptr) - (outbuf) +(curlen)) > (TA_PKT_DATA_SIZE - 1))\
{\
  break;\
}\

/************************类型定义*****************************/

typedef enum
{
    /* 对象操作ID定义 */
    RSE_OPR_QUERY = 0x0000,
    RSE_OPR_DEL = 0x0001,
    RSE_OPR_SUSPEND = 0x0002,
    RSE_OPR_RESTART = 0x0003,
    RSE_OPR_RESUME = 0x0004,
    RSE_OPR_STOP = 0x0005,
    RSE_OPR_START = 0x0006,
    RSE_OPR_PROTECT = 0x0007,
    RSE_OPR_RELEASE_PROTECT = 0x0008,

    /* 对象显示Label ID定义 */
    RSE_LABEL_TASK = 0x0100,
    RSE_LABEL_INTERRUPT = 0x0101,
    RSE_LABEL_SEMA = 0x0102,
    RSE_LABEL_MSGQ = 0x0103,
    RSE_LABEL_TIMER = 0x0104,
    RSE_LABEL_MEMHEAP = 0x0105,
    RSE_LABEL_EXTERINFO = 0x0106,
    RSE_LABEL_MEMPARTITION = 0x0107,
    RSE_LABEL_BLKBOARD = 0x0108,
    RSE_LABEL_FLAGSET = 0x0109,
    RSE_LABEL_RINGBUF = 0x010A,
    RSE_LABEL_CPU = 0x010B,
    RSE_LABEL_IMAGE = 0x010C,
    RSE_LABEL_STORY = 0x010D,
    RSE_LABEL_APPIMAGE = 0x010E,
    RSE_LABEL_OSIMAGE = 0x010F,
    RSE_LABEL_DLLIMAGE = 0x0111,
    RSE_LABEL_DATAIMAGE = 0x0112,
    RSE_LABEL_BINARY_SEMA = 0x0113,
    RSE_LABEL_COUNT_SEMA = 0x0114,
    RSE_LABEL_APERIODIC_TASK = 0x0115,
    RSE_LABEL_PERIODIC_TASK = 0x0116,
    RSE_LABEL_FREEBLK = 0x0117,
    RSE_LABEL_SIMPLE_BINARY_SEMA = 0x0118,
    RSE_LABEL_EXCEPTION = 0x0119,

    /* 对象类型ID定义 */
    RSE_TYPE_ROOT = 0x0300,
    RSE_TYPE_OS = 0x400,
    RSE_TYPE_TASK_MODULE = 0x0500,
    RSE_TYPE_APERIODIC_TASK_MODULE = 0x0501,
    RSE_TYPE_PERIODIC_TASK_MODULE = 0x0502,
    RSE_TYPE_KERNEL_TASK = 0x0503,
    RSE_TYPE_PERIODIC_TASK = 0x0504,
    RSE_TYPE_INTERRUPT_MODULE = 0x600,
    RSE_TYPE_INTERRUPT = 0x0601,
    RSE_TYPE_SEMA_MODULE = 0x700,
    RSE_TYPE_BINARY_SEMA_MODULE = 0x0701,
    RSE_TYPE_COUNT_SEMA_MODULE = 0x0702,
    RSE_TYPE_BINARY_SEMA = 0x0703,
    RSE_TYPE_COUNT_SEMA = 0x0704,
    RSE_TYPE_SIMPLE_BINARY_SEMA_MODULE = 0x0705,
    RSE_TYPE_SIMPLE_BINARY_SEMA = 0x0706,
    RSE_TYPE_MSGQ_MODULE = 0x800,
    RSE_TYPE_MSGQ = 0x0801,
    RSE_TYPE_TIMER_MODULE = 0x900,
    RSE_TYPE_TIMER = 0x0901,
    RSE_TYPE_MEMHEAP_MODULE = 0xA00,
    RSE_TYPE_MEMHEAP = 0x0A01,
    RSE_TYPE_FREEBLK = 0x0A02,
    RSE_TYPE_EXTERINFO_MODULE = 0xB00,
    RSE_TYPE_EXTERINFO = 0x0B01,
    RSE_TYPE_MEMPARTITION_MODULE = 0x0C00,
    RSE_TYPE_MEMPARTITION = 0x0C01,
    RSE_TYPE_BLKBOARD_MODULE = 0x0D00,
    RSE_TYPE_BLKBOARD = 0x0D01,
    RSE_TYPE_FLAGSET_MODULE = 0x0E00,
    RSE_TYPE_FLAGSET = 0x0E01,
    RSE_TYPE_RINGBUF_MODULE = 0x0F00,
    RSE_TYPE_RINGBUF = 0x0F01,
    RSE_TYPE_CPU_MODULE = 0x1000,
    RSE_TYPE_CPU = 0x1001,
    RSE_TYPE_IMAGE_MODULE = 0x1100,
    RSE_TYPE_APPIMAGE_MODULE = 0x1101,
    RSE_TYPE_APPIMAGE = 0x1102,
    RSE_TYPE_OSIMAGE_MODULE = 0x1103,
    RSE_TYPE_OSIMAGE = 0x1104,
    RSE_TYPE_DLLIMAGE_MODULE = 0x1105,
    RSE_TYPE_DLLIMAGE = 0x1106,
    RSE_TYPE_DATAIMAGE_MODULE = 0x1107,
    RSE_TYPE_DATAIMAGE = 0x1108,
    RSE_TYPE_SEGMENT = 0x1109,
    RSE_TYPE_RTP =     0x1200,
    RSE_TYPE_RTP_LIST = 0x1201,
    RSE_TYPE_RTP_INFO = 0x1202,
    RSE_TYPE_RTP_TASK_LIST = 0x1203,
    RSE_TYPE_RTP_SHL_LIST = 0x1204,

    /* 组ID定义 */
    RSE_MODULE_BASE_ATTR = 0x4000,
    RSE_MODULE_PRI_ATTR = 0x4001,
    RSE_MODULE_COUNT_ATTR = 0x4002,
    RSE_MODULE_STACK_ATTR = 0x4003,
    RSE_MODULE_TIMESLICE_ATTR = 0x4004,
    RSE_MODULE_SIGNAL_ATTR = 0x4005,
    RSE_MODULE_WAITOBJ_ATTR = 0x4006,
    RSE_MODULE_PERI_ATTR = 0x4007,
    RSE_MODULE_EXTENSION_ATTR = 0x4008,

    /* 对象属性ID定义 */
    RSE_ATTR_ID = 0x8000,
    RSE_ATTR_NAME = 0x8001,
    RSE_ATTR_STACK_SIZE = 0x8002,
    RSE_ATTR_STACK_CURP = 0x8003,
    RSE_ATTR_STACK_BEGINADDR = 0x8004,
    RSE_ATTR_STACK_ENDADDR = 0x8005,
    RSE_ATTR_STACK_USE = 0x8006,
    RSE_ATTR_ISR_LEVEL = 0x8007,
    RSE_ATTR_PREEMPTIBLE = 0x8008,
    RSE_ATTR_ASR = 0x8009,
    RSE_ATTR_FLOAT_POINT = 0x800A,
    RSE_ATTR_SUSPEND_COUNT = 0x800B,
    RSE_ATTR_PROTECT_STATE = 0x800C,
    RSE_ATTR_TIME_BUDGET = 0x800D,
    RSE_ATTR_INI_TIME_BUGET = 0x800E,
    RSE_ATTR_ENTRY = 0x800F,
    RSE_ATTR_INI_PRIO = 0x8010,
    RSE_ATTR_REAL_PRIO = 0x8011,
    RSE_ATTR_RESOURCE_COUNT = 0x8012,
    RSE_ATTR_TASK_DELHOOK = 0x8013,
    RSE_ATTR_CPU_TICK = 0x8014,
    RSE_ATTR_WAIT_OBJECT = 0x8015,
    RSE_ATTR_WAIT_EVENT = 0x8016,
    RSE_ATTR_SIGNAL_HANDLER = 0x8017,
    RSE_ATTR_ISR_ENABLE = 0x8018,
    RSE_ATTR_STATE = 0x8019,
    RSE_ATTR_PERI_STATE = 0x801A,
    RSE_ATTR_PERI_TICKS = 0x801B,
    RSE_ATTR_PERI_EXECTICKS = 0x801C,
    RSE_ATTR_OWNERID = 0x801D,
    RSE_ATTR_PERI_INTERVAL = 0x801E,
    RSE_ATTR_PERI_DEADLINE = 0x801F,
    RSE_ATTR_DEADLINE_ENTRY = 0x8020,
    RSE_ATTR_DEADLINE_TIMEOUT = 0x8021,
    RSE_ATTR_PERI_SYNTICKS = 0x8022,
    RSE_ATTR_ISR_VECTOR = 0x8023,
    RSE_ATTR_ISR_HANDLE = 0x8024,
    RSE_ATTR_SEMA_ATTRIBUTE = 0x8025,
    RSE_ATTR_SEMA_LOCK = 0x8026,
    RSE_ATTR_NEST_COUNT = 0x8027,
    RSE_ATTR_HOLDER_ID = 0x8028,
    RSE_ATTR_ONLY_OWNER_RLEASE = 0x8029,
    RSE_ATTR_CEILING_PRIO = 0x802A,
    RSE_ATTR_DISCIPLINE = 0x802B,
    RSE_ATTR_WAIT_TASK_COUNT = 0x802C,
    RSE_ATTR_WAIT_TASK_LIST = 0x802D,
    RSE_ATTR_COUNTSEMA_COUNT = 0x802E,
    RSE_ATTR_MSGQ_ATTRIBUTE = 0x802F,
    RSE_ATTR_MSGQ_MAXNUM = 0x8030,
    RSE_ATTR_MSGQ_MAXSIZE = 0x8031,
    RSE_ATTR_MSGQ_PENDING = 0x8032,
    RSE_ATTR_TIMER_TYPE = 0x8033,
    RSE_ATTR_TIMER_INITIAL = 0x8034,
    RSE_ATTR_TIMER_REMAINTIME = 0x8035,
    RSE_ATTR_TIMER_START = 0x8036,
    RSE_ATTR_TIMER_HANDLER = 0x8037,
    RSE_ATTR_TIMER_USR_DATA = 0x8038,
    RSE_ATTR_TIMER_REPEAT = 0x8039,
    RSE_ATTR_TIMER_REPEAT_REMAIN = 0x803A,
    RSE_ATTR_MEMHEAP_TOTALSIZE = 0x803B,
    RSE_ATTR_MEMHEAP_USESIZE = 0x803C,
    RSE_ATTR_MEMHEAP_USEBLKS = 0x803D,
    RSE_ATTR_MEMHEAP_FREESIZE = 0x803E,
    RSE_ATTR_MEMHEAP_MALLOC_TIMES = 0x803F,
    RSE_ATTR_MEMHEAP_FREE_TIMES = 0x8040,
    RSE_ATTR_MEMHEAP_ACCUMFREEBLKS = 0x8041,
    RSE_ATTR_MEMHEAP_MALLOCSIZE = 0x8042,
    RSE_ATTR_MEMHEAP_MALLOCBLKS = 0x8043,
    RSE_ATTR_MEMHEAP_MAXMALLOCSIZE = 0x8044,
    RSE_ATTR_MEMHEAP_MAXFREESIZE = 0x8045,
    RSE_ATTR_MEMHEAP_MALLOCFAIL = 0x8046,
    RSE_ATTR_MEMHEAP_FREEFAIL = 0x8047,
    RSE_ATTR_MEMPAR_BENGINADDR = 0x8048,
    RSE_ATTR_MEMPAR_LENGTH = 0x8049,
    RSE_ATTR_MEMPAR_SIZE = 0x804A,
    RSE_ATTR_MEMPAR_ATTR = 0x804B,
    RSE_ATTR_MEMPAR_USEDBUF = 0x804C,
    RSE_ATTR_BLKBOARD_ATTR = 0x804D,
    RSE_ATTR_BENGINADDR = 0x804E,
    RSE_ATTR_BUFSIZE = 0x804F,
    RSE_ATTR_BLKBOARD_DATASIZE = 0x8050,
    RSE_ATTR_FLAGSET_ATTR = 0x8051,
    RSE_ATTR_FLAGSET = 0x8052,
    RSE_ATTR_RINGBUF_ATTR = 0x8053,
    RSE_ATTR_RINGBUF_BENGINADDR = 0x8054,
    RSE_ATTR_RINGBUF_FREESIZE = 0x8055,
    RSE_ATTR_RINGBUF_EMPTY = 0x8056,
    RSE_ATTR_RINGBUF_FULL = 0x8057,
    RSE_ATTR_TASK_CREATEXTEN = 0x8058,
    RSE_ATTR_TASK_STARTEXTEN = 0x8059,
    RSE_ATTR_TASK_RESTARTEXTEN = 0x805A,
    RSE_ATTR_TASK_DELEXTEN = 0x805B,
    RSE_ATTR_TASK_SWITCHEXTEN = 0x805C,
    RSE_ATTR_TASK_BEGINEXTEN = 0x805D,
    RSE_ATTR_TASK_EXITEXTEN = 0x805E,
    RSE_ATTR_TASK_FATALEXTER = 0x805F,
    RSE_ATTR_CPU_TOTALTICKS = 0x8060,
    RSE_ATTR_CPU_USEPERCENT = 0x8061,
    RSE_ATTR_CPU_IDLEPERCENT = 0x8062,
    RSE_ATTR_TASK_TYPE = 0x8063,
    RSE_ATTR_MODULE_SIZE = 0x8064,
    RSE_ATTR_MODULE_START_ADDR = 0x8065,
    RSE_ATTR_MODULE_STATE = 0x8066,
    RSE_ATTR_MODULE_TOSID = 0x8067,
    RSE_ATTR_MODULE_TYPE = 0x8068,
    RSE_ATTR_MODULE_PATH = 0x8069,
    RSE_ATTR_MODULE_TEXT_ADDR = 0x806a,
    RSE_ATTR_DEBUG_MODE = 0x8070,
    RSE_ATTR_ENABLED_CPU =0x8071
}RSE_ID;

/************************接口声明*****************************/

/**
 * @Brief
 *     获取OS中使能的CPU
 * @return
 *     成功返回CPU使能
 */
cpuset_t taCpuEnabledGet (void);

/*
 * @brief:
 *     根据任务ID获取任务名
 * @param[in]:taskID:任务ID
 * @return:
 *     OK:获取任务名成功
 *     ERROR:获取任务名失败
 */
char *taTaskName(int taskID);

/*
 * @brief:
 *     获取系统中任务列表
 * @param[out]:idList:任务ID列表
 * @param[in]:maxTasks:任务列表能够容纳的最大任务数
 * @return:
 *     任务列表中的任务个数
 */
int taTaskIdListGet(int idList[],int maxTasks);

/*
 * @brief
 * 		截断RTP路径，获取RTP名
 * @return
 *      无
 */
void rsePathTruncate(char *path, char *name, UINT32 maxNameLength);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RSE_CONFIG_H */
