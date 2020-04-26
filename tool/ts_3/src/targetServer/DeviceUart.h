/*****************************************************************
 *    This source code has been made available to you by CoreTek on an AS-IS
 *    basis. Anyone receiving this source is licensed under CoreTek
 *    copyrights to use it in any way he or she deems fit, including
 *    copying it, modifying it, compiling it, and redistributing it either
 *    with or without modifications.
 *
 *    Any person who transfers this source code or any derivative work
 *    must include the CoreTek copyright notice, this paragraph, and the
 *    preceding two paragraphs in the transferred software.
 *
 *    COPYRIGHT   CoreTek  CORPORATION 2004
 *    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
 ***********************************************************************/

/**
 * 
 *
 * @file     DeviceUart.h
 * @brief     串口设备层
 * @author     彭宏
 * @date     2006-1-11
 * @version 3.0
 * @warning 严禁将本文件到处分发
 *
 * 部门：系统部 
 *
 */

#ifndef UART_INCLUDE
#define UART_INCLUDE

#include "Device.h"
#include "windows.h"
#include <QString>
class DeviceUart : public DeviceBase
{
    bool isExit;
    OVERLAPPED RxOverlap;        //读数据的异步结构体,主要成员是一个事件对象
    OVERLAPPED TxOverlap;        //发送据的异步结构体,主要成员是一个事件对象
    HANDLE hUart ;                //串口句柄
    TMutex mutex;
    QString        com;            //串口号
    int            baud;            //波特率
    int            timeout;        //超时
    int            retry;            //重试次数
    /**
    * 功能：设置串口配置
    * @return true表示成功,false表示失败
    */
    bool SetCommState(); //设置串口状态
public:
    /**
    * 功能：得到超时
    */
    int getTimeout() 
    {
        return timeout;
    }
    /**
    * 功能：设置超时
    */
    void setTimeout(int timeout)
    {
        this->timeout = timeout;
    }

    /**
    * 功能：得到重试次数
    */
    int getRetry()
    {
        return retry;
    }
    /**
    * 功能：设置重试次数
    */
    void setRetry(int retry) 
    {
        this->retry = retry;
    }
    DeviceUart(void);
    virtual ~DeviceUart(void);
    /**
     * 功能：打开具体设备
     * @return true表示成功,false表示失败
     */
    virtual bool open(ServerConfig* config);
    /**
     * 功能：关闭具体设备
     * @return true表示成功,false表示失败
     */
    virtual bool close();
    /**
     * 功能：从设备中读取信息
     * @param  pBuff 得到信息缓冲区
     * @param  size  缓冲区的大小
     * @param  timeout 等待时间,-1表示无穷等待,
     * @return 从设备中得到的数据大小
     */
    virtual int  read(char* pBuff, int size);
    /**
     * 功能：向设备发送数据
     * @param  pBuff 发送信息包
     * @param  size 信息包大小
     */
    virtual void write(char* pBuff, int size) ;
    /**
     * 功能：检查设备连接情况
     * @return  true表示连接成功
     */
    virtual bool connect() ;
};
#endif    //DEVICE_UART_INCLUDE

