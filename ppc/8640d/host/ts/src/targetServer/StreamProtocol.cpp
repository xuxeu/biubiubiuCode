/*
* ������ʷ��
*  2006-1-11 ���  ���������������޹�˾
*                    �������ļ���
*/

/**
* @file  streamProtocol.cpp
* @brief
*       ���ܣ�
*       <li>��Э���ʵ��</li>
*/

/************************ͷ�ļ�********************************/
#include ".\streamprotocol.h"
#include "common.h"
//#include "util.h"
#include "deviceUart.h"

/************************�궨��********************************/

/*RFC20���罻��Э�������ASCII�ַ�����*/
#define ASCII_NUL   (0x00)    //�շ���
#define ASCII_SOH   (0x01)    //ͷ��ʼ
#define ASCII_STX   (0x02)    //�ı���ʼ
#define ASCII_ETX   (0x03)    //�ı�����
#define ASCII_EOT   (0x04)   //�������
#define ASCII_ENQ   (0x05)    //ѯ��
#define ASCII_ACK   (0x06)    //Ӧ��
#define ASCII_BEL   (0x07)    //����
#define ASCII_BS    (0x08)    //�˸�
#define ASCII_HT    (0x09)    //ˮƽ�Ʊ��
#define ASCII_LF    (0x0A)    //����
#define ASCII_VT    (0x0B)    //��ֱ�Ʊ��
#define ASCII_FF    (0x0C)    //��ҳ
#define ASCII_CR    (0x0D)    //�س�
#define ASCII_SO    (0x0E)    //Shift Out
#define ASCII_SI    (0x0F)    //Shift In
#define ASCII_DLE   (0x10)    //�������ӱ��
#define ASCII_DC1   (0x11)    //�豸����1
#define ASCII_DC2   (0x12)    //�豸����2
#define ASCII_DC3   (0x13)    //�豸����3
#define ASCII_DC4   (0x14)    //�豸����4
#define ASCII_NAK   (0x15)    //�ؾ�Ӧ��
#define ASCII_SYN   (0x16)    //ͬ������
#define ASCII_ETB   (0x17)    //��������
#define ASCII_CAN   (0x18)    //ȡ��
#define ASCII_EM    (0x19)    //�н����
#define ASCII_SUB   (0x1A)    //���
#define ASCII_ESC   (0x1B)    //ת���ַ���Escape��
#define ASCII_FS    (0x1C)    //�ļ�����
#define ASCII_GS    (0x1D)    //�����
#define ASCII_RS    (0x1E)    //��¼����
#define ASCII_US    (0x1F)    //��Ԫ����
#define ASCII_SP    (0x20)    //�ո�
#define ASCII_DEL   (0x7F)    //ɾ��
/*RFC20���罻��Э�������ASCII�ַ�����*/


#define HEAD_BUF_SIZE    (64)
#define CTL_PACK_SIZE   (2)


/*��ϢЭ��������ƫ����*/
#define TA_TREATY_STREAM_DATA_OFFSET (0)

/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/



/************************ʵ��*********************************/

/*Modified by tangxp for BUG NO.3669 on 2008.7.21 [begin]*/
/*  Modified brief: ����TS��ʼ����,�������Ϊ�������*/
StreamProtocol::StreamProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice)
{
    txPacketNum = TX_START;
    rxPacketNum = RX_START;
    isExit = false;
    waiting_for_ack = 2;
}
/*Modified by tangxp for BUG NO.3669 on 2008.7.22 [end]*/

