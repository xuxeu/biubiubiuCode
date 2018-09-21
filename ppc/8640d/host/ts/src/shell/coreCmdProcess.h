/*
* ������ʷ��
* 2004-12-6  ������  ���������������޹�˾
*                    �������ļ���
* 2004-12-07 ������  ���������������޹�˾
*                    ������ļ���
*/

/**
* @file  coreCmdProcess.h
* @brief
*       ���ܣ�
*       <li>����OS������ඨ��</li>
*/ 

#ifndef _CORECMDPROCESS_H
#define _CORECMDPROCESS_H

/************************ͷ�ļ�********************************/
#include <windows.h>
#include "common.h"
#include "shellMain.h"
#include "..//tsApi//tclientapi.h"
#include <iostream>
#include <string>
#include <list>

/************************�궨��********************************/
#define TA_TASK_SHELLAGENT_RESEARCH        (9)

#define TA_SYS_DEBUGER        (1)

#define MAX_ACCESS_MEM_UNITS         (400)

#define EACH_LINE_PRINT_UNITS      (16)

#define DEFAULT_DISPLAY_MEM_UNITS    (128)

#define IMG_NAME_LENGTH (48)

#define OBJECTS_NAME_LENGTH   (32)

#define RECV_TASK_INFO_END_STR  ("OK:END")

/*Ŀ�����MANAGER�Ĵ����*/
#define TA_SUBA_MANAGER     (0)


/* ����״ֵ̬���� */
#define STATES_READY                            0x00000000 /* ����̬ */
#define STATES_DORMANT                          0x00000001 /* ����̬ */
#define STATES_SUSPENDED                        0x00000002 /* ����̬*/
#define STATES_TRANSIENT                        0x00000004 /* ����״̬ */
#define STATES_DELAYING                         0x00000008 /* ��ʱ̬ */
#define STATES_WAITING_FOR_TIME                 0x00000010 /* ʱ��ȴ�״̬ */

#define STATES_WAITING_FOR_MESSAGE              0x00000080 /* �ȴ����ջ��߷�����Ϣ */
#define STATES_WAITING_FOR_EVENT                0x00000100 /* �ȴ�ʱ�� */
#define STATES_WAITING_FOR_SEMAPHORE            0x00000200 /* �ȴ��ź��� */
#define STATES_WAITING_FOR_MUTEX                0x00000400 /* �ȴ�������*/

#define STATES_DEBUG_STOP                       0x80000000 /* ����ֹͣ״̬ */                                               
#define STATES_EXCEPTION_STOP                   0x40000000 /* �쳣ֹͣ״̬ */

/* ����ֹͣ���� */
#define STATES_ON_STOP_LIST  (STATES_DEBUG_STOP|STATES_EXCEPTION_STOP)

/* ���ڶ��еȴ��ĵȴ�״̬ */
#define STATES_WAITING_ON_TASK_QUEUE (STATES_WAITING_FOR_MESSAGE            | \
                                 STATES_WAITING_FOR_SEMAPHORE          | \
                                 STATES_WAITING_FOR_MUTEX )

/* ����״̬ */
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
	�������ϵͳ�汾�������ַ���
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

//**����ڴ治�ܷ��ʵ�ʱ����ʾN/A�ַ�
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

/************************���Ͷ���******************************/

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
      /* ����ID */
      unsigned int task_id;

      /* �������� */
      char name[OBJECTS_NAME_LENGTH+1] ;
      
      /* ������ִ��ʱ�ж���Ч�ȼ� */
      unsigned int isr_level; 
      
      /* �����Ƿ�Ϊ����ռ */
      unsigned int  preemptible;
      
      /* �����Ƿ�֧��ʱ��Ƭ */
      unsigned int   timeslice;
      
      /* �Ƿ�֧�ָ��� */
      unsigned int  floating_point;

      /* CPUʱ��Ԥ�� */
      unsigned int time_budget;
      
      /* ��ʼCPUʱ��Ԥ�� */
      unsigned int initial_time_budget;
      
      /* ������ڵ� */
      unsigned int entry;
      
      /* �����ʼ���ȼ� */
      unsigned int  initial_priority;
      
      /* ����ʵ�����ȼ� */
      unsigned int  real_priority;
      
      /* �ȴ���Դ�� */
      unsigned int resource_count;
      
      /* ����״̬ */
      unsigned int  state;
      
      /* ջ�ռ��С */
      unsigned int stackSize;
      
      /* ջ��ʼ��ַ */
      unsigned int  stackBase;
     
      /* ʹ��ջ��С */
      unsigned int stackUseSize;
      
      /* ����ʹ��CPUʱ��*/
      unsigned long long cpuTime;         
}ACoreOs_task_information;



typedef bool (*pCheckCoreCmdSubArgv)(string& cmd);
typedef struct _coreCmdTableStruct
{
    coreCmdLableEnum cmdLable;           //�������
    char* cmdStr;     //����ؼ���
	pCheckCoreCmdSubArgv checkSubArgv;/* ����������麯��ָ�룬����ǰָ��Ϊ�����ʾ��ǰ�޶������� */
}coreCmdTableStruct;



class coreCmdProcess
{
public:
    coreCmdProcess();

