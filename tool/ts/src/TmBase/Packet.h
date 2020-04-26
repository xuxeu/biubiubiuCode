/*
* 更改历史：
* 2004-12-6 彭宏  北京科银技术有限公司
*                    创建该文件。
*/

/**
* @file  packet.h
* @brief
*       功能：
*       <li>数据包的管理和包的结构</li>
*/



#ifndef PACK_INCLUDE
#define PACK_INCLUDE

/************************头文件********************************/
#include <list>
#include "mutex.h"
#include "tmBaseInclude.h"

/************************宏定义********************************/
#define PACKET_DATASIZE         (1008+3*1024)                    //包数据大小
#define PACKET_HEADSIZE         16                                //包头大小
#define TA_PACKET_DATASIZE      1024

/************************类型定义******************************/
class Packet
{
private:
    int  size;                        //包的长度
    short des_aid;                    //目标AID
    short src_aid;                    //源AID
    short des_said;                    //目标SAID
    short src_said;                    //源SAID
    int      serialNum;                //序列号
    char data[PACKET_DATASIZE] ;    //缓冲区数据
public:


    /**
    * 功能:设置包源AID
    * @param des_said  包的源AID
    */
    void setSrc_aid(short src_aid)
    {
        this->src_aid = src_aid;
    }

    /**
    * 功能:得到包源AID
    * @return des_said  包的源AID
    */
    short getSrc_aid()
    {
        return src_aid;
    }

    /**
    * 功能:设置包源SAID
    * @param des_said  包的源SAID
    */
    void setSrc_said(short src_said)
    {
        this->src_said = src_said;
    }

    /**
    * 功能:得到包源SAID
    * @return des_said  包的源SAID
    */
    short getSrc_said()
    {
        return src_said;
    }


    /**
    * 功能:设置包目标AID
    * @param des_said  包的目标AID
    */
    void setDes_aid(short des_aid)
    {
        this->des_aid = des_aid;
    }

    /**
    * 功能:得到包目标AID
    * @return short  包的目标AID
    */
    short getDes_aid()
    {
        return des_aid;
    }

    /**
    * 功能:设置包目标SAID
    * @param des_said  包的目标SAID
    */
    void setDes_said(short des_said)
    {
        this->des_said = des_said;
    }


    /**
    * 功能:得到包目标SAID
    * @return short  包的目标SAID
    */
    short getDes_said()
    {
        return des_said;
    }
    /**
    *功能：清空数据缓冲
    */
    void clearData()
    {
        memset(data, 0, sizeof(data));
        return;
    }
    /**
    * 功能:得到包的数据
    * @return char*  包数据
    */
    char* getData()
    {
        return data;
    }

    /**
    * 功能:得到包的大小
    * @return size 设置包大小
    */
    int getSize()
    {
        return size;
    }
    /**
    * 功能:设置包的大小
    * @param size 设置包大小
    */
    void setSize(int size)
    {
        this->size = size;
    }
    /**
    * 功能:得到序列号
    * @return 序列号
    */
    int getSerialNum()
    {
        return serialNum;
    }
    /**
    * 功能:设置序列号
    * @param serialNum 序列号
    */
    void setSerialNum(int serialNum)
    {
        this->serialNum = serialNum;
    }
    /**
    * 功能:根据提供的错误信息生成失败回复包
    * @param
    */
    int setErrorPacket(char *errStr, US strsize, int dialogID = -1)
    {
        char *pBuf = data;

        //如果dialogID为-1或是[0,65535]以外的数字就认为是不设置DID部分
        if((dialogID != -1) && (dialogID >= 0) && (dialogID <= 65535))
        {
            __store(dialogID, pBuf, DIDLEN);
            pBuf += DIDLEN;
            memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
            pBuf += CONTROL_FLAG_LEN;
        }

        if(sizeof(data) < strsize)
        {
            return 0;
        }

        memcpy(pBuf, errStr, strsize);
        pBuf += strsize;
        memcpy(pBuf, CONTROL_FLAG, CONTROL_FLAG_LEN);
        pBuf += CONTROL_FLAG_LEN;
        return (int)(pBuf - data);
    }

};

class PacketManager
{
private:
    static PacketManager* pm;
    const static int LOW_LEVEL = 256;     //最低水位标志
    const static int HIGH_LEVEL = 512;    //最高位标志
    static list<Packet*> freeList;        //保存空闲包链表
    static int waterLevel;                //当前水位值
    static TS_Mutex mutex;                        //互斥对象
    TS_WaitCondition bufferNotFull;        //条件变量,用来判断是否达到高水位判断
public:
    ~PacketManager();
    static PacketManager* getInstance()
    {
        if (pm == NULL)
        {
            //多重检查一次
            TS_MutexLocker lock(&mutex);

            if (pm == NULL)
            {
                pm = new PacketManager;
                atexit(destroy);
            }
        }

        return pm;
    }

    /**
    * @brief:
    *   释放资源
    */
    static void destroy()
    {
        if (NULL != pm)
        {
            delete pm;
            pm = NULL;
        }
    }

    //对分配的包指针由调用者负责释放,也可以通过调用PackFree函数来达到释放的目的
    Packet* alloc() ;
    /**
    * 功能:释放一个缓冲包
    * @param pack 一个Packet对象
    * @return 成功返回true，失败返回false
    */
    bool free(Packet* pack);

    //释放所有包
    void freeAllPacket();
};

/************************接口声明******************************/

#endif // !defined(PACKET_INCLUDE)