StreamProtocol::~StreamProtocol(void)
{
}
/* ���ܣ��������ݰ�
* @param  Packet* ���ݰ�ָ��
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
*/
int StreamProtocol::putpkt(Packet* pack)
{
    /*��ʼ����*/
    TMutexLocker locker(&putMutex);

    int putCount = 0;            //���ͼ���
    char PackBuffer[PACKET_DATASIZE];
    int  PackIndex = 0;
    char  HeadBuffer[PACKET_HEADSIZE];    //�����ͷ
    char* DataBuffer;
    int   HeadIndex = 0;    //��ͷ��ǰ����������
    int   DataIndex = 0;    //��������
    char  ParitySum = 0;    //У���
    int   retryCount = 0;    //���Դ�������

    if (pack->getSize() > TA_PACKET_DATASIZE)
    {
        return -1;    //��������
    }

    DeviceUart* pDeviceUart = dynamic_cast<DeviceUart*>(getDevice());

    if (retry != pDeviceUart->getRetry())
    {
        retry = pDeviceUart->getRetry();
    }

    if (timeout != pDeviceUart->getTimeout())
    {
        timeout = pDeviceUart->getTimeout();
    }

    /*    if (waiting_for_ack != 2) {//��ʾû���յ����������еȴ�һ��ʱ��
            TMutexLocker lock(&waitMutex);
            m_waitCondition.wait(&waitMutex,10);    //��ʱ����ack��
        }*/


    {
        //���а��ķ���
        /*��װ��ͷ*/
        PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ASCII_SOH;
        //�԰�ͷ�����ֽڵ�����
        char *pBuf = HeadBuffer;
        __store(pack->getSize(),pBuf,sizeof(int));
        pBuf += sizeof(int);
        __store(pack->getDes_aid(),pBuf,sizeof(short));
        pBuf += sizeof(short);
        __store(pack->getSrc_aid(),pBuf,sizeof(short));

        pBuf += sizeof(short);
        __store(pack->getDes_said(),pBuf,sizeof(short));
        pBuf += sizeof(short);
        __store(pack->getSrc_said(),pBuf,sizeof(short));
        pBuf += sizeof(short);
        //���к�
        __store(txPacketNum,pBuf,sizeof(int));
        /*Modified by tangxp for BUG NO.3666 on 2008��7��17�� [begin]*/
        /*  Modified brief: ������Ϣ���������к�*/
        pack->setSerialNum(txPacketNum);
        /*Modified by tangxp for BUG NO.3666 on 2008��7��17�� [end]*/
        pBuf += sizeof(int);

        /*���ɰ�ͷ*/
        for(HeadIndex = 0; HeadIndex < PACKET_HEADSIZE; HeadIndex++)
        {
            if(HeadBuffer[HeadIndex] == ASCII_DLE)      //��ͷ�к���DLE
            {
                PackBuffer[PackIndex++] = ASCII_DLE;    //����һ��DLE
            }

            ParitySum = (char)(ParitySum + HeadBuffer[HeadIndex]);  //����У���
            PackBuffer[PackIndex++] = HeadBuffer[HeadIndex];    //���ɰ�ͷ
        }


        /*��װ����*/
        PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ASCII_STX;

        /*��ð���ʵ��*/
        DataBuffer = pack->getData();

        /*���ɰ���*/
        for(DataIndex = 0; DataIndex < pack->getSize(); DataIndex++)
        {
            if(DataBuffer[DataIndex] == ASCII_DLE)      //�����к���DLE
            {
                PackBuffer[PackIndex++] = ASCII_DLE;    //����һ��DLE
            }

            ParitySum = (char)(ParitySum + DataBuffer[DataIndex]);  //����У���
            PackBuffer[PackIndex++] = DataBuffer[DataIndex];    //���ɰ���
        }


        /*��������*/
        PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ASCII_ETX;

        /*����У��*/
        if(ParitySum == ASCII_DLE)
        {
            PackBuffer[PackIndex++] = ASCII_DLE;
        }

        PackBuffer[PackIndex++] = ParitySum;

        /*��ɰ�����װ*/
        PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ASCII_EOT;
        PackBuffer[PackIndex] = '\0';

        /*Modified by tangxp for BUG NO.3668 on 2008��7��18�� [begin]*/
        /*  Modified brief: �����Ѻ�,����Ƿ����ACK*/
        mutex.lock();
        waiting_for_ack = 1;
        mutex.unlock();

        do
        {
            getDevice()->write((char*)PackBuffer,PackIndex);

            {
                TMutexLocker lock(&mutex);

                if (1 == waiting_for_ack)
                {
                    surePacket.wait(&mutex,timeout);    //��ʱ����ack��
                }
            }

            /* �޸Ĵ���ͨ�Ŷ�ACK��ȷ��[begin] 2009.9.8 */
            {
                TMutexLocker lock(&mutex);

                if(0 == waiting_for_ack)
                {
                    //�ѽ���ACK
                    break;
                }
            }
            /* �޸Ĵ���ͨ�Ŷ�ACK��ȷ��[end] 2009.9.8 */

            //���ͺ����˳�
            if (isExit)
            {
                return -1;
            }

            putCount++;        //��ʱ��������Դ�����һ

            if (putCount < retry )
            {
                //���·��Ͱ�
                /*Modified by tangxp for BUG NO.3617 on 2008��7��18�� [begin]*/
                /*  Modified brief: ��ʱ�ط�ʱ,��¼ϵͳ��־*/
                sysLoger(LWARNING, "StreamProtocol::putpkt: send data:%c again, retry times:%d",
                         DataBuffer[0], putCount);
                /*Modified by tangxp for BUG NO.3617 on 2008��7��18�� [end]*/
                continue;
            }

            else
            {
                return -2;//��ʱ
            }

        }
        while(1 == waiting_for_ack);

        /*�ȴ���Ӧ*/
        /*Modified by tangxp for BUG NO.3668 on 2008��7��18�� [end]*/

    }

    /*Modified by tangxp for BUG NO.3669 on 2008.7.22 [begin]*/
    /*  Modified brief: ��1�η���ʱ,ʹ���������,��ֹ��������ʱ,������*/
    if((TX_START == txPacketNum)
            || (TX_PACKET_MAX == ++txPacketNum))
    {
        txPacketNum = TX_PACKET_MIN;
    }

    /*Modified by tangxp for BUG NO.3669 on 2008.7.22 [end]*/

    isExit = false;

    return pack->getSize();
}

