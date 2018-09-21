/*
* 更改历史：
* 2004-12-6  董洪涛  北京科银技术有限公司
*                    创建该文件。
* 2004-12-07 唐兴培  北京科银技术有限公司
*                    评审该文件。
*/

/**
* @file  coreCmdProcess.h
* @brief
*       功能：
*       <li>核心OS命令处理类定义</li>
*/ 

#ifndef _CORECMDPROCESS_H
#define _CORECMDPROCESS_H

/************************头文件********************************/
#include <windows.h>
#include "common.h"
#include "shellMain.h"
#include "..//tsApi//tclientapi.h"
#include <iostream>
#include <string>
#include <list>

/************************宏定义********************************/
#define TA_TASK_SHELLAGENT_RESEARCH        (9)

#define TA_SYS_DEBUGER        (1)

#define MAX_ACCESS_MEM_UNITS         (400)

#define EACH_LINE_PRINT_UNITS      (16)

#define DEFAULT_DISPLAY_MEM_UNITS    (128)

#define IMG_NAME_LENGTH (48)

#define OBJECTS_NAME_LENGTH   (32)

#define RECV_TASK_INFO_END_STR  ("OK:END")

/*目标机端MANAGER的代理号*/
#define TA_SUBA_MANAGER     (0)


/* 任务状态值定义 */
#define STATES_READY                            0x00000000 /* 就绪态 */
#define STATES_DORMANT                          0x00000001 /* 休眠态 */
#define STATES_SUSPENDED                        0x00000002 /* 挂起态*/
#define STATES_TRANSIENT                        0x00000004 /* 过渡状态 */
#define STATES_DELAYING                         0x00000008 /* 延时态 */
#define STATES_WAITING_FOR_TIME                 0x00000010 /* 时间等待状态 */

#define STATES_WAITING_FOR_MESSAGE              0x00000080 /* 等待接收或者发送消息 */
#define STATES_WAITING_FOR_EVENT                0x00000100 /* 等待时间 */
#define STATES_WAITING_FOR_SEMAPHORE            0x00000200 /* 等待信号量 */
#define STATES_WAITING_FOR_MUTEX                0x00000400 /* 等待互斥量*/

#define STATES_DEBUG_STOP                       0x80000000 /* 调试停止状态 */                                               
#define STATES_EXCEPTION_STOP                   0x40000000 /* 异常停止状态 */

/* 处于停止队列 */
#define STATES_ON_STOP_LIST  (STATES_DEBUG_STOP|STATES_EXCEPTION_STOP)

/* 处于队列等待的等待状态 */
#define STATES_WAITING_ON_TASK_QUEUE (STATES_WAITING_FOR_MESSAGE            | \
                                 STATES_WAITING_FOR_SEMAPHORE          | \
                                 STATES_WAITING_FOR_MUTEX )

/* 阻塞状态 */
#define STATES_BLOCKED         ( STATES_DELAYING                | \
                                 STATES_WAITING_FOR_TIME        | \
                                 STATES_WAITING_FOR_EVENT | \
                                 STATES_WAITING_ON_TASK_QUEUE)

#define READ_MEMORY_STR            ("d")
#define WRITE_MEMORY_STR           ("m")
#define SET_USING_DOMAIN_STR       (":")
#define LIST_ALL_TASK_STR          ("i")
#define LIST_APPOINT_TASK_STR      ("ti")
#define STOP_TASK_STR              ("ts")
#define RESUME_TASK_STR            ("tr")
#define GET_TASK_ID_STR            ("taskIdFigure")
#define SHOW_PD_INFO_STR		   ("pdShow")
#define SETUP_DEBUG_EXP_STR	       ("exp")
#define DETACHTASK_STR			   ("detachTask")
#define	DETACHPARTITION_STR        ("detachPartition")
#define DETACHSYSTEM_STR           ("detachSystem")
/*
	定义操作系统版本的名称字符串
*/
#define OS_1X_VERSION "OS1"
#define OS_2X_VERSION "OS2"


#define swapMemUnit16Bit(mem)\
{\
    char temp = 0;\
    do\
    {\
        temp=mem[0];\
        mem[0] = mem[2];\
        mem[2] = temp;\
;\
        temp=mem[1];\
        mem[1] = mem[3];\
        mem[3] = temp;\
    }while(0);\
}