    ~coreCmdProcess();
	

    /* ���������������Ĳ���ϵͳ������ */
    int insertData(string cmd);
  
    /* �������ݰ���Ŀ���shell���� */
    int sendDataToShellAgent(const char* pBuf, int size);

    /* ��������� */
    coreCmdLableEnum getCmdLable(string& cmd);

    /* ���ݴ����� */
    int processData();

    /* ��ts�հ� */
    int recvDataFromTs(char* pBuf, int size ,int pTimeOut = -1); 

    /*��ѯOS��shell�����Ƿ�����*/
    int queryTargetAgent();



    /* ���ڴ���� */
    int m_readMemory(string cmd);

    /* д�ڴ���� */
    int m_writeMemory(string cmd);

    /* �����ڴ���ʵķ��� */
    int m_setDomain(string cmd);

    /* ��ȡ������Ϣ */
    int m_listAllTaskInfo(string cmd);

    /* ��ȡָ��������Ϣ */
    int m_listAppointTaskInfo(string cmd);

    /* ����ָ������  */
    int m_suspendTask(string cmd);

    /* ���ָ������ */
    int m_resumeTask(string cmd);

    /* �����������ƻ�ȡ����ID  */
    int m_getTaskID(string cmd);

    /*��ȡ������Ϣ*/
    int m_showPdInfo(string cmd);

    /* ���ýӹܵ��������쳣*/
    int m_setupDbExp(string cmd);

    


    /* ����Ĭ���ڴ�������� */
    int setDefaultControlMemWidth(unsigned int width);

    /* �������ڴ������Ԫ */
    int readMemUnit(unsigned int addr, int size, char* pbuf, int bufsize);

    /* �������ڴ������Ԫ */
    int writeMemUnit(unsigned int addr, int writesize, char* pwrtbuf, int bufsize);
    
    /* ��ָ����ʽ��ʾ�ڴ浥Ԫ */
    int showReadMemUnit(int addr, char* pbuf,unsigned int readSize);

    /* д�ڴ���� */
    int writeMemLoop(unsigned int addr);
    
    /*��һ���ڴ�����ʮ�������ַ���ת���ɶ�Ӧ�����ݱ�����ָ�����ڴ�λ���С�*/
    void hex2memEx(char *buf, int count);

    /* ����������Ϣ */
    int analysisTaskInfo(char* buf, ACoreOs_task_information* pinfo);
	
    /* ����������Ϣ */
    int analysisParttitionInfo(char* buf, int *id, UINT8 *name, int *type, UINT32 *startAddr, UINT32 *size, int *lowPriority, int *highPriority);
    
    void stop();

	/*
		������С���ڴ����뵥Ԫ16�ֽڶ�ȡ�ڴ��ַ���������ڴ����޷����ʵĵ�ַ
	*/
	bool autoReadBigBlockData(unsigned int readMemoryAddr,unsigned int currentReadSize,unsigned int& readErrAddr,string& saveMsgBuf);

	/*
		��ѯshell�����Ƿ��ڿ���״̬
		����ֵ��
			true:����״̬
			false:æ״̬
	*/
	bool checkTargetShellAgentIsIdle();

	/*
		��ȡ��ǰ�Ƿ���Ҫ���shellAgent�Ƿ����
		����ֵ:
			ture:��Ҫ
			false:����Ҫ
	*/

	bool isCheckShellAgentIsIdle() const;
	
	/*
		���1X�ͷ����а󶨵�����
	*/
	int detachTask();
	
	/*
		���2x�ͷŰ󶨵ķ�������
	*/
	int detachPartition(string cmd);

	/*
		���1x,2x���ָ����ϵͳ����
	*/
	int detachSystem(string cmd);
	
private:
	/*
		���1X���ϵͳ����
	*/
	int detachSystem1x(string cmd);
	/*
		���2X���ϵͳ����
	*/
	int detachSystem2x(string cmd);
	/*
		���2X�в���ָ���ķ����Ƿ����
	*/
	bool checkPatitionIsExist(string patitionName); 

    bool runFlag;

    /* ��Ҫ������ʵ�ʷ��͵Ķ��ڴ�ֵ */
    unsigned int m_readMemorySend;

    /* Ĭ�϶�д�ڴ������С */
    unsigned int m_defaultControlMemUnits;
    
    accessMemoryWidth m_defaultControlMemWidth;

    TMutex coreCmdListMtx;

	//���õ�ǰ�Ƿ���Ҫ���shellAgent�Ƿ����
	bool m_bCheckShellAgentIsIdle;
    /* ��������Ϣ���б� */
    list<string> coreCmdList;

	/*
		��ǰĿ�������ϵͳ�İ汾��
	*/
	string m_osVersion;
public:
	/*
	��ѯshell�����Ƿ�����
	*/
	int queryTargetShellAgent();

	/*
	��ѯOS״̬�Ƿ�����
	*/
	int queryTargetAgentOsStauts();

	/*
	��ѯOS�汾
	*/
	int queryTargetAgentOsVersion();

	/*
		��ȡOS�汾
	*/
	string getOSVersion() const{
		return m_osVersion;
	}

};

/************************�ӿ�����******************************/

#endif

