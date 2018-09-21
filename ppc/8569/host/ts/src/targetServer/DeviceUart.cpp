/*
* ������ʷ��
* 2004-12-6 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  DeviceUart.cpp
* @brief
*       ���ܣ�
*       <li>�����豸��</li>
*/
/************************ͷ�ļ�********************************/

#include "deviceuart.h"
#include "common.h"

/************************�궨��********************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/
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
* ���ܣ��򿪾����豸
* @return true��ʾ�ɹ�,false��ʾʧ��
*/
bool DeviceUart::open(ServerConfig* config)
{
    if (hUart != NULL)
    {
        close( );
    }

    if(NULL == config)
    {
        sysLoger(LWARNING, "DeviceUart::open: server config is null!Can't get config of device!");
        return false;
    }

    UartConfig* pUartconfig = (UartConfig*)config->getDeviceConfig();
    baud = pUartconfig->getBaud();    //�õ�������

    /*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [begin]*/
    /*  Modeified brief: ������չ���ں�ǰ׺,֧��com10���ϴ���*/
    com = QString("\\\\?\\") + pUartconfig->getCom().toAscii().data();    //�õ�Ҫ�򿪵Ĵ��ں�
    /*Modified by tangxp for BUG NO.0002446 on 2008.1.7 [end]*/


    timeout = pUartconfig->getTimeout();    //��ʱ
    retry = pUartconfig->getRetry();    //�õ����Դ���
    //���첽�ķ�ʽ�򿪴���


#ifdef _UNICODE
    //���첽�ķ�ʽ�򿪴���
    hUart = CreateFile( com.utf16(),    //ת����unicode��
                        GENERIC_READ | GENERIC_WRITE,
                        0, NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                        NULL );
#else

    //���첽�ķ�ʽ�򿪴���
    hUart = CreateFile( com.toAscii(),    //ת����Ascii��
                        GENERIC_READ | GENERIC_WRITE,
                        0, NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                        NULL );
#endif


    if (hUart == INVALID_HANDLE_VALUE)
    {
        sysLoger(LWARNING, "DeviceUart:: open COM device failure!COM:%s, Baud:%d, Timeout:%d, Retry:%d", \
                 com.toAscii().data(), baud, timeout, retry);
        return false;
    }

    // ����¾���Ƿ���һ���ʵ����豸
    if (GetFileType( hUart ) != FILE_TYPE_CHAR)
    {
        sysLoger(LWARNING, "DeviceUart:: open COM device as invalid type!");
        CloseHandle(hUart);
        return false;
    }

    //���û�������д��С
    if(!SetupComm( hUart, 8192, 8192 ))
    {
        sysLoger(LWARNING, "DeviceUart:: initialize buffer for the COM device failure!");
        CloseHandle( hUart );
        return false;
    }

    // ɾ��ϵͳ�������ڵ��κζ���

    PurgeComm( hUart, PURGE_TXABORT | PURGE_RXABORT |
               PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

    SetCommState();    //���ô�������
    //���õײ㳬ʱ����
    COMMTIMEOUTS commtimeouts;
    GetCommTimeouts(hUart, &commtimeouts);
    commtimeouts.ReadIntervalTimeout         = 1000;
    SetCommTimeouts(hUart, &commtimeouts);

    /*��ʼ���첽�ṹ*/
    ZeroMemory(&RxOverlap, sizeof(OVERLAPPED));
    ZeroMemory(&TxOverlap, sizeof(OVERLAPPED));

    /*Ϊ���պͷ��͵��첽�ṹ�ֱ𴴽�һ���¼�*/
    RxOverlap.hEvent = CreateEvent(NULL, true, false, NULL); //����һ���ֹ����õ��ź���,��ʼ״̬��û���ź�
    TxOverlap.hEvent = CreateEvent(NULL, true, false, NULL); //����һ���ֹ����õ��ź���,��ʼ״̬��û���ź�
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
* ���ܣ����ô�������
* @return true��ʾ�ɹ�,false��ʾʧ��
*/
bool DeviceUart::SetCommState() //���ô���״̬
{
    DCB dcb;

    ::GetCommState( hUart, &dcb );    //�õ�Ĭ�ϵĴ�������
    //    ::BuildCommDCB( dcbStr,&dcb);    //�޸Ĵ�������
    dcb.ByteSize  = 8;           // data size, xmit, and rcv
    dcb.Parity      = NOPARITY;      // no parity bit
    dcb.StopBits  = ONESTOPBIT;    // one stop bit
    dcb.BaudRate  = baud;

    return ::SetCommState( hUart, &dcb);
}
/**
* ���ܣ��رվ����豸
* @return true��ʾ�ɹ�,false��ʾʧ��
*/
bool DeviceUart::close()
{
    isExit = true;

    /*�رս����첽�¼�*/
    if (RxOverlap.hEvent != NULL)
    {
        SetEvent(RxOverlap.hEvent);
        CloseHandle(RxOverlap.hEvent);
        RxOverlap.hEvent = NULL;
    }

    /*�رշ����첽�¼�*/
    if (TxOverlap.hEvent != NULL)
    {
        SetEvent(TxOverlap.hEvent);
        CloseHandle(TxOverlap.hEvent);
        TxOverlap.hEvent = NULL;
    }

    /*�رմ���*/
    if (hUart != NULL)
    {
        CloseHandle(hUart);
        hUart = NULL;
    }

    return true;
}
/**
* ���ܣ����豸�ж�ȡ��Ϣ
* @param  pBuff �õ���Ϣ������
* @param  size  �������Ĵ�С
* @return ���豸�еõ������ݴ�С
*/
int  DeviceUart::read(char* pBuff, int size)
{
    int fSuccess;
    char character = 's';
    DWORD NumOfBytesRead;
    DWORD readSize = 0;
    char *pBuf = pBuff;        //�����ͷָ��

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
        /*Ͷ��һ���첽������*/
        fSuccess = ReadFile(hUart,
                            &character,
                            1,
                            &NumOfBytesRead,
                            &RxOverlap);

        /*�ȴ������������*/
        fSuccess = GetOverlappedResult(hUart, &RxOverlap, &NumOfBytesRead, true);

        if (!fSuccess || isExit)
        {
            sysLoger(LINFO, "DeviceUart::read: read data failure!");
            return -1;
        }

        /*�����趨ʱ����*/
        ResetEvent(RxOverlap.hEvent);

        if (NumOfBytesRead > 0)
        {
            *pBuf = character;
            pBuf++;
            readSize += NumOfBytesRead;

            if(readSize == size)
            {
                /*���ض������ַ�*/
                return 1;
            }
        }
    }
}
/**
* ���ܣ����豸��������
* @param  pBuff ������Ϣ��
* @param  size ��Ϣ����С
*/
void DeviceUart::write(char* pBuff, int size)
{
    TMutexLocker locker(&mutex);
    int fSuccess;
    DWORD NumOfBytesWrite = 0;    //�Ѿ�д�Ĵ�С

    DWORD WhereToStartWriting  = 0;    //��ʼд��λ��
    int iCount = 0;

    if((NULL == pBuff) || (size <= 0))
    {
        sysLoger(LWARNING, "DeviceUart::write:Bad pointer of pBuff, or invalid size [%d], eixt write!",
                 size);
        return;
    }

    while(1)
    {
        iCount++;
        /*Ͷ��һ���첽д����*/
        fSuccess = WriteFile(hUart,
                             pBuff + WhereToStartWriting,
                             size,
                             &NumOfBytesWrite,
                             &TxOverlap);

        //    ResetEvent(TxOverlap.hEvent);
        /*�ȴ�д���������*/
        fSuccess = GetOverlappedResult(hUart, &TxOverlap, &NumOfBytesWrite, true);
        ResetEvent(TxOverlap.hEvent);

        /*�����趨ʱ����*/
        /*Modified by tangxp for BUG NO.3667 on 2008��6��28�� [begin]*/
        /*  Modified brief: д���ݺ��ж��Ƿ�ɹ�*/
        if (!fSuccess || isExit || !NumOfBytesWrite)
        {
            sysLoger(LINFO, "DeviceUart::write: write data failure, lastError = %d!",
                     GetLastError());
            return;    //����
        }

        /*Modified by tangxp for BUG NO.3667 on 2008��6��28�� [end]*/
        if(fSuccess && (NumOfBytesWrite == (unsigned)size))
        {
            /*���ض������ַ�*/
            return;
        }

        size -= NumOfBytesWrite;
        WhereToStartWriting += NumOfBytesWrite;
        NumOfBytesWrite = 0;
    }
}
/**
* ���ܣ�����豸�������
* @return  true��ʾ���ӳɹ�
*/
bool DeviceUart::connect()
{
    return true;
}

