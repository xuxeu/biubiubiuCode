/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: armConst.h,v $
  * Revision 1.5  2008/03/19 10:25:11  guojc
  * Bug 编号  	0002430
  * Bug 名称 	ts启动后造成Windows任务栏凝固，无法响应快捷键
  *
  * 编码：唐兴培
  * 检视：卿孝海,郭建川
  *
  * Revision 1.1  2008/02/04 05:33:50  zhangxc
  * 设计报告名称：《ACoreOS CVS代码仓库管理》
  * 编写人员：张晓超
  * 检视人员：张宇旻
  * 其它：从现有最新ACoreOS仓库中取出最新的源代码，整理后提交到新的仓库目录结构中管理。
  * 第一次提交src模块。
  *
  * Revision 1.4  2007/04/20 07:10:50  guojc
  * 增加了对ICE DCC功能的支持
  * 增加了对目标板复位功能的支持
  * 增加了对内存宽度可配置的支持
  * 增加了对华邦模拟器调试兼容的支持
  *
  * Revision 1.3  2007/04/16 02:20:53  guojc
  * 修正ICE Server在协助semihosting上面出现的一些问题
  *
  * Revision 1.2  2007/04/02 02:56:53  guojc
  * 提交最新的IS代码，支持脱离于IDE使用，支持配置文件解析
  *
  * Revision 1.1  2006/11/13 03:40:56  guojc
  * 提交ts3.1工程源码,在VC8下编译
  *
  * Revision 1.2  2006/07/03 07:18:20  tanjw
  * 1.修正iceserver与gdb建立连接后执行目标机上电初始化过程中收到gdb命令返回出错的bug
  * 2.修正同时启动多个iceserver日志文件共享冲突的bug
  * 3.修正ts发送线程日志文件记录点.由发送后记录改为发送前记录
  *
  * Revision 1.1.1.1  2006/04/19 06:29:12  guojc
  * no message
  *
  * Revision 1.2  2006/04/14 02:02:27  guojc
  * 增加了对CP15协处理器寄存器修改的支持
  *
  * Revision 1.1.1.1  2006/04/06 03:18:50  zhangym
  * TS代码整理
  *
  * Reversion1.0 2006/02/20 09:00:00 guojc
  * 创建文件;定义文件的基本接口
  */

/**
* @file 	armConst.h
* @brief 	
*     <li>功能： ARM处理器常量类</li>
* @author 	郭建川
* @date 	2006-02-20
* <p>部门：系统部 
*/

#ifndef _ARM_CONST_H
#define _ARM_CONST_H


/************************************引用部分******************************************/
#include "sysTypes.h"
/************************************声明部分******************************************/


/************************************定义部分******************************************/

