/*
* 更改历史：
*  2006-1-11 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  streamProtocol.cpp
* @brief
*       功能：
*       <li>流协议层实现</li>
*/

/************************头文件********************************/
#include ".\streamprotocol.h"
#include "common.h"
//#include "util.h"
#include "deviceUart.h"

/************************宏定义********************************/

/*RFC20网络交换协议所需的ASCII字符定义*/
#define ASCII_NUL   (0x00)    //空符号
#define ASCII_SOH   (0x01)    //头开始
#define ASCII_STX   (0x02)    //文本开始
#define ASCII_ETX   (0x03)    //文本结束
#define ASCII_EOT   (0x04)   //传输结束
#define ASCII_ENQ   (0x05)    //询问
#define ASCII_ACK   (0x06)    //应答
#define ASCII_BEL   (0x07)    //响铃
#define ASCII_BS    (0x08)    //退格
#define ASCII_HT    (0x09)    //水平制表符
#define ASCII_LF    (0x0A)    //换行
#define ASCII_VT    (0x0B)    //垂直制表符
#define ASCII_FF    (0x0C)    //换页
#define ASCII_CR    (0x0D)    //回车
#define ASCII_SO    (0x0E)    //Shift Out
#define ASCII_SI    (0x0F)    //Shift In
#define ASCII_DLE   (0x10)    //数据连接变更
#define ASCII_DC1   (0x11)    //设备控制1
#define ASCII_DC2   (0x12)    //设备控制2
#define ASCII_DC3   (0x13)    //设备控制3
#define ASCII_DC4   (0x14)    //设备控制4
#define ASCII_NAK   (0x15)    //回绝应答
#define ASCII_SYN   (0x16)    //同步空闲
#define ASCII_ETB   (0x17)    //传输块结束
#define ASCII_CAN   (0x18)    //取消
#define ASCII_EM    (0x19)    //中介结束
#define ASCII_SUB   (0x1A)    //替代
#define ASCII_ESC   (0x1B)    //转义字符（Escape）
#define ASCII_FS    (0x1C)    //文件分离
#define ASCII_GS    (0x1D)    //组分离
#define ASCII_RS    (0x1E)    //记录分离
#define ASCII_US    (0x1F)    //单元分离
#define ASCII_SP    (0x20)    //空格
#define ASCII_DEL   (0x7F)    //删除
/*RFC20网络交换协议所需的ASCII字符定义*/


#define HEAD_BUF_SIZE    (64)
#define CTL_PACK_SIZE   (2)


/*消息协议下数据偏移量*/
#define TA_TREATY_STREAM_DATA_OFFSET (0)

/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/



/************************实现*********************************/

