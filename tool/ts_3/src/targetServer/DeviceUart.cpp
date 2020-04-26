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
 * @file     DeviceUart.cpp
 * @brief     串口设备层
 * @author     彭宏
 * @date     2006-1-11
 * @version 3.0
 * @warning 严禁将本文件到处分发
 *
 * 部门：系统部 
 *
 */
#include "deviceuart.h"
#include "common.h"

DeviceUart::DeviceUart(void) 
{
    hUart = NULL;
    isExit = false;
    RxOverlap.hEvent = NULL;
    TxOverlap.hEvent = NULL;
}

DeviceUart::~DeviceUart(void)
{
    close();
}

/**
* 功能：打开具体设备
* @return true表示成功,false表示失败
*/
bool DeviceUart::open(ServerConfig* config)
{
    if (hUart != NULL) 
    {
        close( );
    }
    if(NULL ==config)
    {
        sysLoger(LWARNING, "DeviceUart::open: server config is null!Can't get config of device!");
        return false;
    }
    UartConfig* pUartconfig = (UartConfig*)config->getDeviceConfig();
    baud = pUartconfig->getBaud();    //得到波特率
    /*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [begin]*/
    /*  Modeified brief: 增加扩展串口号前缀,支持com10以上串口*/
    com = QString("\\\\?\\") + pUartconfig->getCom().toAscii().data();    //得到要打开的串口号
    /*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [end]*/
    timeout = pUartconfig->getTimeout();    //超时
    retry = pUartconfig->getRetry();    //得到重试次数
    //以异步的方式打开串口
    /*    hUart = CreateFile( com.utf16(),    //转换成unicode码
                         GENERIC_READ|GENERIC_WRITE,
                         0,NULL,OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                         NULL );*/
        
#ifdef _UNICODE
    //以异步的方式打开串口
        hUart = CreateFile( com.utf16(),    //转换成unicode码
                         GENERIC_READ|GENERIC_WRITE,
                         0,NULL,OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                         NULL );
#else

    //以异步的方式打开串口
        hUart = CreateFile( com.toAscii(),    //转换成Ascii码
                         GENERIC_READ|GENERIC_WRITE,
                         0,NULL,OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                         NULL );
#endif


    if (hUart == INVALID_HANDLE_VALUE)
    {
        sysLoger(LWARNING, "DeviceUart:: open COM device failure!COM:%s, Baud:%d, Timeout:%d, Retry:%d",\
                                            com.toAscii().data(), baud, timeout, retry);
        return false;
    }
     // 检查新句柄是否是一个适当的设备
    if (GetFileType( hUart ) != FILE_TYPE_CHAR) 
    {
        sysLoger(LWARNING, "DeviceUart:: open COM device as invalid type!");
        CloseHandle(hUart);
        return false;
     }
    //设置缓冲区读写大小
    if(!SetupComm( hUart, 8192, 8192 )) 
    {
        sysLoger(LWARNING, "DeviceUart:: initialize buffer for the COM device failure!");
        CloseHandle( hUart );
        return false;
     }

    // 删除系统缓冲区内的任何东西

     PurgeComm( hUart, PURGE_TXABORT | PURGE_RXABORT |
                           PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

    SetCommState();    //设置串口配置
    //设置底层超时参数
    COMMTIMEOUTS commtimeouts;
    GetCommTimeouts(hUart, &commtimeouts);
    commtimeouts.ReadIntervalTimeout         = 1000;
    SetCommTimeouts(hUart, &commtimeouts);

    /*初始化异步结构*/
    ZeroMemory(&RxOverlap,sizeof(OVERLAPPED));
    ZeroMemory(&TxOverlap,sizeof(OVERLAPPED));
    /*为接收和发送的异步结构分别创建一个事件*/
    RxOverlap.hEvent = CreateEvent(NULL,true,false,NULL);    //创建一个手工设置的信号量,初始状态是没有信号
    TxOverlap.hEvent = CreateEvent(NULL,true,false,NULL);    //创建一个手工设置的信号量,初始状态是没有信号
    ResetEvent(TxOverlap.hEvent);
    ResetEvent(RxOverlap.hEvent);
    if(RxOverlap.hEvent == NULL || TxOverlap.hEvent == NULL ) 
    {
        sysLoger(LWARNING, "DeviceUart:: create overlap event failure!");
        CloseHandle( hUart );
        return false;
    }

    return true;
}
/**
* 功能：设置串口配置
* @return true表示成功,false表示失败
*/
bool DeviceUart::SetCommState() //设置串口状态
{
    DCB dcb;

    ::GetCommState( hUart, &dcb );    //得到默认的串口配置
//    ::BuildCommDCB( dcbStr,&dcb);    //修改串口配置
    dcb.ByteSize  = 8;           // data size, xmit, and rcv
    dcb.Parity      = NOPARITY;      // no parity bit
    dcb.StopBits  = ONESTOPBIT;    // one stop bit
    dcb.BaudRate  = baud;
/*    dcb.fNull = 0; //将NULL字符当普通字符传送
    dcb.fOutxCtsFlow = 1; //采用硬件流控制
    dcb.fOutxDsrFlow = 1;
    dcb.fOutX = 0;
    dcb.fInX = 0;
    dcb.fDtrControl =  DTR_CONTROL_HANDSHAKE;
    dcb.fRtsControl =  RTS_CONTROL_HANDSHAKE;

    dcb.fDsrSensitivity = 1;
    dcb.fTXContinueOnXoff = 1;
    dcb.fErrorChar = 0;
    dcb.fAbortOnError = 0;
    dcb.fDummy2 = 0;
    dcb.wReserved = 0;
    dcb.XonLim = 65535;
    dcb.XoffLim = 65535;
    dcb.XonChar = 17;    
    dcb.XoffChar = 19;
    dcb.ErrorChar = 0;
    dcb.EofChar = 0;
    dcb.EvtChar = 0;
    dcb.wReserved1 = 0;*/

//    dcb.fOutX=0;
    return ::SetCommState( hUart, &dcb);
}
/**
* 功能：关闭具体设备
* @return true表示成功,false表示失败
*/
bool DeviceUart::close()
{
    isExit = true;
    /*关闭接收异步事件*/
    if (RxOverlap.hEvent != NULL) 
    {
        SetEvent(RxOverlap.hEvent);
        CloseHandle(RxOverlap.hEvent);
        RxOverlap.hEvent = NULL;
    }
    /*关闭发送异步事件*/
    if (TxOverlap.hEvent != NULL)
    {
        SetEvent(TxOverlap.hEvent);
        CloseHandle(TxOverlap.hEvent);
        TxOverlap.hEvent = NULL;
    }

    /*关闭串口*/
    if (hUart != NULL)
    {
        CloseHandle(hUart);
        hUart = NULL;
    }
    return true;
}
/**
* 功能：从设备中读取信息
* @param  pBuff 得到信息缓冲区
* @param  size  缓冲区的大小
* @param  timeout 等待时间,-1表示无穷等待,
* @return 从设备中得到的数据大小
*/
int  DeviceUart::read(char* pBuff, int size) 
{
    int fSuccess;
    char character = 's';
    DWORD NumOfBytesRead;
    DWORD readSize = 0;
    char *pBuf = pBuff;        //保存好头指针

    if(NULL == pBuf)
    {
        sysLoger(LWARNING, "DeviceUart::read: Received buf is null. No buf for received data! exit read!");
        return -1;
    }
    
    if (size <= 0)
    {
        sysLoger(LWARNING, "DeviceUart::read: Received buf size [%d] is invalid ! exit read!", size);
        return -1;
    }

    while(1)
    {
        /*投递一个异步读请求*/
        fSuccess = ReadFile(hUart,
            &character,
            1,
            &NumOfBytesRead,
            &RxOverlap);

        /*等待读操作的完成*/
        fSuccess = GetOverlappedResult(hUart,&RxOverlap,&NumOfBytesRead,true);
        if (!fSuccess || isExit)
        {
            sysLoger(LINFO, "DeviceUart::read: read data failure!");
            return -1;
        }
            
        /*重新设定时间句柄*/
        ResetEvent(RxOverlap.hEvent);
        if (NumOfBytesRead > 0) 
        {
            *pBuf = character;
            pBuf++;
            readSize += NumOfBytesRead;
            if(readSize == size)
            {
                /*返回读到的字符*/
                return 1    ;
            }
        }
    }
}
/**
* 功能：向设备发送数据
* @param  pBuff 发送信息包
* @param  size 信息包大小
*/
void DeviceUart::write(char* pBuff, int size) 
{
    TMutexLocker locker(&mutex);
    int fSuccess;
    DWORD NumOfBytesWrite = 0;    //已经写的大小

    DWORD WhereToStartWriting  = 0;    //开始写的位置
    int iCount = 0;

    if((NULL == pBuff) ||(size <= 0))
    {
        sysLoger(LWARNING, "DeviceUart::write:Bad pointer of pBuff, or invalid size [%d], eixt write!", 
                                            size);
        return;
    }
    
    while(1)
    {
        iCount++;
        /*投递一个异步写请求*/
        fSuccess = WriteFile(hUart,
                            pBuff+WhereToStartWriting,
                            size,
                            &NumOfBytesWrite,
                            &TxOverlap);
            
    //    ResetEvent(TxOverlap.hEvent);
        /*等待写操作的完成*/
        fSuccess = GetOverlappedResult(hUart,&TxOverlap,&NumOfBytesWrite,true);
        ResetEvent(TxOverlap.hEvent);
        /*重新设定时间句柄*/
        if (!fSuccess || isExit) 
        {
            sysLoger(LINFO, "DeviceUart::write: write data failure!");
            int a = GetLastError();
            return;    //继续
        }

        if(fSuccess && (NumOfBytesWrite == (unsigned)size))
        {
            /*返回读到的字符*/
            sysLoger(LINFO, "DeviceUart::write: write data failure!");
            return;
        }
        size -= NumOfBytesWrite;
        WhereToStartWriting+= NumOfBytesWrite;
        NumOfBytesWrite = 0;
    }
}
/**
* 功能：检查设备连接情况
* @return  true表示连接成功
*/
bool DeviceUart::connect() 
{
    return true;
}