void StreamProtocol::DataError(int type)
{
    sendCtrlPack(ASCII_CAN, type);
}
/**
* ���ܣ�������ȴ�host����Ӧ�����ж��Ƿ���Ҫ�ط���
* @return �����Ҫ�ط��򷵻�true�����򷵻�false��
*/
bool StreamProtocol::NeedSendAgain(void)
{
    int retryCount = 0;

    while(1)
    {
        char rep = GetCtrlPack();

        switch(rep)
        {
        case ASCII_ACK:
            return true;
        case ASCII_CAN:
        {
            int a = 9;
        }
        break;
        default:
        {
            int a = 9;
        }
        break;
        }

        retryCount++;

        if (retryCount < retry)
        {
            return false;
        }

        else
        {
            sysLoger(LINFO, "StreamProtocol::getpkt: send data again, retry times:%d", retryCount);
            return true;
        }
    }
}

/**
* ���ܣ����տ��ư�
* @return ���ؿ����ź�
*/
char StreamProtocol::GetCtrlPack(void)
{
    char CtlBuf[CTL_PACK_SIZE];
    char ch;

    do
    {
        getDevice()->read(&ch,1);
        CtlBuf[0] = ch;
    }
    while(CtlBuf[0] != ASCII_DLE);

    getDevice()->read(&ch,1);
    CtlBuf[1] = ch;

    return CtlBuf[1];
}
/**
* ���ܣ����Ϳ��ư�
* @param  control ---�����͵Ŀ����ź�
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
*/
void StreamProtocol::sendCtrlPack(char control,int serialNum)
{
    char CtlBuf[6]= {0};
    char* pBuf = CtlBuf;
    *pBuf = ASCII_DLE;
    pBuf += sizeof(char);
    *pBuf = control;
    pBuf += sizeof(char);
    this->getDevice()->write(CtlBuf,2);

}
/**
* ���ܣ��õ����ݰ�
* @param  Packet* �������ݰ�
* @return true��ʾ���ͳɹ�,false��ʾ����ʧ��
*/
int StreamProtocol::getpkt(Packet* pack)
{
    char CurStatus = ASCII_EOT;
    char ParitySum = 0;    //У��λ
    char ParityBuf = 0;
    int  ParityGet = 0;
    char ch1;
    int  PackIndex = 0;    //���������ֵ
    char PackBuffer[PACKET_DATASIZE];
    char HeadBuffer[HEAD_BUF_SIZE];
    int  HeadIndex = 0;    //��ͷ������ֵ
    DeviceUart* pDeviceUart = dynamic_cast<DeviceUart*>(getDevice());

    if (pDeviceUart == NULL)
    {
        return -1;
    }

    if (retry != pDeviceUart->getRetry())
    {
        retry = pDeviceUart->getRetry();
    }

    if (timeout != pDeviceUart->getTimeout())
    {
        timeout = pDeviceUart->getTimeout();
    }

    while(1)
    {
        /*��ȡһ���ַ�*/
start:

        if (getDevice()->read(&ch1,1) < 0)
        {
            return -1;
        }

        /*�����������ȴ��°�*/
        if( CurStatus == ASCII_CAN )
        {
            if( ch1 != ASCII_DLE )
            {
                continue;
            }

            getDevice()->read(&ch1,1);

            if( ch1 != ASCII_SOH )
            {
                continue;
            }
        }

reshuffle:

        switch(ch1)
        {
        case ASCII_SOH: //��ͷ��ʼ
        {
            /*���µ�ǰ״̬*/
            CurStatus = ASCII_SOH;

            /*��ʼ���������*/
            ParitySum = 0;
            PackIndex = 0;
            HeadIndex = 0;
            ParityGet = 0;

            /*��ȡ��ͷ��ͬʱ����У���*/
            getDevice()->read(&ch1,1);

            while(ch1 != ASCII_DLE)
            {
                //�����ͷ
                HeadBuffer[HeadIndex++] = ch1;
                ParitySum = (char)(ParitySum + ch1);
                getDevice()->read(&ch1,1);
            }

            break;
        }
        case ASCII_STX: //���忪ʼ
        {
            /*��鵱ǰ״̬*/
            if(CurStatus != ASCII_SOH)
            {
                if(CurStatus != ASCII_CAN)
                {
                    /*�����ط�*/
                    DataError(1);
                    CurStatus = ASCII_CAN;
                }

                break;
            }

            /*���µ�ǰ״̬*/
            CurStatus = ASCII_STX;

            /*����ͷ�����Ƿ���ȷ*/
            if(HeadIndex != PACKET_HEADSIZE)
            {
                if(CurStatus != ASCII_CAN)
                {
                    /*�����ط�*/
                    DataError(2);
                    CurStatus = ASCII_CAN;
                }

                break;
            }

            /*��ȡ���壬ͬʱ����У���*/
            getDevice()->read(&ch1,1);

            while(ch1 != ASCII_DLE)
            {
                PackBuffer[PackIndex++] = ch1;
                ParitySum = (char)(ParitySum + ch1);
                getDevice()->read(&ch1,1);
            }

            break;
        }
        case ASCII_ETX: //�������
        {
            /*��鵱ǰ״̬*/
            if(CurStatus != ASCII_STX)
            {
                if(CurStatus != ASCII_CAN)
                {
                    /*�����ط�*/
                    DataError(3);
                    CurStatus = ASCII_CAN;
                }

                break;
            }

            /*���µ�ǰ״̬*/
            CurStatus = ASCII_ETX;

            /*��ȡУ��λ*/
            getDevice()->read(&ch1,1);

            while(ch1 != ASCII_DLE)
            {
                ParityBuf = ch1;
                ParityGet++;
                getDevice()->read(&ch1,1);
            }

            break;
        }
        case ASCII_EOT: //��β����
        {
            /*��鵱ǰ״̬*/
            if(CurStatus != ASCII_ETX)
            {
                if(CurStatus != ASCII_CAN)
                {
                    /*�����ط�*/
                    DataError(4);
                    CurStatus = ASCII_CAN;
                }

                break;
            }

            /*���µ�ǰ״̬*/
            CurStatus = ASCII_EOT;

            /*���У���Ƿ���ȷ*/
            if(ParityGet != 1 || ParityBuf != ParitySum)
            {
                if(CurStatus != ASCII_CAN)
                {
                    /*�����ط�*/
                    DataError(5);
                    CurStatus = ASCII_CAN;
                }

                break;
            }

            //�Ѱ�ͷ����ȡ����,�Ƚϰ�ͷ�е����к�,������к�С�������˰�
            int serialNum ;
            {
                readPacketHead(HeadBuffer,pack);
                serialNum = pack->getSerialNum();
                /*Modified by tangxp for BUG NO.3669 on 2008.7.21 [begin]*/
                /*  Modified brief: ��TS��TA��ͨ����Ŷθ�ΪΪ0x00000001~0x3fffffff,TA��1�η�����TS����Ź̶�Ϊ0x40000001.
                    TS�յ����Ϊ0x40000001�����ݰ���,�����Լ��Ľ������Ϊ0x00000001,�������ͻظ�.
                    TS��ʼ�������Ϊ0x40000002,TS������,�յ��κ����ݰ�����ͬ�����
                    ����������Ŵﵽ���޺�����ͬ�����߼�*/
                //����ACK
                sendCtrlPack(ASCII_ACK,serialNum);

                if(rxPacketNum == serialNum)
                {
                    //�ɹ�����
                }
                else if(TA_TX_START == serialNum)
                {
                    //TA��һ�η���
                    rxPacketNum = RX_PACKET_MIN - 1;
                }

                else if(RX_START == rxPacketNum)
                {
                    //TS����,ͬ�����
                    rxPacketNum = serialNum;
                }

                else if( ((rxPacketNum>serialNum) && (rxPacketNum<=serialNum+SYNC_RANGE))
                         || (serialNum+SYNC_RANGE >= RX_PACKET_MAX))
                {
                    //�Ѿ�������İ�,����
                    goto start;
                }

                else
                {
                    //���ݰ���ͬ��
                    rxPacketNum = serialNum;
                }
            }

            ++rxPacketNum;

            if (pack->getSize() <= 0)    //�����˰�
            {
                goto start;
            }

            memcpy(pack->getData(),PackBuffer,pack->getSize());

            /*Modified by tangxp for BUG NO.3669 on 2008.7.21 [end]*/

            if (rxPacketNum == RX_PACKET_MAX)    //���������¿�ʼ
            {
                rxPacketNum = RX_PACKET_MIN;
            }


            /*����*/
            return pack->getSize();
        }
        case ASCII_ACK: //�ش�
        {

            {
                TMutexLocker lock(&mutex);
                waiting_for_ack = 0;
                surePacket.wakeAll();    //����Ͷ�
            }
            /*�յ������˵���Ӧ��Ϣ*/
            goto start;
        }
        case ASCII_CAN: //�ط�
        {

            {
                TMutexLocker lock(&mutex);
                waiting_for_ack = 1;
                surePacket.wakeAll();    //����Ͷ�
            }

            goto start;    //�����ȴ�ȷ�ϰ��Ļظ�
        }
        case ASCII_DLE: //�������ӱ��
        {
            switch(CurStatus)
            {
            case ASCII_SOH: /*��ͷ�г���DLE�ַ�*/
            {
                do
                {
                    /*����DLE�ַ�����ȡ�����ͷ*/
                    do
                    {
                        HeadBuffer[HeadIndex++] = ch1;
                        ParitySum = (char)(ParitySum + ch1);
                        getDevice()->read(&ch1,1);
                    }
                    while(ch1 != ASCII_DLE);

                    getDevice()->read(&ch1,1);

                }
                while(ch1 == ASCII_DLE);

                goto reshuffle;

                break;
            }
            case ASCII_STX: /*�����г���DLE�ַ�*/
            {
                do
                {
                    /*����DLE�ַ�����ȡ�������*/
                    do
                    {
                        PackBuffer[PackIndex++] = ch1;
                        ParitySum = (char)(ParitySum + ch1);
                        getDevice()->read(&ch1,1);
                    }
                    while(ch1 != ASCII_DLE);

                    getDevice()->read(&ch1,1);

                }
                while(ch1 == ASCII_DLE);

                goto reshuffle;

                break;
            }
            case ASCII_ETX:
            {
                do
                {

                    /*����DLE�ַ�����ȡ����У��*/
                    do
                    {
                        ParityBuf = ch1;
                        ParityGet++;
                        getDevice()->read(&ch1,1);
                    }
                    while(ch1 != ASCII_DLE);

                    getDevice()->read(&ch1,1);

                }
                while(ch1 == ASCII_DLE);

                goto reshuffle;

                break;
            }
            case ASCII_EOT:
            case ASCII_CAN:
                break; //�ȴ�SOH/ACK/CAN�ĵ���
            default:
            {
                /*���������ڵ�״̬*/
                //        fnTA_printk("ERROR:0\n");
                //assert(0);
                break;
            }
            }

            break;
        }
        case ASCII_SYN: //ͬ��
        {
            //                    /*�յ�һ��SYN�ź�*/
            //                    RxCtrlSignal = ASCII_SYN;
            //                    /*�����յ������ź��¼�*/
            //                    SetEvent(RxCtrlSigEvent);

            //                sendCtrlPack(ASCII_SYN);

            break;
        }
        default:
        {
            /*�����յ��Ĳ����������ڴ�������*/
            if(CurStatus != ASCII_CAN)
            {
                sysLoger(LWARNING, "StreamProtocol::getpkt: Error encoutered when decoding COM"\
                         " packet");
                CurStatus = ASCII_CAN;
            }

            break;
        }
        }//switch(ch1)
    }//while(1)
}
/**
* ���ܣ��ͷŶ����Ӧ����Դ
*/
void StreamProtocol::close()
{
    isExit = true;            //�յ�ȷ�ϰ�
    {
        TMutexLocker lock(&mutex);
        surePacket.wakeAll();    //���ѷ��Ͱ�
    }
    {
        TMutexLocker lock(&waitMutex);
        m_waitCondition.wait(&waitMutex,10);    //���ѷ��Ͱ�
    }
}
