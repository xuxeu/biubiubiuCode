/*******************************************************************************
 *                      北京科银京成有限公司 版权所有
 *       Copyright (C) 2000-2006 CoreTek System Inc. All Rights Reserved
 ******************************************************************************/

/**
  * $Log: lambdaIce.h,v $
  * Revision 1.4  2008/03/19 10:25:11  guojc
  * Bug 编号  	0002430
  * Bug 名称 	ts启动后造成Windows任务栏凝固，无法响应快捷键
  *
  * 编码：唐兴培
  * 检视：卿孝海,郭建川
  *
  * Revision 1.1  2008/02/04 05:33:49  zhangxc
  * 设计报告名称：《ACoreOS CVS代码仓库管理》
  * 编写人员：张晓超
  * 检视人员：张宇旻
  * 其它：从现有最新ACoreOS仓库中取出最新的源代码，整理后提交到新的仓库目录结构中管理。
  * 第一次提交src模块。
  *
  * Revision 1.3  2007/04/20 07:10:50  guojc
  * 增加了对ICE DCC功能的支持
  * 增加了对目标板复位功能的支持
  * 增加了对内存宽度可配置的支持
  * 增加了对华邦模拟器调试兼容的支持
  *
  * Revision 1.2  2007/04/02 02:56:53  guojc
  * 提交最新的IS代码，支持脱离于IDE使用，支持配置文件解析
  *
  * Revision 1.1  2006/11/13 03:40:56  guojc
  * 提交ts3.1工程源码,在VC8下编译
  *
  * Revision 1.1.1.1  2006/04/19 06:29:13  guojc
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
* @file     lambdaIce.h
* @brief     
*     <li>功能： ICE原子操作协议封装类</li>
* @author     郭建川
* @date     2006-02-20
* <p>部门：系统部 
*/

#ifndef _LAMBDA_ICE_H
#define _LAMBDA_ICE_H


/************************************引用部分******************************************/
#include "sysTypes.h"         ///类型重定义
/************************************声明部分******************************************/

/************************************定义部分******************************************/
class RSP_TcpServer;

/**
* @brief
*    RSP_LambdaIce是ICE向ICE Server提供的原子操作命令的协议封装，它对外提供ICE能够提供的一系列原子操作方法\n
*   负责组装控制ICE进行原子操作的控制数据包，通过RX线程发送到ICE\n
* @author     郭建川
* @version 1.0
* @date     2006-02-20
* <p>部门：系统部 
*/
class RSP_LambdaIce
{
public:
    /**
    * @brief 
    *     功能: 设置服务器对象
    * @param[in] vtpServer 服务器对象指针
    */
    T_VOID SetServer(RSP_TcpServer *vtpServer);
    
    /**
    * @brief 
    *     功能:查询ICE大小端
    * @param[in] cpBuf 缓冲区,需要发送给ICE的一些信息
    */
    T_VOID QueryTargetEndian(int IceID);

    /**
    * @brief 
    *     功能:删除与ICE的会话
    * @param[in] IceID ICE的ID号
    */
    T_VOID DeleteSeesion(int IceID);
    /**
    * @brief 
    *     功能:建立与ICE的会话
    * @param[in] cpBuf 缓冲区,需要发送给ICE的一些信息
    */
    T_VOID CreateSeesion(T_CHAR *cpBuf, T_WORD cpBufSize, int IceID);
    /**
    * @brief 
    *     功能: 封装写内存控制包
    * @param[in] wAddr 内存地址
    * @param[in] cpBuf 缓冲区
    * @param[in] wOffset 偏移地址
    * @param[in] wSize 要写的内存大小
    */
    T_VOID WriteMemory(T_WORD wAddr, T_CHAR *cpBuf, T_WORD wOffset, T_WORD wSize);
    
    /**
    * @brief 
    *     功能: 封装读内存控制包
    * @param[in] wAddr 内存地址
    * @param[in] wSize 要读的内存大小
    */
    T_VOID ReadMemory(T_WORD wAddr, T_WORD wSize);
    
    /**
    * @brief 
    *     功能: 封装写所有寄存器控制包
    * @param[in] waRegs 寄存器数组
    * @param[in] waRegs 寄存器数组长度
    */
    T_VOID WriteAllRegisters(T_WORD *waRegs, T_WORD wNum);
    
    /**
    * @brief 
    *     功能: 封装读所有寄存器控制包
    */
    T_VOID ReadAllRegisters();

    /**
    * @brief 
    *     功能: 封装继续运行目标机控制包
    */
    T_VOID ContinueTarget();