#define swapMemUnit32Bit(mem)\
{\
    char temp = 0;\
    do\
    {\
        temp=mem[0];\
        mem[0] = mem[6];\
        mem[6] = temp;\
;\
        temp=mem[1];\
        mem[1] = mem[7];\
        mem[7] = temp;\
;\
        temp=mem[2];\
        mem[2] = mem[4];\
        mem[4] = temp;\
;\
        temp=mem[3];\
        mem[3] = mem[5];\
        mem[5] = temp;\
    }while(0);\
}

//**添加内存不能访问的时候显示N/A字符
#define putUnitBase(memUnit,width)\
{\
    do\
    {\
		bool memeryNAFlags = false; \
		if(*memUnit == 'N' && *(memUnit+1) == 'A'){ \
			memeryNAFlags = true; \
		} \
        putchar(*(memUnit)++);\
		if(memeryNAFlags){ \
			putchar('/');	\
		}\
        putchar(*(memUnit)++);\
        (width)--;\
    }while( (width) > 0 );\
}


#define putMemUnit(memUnit, width)\
{\
    int widthtmp = width;\
    do\
    {\
        if(1 == widthtmp)\
        {\
            putUnitBase(memUnit,widthtmp);\
        }\
        else if(2 == widthtmp)\
        {\
            swapMemUnit16Bit(memUnit);\
            putUnitBase(memUnit,widthtmp);\
        }\
        else if(4 == widthtmp)\
        {\
            swapMemUnit32Bit(memUnit);\
            putUnitBase(memUnit,widthtmp);\
        }\
    }while(0);\
 }


#define putBlankUnit(width)\
{\
    int widthtmp = width;\
    do\
    {\
        putchar(' ');\
        putchar(' ');\
        (widthtmp)--;\
    }while( (widthtmp) > 0 );\
 }


#define putBlankStr(printunit,width)\
{\
    putchar(' ');\
    (printunit) -= (width);\
 }



#define printTaskInfoLable()\
{\
    printf("            NAME                     ID        ENTRY   PRI PRE TIM FLO  STATUS\n");\
    printf("-------------------------------- ---------- ---------- --- --- --- --- --------\n");\
}

#define printSingleTaskInfo(pinfo)\
{\
    printf("%-32s 0x%-8x 0x%-8x %-3d ",\
        pinfo.name, pinfo.task_id, pinfo.entry, pinfo.real_priority);\
    if(0 == pinfo.preemptible)\
    {\
        printf("No  ");\
    }\
    else\
    {\
        printf("Yes ");\
    }\
    if(0 == pinfo.timeslice)\
    {\
        printf("No  ");\
    }\
    else\
    {\
        printf("Yes ");\
    }\
    if(0 == pinfo.floating_point)\
    {\
        printf("No  ");\
    }\
    else\
    {\
        printf("Yes ");\
    }\
}

#define printPdInfoLable()\
{\
    printf("         NAME                 ID          TYPE    START_ADRS  SIZE  L_PRI H_PRI\n");\
    printf("-------------------------- ---------- ----------- ---------- ---------- --- ---\n");\
}

#define printSingleParttitionInfo(name, partitionid, type, startAddr, size, lowPriority, highPriority)\
{\
    printf("%-26s 0x%-8x %-11d 0x%-8x 0x%-8x %-3d %-3d ",\
    name, partitionid, type, startAddr, size, lowPriority, highPriority);\
}



#define printSingleTaskStatus(status)\
{\
    if((state) & STATES_ON_STOP_LIST)\
    {\
        printf("Stopped\n");\
    }\
    else if((state) & STATES_BLOCKED)\
    {\
        printf("Blocked\n");\
    }\
    else if(0 == ((state) & (0X0000000f)))\
    {\
        printf("Ready\n");\
    }\
    else if((state) & STATES_SUSPENDED)\
    {\
        printf("Suspend\n");\
    }\
    else if((state) & STATES_DORMANT)\
    {\
        printf("Dormant\n");\
    }\
    else\
    {\
        printf("Normal\n");\
    }\
}



