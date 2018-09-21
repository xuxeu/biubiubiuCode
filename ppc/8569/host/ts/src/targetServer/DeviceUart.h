/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  DeviceUart.h
* @brief
*       功能：
*       <li>串口设备层</li>
*/



#ifndef UART_INCLUDE
#define UART_INCLUDE

/************************头文件********************************/

#include "Device.h"
#include "windows.h"
#include <QString>

/************************宏定义********************************/
/************************类型定义******************************/
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

/************************接口声明******************************/
#endif    //DEVICE_UART_INCLUDE

