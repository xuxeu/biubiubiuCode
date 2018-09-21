/*
 * 变更历史：
 * 2013-12-18  叶俊      修改串口和控制台的控制命名值，使之与文件系统不冲突。
 * 2012-02-16  时磊      创建该文件。
 */

/*
 * @file sysUart.h
 * @brief
 *       功能:
 *         <li>定义串口设备的数据结构和相关接口。</li>
 */

#ifndef _SYSUART_H
#define _SYSUART_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "taTypes.h"
/*宏定义*/
/*串口设备输入/输出缓冲大小*/
#define UART_INPUT_BUF_SIZE		(512)
#define UART_OUTPUT_BUF_SIZE	(512)

/* ioctl 选项常量定义*/
#define UIO_GET_CFG		(FIOLAST + 10)	/* 获取串口配置*/
#define UIO_SET_CFG		(FIOLAST + 11)	/* 设置串口配置 */
#define UIO_SET_INBUF	(FIOLAST + 12)	/* 改变串口驱动的输入缓冲*/
#define UIO_SET_OUTBUF	(FIOLAST + 13)	/* 改变串口驱动的输出缓冲*/

/*串口设备号定义*/
#define DR_UART_NUMS    (8)
#define DR_UART_COM1	(0)
#define DR_UART_COM2	(1)
#define DR_UART_COM3	(2)
#define DR_UART_COM4	(3)
#define DR_UART_COM5	(4)
#define DR_UART_COM6	(5)
#define DR_UART_COM7	(6)
#define DR_UART_COM8	(7)

#define DR_UART_EVEN_PARITY		(0)
#define DR_UART_ODD_PARITY		(1)
#define DR_UART_NO_PARITY		(2)

#define DR_UART_5bpc	(5)
#define DR_UART_6bpc	(6)
#define DR_UART_7bpc	(7)
#define DR_UART_8bpc	(8)

/*串口运行模式*/
#define DR_UART_POLLING   (0)
#define DR_UART_INTERRUPT (1)

#define DR_IO_GET_ATTRIBUTES	(FIOLAST + 1)
#define DR_IO_SET_ATTRIBUTES  	(FIOLAST + 2)
#define DR_IO_TCDRAIN        	(FIOLAST + 3)

#define DM_FLAGS_NO_DELAY      (0x0001)  /* 没有数据立即返回 */
#define DM_FLAGS_READ          (0x0002)  /* 读标志 */
#define DM_FLAGS_WRITE         (0x0004) /* 写标志 */
#define DM_FLAGS_OPEN          (0x0100)  /* 驱动打开标志 */
#define DM_FLAGS_APPEND        (0x0200)  /* append 方式写*/
#define DM_FLAGS_CREATE        (0x0400)  /* 创建文件*/
#define DM_FLAGS_CLOSE_ON_EXEC (0x0800)  /* 关闭进程 exec() */
#define DM_FLAGS_READ_WRITE    (DM_FLAGS_READ | DM_FLAGS_WRITE)

 #define UART_SENDOVER_INTERRUPT (1)
 #define UART_RECEIVE_INTERRUPT  (2)
 #define UART_RECEIVE_ERROR      (3)
 #define UART_LINE_STATUS_DATA_READY (1)
 #define UART_LINE_STATUS_ERROR  (2)

/*类型定义*/
/*串口配置类型定义*/
typedef struct {
    T_BYTE *name; /* 串口控制台的名字 */
    T_UBYTE	parity_mode; /* 奇偶校验模式 */
    T_UBYTE	bits_per_char; /* 每个字符的位数 */
    T_WORD baudrate; /* 波特率 */
    T_UBYTE	num_of_stop_bits; /* 停止位的个数 */
    T_UBYTE	mode; /* 使用中断或者轮寻 */
} T_DR_UartConfigTable;

/* 提供给ioctl 使用，用来改变串口驱动缓冲大小 */
typedef struct {
    T_UBYTE *base; /* 指向缓冲区基地址 */
    T_UWORD size; /* 缓冲区大小 */
} T_DR_UartBuffer;

