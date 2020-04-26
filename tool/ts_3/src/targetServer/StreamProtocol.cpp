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
 * @file     streamProtocol.h
 * @brief     ��Э���
 * @author     ���
 * @date     2006-1-11
 * @version 3.0
 * @warning �Ͻ������ļ������ַ�
 *
 * ���ţ�ϵͳ�� 
 *
 */
#include ".\streamprotocol.h"
#include "common.h"
//#include "util.h"
#include "deviceUart.h"
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

const int RETRY_COUNT = -1;
//�������кŷ�Χ
const int RX_PACKET_MAX = 0x3fffffff;
const int RX_PACKET_MIN = 0x00000001;
//ͬ������Χ
const int SYNC_RANGE = 0x3;

//�����кŷ�Χ
const int TX_PACKET_MAX = 0x7fffffff;
const int TX_PACKET_MIN = 0x40000001;

StreamProtocol::StreamProtocol(DeviceBase* pDevice) : ProtocolBase(pDevice)
{
    txPacketNum = TX_PACKET_MIN;
    rxPacketNum = RX_PACKET_MIN;
    isExit = false;
    waiting_for_ack = 2;
}

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
    DeviceUart* pDeviceUart = dynamic_cast<DeviceUart*>(getDevice());
    if (retry != pDeviceUart->getRetry())
        retry = pDeviceUart->getRetry();
    if (timeout != pDeviceUart->getTimeout())
        timeout = pDeviceUart->getTimeout();

/*    if (waiting_for_ack != 2) {//��ʾû���յ����������еȴ�һ��ʱ��
        TMutexLocker lock(&waitMutex);  
        m_waitCondition.wait(&waitMutex,10);    //��ʱ����ack��
    }*/


    {    //���а��ķ���
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
        pBuf += sizeof(int);

        /*���ɰ�ͷ*/
        for(HeadIndex = 0; HeadIndex < PACKET_HEADSIZE; HeadIndex++) 
        {
            if(HeadBuffer[HeadIndex] == ASCII_DLE)      //��ͷ�к���DLE
                PackBuffer[PackIndex++] = ASCII_DLE;        //����һ��DLE
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
                PackBuffer[PackIndex++] = ASCII_DLE;        //����һ��DLE
            ParitySum = (char)(ParitySum + DataBuffer[DataIndex]);  //����У���
            PackBuffer[PackIndex++] = DataBuffer[DataIndex];    //���ɰ���
        }


        /*��������*/
        PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ASCII_ETX;

        /*����У��*/
        if(ParitySum == ASCII_DLE)
            PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ParitySum;

        /*��ɰ�����װ*/
        PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ASCII_EOT;
        PackBuffer[PackIndex] = '\0';

        do
        {
            waiting_for_ack = 1;
            getDevice()->write((char*)PackBuffer,PackIndex);
                if (waiting_for_ack ==1)
                {
                    TMutexLocker lock(&mutex);  
                    surePacket.wait(&mutex,timeout);    //��ʱ����ack��
                }

                //���ͺ����˳�
                if (isExit)
                    return -1;

                putCount++;        //��ʱ��������Դ�����һ
                if (putCount < retry ) 
                {    //���·��Ͱ�
                    continue;
                }
                else
                    return -1;

        }while(waiting_for_ack != 0);
            /*�ȴ���Ӧ*/
    }

    txPacketNum++;
    if (txPacketNum == TX_PACKET_MAX)
        txPacketNum = TX_PACKET_MIN;
    isExit = false;

    return pack->getSize();
}

void StreamProtocol::DataError(int type)
{
    type;
 /*    char first;
    char second;
 //  sendCtrlPack(ASCII_CAN);

  getDevice()->read(&first,1);
    getDevice()->read(&second,1);

    while(1)
    {
        if ( (first == ASCII_DLE) && (second == ASCII_EOT) )
          break;
         first = second ;
        getDevice()->read(&second,1);
    }*/
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
            return false;
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
    }while(CtlBuf[0] != ASCII_DLE);
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
     char CtlBuf[6]={0};
    char* pBuf = CtlBuf;
    *pBuf = ASCII_DLE;
    pBuf += sizeof(char);
    *pBuf = control;
    pBuf += sizeof(char);
    __store(serialNum,pBuf,sizeof(int));
    pBuf += sizeof(int);
//    fnTA_xprint("\n %s ~%d",CtlBuf,6);
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
        return -1;
    if (retry != pDeviceUart->getRetry())
        retry = pDeviceUart->getRetry();
    if (timeout != pDeviceUart->getTimeout())
        timeout = pDeviceUart->getTimeout();
    
    while(1) 
    {
        /*��ȡһ���ַ�*/
start:
        if (getDevice()->read(&ch1,1) < 0)
            return -1;
        /*�����������ȴ��°�*/
        if( CurStatus == ASCII_CAN )
        {
              if( ch1 != ASCII_DLE )
                continue;

              getDevice()->read(&ch1,1);

              if( ch1 != ASCII_SOH )
                continue;
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
                {    //�����ͷ
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
                    if (rxPacketNum == serialNum)
                    {
                    }
                    else if( (rxPacketNum>serialNum) && (rxPacketNum<serialNum+SYNC_RANGE) )
                    {
                        //���·���ACK
                        sendCtrlPack(ASCII_ACK,serialNum);
                        goto start;
                    }
                    else
                    {
                        //���ݰ���ͬ��
                        rxPacketNum = serialNum;
                    }                
                }                

                ++rxPacketNum;
                if (pack->getSize() < 0)    //�����˰�
                    goto start;
                memcpy(pack->getData(),PackBuffer,pack->getSize());
                
                sendCtrlPack(ASCII_ACK,serialNum);
                

                if (rxPacketNum == RX_PACKET_MAX)    //���������¿�ʼ
                    rxPacketNum = RX_PACKET_MIN;
                //�л���
                {
                    TMutexLocker lock(&mutex);
                    waiting_for_ack = 0;
                    surePacket.wakeAll();    //����Ͷ�
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
                        do{
                            /*����DLE�ַ�����ȡ�����ͷ*/
                            do{
                                HeadBuffer[HeadIndex++] = ch1;
                                ParitySum = (char)(ParitySum + ch1);
                                getDevice()->read(&ch1,1);
                            }
                            while(ch1 != ASCII_DLE);

                            getDevice()->read(&ch1,1);

                        }while(ch1 == ASCII_DLE);

                        goto reshuffle;

                        break;
                    }
                case ASCII_STX: /*�����г���DLE�ַ�*/
                    {
                        do{
                            /*����DLE�ַ�����ȡ�������*/
                            do{
                                PackBuffer[PackIndex++] = ch1;
                                ParitySum = (char)(ParitySum + ch1);
                                getDevice()->read(&ch1,1);
                            }
                            while(ch1 != ASCII_DLE);

                            getDevice()->read(&ch1,1);

                        }while(ch1 == ASCII_DLE);

                        goto reshuffle;

                        break;
                    }
                case ASCII_ETX:
                    {
                        do {

                            /*����DLE�ַ�����ȡ����У��*/
                            do{
                                ParityBuf = ch1;
                                ParityGet++;
                                getDevice()->read(&ch1,1);
                            }
                            while(ch1 != ASCII_DLE);

                            getDevice()->read(&ch1,1);

                        }while(ch1 == ASCII_DLE);

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