#define printSingleTaskDetailStatus(status)\
{\
    if((state) & STATES_ON_STOP_LIST)\
    {\
        if((state) & STATES_DEBUG_STOP)\
        {\
            printf("Debug Stopped");\
        }\
        else if((state) & STATES_EXCEPTION_STOP)\
        {\
            printf("Exception Stopped");\
        }\
    }\
    else if((state) & STATES_BLOCKED)\
    {\
        if((state) & STATES_DELAYING)\
        {\
            printf("Delaying");\
        }\
        else if((state) & STATES_WAITING_FOR_TIME)\
        {\
            printf("Waiting for time");\
        }\
        else if((state) & STATES_WAITING_FOR_EVENT)\
        {\
            printf("Waiting for event");\
        }\
        else if((state) & STATES_WAITING_FOR_MESSAGE)\
        {\
            printf("Waiting for message");\
        }\
        else if((state) & STATES_WAITING_FOR_SEMAPHORE)\
        {\
            printf("Waiting for semaphore");\
        }\
        else if((state) & STATES_WAITING_FOR_MUTEX)\
        {\
            printf("Waiting for mutex");\
        }\
    }\
    else if(0 == ((state) & (0X0000000f)))\
    {\
        printf("Ready");\
    }\
    else if((state) & STATES_SUSPENDED)\
    {\
        printf("Suspend");\
    }\
    else if((state) & STATES_DORMANT)\
    {\
        printf("Dormant");\
    }\
}

#define printSingleTaskStackInfo(pinfo)\
{\
    printf("stack: base:0x%x    end:0x%x    size:%d    used size:%d\n\n",\
        (pinfo).stackBase,\
        ((pinfo).stackBase - (pinfo).stackSize),\
        (pinfo).stackSize,\
        (pinfo).stackUseSize);\
}


#define printSingleTaskOtherInfo(pinfo)\
{\
    printf("other: isr level:%d    time budget:%d    initial time budget:%d\n",\
        (pinfo).isr_level,\
        (pinfo).time_budget,\
        (pinfo).initial_time_budget);\
    printf("       initial priority:%d    resource count:%d    use cpu time:%d\n\n",\
        (pinfo).initial_priority,\
        (pinfo).resource_count,\
        (pinfo).cpuTime);\
}

/************************类型定义******************************/

typedef enum _accessMemoryWidth
{
    Width_8Bit =  1,
    Width_16Bit = 2,
    Width_32Bit = 4,
} accessMemoryWidth;

typedef enum _coreCmdLableEnum
{
	END_FLAGS_ENUM = -1,
	CORE_ERROR_ARGV = -2,
    NOT_DEFINE_CMD = 0,
    READ_MEMORY = 1,
    WRITE_MEMORY,
    SET_USING_DOMAIN,
    LIST_ALL_TASK,
    LIST_APPOINT_TASK,
    STOP_TASK,
    RESUME_TASK,
    GET_TASK_ID,
    SHOW_PD_INFO,
    SETUP_DEBUG_EXP,
	DETACHTASK,
	DETACHPARTITION,
	DETACHSYSTEM,
}coreCmdLableEnum;



typedef struct 
{
      /* 任务ID */
      unsigned int task_id;

      /* 任务名字 */
      char name[OBJECTS_NAME_LENGTH+1] ;
      
      /* 当任务执行时中断有效等级 */
      unsigned int isr_level; 
      
      /* 任务是否为可抢占 */
      unsigned int  preemptible;
      
      /* 任务是否支持时间片 */
      unsigned int   timeslice;
      
      /* 是否支持浮点 */
      unsigned int  floating_point;

      /* CPU时间预算 */
      unsigned int time_budget;
      
      /* 初始CPU时间预算 */
      unsigned int initial_time_budget;
      
      /* 任务入口点 */
      unsigned int entry;
      
      /* 任务初始优先级 */
      unsigned int  initial_priority;
      
      /* 任务实际优先级 */
      unsigned int  real_priority;
      
      /* 等待资源数 */
      unsigned int resource_count;
      
      /* 任务状态 */
      unsigned int  state;
      
      /* 栈空间大小 */
      unsigned int stackSize;
      
      /* 栈起始地址 */
      unsigned int  stackBase;
     
      /* 使用栈大小 */
      unsigned int stackUseSize;
      
      /* 任务使用CPU时间*/
      unsigned long long cpuTime;         
}ACoreOs_task_information;



typedef bool (*pCheckCoreCmdSubArgv)(string& cmd);
typedef struct _coreCmdTableStruct
{
    coreCmdLableEnum cmdLable;           //命令序号
    char* cmdStr;     //命令关键字
	pCheckCoreCmdSubArgv checkSubArgv;/* 二级参数检查函数指针，若当前指针为空则表示当前无二级参数 */
}coreCmdTableStruct;



class coreCmdProcess
{
public:
    coreCmdProcess();

    ~coreCmdProcess();
	

    /* 插入待发送命令到核心操作系统解析器 */
    int insertData(string cmd);
  
    /* 发送数据包到目标机shell代理 */
    int sendDataToShellAgent(const char* pBuf, int size);