typedef struct {
    T_UWORD flags; /*设备标志*/
    T_WORD major; /*主设备号*/
    T_WORD minor; /*次设备号*/
    T_UBYTE *InBuf; /*指向真实缓冲*/
    T_UWORD InBufSize; /*缓冲大小*/
    T_UWORD InBufHead; /*队列头*/
    T_UWORD InBufTail; /*队列尾*/
    T_WORD InBufMutex; /*InBuf的互斥信号量*/
    T_WORD InBufCntSem;
    T_UWORD InBufWaitTicks;	/*等待的ticks数*/
    T_UWORD InBufDropped; /*Statistics*/
    T_UBYTE *OutBuf;
    T_UWORD OutBufSize;
    T_UWORD OutBufHead;	/*队列头*/
    T_UWORD OutBufTail; /*队列尾*/
    T_WORD OutBufMutex;
    T_WORD OutBufWaitMutex;
    T_UWORD OutBufWaitTicks;
    T_VOLATILE enum {obs_idle, obs_busy}OutBufState;
    T_DR_UartConfigTable config; /* 初始配置 */
} T_DR_UartControl;

/*变量声明*/
T_EXTERN T_WORD wDR_UartCtlBlkMutex;

T_EXTERN T_DR_UartControl** T_DR_UartCtlBlock;

T_EXTERN T_UWORD uwDR_UartNumberOfChannelsConfig;

/* 向量处理函数类型定义 */
typedef void ( *Vector_Handler )( UINT32 vector,void *context,INT32 param);

/*接口声明*/
/*串口设备驱动接口声明*/
/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern INT32 uart_initialize(
T_WORD major,
T_WORD minor,
T_VOID *arg
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern INT32 uart_open(
void *arg,
UINT8 *fileName,
INT32 flags,
INT32 mode
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern INT32 uart_close(
T_VOID *arg
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern INT32 uart_read(
T_VOID *arg,
T_BYTE *buffer,
T_UWORD maxbytes
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern INT32 uart_write(
T_VOID *arg,
T_BYTE *buffer,
T_UWORD maxbytes
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern INT32 uart_control(
T_VOID* args,
T_WORD function,
T_WORD arg
);


/*串口设备接口声明*/
/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_WORD BSP_UartGetDeviceNum(void);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_VOID BSP_UartInit(
T_WORD minor,
T_DR_UartConfigTable *config,
Vector_Handler isr
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_VOID BSP_UartClose(
T_WORD minor
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_WORD BSP_UartPollWrite(
T_WORD minor,
const T_BYTE *buf,
T_WORD len
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_WORD BSP_UartPollRead(
T_WORD minor
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_VOID BSP_UartInterruptWrite(
T_WORD minor,
T_BYTE ch
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_UBYTE BSP_UartInterruptRead(
T_WORD minor
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_VOID BSP_UartDisableReceiveInterrupt(
T_WORD minor
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_VOID BSP_UartEnableReceiveInterrupt(
T_WORD minor
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_VOID BSP_UartDisableSendOverInterrupt(
T_WORD minor
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_VOID BSP_UartEnableSendOverInterrupt(
T_WORD minor
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_UBYTE BSP_UartISRStatus(
T_WORD minor
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_UBYTE BSP_UartLSRStatus(
T_WORD minor
);

/*
* @brief
*       。
* @param[in]  :
* @return
*       :
* @implements
*/
extern void BSP_UartGetVector(
int minor,
int* vector
);

/*串口设备驱动表*/
#define DR_UART_DRIVER_ENTRY \
  { (T_DM_DeviceDriverInitEntry)uart_initialize, (T_DM_DeviceDriverEntry)uart_open, (T_DM_DeviceDriverEntry)uart_close, \
    (T_DM_DeviceDriverEntry)uart_read, (T_DM_DeviceDriverEntry)uart_write, (T_DM_DeviceDriverEntry)uart_control }

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _SYSUART_H */