/*Modified by tangxp for BUG NO.3669 on 2008.7.21 [begin]*/
/*  Modified brief: 设置TS起始发送,接收序号为特殊序号*/
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
/* 功能：发送数据包
* @param  Packet* 数据包指针
* @return true表示发送成功,false表示发送失败
*/
int StreamProtocol::putpkt(Packet* pack)
{
    /*开始发送*/
    TMutexLocker locker(&putMutex);

    int putCount = 0;            //发送计数
    char PackBuffer[PACKET_DATASIZE];
    int  PackIndex = 0;
    char  HeadBuffer[PACKET_HEADSIZE];    //保存包头
    char* DataBuffer;
    int   HeadIndex = 0;    //包头当前拷贝的索引
    int   DataIndex = 0;    //包体索引
    char  ParitySum = 0;    //校验和
    int   retryCount = 0;    //重试次数计数

    if (pack->getSize() > TA_PACKET_DATASIZE)
    {
        return -1;    //包有问题
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

    /*    if (waiting_for_ack != 2) {//表示没有收到结果包则进行等待一段时间
            TMutexLocker lock(&waitMutex);
            m_waitCondition.wait(&waitMutex,10);    //超时堵塞ack包
        }*/


    {
        //进行包的发送
        /*组装包头*/
        PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ASCII_SOH;
        //对包头进行字节的排列
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
        //序列号
        __store(txPacketNum,pBuf,sizeof(int));
        /*Modified by tangxp for BUG NO.3666 on 2008年7月17日 [begin]*/
        /*  Modified brief: 增加消息包设置序列号*/
        pack->setSerialNum(txPacketNum);
        /*Modified by tangxp for BUG NO.3666 on 2008年7月17日 [end]*/
        pBuf += sizeof(int);

        /*生成包头*/
        for(HeadIndex = 0; HeadIndex < PACKET_HEADSIZE; HeadIndex++)
        {
            if(HeadBuffer[HeadIndex] == ASCII_DLE)      //包头中含有DLE
            {
                PackBuffer[PackIndex++] = ASCII_DLE;    //插入一个DLE
            }

            ParitySum = (char)(ParitySum + HeadBuffer[HeadIndex]);  //计算校验和
            PackBuffer[PackIndex++] = HeadBuffer[HeadIndex];    //生成包头
        }


        /*组装包体*/
        PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ASCII_STX;

        /*获得包的实体*/
        DataBuffer = pack->getData();

        /*生成包体*/
        for(DataIndex = 0; DataIndex < pack->getSize(); DataIndex++)
        {
            if(DataBuffer[DataIndex] == ASCII_DLE)      //包体中含有DLE
            {
                PackBuffer[PackIndex++] = ASCII_DLE;    //插入一个DLE
            }

            ParitySum = (char)(ParitySum + DataBuffer[DataIndex]);  //计算校验和
            PackBuffer[PackIndex++] = DataBuffer[DataIndex];    //生成包体
        }


        /*结束包体*/
        PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ASCII_ETX;

        /*生成校验*/
        if(ParitySum == ASCII_DLE)
        {
            PackBuffer[PackIndex++] = ASCII_DLE;
        }

        PackBuffer[PackIndex++] = ParitySum;

        /*完成包的组装*/
        PackBuffer[PackIndex++] = ASCII_DLE;
        PackBuffer[PackIndex++] = ASCII_EOT;
        PackBuffer[PackIndex] = '\0';

        /*Modified by tangxp for BUG NO.3668 on 2008年7月18日 [begin]*/
        /*  Modified brief: 被唤醒后,检查是否接收ACK*/
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
                    surePacket.wait(&mutex,timeout);    //超时堵塞ack包
                }
            }

            /* 修改串口通信对ACK的确认[begin] 2009.9.8 */
            {
                TMutexLocker lock(&mutex);

                if(0 == waiting_for_ack)
                {
                    //已接收ACK
                    break;
                }
            }
            /* 修改串口通信对ACK的确认[end] 2009.9.8 */

            //发送函数退出
            if (isExit)
            {
                return -1;
            }

            putCount++;        //超时则进行重试次数加一

            if (putCount < retry )
            {
                //重新发送包
                /*Modified by tangxp for BUG NO.3617 on 2008年7月18日 [begin]*/
                /*  Modified brief: 超时重发时,记录系统日志*/
                sysLoger(LWARNING, "StreamProtocol::putpkt: send data:%c again, retry times:%d",
                         DataBuffer[0], putCount);
                /*Modified by tangxp for BUG NO.3617 on 2008年7月18日 [end]*/
                continue;
            }

            else
            {
                return -2;//超时
            }

        }
        while(1 == waiting_for_ack);

        /*等待响应*/
        /*Modified by tangxp for BUG NO.3668 on 2008年7月18日 [end]*/

    }

    /*Modified by tangxp for BUG NO.3669 on 2008.7.22 [begin]*/
    /*  Modified brief: 第1次发包时,使用特殊序号,防止反复连接时,被过滤*/
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
* 功能：发包后等待host的响应，并判断是否需要重发。
* @return 如果需要重发则返回true，否则返回false。
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
* 功能：接收控制包
* @return 返回控制信号
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
* 功能：发送控制包
* @param  control ---待发送的控制信号
* @return true表示发送成功,false表示发送失败
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
* 功能：得到数据包
* @param  Packet* 返回数据包
* @return true表示发送成功,false表示发送失败
*/
int StreamProtocol::getpkt(Packet* pack)
{
    char CurStatus = ASCII_EOT;
    char ParitySum = 0;    //校验位
    char ParityBuf = 0;
    int  ParityGet = 0;
    char ch1;
    int  PackIndex = 0;    //保体的索引值
    char PackBuffer[PACKET_DATASIZE];
    char HeadBuffer[HEAD_BUF_SIZE];
    int  HeadIndex = 0;    //包头的索引值
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
        /*读取一个字符*/
start:

        if (getDevice()->read(&ch1,1) < 0)
        {
            return -1;
        }

        /*处理出错包，等待新包*/
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
        case ASCII_SOH: //包头开始
        {
            /*更新当前状态*/
            CurStatus = ASCII_SOH;

            /*初始化各项变量*/
            ParitySum = 0;
            PackIndex = 0;
            HeadIndex = 0;
            ParityGet = 0;

            /*读取包头，同时计算校验和*/
            getDevice()->read(&ch1,1);

            while(ch1 != ASCII_DLE)
            {
                //读完包头
                HeadBuffer[HeadIndex++] = ch1;
                ParitySum = (char)(ParitySum + ch1);
                getDevice()->read(&ch1,1);
            }

            break;
        }
        case ASCII_STX: //包体开始
        {
            /*检查当前状态*/
            if(CurStatus != ASCII_SOH)
            {
                if(CurStatus != ASCII_CAN)
                {
                    /*出错重发*/
                    DataError(1);
                    CurStatus = ASCII_CAN;
                }

                break;
            }

            /*更新当前状态*/
            CurStatus = ASCII_STX;

            /*检查包头长度是否正确*/
            if(HeadIndex != PACKET_HEADSIZE)
            {
                if(CurStatus != ASCII_CAN)
                {
                    /*出错重发*/
                    DataError(2);
                    CurStatus = ASCII_CAN;
                }

                break;
            }

            /*读取包体，同时计算校验和*/
            getDevice()->read(&ch1,1);

            while(ch1 != ASCII_DLE)
            {
                PackBuffer[PackIndex++] = ch1;
                ParitySum = (char)(ParitySum + ch1);
                getDevice()->read(&ch1,1);
            }

            break;
        }
        case ASCII_ETX: //包体结束
        {
            /*检查当前状态*/
            if(CurStatus != ASCII_STX)
            {
                if(CurStatus != ASCII_CAN)
                {
                    /*出错重发*/
                    DataError(3);
                    CurStatus = ASCII_CAN;
                }

                break;
            }

            /*更新当前状态*/
            CurStatus = ASCII_ETX;

            /*读取校验位*/
            getDevice()->read(&ch1,1);

            while(ch1 != ASCII_DLE)
            {
                ParityBuf = ch1;
                ParityGet++;
                getDevice()->read(&ch1,1);
            }

            break;
        }
        case ASCII_EOT: //包尾结束
        {
            /*检查当前状态*/
            if(CurStatus != ASCII_ETX)
            {
                if(CurStatus != ASCII_CAN)
                {
                    /*出错重发*/
                    DataError(4);
                    CurStatus = ASCII_CAN;
                }

                break;
            }

            /*更新当前状态*/
            CurStatus = ASCII_EOT;

            /*检查校验是否正确*/
            if(ParityGet != 1 || ParityBuf != ParitySum)
            {
                if(CurStatus != ASCII_CAN)
                {
                    /*出错重发*/
                    DataError(5);
                    CurStatus = ASCII_CAN;
                }

                break;
            }

            //把包头都读取完了,比较包头中的序列号,如果序列号小于则丢弃此包
            int serialNum ;
            {
                readPacketHead(HeadBuffer,pack);
                serialNum = pack->getSerialNum();
                /*Modified by tangxp for BUG NO.3669 on 2008.7.21 [begin]*/
                /*  Modified brief: 将TS和TA的通信序号段改为为0x00000001~0x3fffffff,TA第1次发包给TS的序号固定为0x40000001.
                    TS收到序号为0x40000001的数据包后,更新自己的接收序号为0x00000001,并处理发送回复.
                    TS起始接收序号为0x40000002,TS重启后,收到任何数据包立即同步序号
                    修正发送序号达到上限后的序号同步的逻辑*/
                //返回ACK
                sendCtrlPack(ASCII_ACK,serialNum);

                if(rxPacketNum == serialNum)
                {
                    //成功接收
                }
                else if(TA_TX_START == serialNum)
                {
                    //TA第一次发包
                    rxPacketNum = RX_PACKET_MIN - 1;
                }

                else if(RX_START == rxPacketNum)
                {
                    //TS重启,同步序号
                    rxPacketNum = serialNum;
                }

                else if( ((rxPacketNum>serialNum) && (rxPacketNum<=serialNum+SYNC_RANGE))
                         || (serialNum+SYNC_RANGE >= RX_PACKET_MAX))
                {
                    //已经处理过的包,丢弃
                    goto start;
                }

                else
                {
                    //数据包不同步
                    rxPacketNum = serialNum;
                }
            }

            ++rxPacketNum;

            if (pack->getSize() <= 0)    //丢弃此包
            {
                goto start;
            }

            memcpy(pack->getData(),PackBuffer,pack->getSize());

            /*Modified by tangxp for BUG NO.3669 on 2008.7.21 [end]*/

            if (rxPacketNum == RX_PACKET_MAX)    //大于则重新开始
            {
                rxPacketNum = RX_PACKET_MIN;
            }


            /*返回*/
            return pack->getSize();
        }
        case ASCII_ACK: //回答
        {

            {
                TMutexLocker lock(&mutex);
                waiting_for_ack = 0;
                surePacket.wakeAll();    //激活发送端
            }
            /*收到主机端的响应消息*/
            goto start;
        }
        case ASCII_CAN: //重发
        {

            {
                TMutexLocker lock(&mutex);
                waiting_for_ack = 1;
                surePacket.wakeAll();    //激活发送端
            }

            goto start;    //继续等待确认包的回复
        }
        case ASCII_DLE: //数据连接变更
        {
            switch(CurStatus)
            {
            case ASCII_SOH: /*包头中出现DLE字符*/
            {
                do
                {
                    /*保存DLE字符，读取其余包头*/
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
            case ASCII_STX: /*包体中出现DLE字符*/
            {
                do
                {
                    /*保存DLE字符，读取其余包体*/
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

                    /*保存DLE字符，读取其余校验*/
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
                break; //等待SOH/ACK/CAN的到来
            default:
            {
                /*出错：不存在的状态*/
                //        fnTA_printk("ERROR:0\n");
                //assert(0);
                break;
            }
            }

            break;
        }
        case ASCII_SYN: //同步
        {
            //                    /*收到一个SYN信号*/
            //                    RxCtrlSignal = ASCII_SYN;
            //                    /*触发收到控制信号事件*/
            //                    SetEvent(RxCtrlSigEvent);

            //                sendCtrlPack(ASCII_SYN);

            break;
        }
        default:
        {
            /*错误：收到的并非是我们期待的数据*/
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
* 功能：释放对象对应的资源
*/
void StreamProtocol::close()
{
    isExit = true;            //收到确认包
    {
        TMutexLocker lock(&mutex);
        surePacket.wakeAll();    //唤醒发送包
    }
    {
        TMutexLocker lock(&waitMutex);
        m_waitCondition.wait(&waitMutex,10);    //唤醒发送包
    }
}
