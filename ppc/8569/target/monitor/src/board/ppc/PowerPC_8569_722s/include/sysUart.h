/*
 * �����ʷ��
 * 2013-12-18  Ҷ��      �޸Ĵ��ںͿ���̨�Ŀ�������ֵ��ʹ֮���ļ�ϵͳ����ͻ��
 * 2012-02-16  ʱ��      �������ļ���
 */

/*
 * @file sysUart.h
 * @brief
 *       ����:
 *         <li>���崮���豸�����ݽṹ����ؽӿڡ�</li>
 */

#ifndef _SYSUART_H
#define _SYSUART_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "taTypes.h"
/*�궨��*/
/*�����豸����/��������С*/
#define UART_INPUT_BUF_SIZE		(512)
#define UART_OUTPUT_BUF_SIZE	(512)

/* ioctl ѡ�������*/
#define UIO_GET_CFG		(FIOLAST + 10)	/* ��ȡ��������*/
#define UIO_SET_CFG		(FIOLAST + 11)	/* ���ô������� */
#define UIO_SET_INBUF	(FIOLAST + 12)	/* �ı䴮�����������뻺��*/
#define UIO_SET_OUTBUF	(FIOLAST + 13)	/* �ı䴮���������������*/

/*�����豸�Ŷ���*/
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

/*��������ģʽ*/
#define DR_UART_POLLING   (0)
#define DR_UART_INTERRUPT (1)

#define DR_IO_GET_ATTRIBUTES	(FIOLAST + 1)
#define DR_IO_SET_ATTRIBUTES  	(FIOLAST + 2)
#define DR_IO_TCDRAIN        	(FIOLAST + 3)

#define DM_FLAGS_NO_DELAY      (0x0001)  /* û�������������� */
#define DM_FLAGS_READ          (0x0002)  /* ����־ */
#define DM_FLAGS_WRITE         (0x0004) /* д��־ */
#define DM_FLAGS_OPEN          (0x0100)  /* �����򿪱�־ */
#define DM_FLAGS_APPEND        (0x0200)  /* append ��ʽд*/
#define DM_FLAGS_CREATE        (0x0400)  /* �����ļ�*/
#define DM_FLAGS_CLOSE_ON_EXEC (0x0800)  /* �رս��� exec() */
#define DM_FLAGS_READ_WRITE    (DM_FLAGS_READ | DM_FLAGS_WRITE)

 #define UART_SENDOVER_INTERRUPT (1)
 #define UART_RECEIVE_INTERRUPT  (2)
 #define UART_RECEIVE_ERROR      (3)
 #define UART_LINE_STATUS_DATA_READY (1)
 #define UART_LINE_STATUS_ERROR  (2)

/*���Ͷ���*/
/*�����������Ͷ���*/
typedef struct {
    T_BYTE *name; /* ���ڿ���̨������ */
    T_UBYTE	parity_mode; /* ��żУ��ģʽ */
    T_UBYTE	bits_per_char; /* ÿ���ַ���λ�� */
    T_WORD baudrate; /* ������ */
    T_UBYTE	num_of_stop_bits; /* ֹͣλ�ĸ��� */
    T_UBYTE	mode; /* ʹ���жϻ�����Ѱ */
} T_DR_UartConfigTable;

/* �ṩ��ioctl ʹ�ã������ı䴮�����������С */
typedef struct {
    T_UBYTE *base; /* ָ�򻺳�������ַ */
    T_UWORD size; /* ��������С */
} T_DR_UartBuffer;

typedef struct {
    T_UWORD flags; /*�豸��־*/
    T_WORD major; /*���豸��*/
    T_WORD minor; /*���豸��*/
    T_UBYTE *InBuf; /*ָ����ʵ����*/
    T_UWORD InBufSize; /*�����С*/
    T_UWORD InBufHead; /*����ͷ*/
    T_UWORD InBufTail; /*����β*/
    T_WORD InBufMutex; /*InBuf�Ļ����ź���*/
    T_WORD InBufCntSem;
    T_UWORD InBufWaitTicks;	/*�ȴ���ticks��*/
    T_UWORD InBufDropped; /*Statistics*/
    T_UBYTE *OutBuf;
    T_UWORD OutBufSize;
    T_UWORD OutBufHead;	/*����ͷ*/
    T_UWORD OutBufTail; /*����β*/
    T_WORD OutBufMutex;
    T_WORD OutBufWaitMutex;
    T_UWORD OutBufWaitTicks;
    T_VOLATILE enum {obs_idle, obs_busy}OutBufState;
    T_DR_UartConfigTable config; /* ��ʼ���� */
} T_DR_UartControl;

/*��������*/
T_EXTERN T_WORD wDR_UartCtlBlkMutex;

T_EXTERN T_DR_UartControl** T_DR_UartCtlBlock;

T_EXTERN T_UWORD uwDR_UartNumberOfChannelsConfig;

/* �������������Ͷ��� */
typedef void ( *Vector_Handler )( UINT32 vector,void *context,INT32 param);

/*�ӿ�����*/
/*�����豸�����ӿ�����*/
/*
* @brief
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
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


/*�����豸�ӿ�����*/
/*
* @brief
*       ��
* @param[in]  :
* @return
*       :
* @implements
*/
extern T_WORD BSP_UartGetDeviceNum(void);

/*
* @brief
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
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
*       ��
* @param[in]  :
* @return
*       :
* @implements
*/
extern void BSP_UartGetVector(
int minor,
int* vector
);

/*�����豸������*/
#define DR_UART_DRIVER_ENTRY \
  { (T_DM_DeviceDriverInitEntry)uart_initialize, (T_DM_DeviceDriverEntry)uart_open, (T_DM_DeviceDriverEntry)uart_close, \
    (T_DM_DeviceDriverEntry)uart_read, (T_DM_DeviceDriverEntry)uart_write, (T_DM_DeviceDriverEntry)uart_control }

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _SYSUART_H */