class RSP_ArmConst
{
public:
	
///处理器的处理模式
T_MODULE T_CONST T_WORD ARM_USER_MODE = 16;
T_MODULE T_CONST T_WORD ARM_FIQ_MODE = 17;
T_MODULE T_CONST T_WORD ARM_IRQ_MODE = 18;
T_MODULE T_CONST T_WORD ARM_SUPER_MODE = 19;
T_MODULE T_CONST T_WORD ARM_ABORT_MODE = 23;
T_MODULE T_CONST T_WORD ARM_UNDEFINED_MODE = 27;
T_MODULE T_CONST T_WORD ARM_SYSTEM_MODE = 31;

T_MODULE T_CONST T_WORD CORE_RUNNING = 0;        ///内核正常运行
T_MODULE T_CONST T_WORD CORE_DEBUG_DBGREQ = 1;   ///调试请求使内核停止
T_MODULE T_CONST T_WORD CORE_DEBUG_BKWTPT = 2;   ///断点,观察点使内核停止
T_MODULE T_CONST T_WORD CORE_DEBUG_WTPTECP = 3;  ///观察点with异常使内核停止
T_MODULE T_CONST T_WORD CORE_POWER_OFF = 4;      ///掉电状态
T_MODULE T_CONST T_WORD CORE_INVALID = 5;      ///	不确定状态


T_MODULE T_CONST T_WORD ARM_REG_NUM = 37;


//ARM37个寄存器
T_MODULE T_CONST T_WORD JTAG_R0	= 0;
T_MODULE T_CONST T_WORD JTAG_R1	= 1;
T_MODULE T_CONST T_WORD JTAG_R2	= 2;
T_MODULE T_CONST T_WORD JTAG_R3	= 3;
T_MODULE T_CONST T_WORD JTAG_R4	= 4;
T_MODULE T_CONST T_WORD JTAG_R5	= 5;
T_MODULE T_CONST T_WORD JTAG_R6	= 6;
T_MODULE T_CONST T_WORD JTAG_R7	= 7;
T_MODULE T_CONST T_WORD JTAG_R8	= 8;
T_MODULE T_CONST T_WORD JTAG_R9	= 9;
T_MODULE T_CONST T_WORD JTAG_R10 = 10;
T_MODULE T_CONST T_WORD JTAG_R11 = 11;
T_MODULE T_CONST T_WORD JTAG_R12 = 12;
T_MODULE T_CONST T_WORD JTAG_R13 = 13;				/* use this for the stack pointer */
T_MODULE T_CONST T_WORD JTAG_R14 = 14;				/* use this for link pointer	*/
T_MODULE T_CONST T_WORD JTAG_R15 = 15;				/* use this for the program counter */
T_MODULE T_CONST T_WORD JTAG_FIQ_R8 = 16;
T_MODULE T_CONST T_WORD JTAG_FIQ_R9 = 17;
T_MODULE T_CONST T_WORD JTAG_FIQ_R10 = 18;
T_MODULE T_CONST T_WORD JTAG_FIQ_R11 = 19;
T_MODULE T_CONST T_WORD JTAG_FIQ_R12	= 20;
T_MODULE T_CONST T_WORD JTAG_FIQ_R13	=	21;
T_MODULE T_CONST T_WORD JTAG_FIQ_R14	=	22;
T_MODULE T_CONST T_WORD JTAG_IRQ_R13	=	23;
T_MODULE T_CONST T_WORD JTAG_IRQ_R14	=	24;
T_MODULE T_CONST T_WORD JTAG_ABT_R13	=	25;
T_MODULE T_CONST T_WORD JTAG_ABT_R14	=	26;
T_MODULE T_CONST T_WORD JTAG_SVC_R13	=	27;		/* same as user mode */
T_MODULE T_CONST T_WORD JTAG_SVC_R14	=	28;
T_MODULE T_CONST T_WORD JTAG_UND_R13	=	29;
T_MODULE T_CONST T_WORD JTAG_UND_R14	=	30;
T_MODULE T_CONST T_WORD JTAG_CPSR		=	31;
T_MODULE T_CONST T_WORD JTAG_FIQ_SPSR	=	32;
T_MODULE T_CONST T_WORD JTAG_IRQ_SPSR	=	33;
T_MODULE T_CONST T_WORD JTAG_ABT_SPSR	=	34;
T_MODULE T_CONST T_WORD JTAG_SVC_SPSR	=	35;
T_MODULE T_CONST T_WORD JTAG_UND_SPSR	=	36;
T_MODULE T_CONST T_WORD JTAG_REG_CNTS	=	37;

T_MODULE T_CONST T_WORD RSP_REG_NUM = 26;  //rsp协议中寄存器的总数

//关于软件断点的thumb和arm态
T_MODULE T_CONST T_WORD  RSP_SB_ARM_STATE = 4;
T_MODULE T_CONST T_WORD  RSP_SB_THUMB_STATE = 2;


//命令序列
T_MODULE T_CONST T_WORD CMD_WRITE_MEM = 0;  ///写内存
T_MODULE T_CONST T_WORD CMD_READ_MEM = 1;  ///读内存
T_MODULE T_CONST T_WORD CMD_READ_REG = 2;  ///读寄存器
T_MODULE T_CONST T_WORD CMD_WRITE_REG = 3;  ///写寄存器
T_MODULE T_CONST T_WORD CMD_RUN_TRT = 4;  ///运行目标机
T_MODULE T_CONST T_WORD CMD_STOP_TRT = 5;  ///停止目标机
T_MODULE T_CONST T_WORD CMD_QUERY_TRT = 6;  ///查询目标机
T_MODULE T_CONST T_WORD CMD_INSERT_HBP = 7;  ///插入硬件断点
T_MODULE T_CONST T_WORD CMD_REMOVE_HBP = 8;  ///删除硬件断点
T_MODULE T_CONST T_WORD CMD_WRITE_INT_MEM = 9;  ///写整型内存
T_MODULE T_CONST T_WORD CMD_INSERT_WBP = 10;  ///插入观察点
T_MODULE T_CONST T_WORD CMD_REMOVE_WBP = 11;  ///删除观察点
T_MODULE T_CONST T_WORD CMD_INIT_CP15 = 12; //更新CP15寄存器
T_MODULE T_CONST T_WORD CMD_QUERY_ICEENDIAN = 13; //查询ICE大小端
/*ICE 3.2 增加[begin]*/
T_MODULE T_CONST T_WORD CMD_CREATE_SESSION = 14; //创建会话
T_MODULE T_CONST T_WORD CMD_DELETE_SESSION = 15; //删除会话
T_MODULE T_CONST T_WORD CMD_RESET_TARGET = 16; //重启目标机
T_MODULE T_CONST T_WORD CMD_QUERY_TARGET_CPUID = 17; //查询目标板CPUID
T_MODULE T_CONST T_WORD CMD_GET_CP15  = 18; //读取CP15寄存器
T_MODULE T_CONST T_WORD CMD_OPEN_DCC_FUNCTION = 19; //启用DCC功能
//T_MODULE T_CONST T_WORD CMD_DELETE_SESSION = 20; //删除会话
/*ICE3.2增加[end]*/

T_MODULE T_CONST T_WORD CMD_NEED_ICE = 100;  ///是否需要和ICE交互的界限
T_MODULE T_CONST T_WORD CMD_REPLY_WRITE_MEM = 101;  ///回复GDB写内存
T_MODULE T_CONST T_WORD CMD_REPLY_S05 = 102;  ///回复GDBS05
T_MODULE T_CONST T_WORD CMD_UPDATE_READ_REG = 103;  ///更新寄存器的值
T_MODULE T_CONST T_WORD CMD_UPDATE_READ_MEM = 104;  ///更新内存缓冲区的值
T_MODULE T_CONST T_WORD CMD_REPLY_READ_MEM = 105;  ///回复GDB读内存
T_MODULE T_CONST T_WORD CMD_REPLY_STOP_TRT = 106;  ///分析停止目标机结果
T_MODULE T_CONST T_WORD CMD_SET_TRT_STATUS = 107;  ///设置目标机状态
T_MODULE T_CONST T_WORD CMD_ANALY_RESULT = 108;  ///分析命令执行结果
T_MODULE T_CONST T_WORD CMD_DELAY = 109;  ///命令延迟，初始化时钟的时候需要
T_MODULE T_CONST T_WORD CMD_UPDATE_TRT_STATUS = 110;  ///根据查询目标机结果更新目标机状态，并推动流程
T_MODULE T_CONST T_WORD CMD_INSERT_SBP = 111;  ///插入软件断点
T_MODULE T_CONST T_WORD CMD_SIMPLE_REPLY_GDB = 112;  ///简单回复GDB操作结果
T_MODULE T_CONST T_WORD CMD_ANALY_INSERT_SBP = 113; //分析插入软件断点命令是否成功
T_MODULE T_CONST T_WORD CMD_UPDATE_CPSR = 114; //更新CPSR
/*ICE 3.2 增加[begin]*/
T_MODULE T_CONST T_WORD CMD_ANALY_QUERY_ICEENDIAN = 115; //分析查询ICE大小端结果
T_MODULE T_CONST T_WORD CMD_ANALY_CREATE_SESSION = 116; //分析创建会话结果
T_MODULE T_CONST T_WORD CMD_ANALY_DELETE_SESSION = 117; //分析删除会话结果
T_MODULE T_CONST T_WORD CMD_ANALY_RESET_TARGET  = 118; //分析重启目标机结果
T_MODULE T_CONST T_WORD CMD_REPLY_IDE_RESET_TARGET = 119;  // 回复IDE重启目标机的结果
T_MODULE T_CONST T_WORD CMD_ANALY_QUER_TARGET_CPUID = 120; //分析查询CPUID结果
T_MODULE T_CONST T_WORD CMD_ANALY_BP_READ_MEM = 121; //分析插入断点的读内存结果，并插入断点
T_MODULE T_CONST T_WORD CMD_ANALY_SIMIHOST_BREAK = 122; //分析simihost断点结果
T_MODULE T_CONST T_WORD CMD_ANALY_OPEN_DCC = 123; //分析启用DCC功能的结果
/*ICE3.2增加[end]*/

};


#endif