    /* 查找命令表 */
    coreCmdLableEnum getCmdLable(string& cmd);

    /* 数据处理函数 */
    int processData();

    /* 从ts收包 */
    int recvDataFromTs(char* pBuf, int size ,int pTimeOut = -1); 

    /*查询OS和shell代理是否启动*/
    int queryTargetAgent();



    /* 读内存操作 */
    int m_readMemory(string cmd);

    /* 写内存操作 */
    int m_writeMemory(string cmd);

    /* 设置内存访问的分区 */
    int m_setDomain(string cmd);

    /* 获取任务信息 */
    int m_listAllTaskInfo(string cmd);

    /* 获取指定任务信息 */
    int m_listAppointTaskInfo(string cmd);

    /* 挂起指定任务  */
    int m_suspendTask(string cmd);

    /* 解挂指定任务 */
    int m_resumeTask(string cmd);

    /* 根据任务名称获取任务ID  */
    int m_getTaskID(string cmd);

    /*获取分区信息*/
    int m_showPdInfo(string cmd);

    /* 设置接管单个调试异常*/
    int m_setupDbExp(string cmd);

    


    /* 设置默认内存操作长度 */
    int setDefaultControlMemWidth(unsigned int width);

    /* 基本读内存操作单元 */
    int readMemUnit(unsigned int addr, int size, char* pbuf, int bufsize);

    /* 基本读内存操作单元 */
    int writeMemUnit(unsigned int addr, int writesize, char* pwrtbuf, int bufsize);
    
    /* 按指定格式显示内存单元 */
    int showReadMemUnit(int addr, char* pbuf,unsigned int readSize);

    /* 写内存操作 */
    int writeMemLoop(unsigned int addr);
    
    /*将一块内存区中十六进制字符串转换成对应的内容保存在指定的内存位置中。*/
    void hex2memEx(char *buf, int count);

    /* 解析任务信息 */
    int analysisTaskInfo(char* buf, ACoreOs_task_information* pinfo);
	
    /* 解析分区信息 */
    int analysisParttitionInfo(char* buf, int *id, UINT8 *name, int *type, UINT32 *startAddr, UINT32 *size, int *lowPriority, int *highPriority);
    
    void stop();

	/*
		按照最小的内存块对齐单元16字节读取内存地址，并返回内存中无法访问的地址
	*/
	bool autoReadBigBlockData(unsigned int readMemoryAddr,unsigned int currentReadSize,unsigned int& readErrAddr,string& saveMsgBuf);

	/*
		查询shell代理是否处于空闲状态
		返回值：
			true:空闲状态
			false:忙状态
	*/
	bool checkTargetShellAgentIsIdle();

	/*
		获取当前是否需要检查shellAgent是否空闲
		返回值:
			ture:需要
			false:不需要
	*/

	bool isCheckShellAgentIsIdle() const;
	
	/*
		针对1X释放所有绑定的任务
	*/
	int detachTask();
	
	/*
		针对2x释放绑定的分区名称
	*/
	int detachPartition(string cmd);

	/*
		针对1x,2x解绑指定的系统调试
	*/
	int detachSystem(string cmd);
	
private:
	/*
		针对1X解绑系统调试
	*/
	int detachSystem1x(string cmd);
	/*
		针对2X解绑系统调试
	*/
	int detachSystem2x(string cmd);
	/*
		针对2X中查找指定的分区是否存在
	*/
	bool checkPatitionIsExist(string patitionName); 

    bool runFlag;

    /* 按要求对齐后实际发送的读内存值 */
    unsigned int m_readMemorySend;

    /* 默认读写内存操作大小 */
    unsigned int m_defaultControlMemUnits;
    
    accessMemoryWidth m_defaultControlMemWidth;

    TMutex coreCmdListMtx;

	//设置当前是否需要检查shellAgent是否空闲
	bool m_bCheckShellAgentIsIdle;
    /* 待发送消息包列表 */
    list<string> coreCmdList;

	/*
		当前目标机操作系统的版本号
	*/
	string m_osVersion;
public:
	/*
	查询shell代理是否启动
	*/
	int queryTargetShellAgent();

	/*
	查询OS状态是否启动
	*/
	int queryTargetAgentOsStauts();

	/*
	查询OS版本
	*/
	int queryTargetAgentOsVersion();

	/*
		获取OS版本
	*/
	string getOSVersion() const{
		return m_osVersion;
	}

};

/************************接口声明******************************/

#endif