    /**
    * @brief 
    *     功能: 封装停止目标机控制包
    */
    T_VOID StopTarget();
    /**
    * @brief 
    *     功能: 封装重启目标机控制包
    *  @param[in]: US resetType 重启的类型
    *   resetType＝0  CPU硬件复位，只是将目标板CPU进行硬件复位，不对目标板外围设备进行复位。
    *   resetType＝1  外设硬件复位，只是将目标板的外围硬件设备进行复位，不对CPU进行复位。
    *   resetType＝2  CPU外设同时硬件复位。
    */
    T_VOID ReSetTarget(T_UWORD resetType);
    
    /**
    * @brief 
    *     功能: 封装查询目标机CPUID CODE
    */
    T_VOID QueryCPUIDCode();
    
    /**
    * @brief 
    *     功能: 封装查询目标机控制包
    */
    T_VOID QueryTarget();
    
    /**
    * @brief 
    *     功能: 封装打开DCC功能的控制包
    */
    T_VOID OpenDccFunction(T_WORD dccAddr);

    /**
    * @brief 
    *     功能: 封装插入断点或观察点
    * @param[in] type 断点类型
    *                           0：软件断点
    *                           1：硬件断点
    *                           2：写观察点
    *                           3：读观察点
    *                           4：访问观察点
    * @param[in] wAddr 断点地址
    * @param[in] wWidth 断点宽度
    */
    T_VOID InsertBreakPointORWatchPoint(T_WORD type, T_WORD wAddr, T_WORD wWidth);
    /**
    * @brief 
    *     功能: 封装删除断点或观察点
    * @param[in] type 断点类型
    *                           0：软件断点
    *                           1：硬件断点
    *                           2：写观察点
    *                           3：读观察点
    *                           4：访问观察点
    * @param[in] wAddr 断点地址
    * @param[in] wWidth 断点宽度
    */
    T_VOID DeleteBreakPointORWatchPoint(T_WORD type, T_WORD wAddr, T_WORD wWidth);
#if 0
    T_VOID InsertBreakPoint(T_WORD wAddr, T_WORD wWidth);
    /**
    * @brief 
    *     功能: 封装插入观察点控制包
    * @param[in] wAddr 观察点地址
    * @param[in] wWidth 观察点宽度
    * @param[in] wAccess 观察点类型
    */
    T_VOID InsertWatchPoint(T_WORD wAddr, T_WORD wWidth, T_WORD wAccess);

    /**
    * @brief 
    *     功能: 封装删除观察点控制包
    * @param[in] wAddr 观察点地址
    * @param[in] wWidth 观察点宽度
    * @param[in] wAccess 观察点类型
    */
    T_VOID RemoveWatchPoint(T_WORD wAddr, T_WORD wWidth, T_WORD wAccess);
    /**
    * @brief 
    *     功能: 封装删除硬件断点控制包
    * @param[in] wAddr 断点地址
    * @param[in] wWidth 断点宽度
    */
    T_VOID RemoveBreakPoint(T_WORD wAddr, T_WORD wWidth);

#endif

    /**
    * @brief 
    *     功能: 封装写内存控制包
    * @param[in] wAddr 内存地址
    * @param[in] cpBuf 缓冲区
    * @param[in] wOffset 偏移地址
    * @param[in] wSize 要写的内存大小
    * @param[in] wDelay 延时时间长度
    */
    T_VOID WriteMemoryWithDelay(T_WORD wAddr, T_CHAR *cpBuf, T_WORD wOffset, T_WORD wSize, T_WORD wDelay);


    /**
    * @brief 
    *     功能: 封装写CP15寄存器的控制包
    * @param[in] wNum CP15寄存器号
    * @param[in] wValue 寄存器值
    */
    T_VOID WriteCp15Reg(T_WORD wNum, T_WORD wValue);

    /**
    * @brief 
    *     功能: 封装读CP15寄存器的控制包
    * @param[in] wNum CP15寄存器号
    * @param[in] wValue 寄存器值
    */
    T_VOID ReadCp15Reg(T_WORD wNum);
    
private:
    T_BOOL m_iceBigEndian;

    RSP_TcpServer *m_server;

    /*
    * 功能: 把int转换成16进制的字符串,必须保证size大小大于转换后的字符串大小
    * @param[out] vcpPtr 保存16进制字符串的缓冲区
    * @param[in] vwSize   缓冲区的大小
    * @param[in] vwIntValue   要转换的整形
    * @return 转换的大小
    */
    T_UWORD Int2hex(T_CHAR *vcpPtr, T_WORD vwSize,T_WORD vwIntValue);
};


#endif